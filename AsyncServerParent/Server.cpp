#include "Server.h"
#include "PacketManager.h"
#include "ClientManager.h"
#include "TCPAcceptor.h"
#include "IPacket.h"
#include "OPacket.h"
#include "Client.h"
#include "HeaderManager.h"
#include "ServicePool.h"
#include <iostream>
#include <google/protobuf/service.h>

Server::Server(const boost::asio::ip::tcp& version)
	:ipVersion(version), pm(nullptr), cm(nullptr), servicePool(nullptr), tcpAcceptor(nullptr)
{
	
}

void Server::createManagers()
{
		servicePool = new ServicePool();
		pm = new PacketManager(this);
		cm = new ClientManager(this);
}

void Server::run(uint16_t port)
{
	tcpAcceptor = boost::make_shared <TCPAcceptor>(this);
	tcpAcceptor->detach(port);
	servicePool->run();
}

HeaderManager* Server::createHeaderManager()
{
	return nullptr;
}
boost::shared_ptr<OPacket> Server::createOPacket()
{
	return boost::make_shared<OPacket>();
}
boost::shared_ptr<IPacket> Server::createIPacket()
{
	return boost::make_shared<IPacket>();
}
boost::shared_ptr<OPacket> Server::createOPacket(boost::shared_ptr<IPacket> iPack, bool copyData)
{
	return boost::make_shared<OPacket>(&(*iPack), copyData);
}

Client * Server::createClient(boost::shared_ptr<TCPConnection> tcpConnection, IDType id)
{
	return new Client(tcpConnection, this, id);
}

void Server::shutdownIO()
{
		if (tcpAcceptor != nullptr) {
				tcpAcceptor->close();
		}
		if (cm != nullptr) {
				cm->close();
		}
}

void Server::destroyManagers()
{
		tcpAcceptor = nullptr;
		if (cm != nullptr)
		{
				delete cm;
				cm = nullptr;
		}
		if (pm != nullptr)
		{
				delete pm;
				pm = nullptr;
		}
}

Server::~Server()
{
		shutdownIO();
		servicePool->stop();
		while (servicePool->isRunning()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(60));
		}
		destroyManagers();
		if (servicePool != nullptr)
		{
				delete servicePool;
				servicePool = nullptr;
		}
		google::protobuf::ShutdownProtobufLibrary();
}
