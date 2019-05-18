HOST = '127.0.0.1'
PORT = 6999

import socket
import struct
import time
import re


def main():
    data = 'a'
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST,PORT))
            while(True):
                time.sleep(3)
                data =s.recv(1024)
                print(data)
                #data =int.from_bytes(data, byteorder='little', signed=True)
                #print(data)
                print(type(data))
    except ConnectionResetError:
        pass
    finally:
        pat = re.compile(b'PRE(.*?)SUF')
        test = pat.findall(data)
        print(test)
if __name__ == "__main__":
    main()