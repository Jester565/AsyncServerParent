/*
Handles sending and receiving OPackets and IPacket or raw data with a single client
*/

#pragma once
#include "Macros.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/thread/mutex.hpp>
#include <vector>
#include <queue>

//Forward declaration
class Server;
class OPacket;
class HeaderManager;

class TCPConnection : public boost::enable_shared_from_this<TCPConnection>
{
public:
	//Initialize TCPConnection using the already existing tcpConnection
	TCPConnection(Server* server, boost::shared_ptr<boost::asio::ip::tcp::socket> boundSocket);

	//Starts the receiving of packets
	virtual void start();

	//Send OPacket over the TCPConnection
	virtual void send(boost::shared_ptr<OPacket> oPack);

	//Send raw data over the TCPConnection
	virtual void send(boost::shared_ptr<std::vector<unsigned char>> sendData);

	//bind function to handle receiving (must be called after receive handler is called)
	virtual void read();

	//accessor for the socket
	boost::shared_ptr<boost::asio::ip::tcp::socket> getSocket()
	{
		return socket;
	}

	//The id of the client that owns this TCPConnection
	void setSender(ClientPtr sender)
	{
		this->sender = sender;
	}

	//Function that is called when data is received
	void asyncReceiveHandler(const boost::system::error_code& error, unsigned int bytes);

	//Function called when data is sent
	void asyncSendHandler(const boost::system::error_code& error, boost::shared_ptr<std::vector<unsigned char>> sendData);

	//Shutsdown the socket
	virtual void close();

	//Deletes headerManager and receive buffer
	virtual ~TCPConnection();

protected:
	//Stores data to be sent when asyncSendHandler is called
	std::queue <boost::shared_ptr<std::vector <unsigned char>>> queueSendData;
	//The buffer to store received packets
	std::vector <unsigned char>* receiveStorage;
	//The boost socket representing a connection to a single client
	boost::shared_ptr<boost::asio::ip::tcp::socket> socket;
	//Prevents the sending boolean from being modified by multiple threads
	boost::mutex sendingMutex;
	//Prevents multiple threads from accessing the queueSendData
	boost::mutex queueSendDataMutex;
	/*
	True if boost::write has been called but the asyncSendHandler has not been called
	If true, then data to be sent will be added to queueSendData so it can later be
	sent by asyncSendHandler
	*/
	bool sending;
	//Allows access to packetManager
	Server* server;
	//Used to serialize and deserialize the packets, allows you to have custom headers
	HeaderManager* hm;
	//The id of the client that owns this TCPConnection
	ClientPtr sender;
	//If false, receive will not be called again
	bool alive;
};
