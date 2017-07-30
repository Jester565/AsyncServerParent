/*
Accepts incoming TCPConnections
*/

#pragma once
#include "Macros.h"
#include <stdint.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>

//Forward declaration
class ClientManager;
class PacketManager;
class Server;

class TCPAcceptor : public boost::enable_shared_from_this <TCPAcceptor>
{
public:
	//Initializes TCPAcceptor (just assigns data members to the params)
	TCPAcceptor(Server* server);

	//Begins running the TCPAcceptor to receive connections from the specified port (this actually doesn't create a new thread, bad naming)
	virtual void detach(uint16_t port);

	//Stops TCPAcceptor from listening
	void close();

	//Puts destructor on virtual table
	virtual ~TCPAcceptor();

protected:
	//The handler called when a connection is made (adds a client to clientManager)
	virtual void asyncAcceptHandler(const boost::system::error_code& ec);
	//Prepare to accept the next connection
	virtual void runAccept();

	//Allows us to access clientManager and other managers
	Server* server;

	/*When accepting a connection, you need to create a socket
	while listening. Once a connection has been made this socket will
	be given to the client and a new socket will be created. Hence the name.
	*/
	boost::shared_ptr <boost::asio::ip::tcp::socket> tempSocket;
	/*
	Boost's acceptor
	*/
	boost::shared_ptr <boost::asio::ip::tcp::acceptor> acceptor;
};

