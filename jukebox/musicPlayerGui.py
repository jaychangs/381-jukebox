from subprocess import call 
import subprocess

import RPi.GPIO as GPIO
from Tkinter import *
import tkFileDialog
import Tkinter
import tkFont
import Tkinter as tk
#import tkSnack
import os
import glob
import errno
import shlex
import ttk

import pygame
import vlc
pygame.mixer.init()
music = vlc.MediaPlayer()
snare = pygame.mixer.Sound("snare.wav")
perc = pygame.mixer.Sound("percuss.wav") 
xylo = pygame.mixer.Sound("xylo.wav") 
drum = pygame.mixer.Sound("drum.wav") 

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

#GPIO.add_event_detect(DE2flag, GPIO.RISING, callback=interrupt, bouncetime=50)

freq_band_1 = shlex.split("sudo amixer -D equal cset numid=2")
freq_band_2 = shlex.split("sudo amixer -D equal cset numid=3")
freq_band_3 = shlex.split("sudo amixer -D equal cset numid=4")
freq_band_4 = shlex.split("sudo amixer -D equal cset numid=5")
freq_band_5 = shlex.split("sudo amixer -D equal cset numid=6")
freq_band_6 = shlex.split("sudo amixer -D equal cset numid=7")
freq_band_7 = shlex.split("sudo amixer -D equal cset numid=8")
freq_band_8 = shlex.split("sudo amixer -D equal cset numid=9")

volume_control = shlex.split("sudo amixer cset numid=1")

play_list_window = None

