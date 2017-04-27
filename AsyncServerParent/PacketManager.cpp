#include "PacketManager.h"
#include "Server.h"
#include "ClientManager.h"
#include "IPacket.h"
#include "OPacket.h"
#include "PKey.h"
#include "Logger.h"
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <list>

PacketManager::PacketManager(Server* server)
	:server(server)
{

}

void PacketManager::addPKey(PKey* pKey)
{
	boost::upgrade_lock <boost::shared_mutex> lock(pKeyMutex);
	bool found = false;
	int index = binarySearchKey(found, pKey->getKey());
	boost::upgrade_to_unique_lock <boost::shared_mutex> uniqueLock(lock);
	if (!found)
	{
		pKeys.insert(pKeys.begin() + index, std::list<PKey*>());
	}
	pKeys.at(index).push_back(pKey);
}

void PacketManager::addPKeys(std::vector <PKey*> aPKeys)
{
	for (int i = 0; i < aPKeys.size(); i++)
		addPKey(aPKeys[i]);
}

bool PacketManager::removePKey(PKey* pKey)
{
	boost::upgrade_lock <boost::shared_mutex> lock(pKeyMutex);
	bool found = false;
	int index = binarySearchKey(found, pKey->getKey());
	if (found)
	{
		for (std::list <PKey*>::iterator it = pKeys.at(index).begin(); it != pKeys.at(index).end(); it++)
		{
			if (pKey == *it)
			{
				boost::upgrade_to_unique_lock <boost::shared_mutex> uniqueLock(lock);
				pKeys.at(index).erase(it);
				if (pKeys.at(index).size() == 0)
				{
					pKeys.erase(pKeys.begin() + index);
				}

				return true;
			}
		}
	}
	return false;
}

int PacketManager::removePKeys(std::vector <PKey*> rPKeys)
{
	int foundCount = 0;
	for (int i = 0; i < rPKeys.size(); i++)
	{
		foundCount += removePKey(rPKeys[i]);
	}
	return rPKeys.size() - foundCount;
}

void PacketManager::process(boost::shared_ptr<IPacket> iPack)
{
	if (iPack->getServerRead())
	{
		serverProcess(iPack);
	}
	else
	{
		boost::shared_ptr<OPacket> oPack = server->createOPacket(iPack, true);
		server->getClientManager()->send(oPack);
	}
}

int PacketManager::binarySearchKey(bool& found, std::string key, int f, int l)
{
	if (pKeys.size() == 0)
	{
		found = false;
		return 0;
	}
	if (l == USE_PKEYS_SIZE)
	{
		l = pKeys.size() - 1;
	}
	std::string midKey = pKeys[(l + f) / 2].front()->getKey();
	if (midKey < key) {
		if ((l + f) / 2 + 1 > l)
		{
			found = false;
			return (l + f) / 2 + 1;
		}
		return binarySearchKey(found, key, (l + f) / 2 + 1, l);
	}
	else if (midKey > key) {
		if ((l + f) / 2 - 1 < f)
		{
			found = false;
			return (l + f) / 2;
		}
		return binarySearchKey(found, key, f, (l + f) / 2 - 1);
	}
	found = true;
	return (l + f) / 2;
}

void PacketManager::serverProcess(boost::shared_ptr<IPacket> iPack)
{
	boost::shared_lock <boost::shared_mutex> lock(pKeyMutex);
	bool found = false;
	int keyI = binarySearchKey(found, iPack->getLocKey());
	if (!found)
	{
		LOG_PRINTF(LOG_LEVEL::Error, "Could not find locKey %d", iPack->getLocKey());
		if (THROW_KEY_NOT_FOUND_EXCEPTION)
			throw std::invalid_argument("Could not find the locKey!");
	}
	else
	{
		for (std::list <PKey*>::const_iterator it = pKeys.at(keyI).begin(); it != pKeys.at(keyI).end(); it++)
			(*it)->run(iPack);
	}
}

PacketManager::~PacketManager()
{
	while (!pKeys.empty())
	{
		while (!pKeys.at(pKeys.size() - 1).empty())
		{
			delete pKeys.at(pKeys.size() - 1).front();
			pKeys.at(pKeys.size() - 1).pop_front();
		}
		pKeys.pop_back();
	}
}