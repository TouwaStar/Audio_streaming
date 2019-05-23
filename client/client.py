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
    #print(message)
    pat = re.compile(b'PRE(.*?)SUF')
    return pat.findall(message)

class Client():
    def __init__(self):
        self.data = []
        self.data_message_size = None
        self.sampling_rate = None
        self.channels = None
        self._pyaudio = None
        self._stream = None

    def _retrieve_message(self, socket, message, size=1024):
        socket.send(b'GIVE_'+message)
        message = socket.recv(size)
        socket.send(b'GOT_'+message)
        return message

    def get_sampling(self, socket):
        message = self._retrieve_message(socket, b"SAMPLING_RATE")
        print(f"Got sampling {message}")
        self.sampling_rate = int(unpack_message(message)[0])
        print(f"After unpacking {self.sampling_rate}")

    def get_data_message_size(self, socket):
        message = self._retrieve_message(socket, b"SIZE")
        print(f"Got data message size {message}")
        self.data_message_size = int(unpack_message(message)[0])
        print(f"After unpacking {self.data_message_size}")
    
    def get_channels(self, socket):
        message = self._retrieve_message(socket, b"CHANNELS")
        print(f"Got channels {message}")
        self.channels = int(unpack_message(message)[0])
        print(f"After unpacking {self.channels}")

    def retrieve_audio_data(self, socket):
        message = socket.recv(self.data_message_size)
        self.data.append(unpack_message(message))
        if b'EOM' in message:
            return False
        return True
    
    def initialize_audio_stream(self):
        print(f"Initializing audio stream with\nchannels: {self.channels}\nsampling rate: {self.sampling_rate}")
        self._pyaudio = pyaudio.PyAudio()
        self._stream = self._pyaudio.open(format=pyaudio.paInt32,
                        channels=1,
                        rate=self.sampling_rate,
                        output=True)

    def play_streamed_data(self, audio_frame):
        #print(f"writing {audio_frame} to audio stream")
        for el in audio_frame:
            self._stream.write(hex(int(el)),exception_on_underflow=True)

    def stop_audio_stream(self):
        self._stream.stop_stream()
        self._stream.close()



def main():
    
    client = Client()

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST,PORT))

            client.get_sampling(s)
            client.get_channels(s)
            client.get_data_message_size(s)
            client.initialize_audio_stream()
            while client.retrieve_audio_data(s):
                client.play_streamed_data(client.data[-1])
                
    except ConnectionResetError:
        pass
    finally:
        client.stop_audio_stream()

if __name__ == "__main__":
    main()