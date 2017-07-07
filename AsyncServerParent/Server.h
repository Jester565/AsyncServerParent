/*
An abstract server that stores managers but also serves as a factory
for OPackets, IPackets, clients, etc.
*/

#pragma once
#include "Macros.h"
#include <stdint.h>
#include <thread>
#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio/ip/tcp.hpp>

//Forward declaration
class ClientManager;
class PacketManager;
class TCPAcceptor;
class TCPConnection;
class HeaderManager;
class IPacket;
class OPacket;
class Client;
class ServicePool;


class Server
{
public:
	//Creates a server using the tcp version specified (IPV4 or IPV6)
	Server(const boost::asio::ip::tcp& version);

	//Creates the clientManager, packetManager, etc. (CALL BEFORE INTERACTING WITH NEARLY EVERYTHING)
	virtual void createManagers();

	//Run the server on the specified port (accepts connections from all addresses)
	virtual void run(uint16_t port);

	//Accessor for clientManager
	ClientManager* getClientManager()
	{
		return cm;
	}

	//Accessor for PacketManager
	PacketManager* getPacketManager()
	{
		return pm;
	}

	//Accessor for servicePool
	ServicePool* getServicePool()
	{
		return servicePool;
	}
	
	//Factory: Creates a HeaderManager to use in TCPConnections (you can easily override this with your own HeaderManager)
	virtual HeaderManager* createHeaderManager();
	//Factory: Creates an OPacket (can override with your own OPacket)
	virtual boost::shared_ptr<OPacket> createOPacket();
	//Factory: Creates an IPacket (can override with your own IPacket)
	virtual boost::shared_ptr<IPacket> createIPacket();
	//Factory: Creates an OPacket from an IPacket
	virtual boost::shared_ptr<OPacket> createOPacket(boost::shared_ptr<IPacket> iPack, bool copyData);
	//Factory: Creates a client with an already existing tcpConnection and the client's unique id which is used to identify it over packets(easily override)
	virtual Client* createClient(boost::shared_ptr<TCPConnection> tcpConnection, IDType id);
	//Accessor for ip version
	virtual boost::asio::ip::tcp getIPVersion()
	{
		return ipVersion;
	}
	//Closes all TCPConnections and listeners
	virtual void shutdownIO();

	//Destroy all managers
	virtual void destroyManagers();

	//Stops servicePool, shutdownIO and destroysManager (STOPS PROTOBUF TOO)
	virtual ~Server();

protected:
	//Runs one boost ASIO io_service for every unused thread (num cores - 1)
	ServicePool* servicePool;
	//Handles packets by linking callbacks to locKeys
	PacketManager* pm;
	//Manages all clients, linking a client object to a unique id
	ClientManager* cm;
	//The ip version of the TCPConnections
	boost::asio::ip::tcp ipVersion;
	//Accepts incoming TCPConnections on the port provided by run
	boost::shared_ptr<TCPAcceptor> tcpAcceptor;
};
