# server.py
# Author: Elad Sheffer

import socket
from struct import *
import uuid
from Crypto.Random import get_random_bytes
from Crypto.Util.Padding import unpad

from database import *
from request import Request
from response import Response
from Crypto.Cipher import PKCS1_OAEP, AES
from Crypto.PublicKey import RSA
import os
import threading
from crc import crc32
from client import Client
from file import File

PORT_FILE = "port.info"
MAX_PORT = 65535
DEFAULT_PORT = 1234  # Default port used by the server
SERVER_VERSION = 3
CLIENT_NAME_LENGTH = 255
PUBLIC_KEY_SIZE = 160

UUID_LENGTH = 16
AES_KEY_SIZE = 16
FILENAME_LENGTH = 255

CLIENT_CODE_REGISTER = 1100
CLIENT_CODE_SEND_PUBLIC_KEY = 1101
CLIENT_CODE_SEND_FILE = 1103
CLIENT_CODE_CKSUM_OK = 1104
CLIENT_CODE_CKSUM_ERR = 1105
CLIENT_CODE_CKSUM_ERR_FINAL = 1106

SERVER_CODE_REGISTRATION_OK = 2100
SERVER_CODE_REGISTRATION_ERR = 2101
SERVER_CODE_SWITCHING_KEYS = 2102
SERVER_CODE_CKSUM_READY = 2103
SERVER_CODE_MESSAGE_RECEIVED = 2104

CLIENT_CLOSED_CONNECTION_1 = 10053
CLIENT_CLOSED_CONNECTION_2 = 10054
CLIENT_ID = 0
CLIENT_NAME = 1
FILE_NAME = 1
FILE_PATH = 2


