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

class Client():
    def __init__(self):
        self.data = []
        self.data_message_size = 0
        self.sampling_rate = 0
        self._stream = None

    def _retrieve_message(self, socket, message, size=1024):
        socket.send(b'GIVE_'+message)
        message = socket.recv(size)
        socket.send(b'GOT_'+message)
        return message

    def get_sampling(self, socket):
        message = self._retrieve_message(socket, "SAMPLING_RATE")
        self.sampling_rate = int(unpack_message(message)[0])

    def get_data_message_sizesize(self, socket):
        message = self._retrieve_message(socket, "SIZE")
        self.data_message_size = int(unpack_message(message)[0])

    def retrieve_audio_data(self, socket):
        message = socket.recv(self.data_message_size)
        self.data.append(message)
        if b'EOM' in message:
            return False
        return True
    
    def initialize_audio_stream(self):
        self.stream = p.open(format=pyaudio.paFloat32,
                        channels=,
                        rate=self.sampling_rate,
                        output=True)

    def play_streamed_data(self, audio_frame):




def main():
    data = []
    message_size_b = b''
    sampling_rate = 0
    
    client = Client()

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST,PORT))

            client.get_sampling(s)
            client.get_data_message_sizesize(s)
            client.initialize_audio_stream
            while client.retrieve_audio_data(s):
                play_streamed_data(frame)
                
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


        #p = pyaudio.PyAudio()
        '''
        stream = p.open(format=pyaudio.paFloat32,
                        channels=2,
                        rate=audio.sampling_rate,
                        output=True)
        '''

if __name__ == "__main__":
    main()