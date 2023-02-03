import time
import os

def is_board_up():
    response = os.system("ping -c 1 VKD0.local")
    if response == 0:
        return True
    else:
        return False

def reboot_and_measure_boot_time():
    with open("/home/pi/Documents/assignment_2/reboot_time.log", "r") as f:
        start_time = f.read()
        start_time = float(start_time)
    while not is_board_up():
        time.sleep(1)
    end_time = time.time()
    boot_time = end_time - start_time
    with open("/home/pi/Documents/assignment_2/boot_time.log", "w") as f:
        f.write(f"Boot time: {boot_time:.2f} seconds\n")

reboot_and_measure_boot_time()