import RPi.GPIO as GPIO
import sys
import time
import pygame

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

buffer0 = 21
buffer1 = 20
buffer2 = 16
buffer3 = 12
buffer4 = 26
buffer5 = 19
buffer6 = 13
buffer7 = 6

DE2flag = 23
PIflag = 24

GPIO.setup(buffer0, GPIO.IN)
GPIO.setup(buffer1, GPIO.IN)
GPIO.setup(buffer2, GPIO.IN)
GPIO.setup(buffer3, GPIO.IN)
GPIO.setup(buffer4, GPIO.IN)
GPIO.setup(buffer5, GPIO.IN)
GPIO.setup(buffer6, GPIO.IN)
GPIO.setup(buffer7, GPIO.IN)
GPIO.setup(DE2flag, GPIO.IN)
GPIO.setup(PIflag, GPIO.OUT)

def receive():
    GPIO.output(PIflag, GPIO.LOW)
    while GPIO.input(DE2flag) == GPIO.LOW:
        pass
    received = GPIO.input(buffer0)
    received = received << 1
    received = received|GPIO.input(buffer1)
    received = received << 1
    received = received|GPIO.input(buffer2)
    received = received << 1
    received = received|GPIO.input(buffer3)
    received = received << 1
    received = received|GPIO.input(buffer4)
    received = received << 1
    received = received|GPIO.input(buffer5)
    received = received << 1
    received = received|GPIO.input(buffer6)
    received = received << 1
    received = received|GPIO.input(buffer7)
    GPIO.output(PIflag, GPIO.HIGH)
    while GPIO.input(DE2flag) == GPIO.HIGH:
        pass
    GPIO.output(PIflag, GPIO.LOW)
    return received

def interrupt(channel):
    char  = chr(receive())
    print char
    if char == 'Z':
        pygame.mixer.music.play()
        while pygame.mixer.music.get_busy() == True:
            continue
    
pygame.mixer.init()
pygame.mixer.music.load("BD0000.mp3")

GPIO.add_event_detect(DE2flag, GPIO.RISING, callback=interrupt, bouncetime=300)

while True:
    print "Waiting for event"
    time.sleep(2)

print "Stopped receiving"


