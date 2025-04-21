import RPi.GPIO as GPIO
import tkinter as tk

GPIO.setmode(GPIO.BCM)

green_pin = 17
yellow_pin = 27
red_pin = 22
GPIO.setup(green_pin, GPIO.OUT)
GPIO.setup(yellow_pin, GPIO.OUT)
GPIO.setup(red_pin, GPIO.OUT)

pwm_green = GPIO.PWM(green_pin, 100)
pwm_yellow = GPIO.PWM(yellow_pin, 100)
pwm_red = GPIO.PWM(red_pin, 100)

pwm_green.start(0)
pwm_yellow.start(0)
pwm_red.start(0)

def set_brightness_green(value):
    pwm_green.ChangeDutyCycle(float(value))

def set_brightness_yellow(value):
    pwm_yellow.ChangeDutyCycle(float(value))

def set_brightness_red(value):
    pwm_red.ChangeDutyCycle(float(value))

window = tk.Tk()
window.title("LED Brightness Control")

green_scale = tk.Scale(window, from_=0, to=100, orient=tk.HORIZONTAL,
                       label="Green Brightness", command=set_brightness_green)
green_scale.pack(pady=10)

yellow_scale = tk.Scale(window, from_=0, to=100, orient=tk.HORIZONTAL,
                        label="Yellow Brightness", command=set_brightness_yellow)
yellow_scale.pack(pady=10)

red_scale = tk.Scale(window, from_=0, to=100, orient=tk.HORIZONTAL,
                     label="Red Brightness", command=set_brightness_red)
red_scale.pack(pady=10)

button = tk.Button(window, text="Exit", command=window.destroy)
button.pack(pady=10)

window.mainloop()

pwm_green.stop()
pwm_yellow.stop()
pwm_red.stop()
GPIO.cleanup()