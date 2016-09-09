#pragma once
#include "Macros.h"
#include <stdint.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>

class ClientManager;
class PacketManager;
class Server;

class TCPAcceptor : public boost::enable_shared_from_this <TCPAcceptor>
{
public:
	TCPAcceptor(Server* server);

	virtual void detach(uint16_t port);

	void setErrorMode(int errorMode)
	{
		this->errorMode = errorMode;
	}

	virtual ~TCPAcceptor();

protected:
	virtual void asyncAcceptHandler(const boost::system::error_code& ec);
	virtual void runAccept();

	Server* server;
	boost::asio::ip::tcp::socket* tempSocket;
	boost::asio::ip::tcp::acceptor* acceptor;
	int errorMode;
};

