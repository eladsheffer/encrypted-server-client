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

const uint16_t SERVER_CODE_REGISTRATION_OK = 2100;
const uint16_t SERVER_CODE_REGISTRATION_ERR = 2101;
const uint16_t SERVER_CODE_SWITCHING_KEYS = 2102;
const uint16_t SERVER_CODE_CKSUM_READY = 2103;
const uint16_t SERVER_CODE_MESSAGE_RECEIVED = 2104;
const uint8_t UUID_LENGTH_RESPONSE = 16;

#pragma pack(push, 1)
class ResponseData {
public:
	// members
	uint8_t version;
	uint16_t code;
	uint32_t payloadSize;
};
#pragma pack(pop)

union ResponseHeader
{
	ResponseData responseData;
	char buffer[sizeof(ResponseData)];
};

class Response {
private:

	// members
	uint8_t version;
	uint16_t code;
	std::array<unsigned char, UUID_LENGTH_RESPONSE> clientId;
	uint32_t payloadSize;
	std::string payload;

public:

	/// <summary>
	/// Ctor
	/// </summary>
	Response();

	/// <summary>
	/// Ctor
	/// </summary>
	/// <param name="version"></param>
	Response(uint8_t version);


	/// <summary>
	/// Ctor
	/// </summary>
	/// <param name="version"></param>
	/// <param name="code"></param>
	/// <param name="payload"></param>
	Response(uint8_t version, uint16_t code, std::array<unsigned char, UUID_LENGTH_RESPONSE> clientId, std::string payload);


	/// <summary>
	/// Dtor
	/// </summary>
	~Response();

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
	/// Returns user id
	/// </summary>
	/// <returns>user id</returns>
	std::array<unsigned char, UUID_LENGTH_RESPONSE> getClientId();

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
	/// Sets code
	/// </summary>
	/// <param name="code"></param>
	void setCode(uint16_t code);


	/// <summary>
	/// Sets payload
	/// </summary>
	/// <param name="payload"></param>
	void setPayload(std::string payload);

	/// <summary>
	/// Parses Request Header
	/// </summary>
	/// <param name="requestHeader"></param>
	/// <returns></returns>
	static ResponseHeader deparseHeader(Response response);
};

