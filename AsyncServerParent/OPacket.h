/*
Represents a packet being sent out to clients
*/

#pragma once
#include "Macros.h"
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <vector>

//Forward declaration
class IPacket;

class OPacket
{
	//Allows HeaderManager to access all private members
	friend class HeaderManager;
public:
	//CAREFUL: YOU SHOULD ONLY BE CALLING THESE CONSTRUCTORS IF YOU ARE 100% SURE YOU WILL BE USING THE DEFAULT OPacket.  If not, you should be using Server::CreateOPacket().

	//Creates OPacket from the IPacket by copying over its locKey, senderID, sendToIDs and (if copyData is true) the iPacket's data
	OPacket(IPacket* iPack, bool copyData);

	OPacket(const char* loc = nullptr, IDType senderID = 0);

	OPacket(const char* loc, IDType senderID, IDType sendToID);

	OPacket(const char* loc, IDType senderID, IDType* sendToIDs, unsigned int sendToIDsSize);

	OPacket(const char* loc, IDType senderID, std::vector <IDType> sendToIDs);

	//Set the locKey (only reads the first two characters of the loc parameter)
	void setLocKey(const char* loc)
	{
		if (loc != nullptr)
		{
			locKey[0] = loc[0];
			locKey[1] = loc[1];
		}
		else
		{
			locKey[0] = loc[UNDEFINED_LOC];
			locKey[1] = loc[UNDEFINED_LOC];
		}
		locKey[2] = '\0';
	}

	//Set ID of the client sending the packet (if its the server, set it to 0)
	void setSenderID(IDType id)
	{
		senderID = id;
	}

	//Append a client id to send the packet to
	void addSendToID(IDType id)
	{
		sendToIDs.push_back(id);
	}

	//Remove a client id to send the packet to (returns true if found, false otherwise)
	bool removeSendToID(IDType id)
	{
		for (int i = 0; i < sendToIDs.size(); i++)
		{
			if (sendToIDs.at(i) == id)
			{
				sendToIDs.erase(sendToIDs.begin() + i);
				return true;
			}
		}
		return false;
	}

	//Clears all sendToIDs
	void clearSendToIDs(IDType id)
	{
		sendToIDs.clear();
	}

	//Accessor locKey (locKey indicates which callback to call)
	const char* getLocKey() const
	{
		return locKey;
	}

	//Accessor for the id of the client who sent the packet (0 if server)
	const IDType getSenderID() const
	{
		return senderID;
	}

	//Accessor for sendToIDs
	virtual std::vector <IDType> getSendToIDs() const
	{
		return sendToIDs;
	}

	//Set the data of the packet
	bool setData(boost::shared_ptr<std::string> data)
	{
		bool reset = !(data == nullptr);
		this->data = data;
		return reset;
	}

	//Accessor for data
	boost::shared_ptr<std::string> getData()
	{
		return data;
	}

	//Outputs packet information as text (good for console print outs)
	friend std::ostream& operator << (std::ostream& oStream, OPacket& oPack)
	{
		oStream << "Printing out packet: " << std::endl << "loc key: " << oPack.locKey << std::endl;
		oStream << "Sender ID: " << oPack.senderID << std::endl;
		oStream << "Send to IDs: ";
		for (int i = 0; i < oPack.sendToIDs.size(); i++)
		{
			oStream << oPack.sendToIDs.at(i);
			if (i != oPack.sendToIDs.size() - 1)
			{
				oStream << ", ";
			}
		}
		oStream << std::endl;
		return oStream;
	}

	//Puts destructor on virtual table
	virtual ~OPacket();

protected:
	//IMPORTANT: This identifies the purpose of the packet.  A locKey is used to determine which callback will be used to run the function
	char locKey[3];
	//The id of the client who is sending the oPacket (0 if the server)
	IDType senderID;
	//The ids to send the packet to)
	std::vector <IDType> sendToIDs;
	//The packets body data
	boost::shared_ptr<std::string> data;
};
