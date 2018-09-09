#include "PKeyOwner.h"
#include "PacketManager.h"
#include "Client.h"

void PKeyOwner::attach(boost::shared_ptr<PacketManager> pm)
{
	for (auto iter = pKeys.begin(); iter != pKeys.end(); iter++)
	{
		pm->addPKey(*iter);
	}
	packetManagers.push_back(pm);
}

void PKeyOwner::addKey(PKeyPtr pKey)
{
	pKeys.push_back(pKey);
	for (auto it = packetManagers.begin(); it != packetManagers.end(); it++) {
		(*it)->addPKey(pKey);
	}
}

void PKeyOwner::removeKey(PKeyPtr pKey)
{
	pKey->flagForRemoval();
	pKeys.remove(pKey);
}

void PKeyOwner::removeKeys()
{
	while (!pKeys.empty())
		removeKey(pKeys.front());
}

PKeyOwner::~PKeyOwner()
{
	removeKeys();
}
