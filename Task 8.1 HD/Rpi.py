import asyncio
from bleak import BleakClient, BleakScanner
import RPi.GPIO as GPIO
import time

# UUID of the distance characteristic
DISTANCE_UUID = "00002a56-0000-1000-8000-00805f9b34fb"
DEVICE_NAME = "ArduinoSensor"
LED_PIN = 18  # PWM-capable GPIO pin on RPi (change as needed)

# PWM intensity mapping
def get_duty_cycle(distance):
    if distance < 10:ser
        return 100
    elif distance < 30:
        return 60
    else:
        return 30

async def run():
    # Setup PWM
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(LED_PIN, GPIO.OUT)
    pwm = GPIO.PWM(LED_PIN, 1000)
    pwm.start(0)

    print("Scanning for ArduinoSensor...")
    device = None
    devices = await BleakScanner.discover()
    for d in devices:
        if DEVICE_NAME in d.name:
            device = d
            break

    if device is None:
        print("ArduinoSensor not found.")
        GPIO.cleanup()
        return

    async with BleakClient(device.address) as client:
        print("Connected to Arduino")

        def handle_notify(_, data):
            distance = int(data[0])
            duty = get_duty_cycle(distance)
            pwm.ChangeDutyCycle(duty)
            print(f"Distance: {distance} cm -> LED Intensity: {duty}%")

        await client.start_notify(DISTANCE_UUID, handle_notify)

        print("Receiving distance updates. Press Ctrl+C to exit.")
        try:
            while True:
                await asyncio.sleep(1)
        except KeyboardInterrupt:
            print("Exiting...")
        finally:
            await client.stop_notify(DISTANCE_UUID)
            pwm.stop()
            GPIO.cleanup()

if __name__ == "__main__":
    asyncio.run(run())