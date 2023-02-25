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
#include "SocketHandler.h"

using boost::asio::ip::tcp;

const size_t CLIENT_NAME = 2;
const size_t CLIENT_ID = 2;
const size_t PRIVATE_KEY = 3;
const std::string CLIENT_DETAILS_PATH = "me.info";
const uint8_t CLIENT_NAME_LENGTH = 255;
const uint8_t CLIENT_VERSION = 3;
const int LINE_OF_PRIVATE_KEY = 3;
const int LINE_OF_FILE_PATH_TO_SEND = 3;
const int NUMBER_OF_FILE_SENDING = 4;

class Client {

private:
	//members
	std::string clientName;
	std::string clientIdHex;
	std::array<unsigned char, UUID_LENGTH> clientIdBytes;
	std::string privateKey;
	std::string aesKey;
	uint32_t cksumOfLastFile;
	unsigned short numberOfTrialsTOSendFile;
	std::string filePath;
	bool connectedToServer;
	FileHandler fileHandler;
	SocketHandler sockHandler;

	/// <summary>
	/// Loads host and port from file
	/// </summary>
	void loadClientName();

	/// <summary>
	/// Loads registration details from file
	/// </summary>
	bool loadRegistrationDetails();

	/// <summary>
	/// Verifies client id hexadecimal
	/// </summary>
	/// <param name="hex"></param>
	/// <returns></returns>
	bool verifyClientIdHex(std::string hex);

	/// <summary>
	/// Converts hexadecimal string to array of bytes
	/// </summary>
	/// <param name="hex"></param>
	/// <returns></returns>
	std::array<unsigned char, UUID_LENGTH> hexToBytesArray(const std::string& hex);

	/// <summary>
	/// Saves private key
	/// </summary>
	/// <param name="privateKey"></param>
	void savePrivateKey(std::string privateKey);

	/// <summary>
	/// Loads path of the file to send to server
	/// </summary>
	bool loadFilePath();

	/// <summary>
	/// Loads content of file to send to server
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="item"></param>
	bool loadFileContent(std::string filePath, FileItem& item);

	/// <summary>
	/// Encrypts content
	/// </summary>
	/// <param name="content"></param>
	/// <param name="size"></param>
	/// <param name="encryptedContent"></param>
	/// <returns></returns>
	bool encryptContent(char* content, size_t contentSize, std::string& encryptedContent);

	/// <summary>
	/// Calculates cksum
	/// </summary>
	/// <param name="content"></param>
	/// <param name="size"></param>
	/// <param name="result"></param>
	bool calculateCksum(unsigned char* content, uint32_t size, uint32_t& result);

	/// <summary>
	/// Handles file response
	/// </summary>
	/// <param name="response"></param>
	void handleFileResponse(Response& response);

	/// <summary>
	/// Saves registration details
	/// </summary>
	/// <param name="response">reponse from server</param>
	void saveRegistrationDetails(Response& response);

	/// <summary>
	/// Re-sends file to server
	/// </summary>
	void reSendFileToServer();

	/// <summary>
	/// Handles AES key received from server
	/// </summary>
	/// <param name="response">reponse from server</param>
	void handleAESKeyFromServer(Response& response);

	/// <summary>
	/// Handles response from server - sends to the relevant function according to the response
	/// </summary>
	/// <param name="response">reponse from server</param>
	void handleResponse(Response& response);

	/// <summary>
	/// Sends request to server
	/// </summary>
	/// <param name="request"></param>
	bool sendRequestToServer(Request& request);

	/// <summary>
	/// Sends file request to server
	/// </summary>
	/// <param name="request"></param>
	/// <param name="fileItem"></param>
	bool sendRequestToServer(Request& request, FileItem& fileItem);

	/// <summary>
	/// Receives response from server
	/// </summary>
	bool receiveResponseFromServer();

	/// <summary>
	/// Clears buffer
	/// </summary>
	/// <param name="message"></param>
	/// <param name="length"></param>
	void clearBuffer(char message[], int length);

	/// <summary>
	/// Calculates cksum
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	uint32_t calculateCksum(std::string filePath);

public:
	/// <summary>
	/// Ctor
	/// </summary>
	Client();

	/// <summary>
	/// Dtor
	/// </summary>
	~Client();

	/// <summary>
	/// Registers to server
	/// </summary>
	void registerToServer();


	/// <summary>
	/// Generates RSA Key Pair
	/// </summary>
	void generateRSAKeyPair();

	/// <summary>
	/// Sends encrypted file to server
	/// </summary>
	void sendFileToServer();

	/// <summary>
	/// Disconnect from server
	/// </summary>
	void disconnect();
};
