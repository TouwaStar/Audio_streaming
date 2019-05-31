from threading import Thread
import pygame
from pygame.locals import *
import threading
import sys


class Gui():
    def __init__(self):
        self.running = False
        self.event = threading.Event()
        self._pause = False



    def start_drawing(self):
        draw_loop = Thread(target=self._draw_loop,daemon=True)

        draw_loop.start()
    
    def _draw_loop(self):
        self.event.set()
        pygame.init()
        screen = pygame.display.set_mode((800, 600))
        pygame.display.set_caption("Client")
        clock = pygame.time.Clock()
        self.running = True
        green = 0
        while self.running:
            screen.fill([0,green,0])
            green += 1
            if green > 200:
                green = 0
            clock.tick(30)
            pygame.display.flip()
            for event in pygame.event.get():
                if event.type == QUIT:
                    self.running = False
                    pygame.quit()
                if event.type == KEYDOWN:
                    if event.key == pygame.K_SPACE:
                        if self._pause:
                            self.event.set()
                            self._pause = not self._pause
                        else:
                            self.event.clear()
                            self._pause = not self._pause


            if not self.running:
                pygame.quit()
            

