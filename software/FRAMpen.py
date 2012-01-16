#!/usr/bin/env python

"""FRAMpen.py: Reads and converts data from the FRAMpen.
http://processors.wiki.ti.com/index.php/FRAMpen
"""

__author__    = "Max Groening, Thomas Bendel"


import serial

MAX_BYTES = 48  # Maximum number of bytes to read
BAUDRATE = 9600
PORT = 0  # Windows ports start from '0'(=COM1), Linux: e.g. '/dev/ttyS1'
TIMEOUT = 15  # Timeout in seconds

##############################################
# No configuration needed beyond this point. #
##############################################

ser = serial.Serial()  # Open serial connection
ser.bautrate = BAUDRATE
ser.port = PORT
ser.timeout = TIMEOUT

print(ser)

ser.open()

if ser.isOpen():
	print("\nStart transmission now!\n")
else:
	print("\nCannot open port!\n")
	exit()


data = ser.read(MAX_BYTES)
ser.close()

print(data)
