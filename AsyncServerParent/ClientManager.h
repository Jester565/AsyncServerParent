#pragma once
#include "Macros.h"
#include <boost/asio/io_service.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <map>

class TCPConnection;
class PacketManager;
class IPacket;
class OPacket;
class Client;
class Server;

class ClientManager
{
public:
	static const unsigned int MAX_CLIENTS = 65535;

	ClientManager(Server* server);

	virtual Client* addClient(boost::shared_ptr <TCPConnection> tcpConnection);

	virtual Client* getClient(IDType id);

	virtual Client* getClient(const std::string& ip, uint16_t port);
	
	virtual bool removeClient(IDType id);

	std::map <IDType, Client*> getIDClientMap() const
	{
		return clients;
	}

	virtual void send(boost::shared_ptr<OPacket> oPack);

	virtual void send(boost::shared_ptr<OPacket> oPack, IDType sendTo);

	virtual void send(boost::shared_ptr<OPacket> oPack, Client* sendToClient);

	virtual void sendToAll(boost::shared_ptr<OPacket> oPack);

	virtual void sendToAllExceptSender(boost::shared_ptr<OPacket> oPack);

	virtual void sendToAllExcept(boost::shared_ptr<OPacket> oPack, IDType excludeID);

	virtual void sendToAllExcept(boost::shared_ptr<OPacket> oPack, IDType* excludeIDs, unsigned int size);

	virtual void send(boost::shared_ptr<std::vector<unsigned char>> sendData, IDType sendToID);

	virtual void close();

	void setErrorMode(int mode)
	{
		errorMode = mode;
	}

	int getErrorMode() const
	{
		return errorMode;
	}

	boost::shared_mutex clientMapMutex;

	virtual ~ClientManager();

protected:

	std::map <IDType, Client*> clients;
	IDType aquireNextID();
	Server* server;
	int errorMode;
};

