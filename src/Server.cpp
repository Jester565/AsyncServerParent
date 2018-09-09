#include "Server.h"
#include "PacketManager.h"
#include "ClientManager.h"
#include "TCPAcceptor.h"
#include "TCPConnection.h"
#include "IPacket.h"
#include "OPacket.h"
#include "Client.h"
#include "HeaderManager.h"
#include "ServicePool.h"
#include <iostream>

Server::Server(const boost::asio::ip::tcp& version)
	:ipVersion(version), clientManager(nullptr), servicePool(nullptr), tcpAcceptor(nullptr)
{
	
}

void Server::createManagers()
{
		servicePool = boost::make_shared<ServicePool>();
		clientManager = boost::make_shared<ClientManager>();
		packetManager = boost::make_shared<PacketManager>(clientManager);
}

void Server::run(uint16_t port)
{
	tcpAcceptor = boost::make_shared<TCPAcceptor>(servicePool, ipVersion, [&](boost::shared_ptr<boost::asio::ip::tcp::socket> socket) {
		auto connection = createTCPConnection(socket);
		auto client = createClient(connection, clientManager->aquireNextID());
		clientManager->addClient(client);
		client->init();
		connection->start();
	});
	tcpAcceptor->detach(port);
	servicePool->run();
}

boost::shared_ptr<OPacket> Server::createOPacket()
{
	return boost::make_shared<OPacket>();
}
boost::shared_ptr<IPacket> Server::createIPacket()
{
	return boost::make_shared<IPacket>();
}

ClientPtr Server::createClient(boost::shared_ptr<TCPConnection> tcpConnection, IDType id)
{
	return boost::make_shared<Client>(id, tcpConnection, packetManager);
}

boost::shared_ptr<TCPConnection> Server::createTCPConnection(boost::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
	return boost::make_shared<TCPConnection>(socket, createHeaderManager());
}

void Server::shutdownIO()
{
		packetManager = nullptr;
		if (tcpAcceptor != nullptr) {
				tcpAcceptor->close();
		}
		if (clientManager != nullptr) {
				clientManager->close();
		}
}

void Server::destroyManagers()
{
		tcpAcceptor = nullptr;
		clientManager = nullptr;
		servicePool = nullptr;
}

Server::~Server()
{
		shutdownIO();
		servicePool->stop();
		while (servicePool->isRunning()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(60));
		}
		destroyManagers();
}
