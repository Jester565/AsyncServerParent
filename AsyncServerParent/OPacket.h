#pragma once
#include "Macros.h"
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <vector>

class IPacket;

class OPacket
{
	friend class HeaderManager;
public:
	OPacket(IPacket* iPack, bool copyData);

	OPacket(const char* loc = nullptr, IDType senderID = 0);

	OPacket(const char* loc, IDType senderID, IDType sendToID);

	OPacket(const char* loc, IDType senderID, IDType* sendToIDs, unsigned int sendToIDsSize);

	OPacket(const char* loc, IDType senderID, std::vector <IDType> sendToIDs);

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

	void setSenderID(IDType id)
	{
		senderID = id;
	}

	void addSendToID(IDType id)
	{
		sendToIDs.push_back(id);
	}

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

	void clearSendToIDs(IDType id)
	{
		sendToIDs.clear();
	}

	const char* getLocKey() const
	{
		return locKey;
	}

	const IDType getSenderID() const
	{
		return senderID;
	}

	virtual std::vector <IDType> getSendToIDs() const
	{
		return sendToIDs;
	}

	bool setData(boost::shared_ptr<std::string> data)
	{
		bool reset = !(data == nullptr);
		this->data = data;
		return reset;
	}

	boost::shared_ptr<std::string> getData()
	{
		return data;
	}

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

	virtual ~OPacket();

protected:
	char locKey[3];
	IDType senderID;
	std::vector <IDType> sendToIDs;
	boost::shared_ptr<std::string> data;
};
