#include "FileHandler.h"

/// <summary>
/// Reads a specified line from file
/// </summary>
/// <param name="filePath"></param>
/// <param name="lineNumber"></param>
/// <returns></returns>
bool FileHandler::readLine(std::string filePath, size_t lineNumber, std::string& destination) {

	bool isSuccessful = false;

	std::ifstream file;
	size_t lineCount = 1;

	try {

		// opening the file
		if (!file.is_open())
			file.open(filePath);

		// looping over lines in the file
		while (!file.eof())
		{
			// reading from the file the current line to destination string
			// the line is overridding the previous line
			std::getline(file, destination);

			// checking if desired line is reached.
			// if no - continue reading line by line
			// if yes - exiting the loop and keeping the current line
			if (lineCount == lineNumber)
				break;

			// counting each line
			lineCount++;
		}

		// if desired line is less than total number of lines of the file:
		if (lineCount < lineNumber) {
			std::cout << "Line not found." << std::endl;
			std::cout << "File Contains " << lineCount << " lines" << std::endl;
			isSuccessful = false;
		}

		else
			isSuccessful = true;
	}

	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		isSuccessful = false;;
	}

	// closing the file if open
	if (file.is_open())
		file.close();

	return isSuccessful;
}

/// <summary>
/// Reads all file to a buffer
/// </summary>
/// <param name="filePath"></param>
/// <param name="destination"></param>
/// <returns></returns>
bool FileHandler::readFile(std::string filePath, char** destination) {

	bool isSuccesful = false;

	std::ifstream file;

	// preparing buffer of the file content
	*destination = { 0 };

	try {

		if (!std::filesystem::exists(filePath)) {
			return false;
		}

		// finding the size of the file
		uint32_t fileSize = (uint32_t)std::filesystem::file_size(filePath);

		// creating the new stream buffer with the relevant size
		*destination = new char[fileSize];

		// opening the file according to its path
		file.open(filePath, std::fstream::binary);

		// reading the file into the destination buffer, according to the size of the file
		file.read(*destination, fileSize);

		isSuccesful = true;
	}

	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;

		if (destination != nullptr)
			delete[] destination;

		isSuccesful = false;;
	}

	// closing the stream file if open
	if (file.is_open())
		file.close();

	return isSuccesful;
}

/// <summary>
/// Writes lines 
/// </summary>
/// <param name="filePath"></param>
/// <param name="source"></param>
/// <returns></returns>
bool FileHandler::writeLines(std::string filePath, std::string source) {

	bool isSuccessful = false;

	std::ofstream file;

	try {
		if (!file.is_open())

			// opening the file and starting writing from the end of file 
			// (appending and not deleting previous content)
			file.open(filePath, std::ios_base::app);

		// writing the source to the end of the file
		file << source << std::endl;

		isSuccessful = true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		isSuccessful = false;
	}

	// closing the stream file if open
	if (file.is_open())
		file.close();

	return isSuccessful;
}

/// <summary>
/// Write line to file from a specified line number
/// </summary>
/// <param name="filePath"></param>
/// <param name="lineNumber"></param>
/// <param name="source"></param>
/// <returns></returns>
bool FileHandler::writeLines(std::string filePath, size_t lineNumber, std::string source) {

	bool isSuccessful = false;

	std::ifstream readFile;
	std::ofstream writeFile;

	try {

		// string to hold each line for insert into vector
		std::string line;
		
		// vector to hold all lines
		std::vector<std::string> lines;

		// opening file for reading
		readFile.open(filePath);

		// reading lines and pushing into vector
		while (std::getline(readFile, line))
		{
			lines.push_back(line);
		}

		if (lines.size() < lineNumber)
			// if line doesn't exist - appending the source tothe end of the vector
			lines.push_back(source);
		else
			// if line already exists - replacing the line with the source
			lines[lineNumber - 1] = source;

		// opening the file for writing, use ofstream::trunc to erase file and replace with vector
		writeFile.open(filePath, std::ofstream::trunc);

		// iterating vector and writing to the file the lines with the new line
		for (int i = 0; i < lines.size(); i++) {
			writeFile << lines[i] << std::endl;
		}

		isSuccessful = true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		isSuccessful = false;
	}

	// closing the stream file if open
	if (readFile.is_open())
		readFile.close();

	// closing the stream file if open
	if (writeFile.is_open())
		writeFile.close();

	return isSuccessful;
}