/*
Maman 15
Author: Elad Sheffer
*/

#include "fileitem.h"

/// <summary>
/// Ctor
/// </summary>
FileItem::FileItem() {
	this->clientId = { 0 };
	this->contentSize = 0;
	this->filename = "";
	this->messageContent = "";
}

/// <summary>
	/// Ctor
	/// </summary>
	/// <param name="clientId"></param>
	/// <param name="filename"></param>
	/// <param name="messageContent"></param>
FileItem::FileItem(std::array<unsigned char, UUID_LENGTH_FILEITEM> clientId, std::string filename, std::string messageContent) {
	this->clientId = clientId;
	this->filename = filename;
	this->filename.resize(FILENAME_LENGTH, '\0');
	this->messageContent = messageContent;
	this->contentSize = messageContent.size();
	this->cksum = 0;
}

/// <summary>
/// Ctor
/// </summary>
/// <param name="clientId"></param>
/// <param name="content_size"></param>
/// <param name="filename"></param>
/// <param name="cksum"></param>
FileItem::FileItem(std::array<unsigned char, UUID_LENGTH_FILEITEM> clientId, uint32_t contentSize, std::string filename, uint32_t cksum) {
	this->clientId = clientId;
	filename = filename.substr(filename.find_first_of('\0'));
	this->filename = filename;
	this->contentSize = contentSize;
	this->cksum = cksum;
}

/// <summary>
/// Dtor
/// </summary>
FileItem::~FileItem() {
	this->clientId = { 0 };
	this->contentSize = 0;
	this->filename = "";
	this->messageContent = "";
	this->cksum = 0;
}

/// <summary>
/// Returns user id
/// </summary>
/// <returns>user id</returns>
std::array<unsigned char, UUID_LENGTH_FILEITEM> FileItem::getClientId() {
	return this->clientId;
}

/// <summary>
/// Returns filename
/// </summary>
/// <returns></returns>
std::string FileItem::getFilename() {
	return this->filename;
}

/// <summary>
/// Returns size of content
/// </summary>
/// <returns>size of content</returns>
uint32_t FileItem::getContentSize() {
	return this->contentSize;
}

/// <summary>
/// Returns message content
/// </summary>
/// <returns>message content</returns>
std::string FileItem::getMessageContent() {
	return this->messageContent;
}

/// <summary>
/// Returns cksum
/// </summary>
/// <returns>cksum</returns>
uint32_t FileItem::getCksum() {
	return this->cksum;
}