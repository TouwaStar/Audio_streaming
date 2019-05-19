HOST = '127.0.0.1'
PORT = 6999

import socket
import struct
import time
import re
import string
import time
import pyaudio


def unpack_message(message):
    print(message)
    pat = re.compile(b'PRE(.*?)SUF')
    return pat.findall(message)[0]


def main():
    data = []
    message_size_b = b''
    sampling_rate = 0
    
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST,PORT))

            message = s.recv(1024)
            print(message)
            if(b'SAMPLING' in message):
                s.send(b"GIVE_SAMPLING_RATE")
                message = s.recv(1024)
                print(message)
                sampling_rate = int(unpack_message(message))
                s.send(b"GOT_SAMPLING_RATE")
            
            s.send(b"GIVE_SIZE")
            message_size_b = s.recv(1024)
            starttime = time.time()
            s.send(b"GOT_SIZE")
            message_size = unpack_message(message_size_b)[0]
            message_size_unpacked = int(message_size)
            while(True):
                message = s.recv(message_size_unpacked)
                if (b'EOM' in message):
                    break
                data.append(message)
                
    except ConnectionResetError:
        pass
    finally:
        pat = re.compile(b'PRE(.*?)SUF')
        #sep = ''
        #bfs = sep.join(data)
        #test = pat.findall(bfs)
        #print(test)
        #for message in data:
        #    print(pat.findall(message))

        print(time.time() - starttime)

        #p = pyaudio.PyAudio()
        '''
        stream = p.open(format=pyaudio.paFloat32,
                        channels=2,
                        rate=audio.sampling_rate,
                        output=True)
        '''

if __name__ == "__main__":
    main()