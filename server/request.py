# request.py
# Author: Elad Sheffer

import struct

UUID_LENGTH = 16


class Request:
    __client_id = None
    __version = None
    __code = None
    __payload = None
    __payload_size = None
    __filename = None

    def __init__(self, client_id=b'', version=0, code=0, payload_size=0, payload=b'', filename=""):
        self.__client_id = struct.unpack('=' + str(UUID_LENGTH) + 's', struct.pack('=' + str(UUID_LENGTH) + 's',
                                                                                   client_id))
        self.__version = struct.unpack('=B', struct.pack('=B', version))
        self.__code = struct.unpack('=H', struct.pack('=H', code))
        self.__payload = payload
        self.__payload_size = struct.unpack('=I', struct.pack('=I', payload_size))
        self.__filename = filename

    def get_client_id(self):
        return self.__client_id[0]

    def get_version(self):
        return self.__version[0]

    def get_code(self):
        return self.__code[0]

    def get_payload_size(self):
        return self.__payload_size[0]

    def get_payload(self):
        return self.__payload

    def get_filename(self):
        return self.__filename

    def set_client_id(self, client_id):
        self.__client_id = struct.unpack('=' + str(UUID_LENGTH) + 's', struct.pack('=' + str(UUID_LENGTH) + 's',
                                                                                   client_id))

    def set_version(self, version):
        self.__version = struct.unpack('=B', struct.pack('=B', version))

    def set_code(self, code):
        self.__code = struct.unpack('=H', struct.pack('=H', code))

    def set_payload(self, payload):
        self.__payload = payload
        self.__payload_size = struct.unpack('=I', struct.pack('=I', self.__payload.__len__()))
