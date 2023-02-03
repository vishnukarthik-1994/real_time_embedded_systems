import os
import time

def reboot_board():
    with open("reboot_time.log", "w") as f:
        f.write(str(time.time()))
    os.system("sudo reboot")

reboot_board()