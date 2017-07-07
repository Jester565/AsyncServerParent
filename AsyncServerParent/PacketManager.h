/*
Links callbacks to locKeys (locKeys are contained in every packet to indicate which function should be called upon its arrival)
*/

#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <vector>
#include <list>
#include <queue>

//Forward declaration
class PKey;
class IPacket;
class Server;

class PacketManager
{
public:
	//Tells binarySearchKey to use the size of pKeys as its last index value
	static const int USE_PKEYS_SIZE = -1;

	//Indicates whether or not to through an exception if a pKey is not found
	static const bool THROW_KEY_NOT_FOUND_EXCEPTION = false;
	
	//Initializes PacketManager (just assigns data members to params)
	PacketManager(Server* server);

	//Add PKey to pKeys
	void addPKey(PKey*);

	//Adds PKey to pKeys
	void addPKeys(std::vector <PKey*>);

	//Removes PKey from pKeys
	bool removePKey(PKey*);

	//Remove multiple PKeys from pKeys
	int removePKeys(std::vector <PKey*>);

	//Given an iPack, if IPacket::serverRead is true, call serverProcess.  If serverRead is false, send the packet to the clients listed in PacketManager.
	void process(boost::shared_ptr<IPacket> iPack);

	//Deletes all pKeys
	~PacketManager();

protected:
	//Given the input packet, find the callback (using IPacket::locKey) and then call the callback
	void serverProcess(boost::shared_ptr<IPacket> iPack);
	//Performs a binarySearch for a PKey in pKeys (used for insertion, searching, and removal)
	int binarySearchKey(bool& found, std::string key, int f = 0, int l = USE_PKEYS_SIZE);
	//Stores a list of PKeys where all PKeys in the list have the same locKey but may have different callbacks
	std::vector <std::list <PKey*>> pKeys;
	//Prevents multiple threads from accessing pKeys at the same time
	boost::shared_mutex pKeyMutex;
	//Gives us access to owners
	Server* server;
};