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
	ClientPtr getSender()
	{
		return sender;
	}

	void setSender(ClientPtr sender) {
		this->sender = sender;
	}

	int getSenderID();

	//Accessor for locKey
	std::string getLocKey() const
	{
		return locKey;
	}

	void setLocKey(const std::string& locKey) {
		this->locKey = locKey;
	}

	//Accessor for sendToClients
	std::vector <IDType> getSendToClients()
	{
		return sendToClients;
	}

	void setSendToClients(const std::vector<IDType>& sendToClients) {
		this->sendToClients = sendToClients;
	}

	//Accessor for getServerRead
	bool getServerRead()
	{
		return serverRead;
	}

	void setServerRead(bool serverRead) {
		this->serverRead = serverRead;
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

	void setData(boost::shared_ptr<std::string> data)
	{
		this->data = data;
	}

	/*
	Outputs packet information to cout
	*/
	friend std::ostream& operator << (std::ostream& oStream, IPacket& iPack)
	{
		oStream << "Printing out packet: " << std::endl << "loc key: " << iPack.locKey << std::endl;
		oStream << "Sent from: " << iPack.getSenderID() << std::endl;
		oStream << "Send to clients: size = " << iPack.sendToClients.size() << "  Items: ";
		for (int i = 0; i < iPack.sendToClients.size(); i++)
		{
			oStream << iPack.sendToClients.at(i) << " ";
		}
		oStream << std::endl;
		return oStream;
	}

	virtual boost::shared_ptr<OPacket> toOPack(bool copyData);

	//Converts the iPacket into an array of OPacket where each sendToClient gets its own OPacket (not sure when you would need this but its there)
	virtual boost::shared_ptr<std::vector<boost::shared_ptr<OPacket>>> convertToOPacks(bool copyData);

	//Just puts the destructor on the virtual table
	virtual ~IPacket();

protected:
	//The client who we received this packet from
	ClientPtr sender;
	//IMPORTANT: This identifies the purpose of the packet.  A locKey is used to determine which callback will be used to run the function
	std::string locKey;

	//SECURITY ISSUE: Fix before use
	//Some packets may need to be read by the server, others may just go directly to other clients.  If serverRead is false, the packet will immediatly be sent to those in its sendToIDs 
	bool serverRead;

	//The raw data for the packets body
	boost::shared_ptr<std::string> data;

	//The IDs that the sender of the packet wants to sendto
	std::vector <IDType> sendToClients;
};
