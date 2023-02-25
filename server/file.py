# file.py
# Author: Elad Sheffer

import struct

UUID_LENGTH = 16


class File:
    __client_id = None
    __filename = None
    __pathname = None

    def __init__(self, client_id=b'', filename="", pathname=""):
        self.__client_id = struct.unpack('=' + str(UUID_LENGTH) + 's', struct.pack('=' + str(UUID_LENGTH) + 's',
                                                                                   client_id))
        self.__filename = filename
        self.__pathname = pathname

    def get_client_id(self):
        return self.__client_id[0]

    def get_filename(self):
        return self.__filename

    def get_pathname(self):
        return self.__pathname

    def set_client_id(self, client_id):
        self.__client_id = struct.unpack('=' + str(UUID_LENGTH) + 's', struct.pack('=' + str(UUID_LENGTH) + 's',
                                                                                   client_id))

    def set_filename(self, filename):
        self.__filename = filename

    def set_pathname(self, pathname):
        self.__pathname = pathname
