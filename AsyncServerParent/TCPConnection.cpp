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

TCPConnection::TCPConnection(Server* server, boost::shared_ptr<boost::asio::ip::tcp::socket> boundSocket)
	:server(server), socket(boundSocket), cID(cID), errorMode(DEFAULT_ERROR_MODE), receiveStorage(nullptr), alive(true), sending(false)
{
		if (socket != nullptr) {
				boost::asio::ip::tcp::no_delay naglesOff(true);
				socket->set_option(naglesOff);
		}
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
		return;
	}
	boost::shared_ptr<IPacket> iPack = hm->decryptHeader(receiveStorage->data(), nBytes, cID);
	if (iPack != nullptr)
	{
		server->getPacketManager()->process(iPack);
	}
	read();
}

void TCPConnection::send(boost::shared_ptr<OPacket> oPack)
{
		boost::shared_ptr<std::vector <unsigned char>> sendData = hm->encryptHeader(oPack);
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
		return;
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

void TCPConnection::close()
{
		if (socket != nullptr) {
				boost::system::error_code ec;
				socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
				if (ec)
				{
						std::cerr << "Error when shutting down TCPConnection: " << ec.message() << std::endl;
				}
				socket->close();
		}
}

TCPConnection::~TCPConnection()
{
	std::cout << "TCP CONNECTION DESTRUCTOR CALLED" << std::endl;
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
