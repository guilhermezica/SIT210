import tkinter as tk
import tkinter.font
from gpiozero import LED
import RPi.GPIO
RPi.GPIO.setmode(RPi.GPIO.BCM)

##hardware
Greenled = LED(14)
Redled = LED(15)
Blueled = LED(16)

## GUI DEFINITIONS ##
win = tk.Tk()
win.title("LED Toggler")
myFont = tkinter.font.Font(family = 'Helvetica', size = 12, weight = "bold")

### EVENT FUNCTIONS ###
def GreenledToggle():
	if Greenled.is_lit:
		Greenled.off()
		GreenledButton["text"] = "Turn Green LED on"
	else:
		Greenled.on()
		GreenledButton["text"] = "Turn Green LED off"

def RedledToggle():
	if Redled.is_lit:
		Redled.off()
		RedledButton["text"] = "Turn Red LED on"
	else:
		Redled.on()
		RedledButton["text"] = "Turn Red LED off"
		
def BlueledToggle():		
	if Blueled.is_lit:
		Blueled.off()
		BlueledButton["text"] = "Turn Blue LED on"
	else:
		Blueled.on()
		BlueledButton["text"] = "Turn Blue LED off"
		

### WIDGETS ###
GreenledButton = tk.Button(win, text = "Turn Green LED on", font = myFont, 
command = GreenledToggle, bg = 'bisque2', height = 1, width = 24)
GreenledButton.grid(row=0, column=1)

RedledButton = tk.Button(win, text = "Turn Red LED on", font = myFont, 
command = RedledToggle, bg = 'bisque2', height = 1, width = 24)
RedledButton.grid(row=1, column=1)

BlueledButton = tk.Button(win, text = "Turn Blue LED on", font = myFont, 
command = BlueledToggle, bg = 'bisque2', height = 1, width = 24)
BlueledButton.grid(row=2, column=1)

win.mainloop()
