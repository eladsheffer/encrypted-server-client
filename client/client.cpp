#include "client.h"
#include "fileitem.h"
/// <summary>
/// Ctor
/// </summary>
Client::Client() {
	this->connectedToServer = false;
	this->cksumOfLastFile = 0;
	this->numberOfTrialsTOSendFile = 1;
	this->clientIdBytes = { 0 };

	if (!std::filesystem::exists(SERVER_FILE_PATH)) {
		std::cout << "Cannot contiune because server file is missing." << std::endl;
	}
	else {

		if (this->sockHandler.connectToServer())
			this->connectedToServer = true;

		this->loadClientName();

		// loads registraion details (if any) - client id and private key that are stored in details file
		this->loadRegistrationDetails();

		// fixed size of client name
		this->clientName.resize(CLIENT_NAME_LENGTH, '\0');

		// connecting to server with host and port that were loaded from file

	}
}

/// <summary>
/// Dtor
/// </summary>
Client::~Client() {
	this->clientName = "";
	this->clientIdHex = "";
	this->clientIdBytes = { 0 };
	this->privateKey = "";
	this->aesKey = "";
	this->connectedToServer = false;
}

/// <summary>
/// Loads host and port from file
/// </summary>
void Client::loadClientName() {
	try {
		// getting client name from file
		if (!fileHandler.readLine(SERVER_FILE_PATH, CLIENT_NAME, this->clientName)) {
			std::cout << "Error in loading client name" << std::endl;
		}

		if (this->clientName.length() > CLIENT_NAME_LENGTH)
			this->clientName.resize(CLIENT_NAME_LENGTH);
	}

	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

/// <summary>
/// Loads registration details from file
/// </summary>
/// <param name="path"></param>
bool Client::loadRegistrationDetails() {
	std::string path = CLIENT_DETAILS_PATH;

	if (!std::filesystem::exists(path)) {

		std::cout << "Client details file is missing." << std::endl;
		return false;
	}

	try {

		// getting the of client id and putting it as s string inside field "clientId"
		std::string clientIdHex;

		if (!fileHandler.readLine(CLIENT_DETAILS_PATH, CLIENT_ID, clientIdHex)) {
			std::cout << "Error in loading registration details file" << std::endl;
			return false;
		}

		if (!this->verifyClientIdHex(clientIdHex)) {
			std::cout << "Error in loading client id from client details file" << std::endl;
			return false;
		}

		this->clientIdHex = clientIdHex;

		this->clientIdBytes = this->hexToBytesArray(this->clientIdHex);

		// getting private key from file and putting it inside "privateKey" field
		if (!fileHandler.readLine(path, PRIVATE_KEY, this->privateKey)) {
			std::cout << "Error in loading private key." << std::endl;
			return false;
		}

		return true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return false;
	}
}

/// <summary>
/// Verifies client id hexadecimal
/// </summary>
/// <param name="hex"></param>
/// <returns></returns>
bool Client::verifyClientIdHex(std::string hex) {

	try {

		if (hex.length() != (size_t)2 * UUID_LENGTH)
			return false;

		std::transform(hex.begin(), hex.end(), hex.begin(), ::tolower);

		for (size_t i = 0; i < hex.length(); i++) {
			if ((hex[i] < '0' || hex[i] > '9') && (hex[i] < 'a' || hex[i] > 'f'))
				return false;
		}

		return true;
	}

	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return false;
	}
}

