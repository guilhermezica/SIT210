import speech_recognition as sr
import RPi.GPIO as GPIO
import time

#GPIO setup
LED_PIN = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_PIN, GPIO.OUT)

#speech recognition setup
recognizer = sr.Recognizer()
mic = sr.Microphone()

def listen_for_commands():
      with mic as source:
            print("Adjusting for ambient noise...")
            recognizer.adjust_for_ambient_noise(source)
            print("Listening for commands...")
            while True:
                  try:
                        audio = recognizer.listen(source)
                        print("Processing...")
                        command = recognizer.recognize_google(audio).lower()
                        print(f"Heard: {command}")
                        
                        if "switch on" in command:
                              GPIO.output(LED_PIN, GPIO.HIGH)
                              print("LED turned ON")
                        elif "switch off" in command:
                              GPIO.output(LED_PIN, GPIO.LOW)
                              print("LED turned OFF")
                        else:
                              print("Unknown command")
                              
                  except sr.UnknownValueError:
                        print("Sorry, could not understand audio")
                  except sr.RequestError as e:
                        print(f"Could not request results; {e}")
                  time.sleep(0.5)
                  
try:
      listen_for_commands()
except KeyboardInterrupt:
      print("Exiting gracefully")
finally:
      GPIO.cleanup()