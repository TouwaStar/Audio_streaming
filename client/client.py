

import socket
import struct
import time
import re
import string
import pyaudio
import numpy
import array
from threading import Thread
import threading
import sys

from gui import Gui


def unpack_message(message):
    #print(message)
    pat = re.compile(b'PRE(.*?)SUF')
    return pat.findall(message)

class Client():
    def __init__(self,socket):
        self.socket = socket
        self.socket.setblocking(0)
        self.socket.settimeout(5)
        self.data = []
        self.data_message_size = None
        self.sampling_rate = None
        self.channels = None
        self.song_length = None
        self.frames_in_message = None
        self.frames_in_song = None
        self.frames_per_second = None
        self._input_step = None
        self._pyaudio = None
        self._stream = None
        self.song_list = []
        self.played_frame = None
        self.Gui = Gui(self)

    def _retrieve_message(self, message, size=1024):
        self.socket.send(b'GIVE_'+message)
        message = None
        self.socket.setblocking(1)
        message = self.socket.recv(size)
        print("Here")
        self.socket.setblocking(0)
        return message

    def get_songs(self):
        self.socket.send(b"GIVE_SONG_LIST")
        while True:
            try:
                message = self.socket.recv(1024)
            except:
                continue
            print(message)
            unpacked = unpack_message(message)
            
            print(unpacked)
            for unpacked_message in unpacked:
                if b'EOM' in unpacked_message:
                    print("EOM IN MESSAGE")
                    return
                print(f"Received Song {unpacked_message}")
                self.song_list.append(unpacked_message)

    def choose_song(self):
        print("PICK SONG NUMBER")
        for i, song in enumerate(self.song_list):
            print(f"{i}. {song}")
        song_pick = int(input())
        self.socket.send(b"SET_SONG_"+self.song_list[song_pick])

    def get_sampling(self):
        message = self._retrieve_message( b"SAMPLING_RATE")
        print(f"Got sampling {message}")
        self.sampling_rate = int(unpack_message(message)[0])
        print(f"After unpacking {self.sampling_rate}")

    def get_data_message_size(self):
        message = self._retrieve_message( b"STREAM_SONG")
        print(f"Got data message size {message}")
        self.data_message_size = int(unpack_message(message)[0])
        print(f"After unpacking {self.data_message_size}")
    
    def get_channels(self):
        message = self._retrieve_message( b"CHANNELS")
        print(f"Got channels {message}")
        self.channels = int(unpack_message(message)[0])
        print(f"After unpacking {self.channels}")

    def get_song_length(self):
        message = self._retrieve_message( b"SONG_LENGTH")
        print(f"Got song length {message}")
        self.song_length = int(unpack_message(message)[0])
        print(f"After unpacking {self.song_length}")

    def retrieve_audio_data(self):
        try:
            message = self.socket.recv(self.data_message_size)
        except:
            return False
        if message:
            message = unpack_message(message)
            if not self.frames_in_message:
                self.frames_in_message = len(message)
                self.frames_per_second = self.frames_in_song / self.song_length
                # This means that whenever we fast forward or rewind the song, we do so by 5 seconds
                self._input_step = int(self.frames_per_second / self.frames_in_message) * 5
            self.data.append(message)

        if b'EOM' in message:
            return False
        return True
    
    def initialize_audio_stream(self):
        print(f"Initializing audio stream with\nchannels: {self.channels}\nsampling rate: {self.sampling_rate}")
        self._pyaudio = pyaudio.PyAudio()
        self._stream = self._pyaudio.open(format=pyaudio.paInt32,
                        channels=self.channels,
                        rate=self.sampling_rate,
                        output=True)

    def play_streamed_data(self, audio_frame):
        intel = array.array('l')
        for el in audio_frame:
            intel.append(int(el))
        self._stream.write(intel.tobytes())

    def stop_audio_stream(self):
        if self._stream:
            self._stream.stop_stream()
            self._stream.close()

    def handle_input(self):
        self.Gui.event.wait()

        if self.Gui.rewind:
            self.Gui.rewind = False
            if self.played_frame - self._input_step >= 0:
                self.played_frame -= self._input_step

        if self.Gui.fast_forward:
            self.Gui.fast_forward = False
            if self.played_frame + self._input_step <= len(self.data)-1:
                self.played_frame += self._input_step

        return self.played_frame

    def play_audio(self):
        temp_ind = 0
        self.played_frame = 0

        received_all_data = False
        while True:
            if not received_all_data:
                if not self.retrieve_audio_data():
                    # Approximate if the song is complete
                    received_length = (len(self.data) * self.frames_in_message)/self.sampling_rate
                    if self.song_length - 5 <= received_length <= self.song_length + 5:
                        received_all_data = True

            self.played_frame = self.handle_input()

            if received_all_data:
                if len(self.data)-1 <= self.played_frame:
                    break

            if len(self.data)-1 >= self.played_frame:
                if b'EOM' in self.data[self.played_frame]:
                    break
                
                self.play_streamed_data(self.data[self.played_frame])
                self.played_frame += 1
                

            
     


def main():
    
    HOST = '127.0.0.1'
    PORT = 8989
    client = None

    if len(sys.argv) >1:
        HOST = sys.argv[1]
    if len(sys.argv) >2:
        PORT = int(sys.argv[2])

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            print("Created a socket")
            s.connect((HOST,PORT))
            print("Connected to a socket")
            
            while True:
                client = Client(s)
                client.song_list = []
                client.get_songs()
                client.choose_song()
                client.get_sampling()
                client.get_channels()
                client.get_song_length()
                client.frames_in_song = client.sampling_rate * client.song_length
                client.get_data_message_size()
                client.initialize_audio_stream()
                client.Gui.start_drawing()
                client.play_audio()
                client.Gui.running = False
                
                
    except ConnectionRefusedError as e:
        print(f"Couldn't connect to server, make sure it's running before connecting\n{repr(e)}")          
    except ConnectionResetError as e:
        print(f"Exception {repr(e)}")
    except Exception as e:
        print(f"Exception {repr(e)}")
    finally:
        s.close()
        if client:
            client.stop_audio_stream()

if __name__ == "__main__":
    main()