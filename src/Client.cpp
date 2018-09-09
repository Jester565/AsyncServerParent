#include "Client.h"
#include "TCPConnection.h"
#include "Logger.h"
#include "PacketManager.h"
#include "IPacket.h"

Client::Client(IDType id, boost::shared_ptr<TCPConnection> tcpConnection, boost::shared_ptr<PacketManager> pm, ClientDisconnectHandler handler)
	:id(id), tcpConnection(tcpConnection), packetManager(pm), disconnectHandler(handler)
{
	Logger::Log(LOG_LEVEL::DebugHigh, "Created client " + std::to_string(getID()));
}

void Client::init() {
	tcpConnection->setDataHandler(std::bind(&Client::onData, shared_from_this(), std::placeholders::_1));
	tcpConnection->setDisconnectHandler(std::bind(&Client::onDisconnect, shared_from_this()));
}

void Client::onData(boost::shared_ptr<IPacket> iPack) {
	iPack->setSender(shared_from_this());
	packetManager->process(iPack);
}

void Client::onDisconnect() {
	if (disconnectHandler != nullptr) {
		disconnectHandler(shared_from_this());
	}
}

void Client::disconnect()
{
	tcpConnection->close();
	tcpConnection = nullptr;
}

Client::~Client()
{
	Logger::Log(LOG_LEVEL::DebugHigh, "Destroyed client " + std::to_string(getID()));
}
