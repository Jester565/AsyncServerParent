#pragma once
#include "Macros.h"
#include <stdint.h>
#include <thread>
#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio/ip/tcp.hpp>

class ClientManager;
class PacketManager;
class TCPAcceptor;
class TCPConnection;
class HeaderManager;
class IPacket;
class OPacket;
class Client;
class ServicePool;

#ifndef IPACKET_TYPE
#define IPACKET_TYPE
typedef IPacket IPacketType;
#endif

#ifndef OPACKET_TYPE
#define OPACKET_TYPE
typedef OPacket OPacketType;
#endif

class Server
{
public:
	Server(const boost::asio::ip::tcp& version);

	virtual void createManagers();

	virtual void run(uint16_t port);

	ClientManager* getClientManager()
	{
		return cm;
	}

	PacketManager* getPacketManager()
	{
		return pm;
	}

	ServicePool* getServicePool()
	{
		return servicePool;
	}

	virtual HeaderManager* createHeaderManager();
	virtual boost::shared_ptr<OPacket> createOPacket();
	virtual boost::shared_ptr<IPacket> createIPacket();
	virtual boost::shared_ptr<OPacket> createOPacket(boost::shared_ptr<IPacket> iPack, bool copyData);
	virtual Client* createClient(boost::shared_ptr<TCPConnection> tcpConnection, IDType id);
	virtual boost::asio::ip::tcp getIPVersion()
	{
		return ipVersion;
	}

	virtual ~Server();

protected:
	ServicePool* servicePool;
	PacketManager* pm;
	ClientManager* cm;
	boost::asio::ip::tcp ipVersion;
	boost::shared_ptr<TCPAcceptor> tcpAcceptor;
};
