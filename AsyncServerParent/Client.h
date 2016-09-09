#pragma once
#include "Macros.h"
#include <mutex>
#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>

class TCPConnection;
class Server;

class Client
{
public:
	Client(boost::shared_ptr<TCPConnection> tcpConnection, Server* server, IDType id);

	boost::shared_ptr <TCPConnection> getTCPConnection()
	{
		return tcpConnection;
	}

	IDType getID() const
	{
		return id;
	}

	virtual ~Client();

protected:
	boost::shared_ptr <TCPConnection> tcpConnection;
	Server* server;
	IDType id;
};
