#include "Client.h"
#include "TCPConnection.h"
#include "Logger.h"
#include "PacketManager.h"

Client::Client(boost::shared_ptr<TCPConnection> tcpConnection, Server* server, IDType id)
	:tcpConnection(tcpConnection), server(server), id(id)
{
	tcpConnection->setSender(shared_from_this());
	packManager = new PacketManager(server);
	Logger::Log(LOG_LEVEL::DebugHigh, "Create client " + std::to_string(getID()));
}

void Client::flagForRemoval()
{
	tcpConnection->close();
	tcpConnection = nullptr;
}

Client::~Client()
{
	Logger::Log(LOG_LEVEL::DebugHigh, "Destroyed client " + std::to_string(getID()));
}
