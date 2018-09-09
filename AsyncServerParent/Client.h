/*
Represents a server client
*/

#pragma once
#include "Macros.h"
#include <mutex>
#include <unordered_map>
#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

//Forward declarations
class TCPConnection;
class Server;
class PKey;
class PacketManager;
class IPacket;

typedef std::function<void(boost::shared_ptr<Client>)> ClientDisconnectHandler;

class Client : public boost::enable_shared_from_this<Client>
{
public:
	//Creates a client from a TCP Connection
	Client(IDType id, boost::shared_ptr<TCPConnection> tcpConnection, boost::shared_ptr<PacketManager> pm, ClientDisconnectHandler handler = nullptr);

	virtual void init();

	virtual void onData(boost::shared_ptr<IPacket> iPack);

	virtual void onDisconnect();

	virtual void setDisconnectHandler(ClientDisconnectHandler handler) {
		disconnectHandler = handler;
	}

	//Accessor for tcpConnection
	boost::shared_ptr<TCPConnection> getTCPConnection()
	{
		return tcpConnection;
	}

	boost::shared_ptr<PacketManager> getPacketManager() {
		return packetManager;
	}

	//Accessor for id
	IDType getID() const
	{
		return id;
	}

	virtual void disconnect();

	virtual ~Client();

protected:
	boost::shared_ptr<PacketManager> packetManager;
	//The TCPConnection to the client
	boost::shared_ptr <TCPConnection> tcpConnection;
	//The id uniquely identifies each client
	IDType id;

	ClientDisconnectHandler disconnectHandler;

	template <typename Derived>
	std::shared_ptr<Derived> shared_from_base()
	{
		return std::static_pointer_cast<Derived>(shared_from_this());
	}
};
