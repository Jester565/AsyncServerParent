/*
One of your classes may have multiple pKeys and you might have
to handle adding and removing them in your constructor and destructor (kind of a pain)
So you should just inherit PKeyOwner and let this manage all of your PKeys for you
*/

#pragma once
#include "PKey.h"
#include <list>

//Forward declaration
class PacketManager;

class PKeyOwner
{
public:
	virtual void attach(boost::shared_ptr<PacketManager> packetManager);
protected:
	//Add a PKey to the PacketManager as well as to pKeys so that it can later be deleted upon destruction
	virtual void addKey(PKeyPtr pKey);

	//Remove the pKey from the PacketManager and pKeys, but don't delete it
	virtual void removeKey(PKeyPtr pKey);

	//Remove all pKeys that have been added by this PKeyOwner
	virtual void removeKeys();

	//Deletes all your PKeys for you
	virtual ~PKeyOwner();

protected:
	std::list<boost::shared_ptr<PacketManager>> packetManagers;
	//Contains all of the pKeys that have been put into packetManager by the PKeyOwner so they can be easily deleted laster
	std::list<PKeyPtr> pKeys;
};