#include "Client.h"
#include "TCPConnection.h"
#include "Logger.h"

Client::Client(boost::shared_ptr<TCPConnection> tcpConnection, Server* server, IDType id)
	:tcpConnection(tcpConnection), server(server), id(id)
{
	tcpConnection->setCID(id);
	Logger::Log(LOG_LEVEL::DebugHigh, "Create client " + std::to_string(getID()));
}

Client::~Client()
{
	tcpConnection->kill();
	Logger::Log(LOG_LEVEL::DebugHigh, "Destroyed client " + std::to_string(getID()));
}
