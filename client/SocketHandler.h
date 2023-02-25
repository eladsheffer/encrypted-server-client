#pragma once
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <fstream>
#include "request.h"
#include "response.h"
#include <filesystem>
#include "RSAWrapper.h"
#include "Base64Wrapper.h"
#include "AESWrapper.h"
#include "fileitem.h"
#include "crc.h"
#include <boost/crc.hpp>
#include <algorithm>
#include "FileHandler.h"

using boost::asio::ip::tcp;

const std::string SERVER_FILE_PATH = "transfer.info";
const size_t IP_ADDRESS = 1;
const std::string DEFAULT_HOST = "127.0.0.1";
const std::string DEFAULT_PORT = "1234";
const int MAX_PORT_VALUE = 65535;

class SocketHandler {


private:

	// members
	boost::asio::io_context io_context;
	tcp::socket sock;
	bool connected;
	std::string host;
	std::string port;
	FileHandler fileHandler;

	// method
	bool load_host_port();
	bool isNumeric(std::string const& str);
	
public:

	SocketHandler();
	~SocketHandler();

	/// <summary>
	/// Connects to server
	/// </summary>
	bool connectToServer();

	/// <summary>
	/// Sends char* buffer stream on the socket
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="size"></param>
	/// <returns></returns>
	bool send(char* buffer, size_t size);

	/// <summary>
	/// Sends string buffer stream on the socket
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="size"></param>
	/// <returns></returns>
	bool send(std::string buffer, size_t size);

	/// <summary>
	/// Receives char* buffer stream on the socket
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="size"></param>
	/// <returns></returns>
	bool receive(char* buffer, size_t size);

	/// <summary>
	/// Receives string buffer stream on the socket
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="size"></param>
	/// <returns></returns>
	bool receive(std::string& buffer, size_t size);

	/// <summary>
	/// Receives unsigned char* buffer stream on the socket
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="size"></param>
	/// <returns></returns>
	bool receive(unsigned char* buffer, size_t size);

};