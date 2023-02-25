# client.py
# Author: Elad Sheffer

import struct

UUID_LENGTH = 16


class Client:
    __client_id = None
    __client_name = None
    __files = None

    def __init__(self, client_id=b'', client_name=""):
        self.__client_id = struct.unpack('=' + str(UUID_LENGTH) + 's', struct.pack('=' + str(UUID_LENGTH) + 's',
                                                                                   client_id))
        self.__client_name = client_name
        self.__files = {}

    def get_client_id(self):
        return self.__client_id[0]

    def get_client_name(self):
        return self.__client_name

    def get_files(self):
        return  self.__files

    def set_client_id(self, client_id):
        self.__client_id = struct.unpack('=' + str(UUID_LENGTH) + 's', struct.pack('=' + str(UUID_LENGTH) + 's',
                                                                                   client_id))

    def set_client_name(self, client_name):
        self.__client_name = client_name

    def add_file(self, file_path, file):
        self.__files[file_path] = file

    def remove_file(self, file_path):
        self.__files.pop(file_path)
