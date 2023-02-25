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

const uint8_t UUID_LENGTH_FILEITEM = 16;
const size_t FILENAME_LENGTH = 255;

#pragma pack(push, 1)
class FileData {

public:
	// members
	std::array<unsigned char, UUID_LENGTH_FILEITEM> clientId;
	uint32_t contentSize;
};
#pragma pack(pop)

union FileHeader
{
	FileData fileData;
	char buffer[sizeof(FileData)];
};

union ContentSizeHeader 
{
	uint32_t contentSize;
	char buffer[sizeof(uint32_t)];
};

union CksumHeader
{
	uint32_t cksum;
	char buffer[sizeof(uint32_t)];
};

class FileItem {
private:

	// members
	std::array<unsigned char, UUID_LENGTH_FILEITEM> clientId;
	uint32_t contentSize;
	std::string filename;
	std::string messageContent;
	uint32_t cksum;

public:
	/// <summary>
	/// Ctor
	/// </summary>
	FileItem();

	/// <summary>
	/// Ctor
	/// </summary>
	/// <param name="clientId"></param>
	/// <param name="filename"></param>
	/// <param name="messageContent"></param>
	FileItem(std::array<unsigned char, UUID_LENGTH_FILEITEM> clientId, std::string filename, std::string messageContent);

	/// <summary>
	/// Ctor
	/// </summary>
	/// <param name="clientId"></param>
	/// <param name="content_size"></param>
	/// <param name="filename"></param>
	/// <param name="cksum"></param>
	FileItem(std::array<unsigned char, UUID_LENGTH_FILEITEM> clientId, uint32_t content_size ,std::string filename, uint32_t cksum);

	/// <summary>
	/// Dtor
	/// </summary>
	~FileItem();

	/// <summary>
	/// Returns user id
	/// </summary>
	/// <returns>user id</returns>
	std::array<unsigned char, UUID_LENGTH_FILEITEM> getClientId();

	/// <summary>
	/// Returns filename
	/// </summary>
	/// <returns></returns>
	std::string getFilename();

	/// <summary>
	/// Returns size of content
	/// </summary>
	/// <returns>size of content</returns>
	uint32_t getContentSize();

	/// <summary>
	/// Returns message content
	/// </summary>
	/// <returns> message content</returns>
	std::string getMessageContent();

	/// <summary>
	/// Returns cksum
	/// </summary>
	/// <returns>cksum</returns>
	uint32_t getCksum();
};