class playmusic:
	#GPIO.add_event_detect(DE2flag, GPIO.RISING, callback=interrupt, bouncetime=50)
	def __init__(self):
		global music
		global play_list_window
		
		self.root=tk.Tk()		

		self.root.title("Jukebox Music Player")
	
		self.root.configure(background = 'black')
		self.root.geometry('500x500+500+300')
		
		top = Frame(self.root)
		center = Frame(self.root)
		lyrics = Frame(self.root)
		progress = ttk.Frame(self.root)
		bottom = Frame(self.root)
		top.pack(side=TOP, fill = BOTH, expand = False)
		center.pack(side=TOP, fill = BOTH, expand = True)
		lyrics.pack(side=TOP, fill=BOTH, expand = True)
		progress.pack(side=TOP, fill = BOTH, expand = False)
		bottom.pack(side= BOTTOM, fill =BOTH, expand = False)
	
	
		self.filename = Tkinter.StringVar()
		self.name = Tkinter.StringVar()
		self.play_list = Tkinter.StringVar()
		self.artist = Tkinter.StringVar()
		self.title = Tkinter.StringVar()
		self.position = Tkinter.IntVar()
		
		title = Label(top, text ="Jukebox Music Player", fg='white', bg='RoyalBlue4')
		title.pack(in_=top, fill=BOTH, expand =False)

		play_list_window = Listbox(self.root, fg='white', bg='SteelBlue4')
		play_list_window.pack(in_=center, fill = BOTH, expand = True)

		self.pbar = ttk.Progressbar(progress,orient='horizontal',mode='determinate', variable=self.position)
	
		self.pbar.pack(in_=progress, fill = BOTH, expand = True)
		
		self.lyr = Text(self.root, height =5, width=5)
		self.lyr.pack(in_=lyrics,side = TOP, fill = BOTH, expand=True)
			
		self.getlyr = Button(lyrics, command = self.getlyrics, width = 5, height =1, text = 'Get Lyrics')
		self.ainput = Entry(lyrics, width = 5, text = 'Enter Artist',textvariable = self.artist)
		self.tinput = Entry(lyrics, width = 5, text = 'Enter Title', textvariable = self.title)
		self.ainput.pack(in_=lyrics, side = LEFT, fill = BOTH, expand =True)
		self.tinput.pack(in_=lyrics, side = LEFT, fill = BOTH, expand =True)
		self.getlyr.pack(in_=lyrics, side = LEFT, fill = BOTH, expand =True)

		play_button = Button(bottom, width = 5, height = 1, text = 'Play',
			fg='white', command =self.play, bg="DodgerBlue4")

		stop_button = Button(bottom, width = 5, height = 1, text = 'Stop',
                fg='white', command = self.stop, bg="SpringGreen4")
	        pause_button = Button(bottom, width = 5, height = 1, text = 'Pause',
	                fg='white', command = self.pause, bg="DodgerBlue4")
		
		play_button.pack(in_=bottom,side=LEFT, fill = BOTH, expand = True)
		pause_button.pack(in_=bottom,side=LEFT, fill = BOTH, expand = True)
		stop_button.pack(in_=bottom,side=LEFT, fill = BOTH, expand = True)

		play_list_window.bind('<Button-1>', callback)
		self.move()
		#self.getcommand()
		GPIO.add_event_detect(DE2flag, GPIO.RISING, callback=interrupt, bouncetime=50)	
		self.root.mainloop()
	 	
	def play(self):
		global music
		music.play()
		print "started playing"
		self.pbar.start(1200)
	def stop(self):
		global music
		music.stop()
		self.pbar.stop()
		self.pbar.step(-self.position.get())
		
        def pause(self):
		global music
		music.pause()
		self.pbar.stop()
				

	def move(self):
		global play_list_window
	        path = '/home/pi/EECE381Module2/jukebox/*.m4a'
       	 	files = glob.glob(path)
		index = play_list_window.size()
		songs = play_list_window.get(0, index-1)
		
        	for name in files:
			f=open(name)
			y='temp.m4a'
			x = os.path.basename(f.name)
			if x in songs:
			 	string = "no"
			elif y in f.name:
				string = "no"
			else:
        			play_list_window.insert(END, x)
		self.root.after(100,self.move)
	
	def getlyrics(self):	
		i=open("text1.txt", "w")
		print self.artist.get()
		print self.title.get()
		y = subprocess.call("./collectlyrics.py -a '"+ str(self.artist.get())+"' -t '"+str(self.title.get())+"' > text1.txt",shell = True)
		self.lyr.delete(1.0, END)
		x = open("text1.txt", "r")	
		self.lyr.insert(1.0, x.read()) 

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
	idChar = chr(receive())
	print idChar
	if idChar == 'A':
		amp_value = receive()
		print amp_value
		cmd = freq_band_1
		cmd.append(str(amp_value))
		print cmd
		call(cmd)
		cmd.pop()
	elif idChar == 'B':
		amp_value = receive()
		print amp_value
		cmd = freq_band_2
		cmd.append(str(amp_value))
		print cmd
		call(cmd)
		cmd.pop()
	elif idChar == 'C':
		amp_value = receive()
		print amp_value
		cmd = freq_band_3
		cmd.append(str(amp_value))
		print cmd
		call(cmd)
		cmd.pop()
	elif idChar == 'D':
		amp_value = receive()
		print amp_value
		cmd = freq_band_4
		cmd.append(str(amp_value))
		print cmd
		call(cmd)
		cmd.pop()
	elif idChar == 'E':
		amp_value = receive()
		print amp_value
		cmd = freq_band_5
		cmd.append(str(amp_value))
		print cmd
		call(cmd)
		cmd.pop()
	elif idChar == 'F':
		amp_value = receive()
		print amp_value
		cmd = freq_band_6
		cmd.append(str(amp_value))
		print cmd
		call(cmd)
		cmd.pop()
	elif idChar == 'G':
		amp_value = receive()
		print amp_value
		cmd = freq_band_7
		cmd.append(str(amp_value))
		print cmd
		call(cmd)
		cmd.pop()
	elif idChar == 'H':
		amp_value = receive()
		print amp_value
		cmd = freq_band_8
		cmd.append(str(amp_value))
		print cmd
		call(cmd)
		cmd.pop()
	elif idChar == 'V':
		amp_value = receive()
		print amp_value
		cmd = volume_control
		cmd.append(str(amp_value)+'%')
		print cmd
		call(cmd)
		cmd.pop()
	elif idChar == 'S':
		global snare
		snare.play()
	elif idChar == 'T':
		global perc
		perc.play()
	elif idChar == 'U':
		global xylo
		xylo.play()
	elif idChar == 'W':
		global drum
		drum.play()
				
							
def callback(event):
		global music
		music.stop()
		widget = event.widget
		selection = widget.curselection()
		value = widget.get(selection[0])
		value = os.path.basename(value)
		print value
		music = vlc.MediaPlayer(value)

if __name__== "__main__":
	playmusic()
	
