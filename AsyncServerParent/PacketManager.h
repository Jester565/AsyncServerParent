/*
Links callbacks to locKeys (locKeys are contained in every packet to indicate which function should be called upon its arrival)
*/

#pragma once
#include "Macros.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <unordered_map>
#include <list>
#include <queue>

//Forward declaration
class PKey;
class IPacket;
class ClientManager;

class PacketManager
{
public:
	//Tells binarySearchKey to use the size of pKeys as its last index value
	static const int USE_PKEYS_SIZE = -1;

	//Indicates whether or not to through an exception if a pKey is not found
	static const bool THROW_KEY_NOT_FOUND_EXCEPTION = false;
	
	//Initializes PacketManager (just assigns data members to params)
	PacketManager(boost::shared_ptr<ClientManager> clientManager);

	//Add PKey to pKeys
	void addPKey(PKeyPtr);

	//Adds PKey to pKeys
	void addPKeys(std::vector <PKeyPtr>);

	//Removes PKey from pKeys
	bool removePKey(PKeyPtr);

	//Remove multiple PKeys from pKeys
	int removePKeys(std::vector <PKeyPtr>);

	//Given an iPack, if IPacket::serverRead is true, call serverProcess.  If serverRead is false, send the packet to the clients listed in PacketManager.
	void process(boost::shared_ptr<IPacket> iPack);

	bool hasPKey(const std::string& key);

	bool hasPKey(PKeyPtr);

	//Deletes all pKeys
	~PacketManager();

protected:
	boost::shared_ptr<ClientManager> clientManager;

	//Given the input packet, find the callback (using IPacket::locKey) and then call the callback
	void serverProcess(boost::shared_ptr<IPacket> iPack);
	//Stores a list of PKeys where all PKeys in the list have the same locKey but may have different callbacks
	std::unordered_multimap<std::string, PKeyPtr> pKeys;
	//Prevents multiple threads from accessing pKeys at the same time
	boost::shared_mutex pKeyMutex;
};