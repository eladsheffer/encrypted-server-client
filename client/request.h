/*
Maman 15
Author: Elad Sheffer
*/

#pragma once
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <array>

const uint16_t CLIENT_CODE_REGISTER = 1100;
const uint16_t CLIENT_CODE_SEND_PUBLIC_KEY = 1101;
const uint16_t CLIENT_CODE_SEND_FILE = 1103;
const uint16_t CLIENT_CODE_CKSUM_OK = 1104;
const uint16_t CLIENT_CODE_CKSUM_ERR = 1105;
const uint16_t CLIENT_CODE_CKSUM_ERR_FINAL = 1106;
const uint8_t UUID_LENGTH = 16;

#pragma pack(push, 1)
class RequestData {

public:
	// members
	std::array<unsigned char, UUID_LENGTH> clientId;
	uint8_t version;
	uint16_t code;
	uint32_t payloadSize;
};
#pragma pack(pop)

union RequestHeader
{
	RequestData requestData;
	char buffer[sizeof(RequestData)];
};

class Request {
private:

	// members
	std::array<unsigned char, UUID_LENGTH> clientId;
	uint8_t version;
	uint16_t code;
	std::string filename;
	std::string payload;
	uint32_t payloadSize;

public:
	/// <summary>
	/// Ctor
	/// </summary>
	Request();

	/// <summary>
	/// Ctor
	/// </summary>
	/// <param name="version"></param>
	/// <param name="code"></param>
	/// <param name="payload"></param>
	Request(uint8_t version, uint16_t code, std::string payload);

	/// <summary>
	/// Ctor
	/// </summary>
	/// <param name="clientId"></param>
	/// <param name="version"></param>
	/// <param name="code"></param>
	/// <param name="payloadSize"></param>
	Request(std::array<unsigned char, UUID_LENGTH> clientId, uint8_t version, uint16_t code, uint32_t payloadSize);

	/// <summary>
	/// Ctor
	/// </summary>
	/// <param name="clientId"></param>
	/// <param name="version"></param>
	/// <param name="code"></param>
	/// <param name="payload"></param>
	Request(std::array<unsigned char, UUID_LENGTH> clientId, uint8_t version, uint16_t code, std::string payload);

	/// <summary>
	/// Ctor
	/// </summary>
	/// <param name="clientId"></param>
	/// <param name="version"></param>
	/// <param name="code"></param>
	Request(std::array<unsigned char, UUID_LENGTH> clientId, uint8_t version, uint16_t code);

	/// <summary>
	/// Ctor
	/// </summary>
	/// <param name="clientId"></param>
	/// <param name="version"></param>
	/// <param name="filename"></param>
	Request(std::array<unsigned char, UUID_LENGTH> clientId, uint8_t version, std::string filename);

	/// <summary>
	/// Dtor
	/// </summary>
	~Request();

	/// <summary>
	/// Returns user id
	/// </summary>
	/// <returns>user id</returns>
	std::array<unsigned char, UUID_LENGTH> getClientId();

	/// <summary>
	/// Returns version
	/// </summary>
	/// <returns>version</returns>
	uint8_t getVersion();


	/// <summary>
	/// Returns code
	/// </summary>
	/// <returns>code</returns>
	uint16_t getCode();

	/// <summary>
	/// Returns filename
	/// </summary>
	/// <returns></returns>
	std::string getFilename();

	/// <summary>
	/// Returns size of payload
	/// </summary>
	/// <returns>size of payload</returns>
	uint32_t getPayloadSize();

	/// <summary>
	/// Returns payload
	/// </summary>
	/// <returns>payload</returns>
	std::string getPayload();

	/// <summary>
	/// Sets filename
	/// </summary>
	/// <param name="filename"></param>
	void setFilename(std::string filename);

	/// <summary>
	/// Sets payload
	/// </summary>
	/// <param name="payload"></param>
	void setPayload(std::string payload);

	/// <summary>
	/// Sets code
	/// </summary>
	/// <param name="code"></param>
	void setCode(uint16_t code);

	/// <summary>
	/// Parses Request Header
	/// </summary>
	/// <param name="requestHeader"></param>
	/// <returns></returns>
	static Request parseHeader(RequestHeader requestHeader);
};

