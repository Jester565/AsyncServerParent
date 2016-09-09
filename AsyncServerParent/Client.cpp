#include "Client.h"
#include "TCPConnection.h"
#include <iostream>  //temp include

Client::Client(boost::shared_ptr<TCPConnection> tcpConnection, Server* server, IDType id)
	:tcpConnection(tcpConnection), server(server), id(id)
{
	tcpConnection->setCID(id);
}

Client::~Client()
{
	tcpConnection->kill();
	std::cout << "Client destructor called" << std::endl;
}