/// <summary>
/// Converts hexadecimal string to array of bytes
/// </summary>
/// <param name="hex"></param>
/// <returns></returns>
std::array<unsigned char, UUID_LENGTH> Client::hexToBytesArray(const std::string& hex) {
	std::array<unsigned char, UUID_LENGTH> bytes = { 0 };
	try {
		// looping over the string
		for (unsigned int i = 0; i < hex.length(); i += 2) {

			// splitting every 2 chars of the string
			std::string byteString = hex.substr(i, 2);

			// converting the 2 chars into a char representation of the hexadecimal base
			// number
			char byte = (char)strtol(byteString.c_str(), NULL, 16);

			// setting the char to the array of unsigned char
			bytes[i / 2] = byte;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return bytes;
}

/// <summary>
/// Registers to server
/// </summary>
void Client::registerToServer() {

	if (!this->connectedToServer) {
		std::cout << "No connection to server" << std::endl;
		return;
	}


	if (!std::filesystem::exists(SERVER_FILE_PATH)) {
		std::cout << "Cannot contiune because server file is missing." << std::endl;
		return;
	}

	try {

		// creating a request with the relevant details and "register to server" code
		Request request(CLIENT_VERSION, CLIENT_CODE_REGISTER, this->clientName);

		// sending the request to the server
		if (!this->sendRequestToServer(request))
			return;

		std::cout << "Sent request to server - registration to server" << std::endl;

		// receiving response to the request from server
		this->receiveResponseFromServer();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

/// <summary>
/// Saves private key
/// </summary>
/// <param name="privateKey"></param>
void Client::savePrivateKey(std::string privateKey) {
	std::string path = CLIENT_DETAILS_PATH;;

	// if the file of client details is missing or the private key is already saved if the file
	// the method exist without doing anything
	if (!std::filesystem::exists(path)) {
		return;
	}


	if (!fileHandler.writeLines(path, LINE_OF_PRIVATE_KEY, privateKey)) {
		std::cout << "Error in saving private key." << std::endl;
	}
}

/// <summary>
/// Generates RSA Key Pair
/// </summary>
void Client::generateRSAKeyPair() {

	if (!this->connectedToServer) {
		std::cout << "No connection to server" << std::endl;
		return;
	}

	if (this->clientIdHex == "") {
		std::cout << "Cannot perform the task of generating RSA key pair and asking for AES key. registraion is needed first." << std::endl;
		return;
	}


	try {
		// Creating an RSA decryptor. this is done here to generate a new private/public key pair
		RSAPrivateWrapper rsapriv;

		// Getting the private key and encode it as base64
		std::string base64PrivateKey = Base64Wrapper::encode(rsapriv.getPrivateKey());

		// save private key in client details file
		this->savePrivateKey(base64PrivateKey);

		// save private key in field "private key"
		this->privateKey = base64PrivateKey;

		// Getting the public key
		std::string publicKey = rsapriv.getPublicKey();

		// setting clien name at the payload
		std::string payload = this->clientName;

		// appending the public key to the payload 
		payload.append(publicKey);

		// creating a request of sending public key and getting AES key to the server with
		// the payload of client name and public key
		Request request(clientIdBytes, CLIENT_VERSION, CLIENT_CODE_SEND_PUBLIC_KEY, payload);

		// sending the request to server
		if (!this->sendRequestToServer(request))
			return;

		std::cout << "Sent request to server - sent public RSA key, asking for AES key" << std::endl;

		// receiving response to the request from server
		this->receiveResponseFromServer();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

/// <summary>
/// Sends encrypted file to server
/// </summary>
/// <param name="path"></param>
void Client::sendFileToServer() {

	if (!this->connectedToServer) {
		std::cout << "No connection to server" << std::endl;
		return;
	}

	if (this->clientIdHex == "") {
		std::cout << "Cannot perform the task of sending file. Registraion is needed first." << std::endl;
		return;
	}

	try {
		if (this->loadFilePath() == false) {
			std::cout << "Server File doesn't exist or couldn't be loaded properly." << std::endl;
			return;
		}

		// creating a request to send to sever with the relevant information
		Request request = Request(this->clientIdBytes, CLIENT_VERSION, CLIENT_CODE_SEND_FILE);

		// creating a file item to send to sever with the relevant information
		// file item gets its information from method "loadFileContent"
		FileItem fileItem;
		if (this->loadFileContent(this->filePath, fileItem) == false) {
			std::cout << "File in path: " << filePath << " doesn't exist" << std::endl;
			return;
		}

		std::cout << "Sending file request to server - file: \"" << fileItem.getFilename() << "\"" << std::endl;

		// sending request to server with request and file item objects
		if (!this->sendRequestToServer(request, fileItem))
			return;

		std::cout << "Waiting for response from server. Server needs to calculate cksum. It may take a while. Please wait..." << std::endl;
		this->receiveResponseFromServer();
	}

	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

}


/// <summary>
/// Loads path of the file to send to server
/// </summary>
bool Client::loadFilePath() {

	try {

		if (!std::filesystem::exists(SERVER_FILE_PATH)) {
			return false;
		}

		if (!this->fileHandler.readLine(SERVER_FILE_PATH, LINE_OF_FILE_PATH_TO_SEND, this->filePath)) {
			std::cout << "Error in loading file path." << std::endl;
			return false;
		}

		// converting filepath to lowercase
		std::transform(this->filePath.begin(), this->filePath.end(), this->filePath.begin(), ::tolower);

		return true;
	}

	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return false;
	}
}

/// <summary>
/// Loads content of file to send to server
/// </summary>
/// <param name="filePath"></param>
/// <param name="item"></param>
bool Client::loadFileContent(std::string filePath, FileItem& fileItem) {

	char* buffer = { 0 };
	try {

		if (!std::filesystem::exists(this->filePath)) {
			return false;
		}

		// extracting file name of file path
		std::string filename = this->filePath.substr(this->filePath.find_last_of('\\') + 1);

		// setting buffer
		//uint32_t actualBufferSize;
		uint32_t fileSize = (uint32_t)std::filesystem::file_size(this->filePath);

		// reading the content of the file and putting it in the buffer
		std::cout << "Reading file \"" << filename << "\" from disk..." << std::endl;
		if (!this->fileHandler.readFile(filePath, &buffer)) {
			std::cout << "Failed reading the file." << std::endl;
			return false;
		}

		// encrypting the content of the file and putting it inside a string
		std::cout << "Encrypting file \"" << filename << "\"..." << std::endl;
		std::string encryptedFileContent;
		if (!this->encryptContent(buffer, fileSize, encryptedFileContent))
		{
			std::cout << "Encryption failed." << std::endl;
			if (buffer != nullptr)
				delete[] buffer;
			return false;
		}

		// creating a file item with the relevant information to send to server
		fileItem = FileItem(this->clientIdBytes, filename, encryptedFileContent);

		// doing a cksum calculation and putting the number inside field "cksumOfLastFile"
		this->calculateCksum((unsigned char*)buffer, fileSize, this->cksumOfLastFile);

		if (buffer != nullptr)
			delete[] buffer;

		return true;
	}

	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;

		if (buffer != nullptr)
			delete[] buffer;

		return false;
	}
}

/// <summary>
/// Encrypts content
/// </summary>
/// <param name="content"></param>
/// <param name="size"></param>
/// <param name="encryptedContent"></param>
/// <returns></returns>
bool Client::encryptContent(char* content, size_t contentSize, std::string& encryptedContent) {

	bool isSuccessful = false;

	try {
		// getting AES key from field "aesKey" and converting it from string to
			// AESWrapper object
		AESWrapper aes((const unsigned char*)this->aesKey.c_str(), (unsigned int)this->aesKey.length());

		// encrypting the content of the file and putting it inside a string
		encryptedContent = aes.encrypt(content, contentSize);

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
/// Calculates cksum
/// </summary>
/// <param name="content"></param>
/// <param name="size"></param>
/// <param name="result"></param>
bool Client::calculateCksum(unsigned char* content, uint32_t size, uint32_t& result) {

	CRC crc;

	try {
		crc.update((unsigned char*)content, size);
		result = crc.digest();
		return true;
	}

	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return false;
	}
}

/// <summary>
/// Handles response from server - sends to the relevant function according to the response
/// </summary>
/// <param name="response">reponse from server</param>
void Client::handleResponse(Response& response) {

	// getting code from server
	uint16_t code = response.getCode();

	// switch on code of server response
	switch (code) {


	case SERVER_CODE_REGISTRATION_OK:
		std::cout << "Received response from server - successful registration" << std::endl << std::endl;
		this->saveRegistrationDetails(response);
		break;


	case SERVER_CODE_REGISTRATION_ERR:
		std::cout << "Received response from server - registration failed" << std::endl << std::endl;
		break;


	case SERVER_CODE_SWITCHING_KEYS:
		std::cout << "Received response from server - received public key" << std::endl << std::endl;
		this->handleAESKeyFromServer(response);
		break;


	case SERVER_CODE_CKSUM_READY:
		std::cout << "Received response from server - received cksum" << std::endl << std::endl;
		this->handleFileResponse(response);
		break;

	case SERVER_CODE_MESSAGE_RECEIVED:
		break;

	default:
		break;
	}
}

/// <summary>
/// Saves registration details
/// </summary>
/// <param name="response">reponse from server</param>
void Client::saveRegistrationDetails(Response& response) {

	std::string path = CLIENT_DETAILS_PATH;

	// if client details file already exists - the details already exist
	// and there is no need to save them again, just after registration in the server
	if (std::filesystem::exists(path)) {
		return;
	}

	std::ostringstream oss;

	this->clientIdBytes = response.getClientId();

	try {

		// writing client name on first line of the file
		if (!this->fileHandler.writeLines(path, this->clientName)) {
			std::cout << "Error in saving client name on client details file." << std::endl;
		}

		// looping over client id which is a vector of unsigned chars
		for (size_t i = 0; i < clientIdBytes.size(); ++i) {

			// if the unsigned char of 2 bytes is smaller than 10 in hexadecimal base
			// meaning is a single-figure number in hexadecimal base
			// so the algorythm pads it with '0' on the left side
			// in order to keep a 2-figure number
			// after that - converting every 2 bytes into an hexadecimal base number
			// and writing it both to an output stream string
			if ((uint16_t)clientIdBytes[i] < 0x10) {
				oss << '0' << std::hex << (uint16_t)clientIdBytes[i];
			}
			else {
				oss << std::hex << (uint16_t)clientIdBytes[i];
			}
		}

		// saving the output string stream as a string field "clientId"
		this->clientIdHex = oss.str();

		if (!this->fileHandler.writeLines(path, this->clientIdHex)) {
			std::cout << "Error in saving client id on client details file." << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

/// <summary>
/// Handles file response
/// </summary>
/// <param name="response"></param>
void Client::handleFileResponse(Response& response) {

	try {
		ContentSizeHeader contentSizeHeader = { 0 };
		std::string filename;
		CksumHeader cksumHeader = { 0 };

		// reading content size and storing it inside a variable
		this->sockHandler.receive(contentSizeHeader.buffer, sizeof(uint32_t));
		uint32_t contentSize = contentSizeHeader.contentSize;

		// reading filename with a determined length and storing it inside a string variable
		filename.resize(FILENAME_LENGTH, '\0');
		this->sockHandler.receive(filename, FILENAME_LENGTH);

		// reading cksum and storing it inside a variable
		this->sockHandler.receive(cksumHeader.buffer, sizeof(uint32_t));
		uint32_t cksumFromServer = cksumHeader.cksum;

		// printing the cksum of file from this pc vs. cksum of the file from server
		std::cout << "cksum from file in this pc:\t " << this->cksumOfLastFile << std::endl;
		std::cout << "cksum from file saved in server: " << cksumFromServer << std::endl;

		// creating a request with relevant information to send to the server
		// code of request will be determined after comparison of client and server cksums
		Request request(this->clientIdBytes, CLIENT_VERSION, filename);

		if (this->cksumOfLastFile == cksumFromServer) {

			// sending a request announcing that
			// the cksums of client (original) file and server file are equal
			std::cout << "Cksum succeded" << std::endl;
			request.setCode(CLIENT_CODE_CKSUM_OK);
			if (!this->sendRequestToServer(request))
				return;

			// receiving a response to the file request from the server
			this->receiveResponseFromServer();
		}
		else if (this->numberOfTrialsTOSendFile < NUMBER_OF_FILE_SENDING)
		{
			// cksums of client(original) file and server file are not equal
			std::cout << "Cksum failed " << this->numberOfTrialsTOSendFile << " time/s" << std::endl << std::endl;

			// the client will re-send the file to the server
			// number of trials is up by one
			this->numberOfTrialsTOSendFile++;
			std::cout << "SENDING FILE AGAIN: TRIAL NUMBER " << this->numberOfTrialsTOSendFile << std::endl;
			std::cout << "------------------------------------" << std::endl;

			// sending a request of that cksum failed and the client will re-send the file
			request.setCode(CLIENT_CODE_CKSUM_ERR);
			if (!this->sendRequestToServer(request))
				return;

			// re-sending file to server (with response handling)
			this->reSendFileToServer();
		}

		else {
			// cksums of client(original) file and server file are not equal
			// this was the last trial and client won't sending the file anymore
			std::cout << "Cksum failed " << this->numberOfTrialsTOSendFile << " times" << std::endl;
			std::cout << "This was the last time" << std::endl;

			// reseting number of trials and cksum of a file
			this->numberOfTrialsTOSendFile = 1;
			this->cksumOfLastFile = 0;

			// creating a request with information of cksum failue for the last time
			request.setCode(CLIENT_CODE_CKSUM_ERR_FINAL);

			// sending the request
			if (!this->sendRequestToServer(request))
				return;

			// receiving a response to the file request from the server
			this->receiveResponseFromServer();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

/// <summary>
/// Re-sends file to server
/// </summary>
void Client::reSendFileToServer() {

	try {
		// loading the content of the file
		FileItem fileItem;
		this->loadFileContent(this->filePath, fileItem);

		// creating a request of sending a file
		Request fileRequest(this->clientIdBytes, CLIENT_VERSION, CLIENT_CODE_SEND_FILE);

		// sending the request with the file content
		if (!this->sendRequestToServer(fileRequest, fileItem))
			return;

		std::cout << "Waiting for response from server. Server needs to calculate cksum. It may take a while. Please wait..." << std::endl;
		this->receiveResponseFromServer();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

/// <summary>
/// Handles AES key received from server
/// </summary>
/// <param name="response">reponse from server</param>
void Client::handleAESKeyFromServer(Response& response) {

	try {
		// this is the encrypted AES key as a string - needs to be encrypted
		std::string aesKeyCipher = response.getPayload();

		// creating an RSA decryptor using the existing private key of the client
		RSAPrivateWrapper rsapriv_other(Base64Wrapper::decode(this->privateKey));

		// decrypting the AES key with the private key decryptor
		std::string aesKey = rsapriv_other.decrypt(aesKeyCipher);

		// saving the AES key in the "aesKey" field
		this->aesKey = aesKey;
	}

	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

/// <summary>
/// Sends request to server
/// </summary>
/// <param name="request"></param>
bool Client::sendRequestToServer(Request& request) {

	bool isSuccessful = false;

	try {

		// preparing the request header items to send to server
		RequestHeader requestHeader = { 0 };
		requestHeader.requestData.clientId = request.getClientId();
		requestHeader.requestData.version = request.getVersion();
		requestHeader.requestData.code = request.getCode();
		requestHeader.requestData.payloadSize = request.getPayloadSize();

		// sending header items (meta-data) of request as a stream to server
		this->sockHandler.send(requestHeader.buffer, sizeof(RequestData));

		// if the payload is not empty - sending payload to server
		if (request.getPayloadSize() > 0)
			this->sockHandler.send(request.getPayload(), request.getPayloadSize());

		// if the request is a cksum request - sending also filename
		if (request.getCode() == CLIENT_CODE_CKSUM_OK || request.getCode() == CLIENT_CODE_CKSUM_ERR
			|| request.getCode() == CLIENT_CODE_CKSUM_ERR_FINAL) {
			this->sockHandler.send(request.getFilename(), FILENAME_LENGTH);
		}

		isSuccessful = true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return isSuccessful;
}

/// <summary>
/// Sends file item to server
/// </summary>
/// <param name="request"></param>
/// <param name="fileItem"></param>
bool Client::sendRequestToServer(Request& request, FileItem& fileItem) {

	bool isSuccessful = false;

	try {

		// preparing the request header items to send to server
		RequestHeader requestHeader = { 0 };
		requestHeader.requestData.clientId = request.getClientId();
		requestHeader.requestData.version = request.getVersion();
		requestHeader.requestData.code = request.getCode();
		requestHeader.requestData.payloadSize = request.getPayloadSize();

		// sending header items (meta-data) of request as a stream to server
		this->sockHandler.send(requestHeader.buffer, sizeof(RequestData));


		// preparing the file header items to send to server
		FileHeader fileHeader = { 0 };
		fileHeader.fileData.clientId = fileItem.getClientId();
		fileHeader.fileData.contentSize = fileItem.getContentSize();

		// sending header items (meta-data) of file item as a stream to server
		this->sockHandler.send(fileHeader.buffer, sizeof(FileData));

		// sending filename to server
		this->sockHandler.send(fileItem.getFilename().data(), FILENAME_LENGTH);

		// sending content of the file to server
		this->sockHandler.send(fileItem.getMessageContent(), fileItem.getContentSize());

		isSuccessful = true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return isSuccessful;
}

/// <summary>
/// Receives response from server
/// </summary>
/// <param name="response"></param>
bool Client::receiveResponseFromServer() {

	bool isSuccessful = false;

	try {
		// setting stream buffers
		ResponseHeader responseHeader = { 0 };

		boost::system::error_code error_code;

		// reading response header from server
		this->sockHandler.receive(responseHeader.buffer, sizeof(ResponseData));

		// storing the header items inside relevant variables
		uint8_t version = responseHeader.responseData.version;
		uint16_t code = responseHeader.responseData.code;
		uint32_t payloadSize = responseHeader.responseData.payloadSize;

		// reading client id from server
		std::array<unsigned char, UUID_LENGTH> clientId = { 0 };
		this->sockHandler.receive(clientId.data(), UUID_LENGTH);

		// setting payload buffer
		std::string payload;

		// if there's payload - receiving it from server
		if (payloadSize > 0) {
			payload.resize(payloadSize);
			this->sockHandler.receive(payload, payloadSize);
		}

		// creating a response object with information received from server
		Response response(version, code, clientId, payload);

		this->handleResponse(response);

		isSuccessful = true;
	}

	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return isSuccessful;
}

/// <summary>
/// Clears buffer
/// </summary>
/// <param name="message"></param>
/// <param name="length"></param>
void Client::clearBuffer(char message[], int length) {
	for (int i = 0; i < length; i++)
		message[i] = '\0';
}

/// <summary>
/// Calculates cksum
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
uint32_t Client::calculateCksum(std::string filePath) {
	uint32_t cksum = 0;
	std::ifstream fileToCheck;

	// getting the size of the file according to the file path
	uint32_t fileSize = (uint32_t)std::filesystem::file_size(filePath);

	// creating the new stream buffer with the relevant size
	char* buffer = new char[fileSize];

	try {
		fileToCheck.read(buffer, fileSize);

		// calculatin cksum and putting the number inside cksum
		CRC crc;
		crc.update((unsigned char*)buffer, fileSize);
		cksum = crc.digest();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return cksum;
}

/// <summary>
/// Disconnect from server
/// </summary>
void Client::disconnect() {

	// sending a request with code 0 to announce of disconnecting
	Request request;
	this->sendRequestToServer(request);
}