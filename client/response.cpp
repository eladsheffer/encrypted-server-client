/*
Maman 15
Author: Elad Sheffer
*/

#include "response.h"

/// <summary>
/// Ctor
/// </summary>
Response::Response() {
	this->version = 0;
	this->code = 0;
	this->payload = "";
	this->payloadSize = 0;
}

/// <summary>
/// Ctor
/// </summary>
/// <param name="version"></param>
Response::Response(uint8_t version) {
	this->version = version;
	this->code = 0;
	this->payloadSize = 0;
	this->payload = "";
}

Response::Response(uint8_t version, uint16_t code, std::array<unsigned char, UUID_LENGTH_RESPONSE> clientId, std::string payload) {
	this->version = version;
	this->code = code;
	this->payload = payload;
	this->payloadSize = (uint32_t)payload.size();
	this->clientId = clientId;
}

/// <summary>
/// Dtor
/// </summary>
Response::~Response() {
	this->version = 0;
	this->code = 0;
	this->clientId = { 0 };
	this->payload = "";
	this->payloadSize = 0;
}

/// <summary>
/// Returns version
/// </summary>
/// <returns>version</returns>
uint8_t Response::getVersion() {
	return this->version;
}

/// <summary>
/// Returns code
/// </summary>
/// <returns>code</returns>
uint16_t Response::getCode() {
	return this->code;
}

/// <summary>
/// Returns size of payload
/// </summary>
/// <returns>size of payload</returns>
uint32_t Response::getPayloadSize() {
	return this->payloadSize;
}

/// <summary>
/// Returns user id
/// </summary>
/// <returns>user id</returns>
std::array<unsigned char, UUID_LENGTH_RESPONSE> Response::getClientId() {
	return this->clientId;
}

/// <summary>
/// Returns payload
/// </summary>
/// <returns>payload</returns>
std::string Response::getPayload() {
	return this->payload;
}

/// <summary>
/// Sets code
/// </summary>
/// <param name="code"></param>
void Response::setCode(uint16_t code) {
	this->code = code;
}

/// <summary>
/// Sets payload
/// </summary>
/// <param name="payload"></param>
void Response::setPayload(std::string payload) {
	this->payload = payload;
	this->payloadSize = (uint32_t)payload.size();

}

/// <summary>
/// Parses Request Header
/// </summary>
/// <param name="requestHeader"></param>
/// <returns></returns>
ResponseHeader Response::deparseHeader(Response response) {

	ResponseHeader responseHeader = { 0 };
	responseHeader.responseData.version = response.getVersion();
	responseHeader.responseData.code = response.getCode();
	responseHeader.responseData.payloadSize = response.getPayloadSize();

	return responseHeader;
}
