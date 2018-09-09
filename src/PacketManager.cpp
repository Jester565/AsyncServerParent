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

PacketManager::PacketManager(boost::shared_ptr<ClientManager> clientManager)
	:clientManager(clientManager)
{

}

void PacketManager::addPKey(PKeyPtr pKey)
{
	boost::unique_lock <boost::shared_mutex> uniqueLock(pKeyMutex);
	pKeys.insert(std::make_pair(pKey->getKey(), pKey));
}

void PacketManager::addPKeys(std::vector<PKeyPtr> aPKeys)
{
	for (int i = 0; i < aPKeys.size(); i++)
		addPKey(aPKeys[i]);
}

bool PacketManager::removePKey(PKeyPtr pKey)
{
	boost::unique_lock <boost::shared_mutex> lock(pKeyMutex);
	auto iters = pKeys.equal_range(pKey->getKey());
	for (auto it = iters.first; it != iters.second; it++) {
		if (pKey == it->second) {
			pKeys.erase(it);
			return true;
		}
	}
	Logger::Log(LOG_LEVEL::Warning, "Failed to remove pkey");
	return false;
}

int PacketManager::removePKeys(std::vector <PKeyPtr> rPKeys)
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
		boost::shared_ptr<OPacket> oPack = iPack->toOPack(true);
		clientManager->send(oPack);
	}
}

void PacketManager::serverProcess(boost::shared_ptr<IPacket> iPack)
{
	boost::shared_lock <boost::shared_mutex> lock(pKeyMutex);
	auto iters = pKeys.equal_range(iPack->getLocKey());
	for (auto iter = iters.first; iter != iters.second;) {
		if (!iter->second->run(iPack)) {
			iter = pKeys.erase(iter);
		}
		else {
			iter++;
		}
	}
}


bool PacketManager::hasPKey(const std::string & key)
{
	boost::shared_lock <boost::shared_mutex> lock(pKeyMutex);
	return (pKeys.find(key) != pKeys.end());
}


bool PacketManager::hasPKey(PKeyPtr pKey)
{
	boost::shared_lock <boost::shared_mutex> lock(pKeyMutex);
	auto iters = pKeys.equal_range(pKey->getKey());
	for (auto iter = iters.first; iter != iters.second;) {
		if (iter->second == pKey) {
			return true;
		}
	}
	return false;
}

PacketManager::~PacketManager()
{

}