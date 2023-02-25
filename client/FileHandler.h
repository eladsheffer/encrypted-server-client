#pragma once
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <fstream>
#include <filesystem>


class FileHandler {

public:
	
	/// <summary>
	/// Reads a specified line from file
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="lineNumber"></param>
	/// <returns></returns>
	bool readLine(std::string filePath, size_t lineNumber, std::string& destination);


	/// <summary>
	/// Reads all file to a buffer
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="destination"></param>
	/// <returns></returns>
	bool readFile(std::string filePath, char** destination);

	/// <summary>
	/// Writes lines 
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="source"></param>
	/// <returns></returns>
	bool writeLines(std::string filePath, std::string source);


	/// <summary>
	/// Write line to file from a specified line number
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="lineNumber"></param>
	/// <param name="source"></param>
	/// <returns></returns>
	bool writeLines(std::string filePath, size_t lineNumber, std::string source);
};