class Server:
    # the fields of the class
    port = None
    database = None
    __client_map = None
    __file_map = None

    def __init__(self):
        self.load_port()
        self.database = Database()
        self.load_data_from_database()
        self.open_server()

    def load_port(self):
        """
        Loads and returns port from file or return default port if loading is not successful
        :return: port
        """
        port = None
        port_file = PORT_FILE
        try:

            if not os.path.exists(port_file):
                self.port = DEFAULT_PORT
                return

            # opening the file of the port and reading port
            with open(port_file, "r") as file:

                if not file.readable():
                    print("Unable to read file")

                else:
                    # reading first (and only) line of the file of the port and stripping from white spaces if any
                    line = file.readline()
                    port = line.strip()

                    # checking if port exists and that is a number in the right range
                    if port is not None and str(port).isnumeric() and int(port) in range(0, MAX_PORT + 1):
                        self.port = int(port)
                    else:
                        self.port = DEFAULT_PORT

        except Exception as e:
            print("Exception occurred: " + repr(e))
            # taking default port in case there's a problem with the file opening or reading
            port = DEFAULT_PORT

        finally:
            return port

    def load_data_from_database(self):
        """
        Loads data from data base
        :return:
        """
        try:
            # creating maps of client and of files
            self.__client_map = {}
            self.__file_map = {}

            # retrieving client list from database
            clients = self.database.get_client_list()

            # looping over client list and inserting each client to the client map
            # with client id as key and client as value
            for item in clients:
                client = Client(item[CLIENT_ID], item[CLIENT_NAME])
                self.__client_map[item[CLIENT_ID]] = client

            # retrieving file list from database
            files = self.database.get_file_list()

            # looping over file list and inserting each file to the file map with file path as key and file object as
            # value
            for item in files:
                file = File(item[CLIENT_ID], item[FILE_NAME], item[FILE_PATH])
                self.__file_map[item[FILE_PATH]] = file

                # inserting files of each specific client to file list at its client object
                self.__client_map[item[CLIENT_ID]].add_file(item[FILE_PATH], file)

        except Exception as e:
            print("Exception occurred: " + repr(e))

    def open_server(self):
        """
        opens the server and starts listening to clients
        :return:
        """
        try:
            # opening a socket in TCP with IP4 address
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as self.sock:
                self.sock.bind(('', self.port))

                # put the socket into listening mode
                self.sock.listen()

                print("Server is on.")
                print(f"Listening on port: {self.port}")
                print("Waiting for clients to connect...\n")

                # a forever loop until client wants to exit
                while True:
                    # establish connection with client
                    conn, addr = self.sock.accept()

                    print(f'New client connected from {addr[0]} : {addr[1]}\n')

                    # Start a new thread of client and return its identifier
                    threading.Thread(target=self.session, args=(conn, addr)).start()

        except Exception as e:
            print("Exception occurred: " + repr(e))

    def session(self, conn, addr):
        """
        opens a session with a client
        :param conn:
        :param addr:
        :return:
        """
        try:
            while True:

                # receiving request data (header) from client
                frmt = '<' + str(UUID_LENGTH) + 'sBHI'
                request_data = conn.recv(calcsize(frmt))

                if request_data:
                    # converting the request data stream to items of request object
                    client_id, client_version, code, payload_size = unpack(frmt, request_data)

                    # creating a request object with the relevant information that came from the client
                    request = Request(client_id, client_version, code, payload_size)

                    # receiving payload according to payload size
                    payload = conn.recv(payload_size)

                    # handling request according to the code inside the request
                    self.handle_request(conn, request, payload)

                    print("Waiting for clients to send request...")

                else:

                    # if 0 data received from client:
                    print("Client closed connection.")
                    print(f'closing connection from host: {addr[0]} port: {addr[1]}\n')
                    print("Waiting for clients to send request...")
                    conn.close()
                    break
        except Exception as e:

            # in case of exception - closing connection with the client
            if e.args[0] == CLIENT_CLOSED_CONNECTION_1 or e.args[0] == CLIENT_CLOSED_CONNECTION_2:
                print("Client closed connection.")
            else:
                print("Exception occurred: " + repr(e))
            print(f'closing connection from host: {addr[0]} port: {addr[1]}\n')
            print("Waiting for clients to send request...")
            conn.close()

    @staticmethod
    def handle_client_doesnt_exit(conn):
        """
        Handles case client doesn't exist
        :param conn:
        :return:
        """
        try:
            print("client doesn't exist in database\n")

            # sending error because client doesn't exist:
            response = Response(version=SERVER_VERSION, code=SERVER_CODE_REGISTRATION_ERR)

            # converting response object to response data stream and sending response to the client
            # with error code and client id
            response_data = pack('=BHI' + str(UUID_LENGTH) + 's', response.get_version(),
                                 response.get_code(),
                                 response.get_payload_size(), response.get_client_id())
            conn.send(response_data)

        except Exception as e:
            print("Exception occurred: " + repr(e))

    def handle_request(self, conn, request, payload):
        """
        Handles request from client
        :param conn:
        :param request:
        :param payload:
        :return:
        """
        code = request.get_code()
        try:
            if code != CLIENT_CODE_REGISTER and not self.database.client_exists_by_id(request.get_client_id()):
                self.handle_client_doesnt_exit(conn)
                return
        except Exception as e:
            print("Exception occurred: " + repr(e))

        if code == CLIENT_CODE_REGISTER:
            self.handle_client_registration(conn, payload)
        elif code == CLIENT_CODE_SEND_PUBLIC_KEY:
            self.send_aes_key_to_client(conn, request, payload)
        elif code == CLIENT_CODE_SEND_FILE:
            self.handle_file_request(conn, request)
        elif code == CLIENT_CODE_CKSUM_OK:
            self.handle_cksum_ok(conn, request)
        elif code == CLIENT_CODE_CKSUM_ERR:
            self.handle_cksum_err(conn, request)
        elif code == CLIENT_CODE_CKSUM_ERR_FINAL:
            self.handle_cksum_err_final(conn, request)

    def handle_cksum_err_final(self, conn, request):
        """
        Handles cksum mismatch between server and client - for the last time
        :param conn:
        :param request:
        :return:
        """
        try:
            # receiving filename data from client
            frmt = '<' + str(FILENAME_LENGTH) + 's'
            filename = conn.recv(calcsize(frmt))

            # converting the filename data to filename string and stripping from null terminated chars
            filename = str(filename.decode('UTF-8')).strip("\0").lower()

            client_name = self.database.get_client_name(request.get_client_id())
            if client_name:
                print(f"Received request from client \"{client_name}\" - cksum of file \"{filename}\" failed. This was "
                      f"the last time")
            else:
                print(f"Received request from client - cksum of file \"{filename}\" failed. This was "
                      f"the last time")

            print("Deleting this file\n")

            # deleting file from disk of server - according to its path
            file_path = f"files\\{request.get_client_id().hex()}\\{filename}"
            if os.path.exists(file_path):
                os.remove(file_path)

            # deleting file details from database
            self.database.delete_file_of_client(request.get_client_id(), filename)

            # deleting file item from file map
            if self.__client_map.get(request.get_client_id()) is not None \
                    and self.__client_map.get(request.get_client_id()).get_files().get(file_path) is not None:
                self.__client_map.get(request.get_client_id()).remove_file(file_path)

            if self.__file_map.get(file_path) is not None:
                self.__file_map.pop(file_path)

            # creating a response object with the relevant information to send to client
            # "switching keys" code and encrypted aes key
            response = Response(version=SERVER_VERSION, code=SERVER_CODE_MESSAGE_RECEIVED,
                                client_id=request.get_client_id())

            # converting response object to response data stream and sending response to the client
            # with "message received" code
            response_data = pack('=BHI' + str(UUID_LENGTH) + 's',
                                 response.get_version(), response.get_code(), response.get_payload_size(),
                                 response.get_client_id())
            conn.send(response_data)

        except Exception as e:
            print("Exception occurred: " + repr(e))

    def handle_cksum_err(self, conn, request):
        """
        Handles cksum mismatch between server and client
        :param request:
        :param conn:
        :return:
        """
        try:
            # receiving filename data from client
            frmt = '<' + str(FILENAME_LENGTH) + 's'
            filename = conn.recv(calcsize(frmt))

            # converting the filename data to filename string and stripping from null terminated chars
            filename = str(filename.decode('UTF-8')).strip("\0")
            client_name = self.database.get_client_name(request.get_client_id())
            if client_name:
                print(f"Received request from client \"{client_name}\" - cksum of file \"{filename}\" failed. client "
                      f"will send file again...\n")
            else:
                print(f"Received request from client - cksum of file \"{filename}\" failed. client "
                      f"will send file again...\n")

        except Exception as e:
            print("Exception occurred: " + repr(e))

    def handle_cksum_ok(self, conn, request):
        """
        Handles cksum match between server and client
        :param conn:
        :param request:
        :return:
        """
        try:
            client_name = self.database.get_client_name(request.get_client_id())
            if client_name:
                print(f"Received request from client \"{client_name}\" - cksum succeeded")
            else:
                print(f"Received request from client - cksum succeeded")

            # receiving filename data from client
            frmt = '<' + str(FILENAME_LENGTH) + 's'
            filename = conn.recv(calcsize(frmt))

            # converting the filename data to filename string and stripping from null terminated chars
            filename = str(filename.decode('UTF-8')).strip("\0").lower()

            # updating verification to "True" in file details on the database
            self.database.update_cksum_verification(request.get_client_id(), filename, True)

            # creating a response object with the relevant information to send to client
            # "switching keys" code and encrypted aes key
            response = Response(version=SERVER_VERSION, code=SERVER_CODE_MESSAGE_RECEIVED,
                                client_id=request.get_client_id())

            # converting response object to response data stream and sending response to the client
            # with "message received" code
            response_data = pack('=BHI' + str(UUID_LENGTH) + 's',
                                 response.get_version(), response.get_code(), response.get_payload_size(),
                                 response.get_client_id())
            conn.send(response_data)

            print("Sent response to client - confirm message reception\n")

        except Exception as e:
            print("Exception occurred: " + repr(e))

    def handle_client_registration(self, conn, payload):
        """
        Handles client registration request
        :param conn:
        :param payload:
        :return:
        """
        print("Received request from client - registration to server")
        try:
            # converting the payload data to client name string and stripping from null terminated chars
            client_name = str(payload.decode('UTF-8')).strip("\0")
            print(f"Client is asking to register with client name \"{client_name}\"")
            if not self.database.client_exists_by_name(client_name):
                # in case client name doesn't exist yet:
                # generating a random client id
                client_id = uuid.uuid4().bytes_le

                # creating a response with a successful registration code and client id
                response = Response(version=SERVER_VERSION, code=SERVER_CODE_REGISTRATION_OK, client_id=client_id)

                # creating new client and inserting it to client map with client_id as key and client object as value
                client = Client(client_id=client_id, client_name=client_name)
                self.__client_map[client_id] = client

                # inserting new client details to the database
                self.database.insert_new_client(client_name, response)

                print("Sending response to client: Successful Registration\n")
            else:
                # in case client name already exists:
                response = Response(version=SERVER_VERSION, code=SERVER_CODE_REGISTRATION_ERR)

                print("Sending response to client: Registration Failed\n")

            # converting response object to response data stream and sending response to the client
            # with a successful registration code and client id
            response_data = pack('=BHI' + str(UUID_LENGTH) + 's', response.get_version(), response.get_code(),
                                 response.get_payload_size(), response.get_client_id())
            conn.send(response_data)

        except Exception as e:
            print("Exception occurred: " + repr(e))

    def send_aes_key_to_client(self, conn, request, payload):
        """
        Sends public key to client
        :param conn:
        :param request:
        :param payload:
        :return:
        """
        try:
            client_name = self.database.get_client_name(request.get_client_id())
            if client_name:
                print(f"Received request from client \"{client_name}\" - client is asking for AES key")
            else:
                print(f"Received request from client - client is asking for AES key")
            client_id = request.get_client_id()

            # receiving client name and public key from client
            frmt = '<' + str(CLIENT_NAME_LENGTH) + 's' + str(PUBLIC_KEY_SIZE) + 's'
            client_name, public_key = unpack(frmt, payload)

            # converting the client name data to client name string and stripping from null terminated chars
            client_name = client_name.decode('UTF-8').strip("\0")

            # generating a random AES key
            aes_key = get_random_bytes(AES_KEY_SIZE)

            # inserting keys (public key and AES key) to database
            self.database.insert_keys_of_client(client_name, public_key, aes_key)

            # importing RSA key from public key of client
            rsa_public_key = RSA.importKey(public_key)
            cipher = PKCS1_OAEP.new(rsa_public_key)

            # encrypting aes key with RSA public key
            cipher_aes_key = cipher.encrypt(aes_key)

            # getting the length of encrypted aes key
            cipher_aes_key_length = len(cipher_aes_key)

            # creating a response object with the relevant information to send to client
            # "switching keys" code and encrypted aes key
            response = Response(version=SERVER_VERSION, code=SERVER_CODE_SWITCHING_KEYS, client_id=client_id,
                                payload=cipher_aes_key)

            # converting response object to response data stream and sending response to the client
            # with "switching keys" code and encrypted aes key
            response_data = pack('=BHI' + str(UUID_LENGTH) + 's' + str(cipher_aes_key_length) + 's',
                                 response.get_version(), response.get_code(), response.get_payload_size(),
                                 response.get_client_id(), response.get_payload())
            conn.send(response_data)

            print("Sent response to client - sent key\n")

        except Exception as e:
            print("Exception occurred: " + repr(e))

    def handle_file_request(self, conn, request):
        """
        Handle file request from client
        :param conn:
        :param request:
        :return:
        """
        try:
            # receiving request data from client
            frmt = '=' + str(UUID_LENGTH) + 'sI' + str(FILENAME_LENGTH) + 's'
            file_data = conn.recv(calcsize(frmt))

            if file_data:
                # converting the request data stream to items of request object
                client_id, content_size, filename = unpack(frmt, file_data)
                filename = str(filename.decode('UTF-8')).strip("\0").lower()

                encrypted_content_file = conn.recv(content_size)

                # continue processing the file
                self.process_file_content(conn, request, filename, encrypted_content_file)

        except Exception as e:
            print("Exception occurred: " + repr(e))

    def process_file_content(self, conn, request, filename, encrypted_content_file):
        """
        Processes file content - saves it on server disk and checks cksum
        :param conn:
        :param request:
        :param filename:
        :param encrypted_content_file:
        :return:
        """
        try:
            client_name = self.database.get_client_name(request.get_client_id())
            if client_name:
                print(f'Received request from client \"{client_name}\" - file: "{filename}" to store in server')
            else:
                print(f'Received request from client - file: "{filename}" to store in server')

            # getting AES key from database
            aes_key = self.database.get_aes_key_of_client(request.get_client_id())

            # setting initialization vector to zeros of block size
            iv = ("\x00" * AES.block_size).encode("utf8")

            # getting AES key object
            cipher = AES.new(aes_key, AES.MODE_CBC, iv=iv)

            # decrypting file content with AES key object
            decrypted_content_file = unpad(cipher.decrypt(encrypted_content_file), AES.block_size)

            # creating "files" directory if not exist
            dir_name = f"files"
            if not os.path.exists(dir_name):
                os.mkdir(dir_name)
            # creating client id directory inside "files" directory, if not exist
            dir_name = dir_name + "\\" + request.get_client_id().hex()
            if not os.path.exists(dir_name):
                os.mkdir(dir_name)

            # appending file path of the file
            file_path = f"files\\{request.get_client_id().hex()}\\{filename}"

            # opening file to write the file content sent by the client
            with open(file_path, "wb") as file:
                if not file.writable():
                    print("Unable to write file")
                else:

                    # writing file content of the client to output file in server disk
                    file.write(decrypted_content_file)

            # creating new file object and inserting it to file map with file path as key and file object as value
            file = File(client_id=request.get_client_id(), filename=filename, pathname=file_path)
            self.__file_map[file_path] = file
            self.__client_map[request.get_client_id()].add_file(file_path, file)

            # calculating cksum of the decrypted content file
            print(f"Calculating cksum of file \"{filename}\". It may take a while. Please Wait...")
            cksum = self.cksum_calc(file_path)

            if not self.database.file_exists(request.get_client_id(), filename):
                # inserting file details to files table in database
                self.database.insert_file_details(request.get_client_id(), filename, file_path, False)
            else:
                # just updating verified cksum to "False"
                self.database.update_cksum_verification(request.get_client_id(), filename, False)

            # creating a response object with the relevant information to send to client
            # "cksum ready" code
            response = Response(SERVER_VERSION, SERVER_CODE_CKSUM_READY, request.get_client_id())

            # padding filename with null terminated chars to get to client name length
            filename = filename + (CLIENT_NAME_LENGTH - len(filename)) * "\x00"

            # converting filename from string to bytes
            filename_data = filename.encode("utf8")

            # converting response object to response data stream and sending response to the client
            response_data = pack('=BHI' + str(UUID_LENGTH) + 's' + 'I',
                                 response.get_version(), response.get_code(), response.get_payload_size(),
                                 response.get_client_id(), len(decrypted_content_file))
            conn.send(response_data)

            # packing filename and cksum and sending them to the client
            response_data = pack('<' + str(FILENAME_LENGTH) + 's' + 'I', filename_data, cksum)
            conn.send(response_data)

            print(f"Sent file confirmation with cksum ({cksum}) to client\n")

        except Exception as e:
            print("Exception occurred: " + repr(e))

    @staticmethod
    def cksum_calc(file_path):
        """
        Calculates cksum of file
        :param file_path:
        :return:
        """
        with open(file_path, "rb") as fd:
            digest = crc32()
            while buf := fd.read(4096):
                digest.update(buf)
            return digest.digest()


if __name__ == "__main__":
    server = Server()
