#include "TCPConnection.h"
#include "Server.h"
#include "PacketManager.h"
#include "HeaderManager.h"
#include "ClientManager.h"
#include "OPacket.h"
#include "IPacket.h"
#include "Logger.h"
#include "Client.h"
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

TCPConnection::TCPConnection(Server* server, boost::shared_ptr<boost::asio::ip::tcp::socket> boundSocket)
	:server(server), socket(boundSocket), receiveStorage(nullptr), alive(true), sending(false)
{
		if (socket != nullptr) {
			//This will disable nagles algorithm which is supposed to reduce the number of packets coming over a network (putting many small packets into one large packet).  However, I'm in favor of sending packets as fast as possible
				boost::asio::ip::tcp::no_delay naglesOff(true);
				socket->set_option(naglesOff);
		}
		//Create the headerManager that will be serializing and derserializing raw data to IPacket and OPacket objects
		hm = server->createHeaderManager();
}

void TCPConnection::start()
{
	read();
	//Create the buffer for storing received data
	receiveStorage = new std::vector<unsigned char>();
	receiveStorage->resize(MAX_DATA_SIZE);
}

void TCPConnection::read()
{
	//If the TCPConnection has not been closed, continue reading
	if (alive)
	{
		//Link the asyncReceiveCallback to be called when data is received
		socket->async_read_some(boost::asio::buffer(*receiveStorage, MAX_DATA_SIZE), boost::bind(&TCPConnection::asyncReceiveHandler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}

/*
A WORD OF WARNING: when asyncReceiveHandler is called, it can actually contain data from multiple packets.
This implementation doesn't handle that scenario but the TCPConnection in Boost_WS_Server will.
*/

void TCPConnection::asyncReceiveHandler(const boost::system::error_code& error, unsigned int nBytes)
{
	//If theres an error, that could just mean the client closed the connection
	if (error)
	{
		//if the client just closed the connection, remove ourselves rom the clientManager
		if (error == boost::asio::error::connection_reset)
		{
			server->getClientManager()->removeClient(sender->getID());
			return;
		}

		LOG_PRINTF(LOG_LEVEL::Error, "Error occured in TCP Reading: %s%s%s", error, " - ", error.message());
		return;
	}
	//Parse the raw message and convert it into an IPacket
	boost::shared_ptr<IPacket> iPack = hm->decryptHeader(receiveStorage->data(), nBytes, sender);
	if (iPack != nullptr)
	{
		LOG_PRINTF(LOG_LEVEL::DebugLow, "Received pack %s from id %d", iPack->getLocKey(), sender->getID());
		//send the iPacket to packetManager so the correct callback can be called
		sender->getPacketManager()->process(iPack);
	}
	//begin reading again
	read();
}

//Send the OPacket to the client
void TCPConnection::send(boost::shared_ptr<OPacket> oPack)
{
	LOG_PRINTF(LOG_LEVEL::DebugLow, "Sending pack %s to id %d", oPack->getLocKey(), sender->getID());
	//convert packet to binary	
	boost::shared_ptr<std::vector <unsigned char>> sendData = hm->encryptHeader(oPack);
	//prevent multiple thread sfrom modifying the queue of data to be sent	
	sendingMutex.lock();
	//If we are not waiting on a sendHandler to be called, then we will initiate an async_write 	
	if (!sending)
		{
				sending = true;
				sendingMutex.unlock();
				boost::asio::async_write(*socket, boost::asio::buffer(*sendData, sendData->size()), boost::bind(&TCPConnection::asyncSendHandler, shared_from_this(), boost::asio::placeholders::error, sendData));
		}
		else  //if we are waiting for a sendHandler to be called, might as well add this data to a queue so that sendHandler can send it rather than making a whole extra call to async_write
		{
				queueSendDataMutex.lock();
				sendingMutex.unlock();
				queueSendData.push(sendData);
				queueSendDataMutex.unlock();
		}
}

//send raw data to the client
void TCPConnection::send(boost::shared_ptr<std::vector<unsigned char>> sendData)
{
		sendingMutex.lock();
		if (!sending)
		{
				sending = true;
				sendingMutex.unlock();
				boost::asio::async_write(*socket, boost::asio::buffer(*sendData, sendData->size()), boost::bind(&TCPConnection::asyncSendHandler, shared_from_this(), boost::asio::placeholders::error, sendData));
		}
		else
		{
				queueSendDataMutex.lock();
				sendingMutex.unlock();
				queueSendData.push(sendData);
				queueSendDataMutex.unlock();
		}
}

void TCPConnection::asyncSendHandler(const boost::system::error_code& error, boost::shared_ptr<std::vector<unsigned char>> sendData)
{
	sendingMutex.lock();
	sending = false;
	sendingMutex.unlock();
	if (error)
	{
		if (error == boost::asio::error::connection_reset)
		{
			server->getClientManager()->removeClient(sender->getID());
			return;
		}
		Logger::Log(LOG_LEVEL::Error, "An error occured in TCP Sending: " + error.message());
		return;
	}
	queueSendDataMutex.lock();
	while (!queueSendData.empty())
	{
		boost::shared_ptr<std::vector <unsigned char>> sendData = queueSendData.front();
		queueSendData.pop();
		boost::asio::write(*socket, boost::asio::buffer(*sendData, sendData->size()));
	}
	queueSendDataMutex.unlock();
}

void TCPConnection::close()
{
	this->alive = false;
	if (socket != nullptr) {
		boost::system::error_code ec;
		socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		if (ec)
		{
			Logger::Log(LOG_LEVEL::Error, "Error when shutting down TCPConnection: " + ec.message());
		}
		socket->close();
	}
}

TCPConnection::~TCPConnection()
{
	if (hm != nullptr)
	{
		delete hm;
		hm = nullptr;
	}
	if (receiveStorage != nullptr) {
		delete receiveStorage;
		receiveStorage = nullptr;
	}
}
