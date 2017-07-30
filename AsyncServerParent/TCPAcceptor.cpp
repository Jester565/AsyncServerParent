#include "TCPAcceptor.h"
#include "Server.h"
#include "ClientManager.h"
#include "TCPConnection.h"
#include "ServicePool.h"
#include "Logger.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio/placeholders.hpp>

using namespace boost::asio::ip;

TCPAcceptor::TCPAcceptor(Server* server)
	:server(server)
{

}

void TCPAcceptor::runAccept()
{
	tempSocket = boost::make_shared<boost::asio::ip::tcp::socket>(server->getServicePool()->getNextIOService());
	acceptor->async_accept(*tempSocket, boost::bind(&TCPAcceptor::asyncAcceptHandler, shared_from_this(), boost::asio::placeholders::error));
}

void TCPAcceptor::detach(uint16_t port)
{
	acceptor = boost::make_shared<tcp::acceptor>(server->getServicePool()->getFirstIOService(), tcp::endpoint(server->getIPVersion(), port));
	runAccept();
}

void TCPAcceptor::asyncAcceptHandler(const boost::system::error_code& error)
{
	if (error)
	{
		Logger::Log(LOG_LEVEL::Error, "Error occured in TCPAcceptor: " + error.message());
		return;
	}
	boost::shared_ptr<TCPConnection> tcpConnection = boost::make_shared<TCPConnection>(server, tempSocket);
	tcpConnection->start();
	server->getClientManager()->addClient(tcpConnection);
	runAccept();
}

void TCPAcceptor::close()
{
	Logger::Log(LOG_LEVEL::Error, "Closing Acceptor");
		if (acceptor != nullptr) {
				acceptor->close();
		}
		if (tempSocket != nullptr) {
				boost::system::error_code ec;
				tempSocket->shutdown(tcp::socket::shutdown_both, ec);
				if (ec) {
					Logger::Log(LOG_LEVEL::Error, "Error on TCPAccept closing: " + ec.message());
				}
				tempSocket->close();
		}
}

TCPAcceptor::~TCPAcceptor()
{

}
