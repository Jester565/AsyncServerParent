/*
	ABSTRACT CLASS: Deserializes all OPackets sent through a TCPConnection and serializes IPackets received by a tcpconnection
*/

#pragma once
#include "Macros.h"
#include <boost/shared_ptr.hpp>
#include <vector>

class OPacket;
class IPacket;


typedef std::function<boost::shared_ptr<IPacket>()> GenIPacketHandler;
class HeaderManager
{
public:
	//Initializes HeaderManager (doesn't do anything special except assign the parameters to their corresponding data member)
	HeaderManager(GenIPacketHandler handler, unsigned int initialReceiveSize = 0);

	unsigned int getInitialReceiveSize() {
		return initialReceiveSize;
	}

	//Given an oPacket, output bytes representing the entire packet (this will call encryptHeaderAsBigEndian or littleEndian depending on your machine)
	virtual boost::shared_ptr<std::vector<unsigned char>> serializePacket(boost::shared_ptr<OPacket> pack) = 0;

	//Given bytes, output an IPacket (this will call decryptHeaderAsLittleEndian or decryptHeaderAsBigEndian depending on your machine)
	virtual boost::shared_ptr<IPacket> parsePacket(unsigned char* data, std::size_t size, unsigned int& bytesToReceive) = 0;

	//Doesn't do anything, just puts the destructor in the virtual table
	virtual ~HeaderManager();

protected:
	GenIPacketHandler genIPacketHandler;
	unsigned int initialReceiveSize;
	bool bEndian;
};
