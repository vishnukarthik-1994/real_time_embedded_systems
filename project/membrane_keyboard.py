import RPi.GPIO as GPIO
import time
from gpiozero import TonalBuzzer
from gpiozero.tones import Tone
import socket

# Set up socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
laptop_ip = "169.254.107.194"  # replace with your laptop's IP address
port = 12345  # choose a port number (can be any integer)

print("Connecting to laptop")
# Connect to laptop
sock.connect((laptop_ip, port))
print("Connected to laptop")

# Set up GPIO pins for keypad
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

# Define keypad rows and columns
#rows = [14, 15, 18, 23]
rows = [14, 15, 26, 23]
cols = [6, 25, 8, 7]

# Define keypad matrix
keypad = [
    ["1", "2", "3", "A"],
    ["4", "5", "6", "B"],
    ["7", "8", "9", "C"],
    ["*", "0", "#", "D"]
]

# Set up GPIO pins for keypad matrix
for row in rows:
    GPIO.setup(row, GPIO.OUT)
    GPIO.output(row, GPIO.HIGH)

for col in cols:
    GPIO.setup(col, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Define MIDI note mappings
notes = {
    "1": 60, # C4
    "2": 62, # D4
    "3": 64, # E4
    "A": 65, # F4
    "4": 67, # G4
    "5": 69, # A4
    "6": 71, # B4
    "B": 72, # C5
    "7": 74, # D5
    "8": 76, # E5
    "9": 77, # F5
    "C": 79, # G5
    "*": 81, # A5
    "0": 83, # B5
    "#": 84, # C6
    "D": 86, # D6
}

buzzer = TonalBuzzer(21, octaves=3)
velocity = 127

# Loop through keypad matrix and read MIDI notes
while True:
    for i, row in enumerate(rows):
        GPIO.output(row, GPIO.LOW)
        for j, col in enumerate(cols):
            if GPIO.input(col) == GPIO.LOW:
                note = notes[keypad[i][j]]
                packet = "note:" + str(note)
                sock.sendall(packet.encode())
                #buzzer.play(Tone(note))
                time.sleep(0.5)
                #buzzer.stop()
        GPIO.output(row, GPIO.HIGH)
        time.sleep(0.01)