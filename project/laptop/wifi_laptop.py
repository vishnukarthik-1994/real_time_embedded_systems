import socket
import time
import rtmidi

midiout = rtmidi.MidiOut()
available_ports = midiout.get_ports()

# here we're printing the ports to check that we see the one that loopMidi created. 
# In the list we should see a port called "loopMIDI port".
print(available_ports)

# Attempt to open the port
if available_ports:
    midiout.open_port(1)
else:
    midiout.open_virtual_port("My virtual output")

# Set up socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
laptop_ip = "169.254.107.194"  # replace with your laptop's IP address
port = 12345  # same port as Raspberry Pi

# Bind socket to laptop's IP address and port
sock.bind((laptop_ip, port))
print("Listening for data...")

# Listen for incoming connections
sock.listen(2)

# Accept incoming connection from Raspberry Pi
conn, addr = sock.accept()
print("Connected to Raspberry Pi:", addr)

# Receive data from Raspberry Pi
while 1:
    data = conn.recv(1024)
    data = data.decode()
    if "vel" in data:
        data = data.replace("vel:", '')
        print("Received int vel:", int(data))
        note_on = [0x90, 0, int(data)]
        note_off = [0x80, 0, 0]
        midiout.send_message(note_on)
        time.sleep(0.5) 
        midiout.send_message(note_off)   
    elif "note" in data:
        data = data.replace("note:", '')
        print("Received int note:", int(data))
        note_on = [0x90, int(data), 127]
        note_off = [0x80, int(data), 0]
        midiout.send_message(note_on)
        time.sleep(0.5) 
        # I tried running the script without having to invoke the sleep function but it doesn't work. 
        # If someone could enlighten me as to why this is, I'd be more than grateful.
        midiout.send_message(note_off)
    else:
        time.sleep(0.5)

# Close socket
conn.close()
