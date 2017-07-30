/*
Manages all clients. This will handle client removal, 
client connection, and will allow you to send packets to clients
based on their unique id.
*/

#pragma once
#include "Macros.h"
#include <boost/asio/io_service.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <unordered_map>

//Forward declaration
class TCPConnection;
class PacketManager;
class IPacket;
class OPacket;
class Client;
class Server;

typedef std::unordered_map <IDType, ClientPtr>::iterator ClientIter;

class ClientManager
{
public:
	//This is the maximum number of clients that can connect at once
	static const unsigned int MAX_CLIENTS = 65535;

	//Initializes ClientManager (doesn't do anything special except assign the parameters to their corresponding data member)
	ClientManager(Server* server);

	//Gets a unique id and then adds created Client obj to clients
	virtual ClientPtr addClient(boost::shared_ptr <TCPConnection> tcpConnection);

	//Get client from clients map using a unique id
	virtual ClientPtr getClient(IDType id);

	//Get client from clients map using their address and port (I don't use this often but its there)
	virtual ClientPtr getClient(const std::string& ip, uint16_t port);
	
	//Destroy client and remove it from clients map
	virtual bool removeClient(IDType id);

	//Accessor for clients map
	std::unordered_map <IDType, ClientPtr> getIDClientMap() const
	{
		return clients;
	}

	//Send the oPacket to the clients whose ids are in OPacket::sendToIds
	virtual void send(boost::shared_ptr<OPacket> oPack);

	//Send the oPacket only to the sendTo id (will not look at OPacket::sendToIds)
	virtual void send(boost::shared_ptr<OPacket> oPack, IDType sendTo);

	//Send the oPacket only to the sendToClient (will not look at OPacket::sendToIds)
	virtual void send(boost::shared_ptr<OPacket> oPack, ClientPtr sendToClient);

	//Sends packet to all connected clients (will not look at OPacket::sendToIds)
	virtual void sendToAll(boost::shared_ptr<OPacket> oPack);

	//Sends packet to all connected clients except for the client with the id of OPacket::senderID (will not look at OPacket::sendToIds)
	virtual void sendToAllExceptSender(boost::shared_ptr<OPacket> oPack);

	//Sends packet to all connected clients except the client with the excludeID (will not look at OPacket::sendToIds)
	virtual void sendToAllExcept(boost::shared_ptr<OPacket> oPack, IDType excludeID);

	//Sends packet to all connected client except for the clients with ids in the exclueIDs array (will not look at OPacket::sendToIds)
	virtual void sendToAllExcept(boost::shared_ptr<OPacket> oPack, IDType* excludeIDs, unsigned int size);

	//Sends raw data to the client with the sendToID
	virtual void send(boost::shared_ptr<std::vector<unsigned char>> sendData, IDType sendToID);

	//Will close the TCPConnection of all clients (does not delete the clients though), this is used when destroying the server because Boost::Asio is very picky about destruction
	virtual void close();

	//Prevents multiple threads from accessing/modifying the clients map
	boost::shared_mutex clientMapMutex;

	//Will delete all clients
	virtual ~ClientManager();

protected:
	//unordered_map allows for faster access (its Java's hashmap equivalent)
	std::unordered_map <IDType, ClientPtr> clients;
	//Iterates through all the ids (starting at 1) and looks for the lowest availible one (maybe this could be more efficient)
	IDType aquireNextID();
	//Stores the server so we can access and owners and call CreateClient()
	Server* server;
};

