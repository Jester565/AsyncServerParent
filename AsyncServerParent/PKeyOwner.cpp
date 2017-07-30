#include "PKeyOwner.h"
#include "PacketManager.h"
#include "Client.h"

void PKeyOwner::follow(ClientPtr client)
{
	PacketManager* pm = client->getPacketManager();
	for (auto iter = pKeys.begin(); iter != pKeys.end(); iter++)
	{
		pm->addPKey(*iter);
	}
}

void PKeyOwner::addKey(PKeyPtr pKey)
{
	pKeys.push_back(pKey);
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
