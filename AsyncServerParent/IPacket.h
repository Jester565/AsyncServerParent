/*
Represents a packet received from a client
*/

#pragma once
#include "Macros.h"
#include <vector>
#include <string>
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

//Forward declartion
class OPacket;

class IPacket
{
	//This allows HeaderManager to look at all of our private fields
	friend class HeaderManager;
public:
	//CAREFUL: YOU SHOULD ONLY BE CALLING THESE CONSTRUCTORS IF YOU ARE 100% SURE YOU WILL BE USING THE DEFAULT IPacket.  If not, you should be using Server::CreateIPacket().
	
	//Initializes IPacket by setting locKey to a value that indicates the packet is undefined
	IPacket();

	//Accessor for sentFromID
	IDType getSentFromID()
	{
		return sentFromID;
	}

	//Accessor for locKey
	const char* getLocKey() const
	{
		return locKey;
	}

	//Accessor for sendToClients
	std::vector <IDType> getSendToClients()
	{
		return sendToClients;
	}

	//Accessor for getServerRead
	bool getServerRead()
	{
		return serverRead;
	}

	//Accessor for the packets body data
	const boost::shared_ptr<std::string> getData() const
	{
		return data;
	}
	
	//Set the packet body data
	template <typename T>
	void setData(T begin, T end)
	{
		data = boost::make_shared <std::string>(begin, end);
	}

	/*
	gets the size of the packets body data
	I would avoid using this and just use the size method on your actual data
	because this will return the data size given by the header, not of the actual
	data itself
	*/
	uint32_t getDataSize()
	{
		if (data == nullptr)
		{
			return dataSize;
		}
		else
		{
			return data->size();
		}
	}

	/*
	Sets the size of the data (the TCPConnection only needs this)
	*/
	void setDataSize(uint32_t dataSize)
	{
		this->dataSize = dataSize;
	}

	/*
	Outputs packet information to cout
	*/
	friend std::ostream& operator << (std::ostream& oStream, IPacket& iPack)
	{
		oStream << "Printing out packet: " << std::endl << "loc key: " << iPack.locKey << std::endl;
		oStream << "Sent from: " << iPack.sentFromID << std::endl;
		oStream << "Send to clients: size = " << iPack.sendToClients.size() << "  Items: ";
		for (int i = 0; i < iPack.sendToClients.size(); i++)
		{
			oStream << iPack.sendToClients.at(i) << " ";
		}
		oStream << std::endl;
		return oStream;
	}

	//Converts the iPacket into an array of OPacket where each sendToClient gets its own OPacket (not sure when you would need this but its there)
	virtual std::vector<boost::shared_ptr<OPacket>>* convertToOPacks(bool copyData);

	//Just puts the destructor on the virtual table
	virtual ~IPacket();

protected:
	//The dataSize that the header has predicted (may not be the size of the actual data)
	uint32_t dataSize;
	//The id of the client who we received this packet from
	IDType sentFromID;
	//IMPORTANT: This identifies the purpose of the packet.  A locKey is used to determine which callback will be used to run the function
	char locKey[3];

	//SECURITY ISSUE: Fix before use
	//Some packets may need to be read by the server, others may just go directly to other clients.  If serverRead is false, the packet will immediatly be sent to those in its sendToIDs 
	bool serverRead;

	//The raw data for the packets body
	boost::shared_ptr<std::string> data;

	//The IDs that the sender of the packet wants to sendto
	std::vector <IDType> sendToClients;
};
