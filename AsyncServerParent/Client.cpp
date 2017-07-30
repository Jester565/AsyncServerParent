#include "Client.h"
#include "TCPConnection.h"
#include "Logger.h"
#include "PacketManager.h"

Client::Client(boost::shared_ptr<TCPConnection> tcpConnection, Server* server, IDType id)
	:tcpConnection(tcpConnection), server(server), id(id)
{
	packManager = new PacketManager(server);
	Logger::Log(LOG_LEVEL::DebugHigh, "Created client " + std::to_string(getID()));
}

void Client::init() {
	tcpConnection->setSender(shared_from_this());
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
