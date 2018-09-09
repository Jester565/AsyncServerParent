#include "ClientManager.h"
#include "Server.h"
#include "IPacket.h"
#include "OPacket.h"
#include "TCPConnection.h"
#include "Client.h"
#include "Logger.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>
using namespace boost::asio::ip;

ClientManager::ClientManager()
{

}

void ClientManager::addClient(ClientPtr client)
{
	client->setDisconnectHandler([&](ClientPtr disconnectedClient) {
		removeClient(disconnectedClient->getID());
	});
	boost::unique_lock <boost::shared_mutex> lock(clientMapMutex);
	clients.emplace(client->getID(), client);
}

ClientPtr ClientManager::getClient(IDType id)
{
	boost::shared_lock <boost::shared_mutex> lock(clientMapMutex);
	ClientIter mapFind = clients.find(id);
	if (mapFind != clients.end())
	{
		return mapFind->second;
	}
	return nullptr;
}

ClientPtr ClientManager::getClient(const std::string & ip, uint16_t port)
{
	boost::shared_lock <boost::shared_mutex> lock(clientMapMutex);
	for (auto it = clients.begin(); it != clients.end(); it++)
	{
		auto rEP = it->second->getTCPConnection()->getSocket()->remote_endpoint();
		if (rEP.address().to_string() == ip && rEP.port() == port)
		{
			return it->second;
		}
	}
	return nullptr;
}

bool ClientManager::removeClient(IDType id)
{
	boost::upgrade_lock <boost::shared_mutex> lock(clientMapMutex);
	ClientIter idFind = clients.find(id);
	if (idFind == clients.end())
	{
		Logger::Log(LOG_LEVEL::Warning, "Could not find and remove client " + std::to_string(id));
		return false;
	}
	else
	{
		boost::upgrade_to_unique_lock <boost::shared_mutex> uniqueLock(lock);
		clients.erase(idFind);
		return true;
	}
}

IDType ClientManager::aquireNextID()
{
	boost::shared_lock <boost::shared_mutex> lock(clientMapMutex);
	for (int i = 1; i < MAX_CLIENTS; i++)
	{
		if (clients.find(i) == clients.end())
		{
			return i;
		}
	}
	return 0;
}

void ClientManager::send(boost::shared_ptr<OPacket> oPack)
{
	if (oPack->getSendToIDs().at(0) == BROADCAST_ID)
	{
		sendToAllExceptSender(oPack);
		return;
	}
	for (int i = 0; i < oPack->getSendToIDs().size(); i++)
	{
		ClientPtr client = getClient(oPack->getSendToIDs().at(i));
		if (client != nullptr)
		{
			send(oPack, client);
		}
		else
		{
			Logger::Log(LOG_LEVEL::Error, "Error occured in ClientManager:send, could not find the client id: " + std::to_string(oPack->getSendToIDs().at(i)));
		}
	}
}

void ClientManager::send(boost::shared_ptr<OPacket> oPack, IDType sendToID)
{
	ClientPtr client = getClient(sendToID);
	if (client == nullptr)
	{
		Logger::Log(LOG_LEVEL::Error, "Error occured in ClientManager::send, could not find client id: " + std::to_string(sendToID));
		return;
	}
	if (client->getTCPConnection() != nullptr) {
		client->getTCPConnection()->send(oPack);
	}
}

void ClientManager::send(boost::shared_ptr<std::vector<unsigned char>> sendData, IDType sendToID)
{
	ClientPtr client = getClient(sendToID);
	if (client == nullptr)
	{
		Logger::Log(LOG_LEVEL::Error, "Error occured in ClientManager:send, could not find client nubmer" + std::to_string(sendToID));
		return;
	}
	if (client->getTCPConnection() != nullptr) {
		client->getTCPConnection()->send(sendData);
	}
}

void ClientManager::close()
{
		clientMapMutex.lock();
		for (auto it = clients.begin(); it != clients.end(); it++) {
			if (it->second->getTCPConnection() != nullptr) {
				it->second->getTCPConnection()->close();
			}
		}
		clientMapMutex.unlock();
}

void ClientManager::send(boost::shared_ptr<OPacket> oPack, ClientPtr client)
{
	if (client == nullptr)
	{
		Logger::Log(LOG_LEVEL::Error, "Error in ClientManager::send, the client is nullptr");
	}
	if (client->getTCPConnection() != nullptr) {
		client->getTCPConnection()->send(oPack);
	}
}

void ClientManager::sendToAll(boost::shared_ptr<OPacket> oPack)
{
	boost::shared_lock <boost::shared_mutex> lock(clientMapMutex);
	for (auto iter = clients.begin(); iter != clients.end(); iter++)
	{
		oPack->setSenderID(iter->first);
		send(oPack, iter->second);
	}
}

void ClientManager::sendToAllExceptSender(boost::shared_ptr<OPacket> oPack)
{
	sendToAllExcept(oPack, oPack->getSenderID());
}

void ClientManager::sendToAllExcept(boost::shared_ptr<OPacket> oPack, IDType excludeID)
{
	boost::shared_lock <boost::shared_mutex> lock(clientMapMutex);
	for (auto iter = clients.begin(); iter != clients.end(); iter++)
	{
		if (iter->first != excludeID)
		{
			send(oPack, iter->second);
		}
	}
}

void ClientManager::sendToAllExcept(boost::shared_ptr<OPacket> oPack, IDType* excludeIDs, unsigned int size)
{
	boost::shared_lock <boost::shared_mutex> lock(clientMapMutex);
	for (auto iter = clients.begin(); iter != clients.end(); iter++)
	{
		for (int i = 0; i < size; i++)
		{
			if (iter->first == excludeIDs[i])
			{
				continue;
			}
		}
		oPack->setSenderID(iter->first);
		send(oPack);
	}
}

ClientManager::~ClientManager()
{
	clients.clear();
}
