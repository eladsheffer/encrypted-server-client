#include "client.h"
#include "RSAWrapper.h"
#include "Base64Wrapper.h"

int main(int argc, char* argv[])
{
	Client client;
	client.registerToServer();
	client.generateRSAKeyPair();
	client.sendFileToServer();
}