#!/usr/bin/env python3
import serial
import time
import socket

pot_delta = 10

def map_range(x, in_min, in_max, out_min, out_max):
  return (x - in_min) * (out_max - out_min) // (in_max - in_min) + out_min

# Set up socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
laptop_ip = "169.254.107.194"  # replace with your laptop's IP address
port = 12346  # choose a port number (can be any integer)

print("Connecting to laptop")
# Connect to laptop
sock.connect((laptop_ip, port))
print("Connected to laptop")

if __name__ == '__main__':
    curr_pot_reading = 0
    prev_pot_reading = 0
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.reset_input_buffer()
    while True:
        curr_pot_reading = ser.readline().decode('utf-8').rstrip()
        if curr_pot_reading.isdigit():
            curr_pot_reading = int(curr_pot_reading)
        else:
            continue
        #print(curr_pot_reading)
        if (curr_pot_reading - prev_pot_reading > pot_delta):
            vel = map_range(curr_pot_reading, 0, 1023, 0, 127)
            packet = "vel:" + str(vel)
            sock.sendall(packet.encode())

        time.sleep(1)
        prev_pot_reading = curr_pot_reading

