#include "TCPConnection.h"
#include "Server.h"
#include "PacketManager.h"
#include "HeaderManager.h"
#include "ClientManager.h"
#include "OPacket.h"
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>

int TCPConnection::ReceiveCount = 0;
int TCPConnection::ReceiveBytes = 0;

int TCPConnection::SentCount = 0;

TCPConnection::TCPConnection(Server* server, boost::asio::ip::tcp::socket* boundSocket)
	:server(server), socket(boundSocket), cID(cID), errorMode(DEFAULT_ERROR_MODE), receiveStorage(nullptr), alive(true), sending(false)
{
	hm = server->createHeaderManager();
}

void TCPConnection::start()
{
	read();
}

void TCPConnection::read()
{
	if (alive)
	{
		if (receiveStorage == nullptr)
		{
			receiveStorage = new std::vector<unsigned char>();
			receiveStorage->resize(MAX_DATA_SIZE);
		}
		socket->async_read_some(boost::asio::buffer(*receiveStorage, MAX_DATA_SIZE), boost::bind(&TCPConnection::asyncReceiveHandler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}

void TCPConnection::asyncReceiveHandler(const boost::system::error_code& error, unsigned int nBytes)
{
	ReceiveCount++;
	ReceiveBytes += nBytes;
	if (error)
	{
		if (error == boost::asio::error::connection_reset)
		{
			std::cout << "Connection Closed" << std::endl;
			server->getClientManager()->removeClient(cID);
			return;
		}
		std::cerr << "Error occured in TCP Reading: " << error << " - " << error.message() << std::endl;
		switch (errorMode)
		{
		case THROW_ON_ERROR:
			throw error;
			break;
		case RETURN_ON_ERROR:
			return;
		case RECALL_ON_ERROR:
			read();
			return;
		};
	}
	boost::shared_ptr<IPacket> iPack = hm->decryptHeader(receiveStorage, nBytes, cID);
	if (iPack != nullptr)
	{
		server->getPacketManager()->process(iPack);
	}
	read();
}

void TCPConnection::send(boost::shared_ptr<OPacket> oPack)
{
	boost::shared_ptr<std::vector <unsigned char>> sendData = hm->encryptHeader(oPack);
	if (sendData->size() > MAX_DATA_SIZE)
	{
		std::cerr << "Data of " << oPack->getLocKey() << " had a size of " << sendData->size() << " exceeding the MAX_DATA_SIZE of " << MAX_DATA_SIZE << std::endl;
	}
	sendingMutex.lock();
	if (!sending)
	{
		sending = true;
		sendingMutex.unlock();
		boost::asio::async_write(*socket, boost::asio::buffer(*sendData, sendData->size()), boost::bind(&TCPConnection::asyncSendHandler, shared_from_this(), boost::asio::placeholders::error, sendData));
	}
	else
	{
		sendingMutex.unlock();
		queueSendDataMutex.lock();
		queueSendData.push(sendData);
		queueSendDataMutex.unlock();
	}
}

void TCPConnection::send(boost::shared_ptr<std::vector<unsigned char>> sendData)
{
	if (sendData->size() > MAX_DATA_SIZE)
	{
		std::cerr << "Raw data had a size of " << sendData->size() << " exceeding the MAX_DATA_SIZE of " << MAX_DATA_SIZE << std::endl;
	}
	sendingMutex.lock();
	if (!sending)
	{
		sending = true;
		sendingMutex.unlock();
		boost::asio::async_write(*socket, boost::asio::buffer(*sendData, sendData->size()), boost::bind(&TCPConnection::asyncSendHandler, shared_from_this(), boost::asio::placeholders::error, sendData));
	}
	else
	{
		sendingMutex.unlock();
		queueSendDataMutex.lock();
		queueSendData.push(sendData);
		queueSendDataMutex.unlock();
	}
	//socket->async_write_some(boost::asio::buffer(*sendData), boost::bind(&TCPConnection::asyncSendHandler, shared_from_this(), boost::asio::placeholders::error, sendData));	//do not know if this is thread safe as of now consider using strands
}

void TCPConnection::asyncSendHandler(const boost::system::error_code& error, boost::shared_ptr<std::vector<unsigned char>> sendData)
{
	SentCount++;
	sendingMutex.lock();
	sending = false;
	sendingMutex.unlock();
	if (error)
	{
		if (error == boost::asio::error::connection_reset)
		{
			server->getClientManager()->removeClient(cID);
			return;
		}
		std::cerr << "An error occured in TCP Sending: " << error.message() << std::endl;
		switch (errorMode)
		{
		case THROW_ON_ERROR:
			throw error;
			break;
		case RETURN_ON_ERROR:
			return;
			break;
		};
	}
	queueSendDataMutex.lock();
	while (!queueSendData.empty())
	{
		std::cout << "Queue used" << std::endl;
		boost::shared_ptr<std::vector <unsigned char>> sendData = queueSendData.front();
		queueSendData.pop();
		boost::asio::write(*socket, boost::asio::buffer(*sendData, sendData->size()));
	}
	queueSendDataMutex.unlock();
}

TCPConnection::~TCPConnection()
{
	std::cout << "TCP CONNECTION DESTRUCTOR CALLED" << std::endl;
	if (socket != nullptr)
	{
		boost::system::error_code ec;
		socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		if (ec)
		{
			std::cerr << "Error when shutting down TCPConnection: " << ec.message() << std::endl;
		}
		socket->close();
	}
	if (hm != nullptr)
	{
		delete hm;
		hm = nullptr;
	}
	if (socket != nullptr)
	{
		delete socket;
		socket = nullptr;
	}
}
