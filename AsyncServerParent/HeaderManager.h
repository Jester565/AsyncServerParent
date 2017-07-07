/*
	ABSTRACT CLASS: Deserializes all OPackets sent through a TCPConnection and serializes IPackets received by a tcpconnection
*/

#pragma once
#include "Macros.h"
#include <boost/shared_ptr.hpp>
#include <vector>

//Forwared declaration
class Server;
class OPacket;
class IPacket;

class HeaderManager
{
public:
	//Initializes HeaderManager (doesn't do anything special except assign the parameters to their corresponding data member)
	HeaderManager(Server* server);

	//Given an oPacket, output bytes representing the entire packet (this will call encryptHeaderAsBigEndian or littleEndian depending on your machine)
	virtual boost::shared_ptr<std::vector<unsigned char>> encryptHeader(boost::shared_ptr<OPacket> pack);

	//Given bytes, output an IPacket (this will call decryptHeaderAsLittleEndian or decryptHeaderAsBigEndian depending on your machine)
	virtual boost::shared_ptr<IPacket> decryptHeader(unsigned char* data, unsigned int size, IDType cID);

	//Doesn't do anything, just puts the destructor in the virtual table
	virtual ~HeaderManager();

protected:
	virtual boost::shared_ptr<std::vector<unsigned char>> encryptHeaderAsBigEndian(boost::shared_ptr<OPacket> pack);

	virtual boost::shared_ptr<std::vector<unsigned char>> encryptHeaderToBigEndian(boost::shared_ptr<OPacket> pack);

	virtual boost::shared_ptr<IPacket> decryptHeaderAsBigEndian(unsigned char* data, unsigned int size, IDType cID);

	virtual boost::shared_ptr<IPacket> decryptHeaderFromBigEndian(unsigned char* data, unsigned int size, IDType cID);

	//This builds the iPack parameter, settings its data members to the rest of the parameters (you could just construct an IPacket yourself too but this helps share code)
	void setIPack(boost::shared_ptr<IPacket> iPack, char* locKey, IDType sentFromID, std::vector<IDType>& sendToIDs, boost::shared_ptr<std::string> mainData, bool serverRead = true);

	//Allows you access owners, this isn't used in this abstract HeaderManager, but may be useful in children
	Server* server;

	bool bEndian;
};
