/*
Represents a server client
*/

#pragma once
#include "Macros.h"
#include <mutex>
#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>

//Forward declarations
class TCPConnection;
class Server;

class Client
{
public:

	//Creates a client from a TCP Connection
	Client(boost::shared_ptr<TCPConnection> tcpConnection, Server* server, IDType id);

	//Accessor for tcpConnection
	boost::shared_ptr <TCPConnection> getTCPConnection()
	{
		return tcpConnection;
	}

	//Accessor for id
	IDType getID() const
	{
		return id;
	}

	virtual ~Client();

protected:
	//The TCPConnection to the client
	boost::shared_ptr <TCPConnection> tcpConnection;
	/*Provides a way for the client to look at the objects that own it.
	This may be considered bad practice so this may have to be reworked*/
	Server* server;
	//The id uniquely identifies each client
	IDType id;
};
