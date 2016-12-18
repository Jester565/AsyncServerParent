#pragma once
#include "Macros.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/thread/mutex.hpp>
#include <vector>
#include <queue>

class Server;
class OPacket;
class HeaderManager;

class TCPConnection : public boost::enable_shared_from_this<TCPConnection>
{
public:

	TCPConnection(Server* server, boost::shared_ptr<boost::asio::ip::tcp::socket> boundSocket);

	virtual void start();

	virtual void send(boost::shared_ptr<OPacket> oPack);

	virtual void send(boost::shared_ptr<std::vector<unsigned char>> sendData);

	virtual void read();

	void setErrorMode(int mode)
	{
		this->errorMode = mode;
	}

 boost::shared_ptr<boost::asio::ip::tcp::socket> getSocket()
	{
		return socket;
	}

	void setCID(IDType cID)
	{
		this->cID = cID;
	}

	void kill()
	{
		this->alive = false;
	}

	void asyncReceiveHandler(const boost::system::error_code& error, unsigned int bytes);

	void asyncSendHandler(const boost::system::error_code& error, boost::shared_ptr<std::vector<unsigned char>> sendData);

	virtual void close();

	virtual ~TCPConnection();

protected:
	std::queue <boost::shared_ptr<std::vector <unsigned char>>> queueSendData;
	std::vector <unsigned char>* receiveStorage;
	boost::shared_ptr<boost::asio::ip::tcp::socket> socket;
	boost::mutex sendingMutex;
	boost::mutex queueSendDataMutex;
	bool sending;
	Server* server;
	HeaderManager* hm;
	IDType cID;
	int errorMode;
	bool alive;
};
