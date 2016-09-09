#include "TCPAcceptor.h"
#include "Server.h"
#include "ClientManager.h"
#include "TCPConnection.h"
#include "ServicePool.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio/placeholders.hpp>
#include <iostream>

using namespace boost::asio::ip;

TCPAcceptor::TCPAcceptor(Server* server)
	:server(server), errorMode(DEFAULT_ERROR_MODE)
{

}

void TCPAcceptor::runAccept()
{
	tempSocket = new boost::asio::ip::tcp::socket(server->getServicePool()->getNextIOService());
	acceptor->async_accept(*tempSocket, boost::bind(&TCPAcceptor::asyncAcceptHandler, shared_from_this(), boost::asio::placeholders::error));
}

void TCPAcceptor::detach(uint16_t port)
{
	acceptor = new tcp::acceptor(server->getServicePool()->getNextIOService(), tcp::endpoint(server->getIPVersion(), port));
	runAccept();
}

void TCPAcceptor::asyncAcceptHandler(const boost::system::error_code& error)
{
	std::cout << "Accepted" << std::endl;
	if (error)
	{
		std::cerr << "Error occured in TCPAcceptor: " << error.message() << std::endl;
		switch (errorMode)
		{
		case THROW_ON_ERROR:
			throw error;
			break;
		case RETURN_ON_ERROR:
			return;
			break;
		case RECALL_ON_ERROR:
			runAccept();
			return;
		};
	}
	boost::shared_ptr<TCPConnection> tcpConnection = boost::make_shared<TCPConnection>(server, tempSocket);
	tcpConnection->start();
	server->getClientManager()->addClient(tcpConnection);
	runAccept();
}

TCPAcceptor::~TCPAcceptor()
{
	if (acceptor != nullptr)
	{
		delete acceptor;
		acceptor = nullptr;
	}
	if (tempSocket != nullptr)
	{
		delete tempSocket;
		tempSocket = nullptr;
	}
}
