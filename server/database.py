# database.py
# Author: Elad Sheffer

import sqlite3
from datetime import datetime

DB_FILE = "server.db"


class Database:
    def __init__(self):
        self.create_tables()

    @staticmethod
    def create_tables():
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            conn.executescript("""
            CREATE TABLE IF NOT EXISTS clients(client_id CHAR(16) NOT NULL PRIMARY KEY, 
            client_name VARCHAR(127) NOT NULL, public_key CHAR(160), last_seen DATE, AES_key CHAR(32));
            CREATE TABLE IF NOT EXISTS files(client_id CHAR(16) NOT NULL, filename VARCHAR(255) NOT NULL, 
            pathname VARCHAR(255) NOT NULL PRIMARY KEY, verified NUMERIC(1) NOT NULL);
            """)

            conn.commit()

        except Exception as e:
            print("Exception occurred: " + repr(e))

        finally:
            conn.close()

    @staticmethod
    def client_exists_by_name(client_name):
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            c = conn.cursor()
            c.execute("SELECT * FROM clients where client_name = ?;", [client_name])
            conn.commit()
            results = c.fetchall()
            if not results:
                return False
            return True

        except Exception as e:
            print("Exception occurred: " + repr(e))

        finally:
            conn.close()

    @staticmethod
    def client_exists_by_id(client_id):
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            c = conn.cursor()
            c.execute("SELECT * FROM clients where client_id = ?;", [client_id])
            conn.commit()
            results = c.fetchall()
            if not results:
                return False
            return True

        except Exception as e:
            print("Exception occurred: " + repr(e))

        finally:
            conn.close()

    @staticmethod
    def get_client_name(client_id):
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            c = conn.cursor()
            c.execute("SELECT client_name FROM clients where client_id = ?;", [client_id])
            conn.commit()
            results = c.fetchall()
            if not results:
                return False
            client_name = results[0][0]
            return client_name

        except Exception as e:
            print("Exception occurred: " + repr(e))
            return False

        finally:
            conn.close()

    @staticmethod
    def get_client_list():
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            c = conn.cursor()
            c.execute("SELECT * FROM clients")
            conn.commit()
            results = c.fetchall()
            return results

        except Exception as e:
            print("Exception occurred: " + repr(e))

        finally:
            conn.close()

    @staticmethod
    def file_exists(client_id, filename):
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            c = conn.cursor()
            c.execute("SELECT * FROM files where client_id = ? AND filename = ?;", [client_id, filename])
            conn.commit()
            results = c.fetchall()
            if not results:
                return False
            return True

        except Exception as e:
            print("Exception occurred: " + repr(e))

        finally:
            conn.close()

    @staticmethod
    def get_file_list():
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            c = conn.cursor()
            c.execute("SELECT * FROM files")
            conn.commit()
            results = c.fetchall()
            return results

        except Exception as e:
            print("Exception occurred: " + repr(e))

        finally:
            conn.close()

    @staticmethod
    def insert_new_client(client_name, response):
        conn = None
        client_id = response.get_client_id()
        date_time = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        try:
            conn = sqlite3.connect(DB_FILE)
            conn.execute("INSERT INTO clients (client_id, client_name, last_seen) VALUES(?, ?, ?)", [client_id,
                                                                                                     client_name,
                                                                                                     date_time])
            conn.commit()
        except Exception as e:
            print("Exception occurred: " + repr(e))

        finally:
            conn.close()

    @staticmethod
    def insert_keys_of_client(client_name, public_key, aes_key):
        conn = None
        date_time = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        try:
            conn = sqlite3.connect(DB_FILE)
            conn.execute("UPDATE clients SET public_key = ?, AES_key = ?, last_seen = ? WHERE client_name = ?",
                         [public_key, aes_key, date_time, client_name])

            conn.commit()
        except Exception as e:
            print("Exception occurred: " + repr(e))

        finally:

            conn.close()

    @staticmethod
    def get_aes_key_of_client(client_id):
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            c = conn.cursor()
            c.execute("SELECT AES_key FROM clients where client_id = ?;", [client_id])
            conn.commit()
            results = c.fetchall()
            return results[0][0]

        except Exception as e:
            print("Exception occurred: " + repr(e))

        finally:
            conn.close()

    @staticmethod
    def update_last_seen(client_id):
        date_time = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            conn.execute("UPDATE clients SET last_seen = ? WHERE client_id = ?", [date_time, client_id])
            conn.commit()
        except Exception as e:
            print("Exception occurred: " + repr(e))

        finally:
            conn.close()

    @staticmethod
    def insert_file_details(client_id, filename, pathname, verified):
        date_time = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            conn.execute("INSERT INTO files (client_id, filename, pathname, verified) VALUES(?, ?, ?, ?)", [client_id,
                                                                                                            filename,
                                                                                                            pathname,
                                                                                                            verified])
            conn.execute("UPDATE clients SET last_seen = ? WHERE client_id = ?", [date_time, client_id])
            conn.commit()
        except Exception as e:
            print("Exception occurred: " + repr(e))

        finally:
            conn.close()

    @staticmethod
    def update_cksum_verification(client_id, filename, verified):
        date_time = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            conn.execute("UPDATE files SET verified = ? WHERE client_id = ? AND filename = ?", [verified, client_id,
                                                                                                filename])
            conn.execute("UPDATE clients SET last_seen = ? WHERE client_id = ?", [date_time, client_id])
            conn.commit()
        except Exception as e:
            print("Exception occurred: " + repr(e))
        finally:
            conn.close()

    @staticmethod
    def delete_file_of_client(client_id, filename):
        date_time = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        conn = None
        try:
            conn = sqlite3.connect(DB_FILE)
            conn.execute("DELETE FROM files WHERE client_id = ? AND filename = ?", [client_id, filename])
            conn.execute("UPDATE clients SET last_seen = ? WHERE client_id = ?", [date_time, client_id])
            conn.commit()
        except Exception as e:
            print("Exception occurred: " + repr(e))
        finally:
            conn.close()
