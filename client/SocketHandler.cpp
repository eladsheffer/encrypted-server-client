#include "SocketHandler.h"


SocketHandler::SocketHandler() : sock(io_context) {
	this->connected = false;
}

SocketHandler::~SocketHandler() {
	this->sock.close();
}


/// <summary>
/// Loads host and port from file
/// </summary>
bool SocketHandler::load_host_port() {

	bool isSuccessful = false;

	try {
		std::string ip;
		// reading line of ip address - host and port
		if (!fileHandler.readLine(SERVER_FILE_PATH, IP_ADDRESS, ip)) {
			std::cout << "Error in loading ip address." << std::endl;
		}

		// splitting the address to host part according to the symbol ':'
		// and checking validity
		std::string host = ip.substr(0, ip.find(":"));
		boost::system::error_code ec;
		boost::asio::ip::address::from_string(host, ec);
		if (ec)
			this->host = DEFAULT_HOST;
		else
			this->host = host;

		// splitting the address to port according to the symbol ':'
		// and checking validity
		std::string port = ip.substr(ip.find(":") + 1, ip.length() - 1);
		if (isNumeric(port) && stoi(port) >= 0 || stoi(port) <= MAX_PORT_VALUE)
			this->port = port;
		else
			this->port = DEFAULT_PORT;

		isSuccessful = true;
	}

	catch (std::exception& e)
	{
		// in case of and exception - determining default values of host and port 
		this->host = DEFAULT_HOST;
		this->port = DEFAULT_PORT;
		std::cerr << "Exception: " << e.what() << std::endl;
		isSuccessful = false;
	}

	return isSuccessful;
}

/// <summary>
/// Returns true if the string is numeric, false otherwise
/// </summary>
/// <param name="str"></param>
/// <returns>true if the string is numeric, false otherwise</returns>
bool SocketHandler::isNumeric(std::string const& str)
{
	auto it = str.begin();

	// looping over string till the end or till a char is not a digit
	while (it != str.end() && std::isdigit(*it)) {
		it++;
	}

	// if the loop is stopped in the middle - returning false
	// if the loop came to its end - all the chars are digits and thus returning true
	return !str.empty() && it == str.end();
}

/// <summary>
/// Connects to server
/// </summary>
bool SocketHandler::connectToServer() {
	try
	{

		if (!this->load_host_port()) {
			std::cout << "Error in loading host and port details" << std::endl << std::endl;
			return false;
		}

		tcp::resolver resolver(this->io_context);
		boost::asio::connect(this->sock, resolver.resolve(this->host, this->port));

		std::cout << "Connecting to Server" << std::endl << std::endl;
		this->connected = true;
		return true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		this->sock.close();
		return false;
	}
}


/// <summary>
/// Sends char* buffer stream on the socket
/// </summary>
/// <param name="buffer"></param>
/// <param name="size"></param>
/// <returns></returns>
bool SocketHandler::send(char* buffer, size_t size) {

	bool isSuccessful = false;

	try {
		this->sock.write_some(boost::asio::buffer(buffer, size));
		isSuccessful = true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		isSuccessful = false;
	}

	return isSuccessful;
}

/// <summary>
/// Sends string buffer stream on the socket
/// </summary>
/// <param name="buffer"></param>
/// <param name="size"></param>
/// <returns></returns>
bool SocketHandler::send(std::string buffer, size_t size) {

	bool isSuccessful = false;

	try {
		this->sock.write_some(boost::asio::buffer(buffer, size));
		isSuccessful = true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		isSuccessful = false;
	}

	return isSuccessful;
}


/// <summary>
/// Receives char* buffer stream on the socket
/// </summary>
/// <param name="buffer"></param>
/// <param name="size"></param>
/// <returns></returns>
bool SocketHandler::receive(char* buffer, size_t size) {

	bool isSuccessful = false;

	try {
		size_t reply_length = this->sock.read_some(boost::asio::buffer(buffer, size));
		if (reply_length == 0)
			isSuccessful = false;
		else
			isSuccessful = true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		isSuccessful = false;
	}

	return isSuccessful;
}

/// <summary>
/// Receives string buffer stream on the socket
/// </summary>
/// <param name="buffer"></param>
/// <param name="size"></param>
/// <returns></returns>
bool SocketHandler::receive(std::string& buffer, size_t size) {

	bool isSuccessful = false;

	try {
		size_t reply_length = this->sock.read_some(boost::asio::buffer(buffer, size));
		if (reply_length == 0)
			isSuccessful = false;
		else
			isSuccessful = true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		isSuccessful = false;
	}

	return isSuccessful;
}

/// <summary>
/// Receives unsigned char* buffer stream on the socket
/// </summary>
/// <param name="buffer"></param>
/// <param name="size"></param>
/// <returns></returns>
bool SocketHandler::receive(unsigned char* buffer, size_t size) {

	bool isSuccessful = false;

	try {
		size_t reply_length = this->sock.read_some(boost::asio::buffer(buffer, size));
		if (reply_length == 0)
			isSuccessful = false;
		else
			isSuccessful = true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		isSuccessful = false;
	}

	return isSuccessful;
}