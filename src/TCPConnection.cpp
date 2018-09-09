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


TCPConnection::TCPConnection(boost::shared_ptr<boost::asio::ip::tcp::socket> boundSocket, boost::shared_ptr<HeaderManager> headerManager, DataHandler dataHandler, DisconnectHandler disconnectHandler)
	:socket(boundSocket), headerManager(headerManager), alive(true), receiveStorage(nullptr)
{
	if (socket != nullptr) {
		//This will disable nagles algorithm which is supposed to reduce the number of packets coming over a network (putting many small packets into one large packet).  However, I'm in favor of sending packets as fast as possible
		boost::asio::ip::tcp::no_delay naglesOff(true);
		socket->set_option(naglesOff);
	}
}

void TCPConnection::start()
{
	//Create the buffer for storing received data
	receiveStorage = boost::make_shared<std::vector<unsigned char>>();
	receiveStorage->resize(MAX_DATA_SIZE);
	read(headerManager->getInitialReceiveSize());
}

void TCPConnection::read(unsigned int readSize)
{
	//If the TCPConnection has not been closed, continue reading
	if (alive)
	{
			if (readSize > 0) {
				//Link the asyncReceiveCallback to be called when data size is received
				boost::asio::async_read(*socket, boost::asio::buffer(*receiveStorage, MAX_DATA_SIZE), boost::asio::transfer_exactly(readSize), boost::bind(&TCPConnection::asyncReceiveHandler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			}
			else {
				//Receive some unspecified amount of data
				socket->async_read_some(boost::asio::buffer(*receiveStorage, MAX_DATA_SIZE), boost::bind(&TCPConnection::asyncReceiveHandler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			}
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
		LOG_PRINTF(LOG_LEVEL::Error, "Error occured in TCP Reading: %s%s%s", error, " - ", error.message());
		if (disconnectHandler != nullptr) {
			disconnectHandler();
		}
		return;
	}
	unsigned int bytesToRead = 0;
	boost::shared_ptr<IPacket> iPack = headerManager->parsePacket(receiveStorage->data(), nBytes, bytesToRead);
	if (iPack != nullptr && dataHandler != nullptr) {
		dataHandler(iPack);
	}
	//begin reading again
	read(bytesToRead);
}

//Send the OPacket to the client
void TCPConnection::send(boost::shared_ptr<OPacket> oPack)
{
	//LOG_PRINTF(LOG_LEVEL::DebugLow, "Sending pack %s to id", oPack->getLocKey());
	//convert packet to binary	
	boost::shared_ptr<std::vector<unsigned char>> sendData = headerManager->serializePacket(oPack);
	//If we are not waiting on a sendHandler to be called, then we will initiate an async_write
	send(sendData);
}

//send raw data to the client
void TCPConnection::send(boost::shared_ptr<std::vector<unsigned char>> sendData)
{
	queueSendDataMutex.lock();
	bool empty = queueSendData.empty();
	queueSendData.push(sendData);
	if (empty)
	{
		boost::asio::async_write(*socket, boost::asio::buffer(*sendData, sendData->size()), boost::bind(&TCPConnection::asyncSendHandler, shared_from_this(), boost::asio::placeholders::error, sendData));
	}
	queueSendDataMutex.unlock();
}

void TCPConnection::asyncSendHandler(const boost::system::error_code& error, boost::shared_ptr<std::vector<unsigned char>> sendData)
{
	if (error)
	{
		Logger::Log(LOG_LEVEL::Error, "An error occured in TCP Sending: " + error.message());
		if (disconnectHandler != nullptr) {
			disconnectHandler();
		}
		return;
	}
	queueSendDataMutex.lock();
	queueSendData.pop();
	if (!queueSendData.empty()) {
		boost::shared_ptr<std::vector<unsigned char>> sendData = queueSendData.front();
		boost::asio::async_write(*socket, boost::asio::buffer(*sendData, sendData->size()), boost::bind(&TCPConnection::asyncSendHandler, shared_from_this(), boost::asio::placeholders::error, sendData));
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

}
