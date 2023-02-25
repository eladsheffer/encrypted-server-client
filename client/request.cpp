/*
Maman 15
Author: Elad Sheffer
*/

#include "request.h"

/// <summary>
/// Ctor
/// </summary>
Request::Request() {
	this->clientId = { 0 };
	this->version = 0;
	this->code = 0;
	this->payloadSize = 0;
	this->payload = "";
}

/// <summary>
/// Ctor
/// </summary>
/// <param name="version"></param>
/// <param name="code"></param>
/// <param name="payload"></param>
Request::Request(uint8_t version, uint16_t code, std::string payload) {
	this->clientId = { 0 };
	this->version = version;
	this->code = code;
	this->payload = payload;
	this->payloadSize = (uint32_t)this->payload.size();
}


/// <summary>
/// Ctor
/// </summary>
/// <param name="clientId"></param>
/// <param name="version"></param>
/// <param name="code"></param>
/// <param name="payloadSize"></param>
Request::Request(std::array<unsigned char, UUID_LENGTH> clientId, uint8_t version, uint16_t code, uint32_t payloadSize) {
	this->clientId = clientId;
	this->version = version;
	this->code = code;
	this->payloadSize = payloadSize;
	this->payload = "";
}



/// <summary>
/// 
/// </summary>
/// <param name="clientId"></param>
/// <param name="version"></param>
/// <param name="code"></param>
Request::Request(std::array<unsigned char, UUID_LENGTH> clientId, uint8_t version, uint16_t code) {
	this->clientId = clientId;
	this->version = version;
	this->code = code;
	this->payload = "";
	this->payloadSize = 0;
}

/// <summary>
/// Ctor
/// </summary>
/// <param name="clientId"></param>
/// <param name="version"></param>
/// <param name="code"></param>
/// <param name="payload"></param>
Request::Request(std::array<unsigned char, UUID_LENGTH> clientId, uint8_t version, uint16_t code, std::string payload) {
	this->clientId = clientId;
	this->version = version;
	this->code = code;
	this->payload = payload;
	this->payloadSize = (uint32_t)payload.size();
}

/// <summary>
/// Ctor
/// </summary>
/// <param name="clientId"></param>
/// <param name="version"></param>
/// <param name="code"></param>
/// <param name="filename"></param>
Request::Request(std::array<unsigned char, UUID_LENGTH> clientId, uint8_t version, std::string filename) {
	this->clientId = clientId;
	this->version = version;
	this->filename = filename;
	this->code = 0;
	this->payloadSize = 0;
	this->payload = "";
}

/// <summary>
/// Dtor
/// </summary>
Request::~Request() {
	this->clientId = { 0 };
	this->version = 0;
	this->code = 0;
	this->filename = "";
	this->payloadSize = 0;
	this->payload = "";
}

/// <summary>
/// Returns user id
/// </summary>
/// <returns>user id</returns>
std::array<unsigned char, UUID_LENGTH> Request::getClientId() {
	return this->clientId;
}

/// <summary>
/// Returns version
/// </summary>
/// <returns>version</returns>
uint8_t Request::getVersion() {
	return this->version;
}


/// <summary>
/// Returns code
/// </summary>
/// <returns>code</returns>
uint16_t Request::getCode() {
	return this->code;
}

/// <summary>
/// Returns filename
/// </summary>
/// <returns></returns>
std::string Request::getFilename(){
	return this->filename;
}

/// <summary>
/// Returns size of payload
/// </summary>
/// <returns>size of payload</returns>
uint32_t Request::getPayloadSize() {
	return this->payloadSize;
}


/// <summary>
/// Returns payload
/// </summary>
/// <returns>payload</returns>
std::string Request::getPayload() {
	return this->payload;
}

/// <summary>
/// Sets filename
/// </summary>
/// <param name="filename"></param>
void Request::setFilename(std::string filename) {
	this->filename = filename;
}

/// <summary>
/// Sets payload
/// </summary>
/// <param name="payload"></param>
void Request::setPayload(std::string payload) {
	this->payload = payload;
	this->payloadSize = (uint32_t)payload.size();
}

/// <summary>
/// Sets code
/// </summary>
/// <param name="code"></param>
void Request::setCode(uint16_t code) {
	this->code = code;
}

/// <summary>
/// Parses Request Header
/// </summary>
/// <param name="requestHeader"></param>
/// <returns></returns>
Request Request::parseHeader(RequestHeader requestHeader)
{
	// setting request header items
	std::array<unsigned char, UUID_LENGTH> clientId = requestHeader.requestData.clientId;
	uint8_t version = requestHeader.requestData.version;
	uint16_t code = requestHeader.requestData.code;
	uint32_t payloadSize = requestHeader.requestData.payloadSize;

	Request request(clientId, version, code, payloadSize);
	return request;
}