
from multiprocessing import Process
import pygame

class Gui():
    def __init__(self):
        self.running = False



    def start_drawing(self):
        pygame.init()
        screen = pygame.display.set_mode((800, 600))
        pygame.display.set_caption("Client")
        clock = pygame.time.Clock()
        draw_loop = Process(target=self._draw_loop, args=(screen,clock))
        draw_loop.start()
    
    def _draw_loop(self, screen, clock):
        self.running = True
        while self.running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
            clock.tick(10)
            pygame.display.flip()

