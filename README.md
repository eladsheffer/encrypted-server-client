# encrypted-server-client
Client sends request for registration with client name, the server checks if the client name already exists on DB.
In case client exists on DB - server returns failure response.
In case client doesn't exist - server creates client id for client and registers client with client name and client id to DB.

Client creates RSA key pair saves the private key and sends the public key with request for AES key.
Server receives the request, creates AES key, saves public key (which he got from client)  and AES key on DB, encrypts AES key with the public key and sends the encrypted AES key to client.

Client receives encrypted AES key, and decrypts it.

Client encrypts a file he wants to send to server with AES key, and sends the encypted file to server.
Server receives encrypted file, decrypts it and saves it on disk of server and the details on DB.
Server calculates cksum of the file and sends to client.

Client calculates cksum of the file as well and compares to the cksum from server.
If the cksums are equal - client sends a success message, server marks the file as "verified" and sends response 2104 - confirms message reception, thank you.
if the cksums are not equal for the first 3 retrials - client sends 1105 message and then immidiately re-sends the file.
if the cksums are not equal for the 4th time - client sends 1106 message and stops sending the file. 
server sends response 2104 - confirms message reception, thank you.
