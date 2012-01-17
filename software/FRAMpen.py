#!/usr/bin/env python

"""FRAMpen.py: Reads and converts data from the FRAMpen.
http://processors.wiki.ti.com/index.php/FRAMpen
"""

__author__    = "Max Groening, Thomas Bendel"

#################
# Configuration #
#################

MAX_BYTES = 60  # Maximum number of bytes to read
BAUDRATE = 9600
PORT = 2  # Windows ports start from '0'(=COM1), Linux: e.g. '/dev/ttyS1'
TIMEOUT = 30 # Timeout in seconds

DATA_FILENAME = 'raw.dat'
DEBUG = True

##############################################
# No configuration needed beyond this point. #
##############################################

import serial  # PySerial

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


raw_data = ser.read(MAX_BYTES)
ser.close()

# Convert byte (char) into decimal value
data_8 = []
for entry in raw_data:
	data_8.append(ord(entry))

# Join too (decimal) 'bytes' back into one word
# Shift 'high byte' back by multiplying with 8
data_16 = []
for high_byte, low_byte in zip(data_8[0::2], data_8[1::2]):
	data_16.append(high_byte * 256 + low_byte)

if DEBUG:
	print(data_8)
	print(data_16)

# i = 0
# while (i < len(data_8)):
# 	# Shift 'high byte' back by multiply with 8
# 	data_16.append(data_8[i] * 256 + data_8[i+1])
# 	i += 2

# Separate data by X, Y, and Z coordinate
data_x = []
data_y = []
data_z = []

for i in range(len(data_16)):
	j = i % 3  # j = 0, 1, 2, 0, 1, 2, ...
	if (j == 0):
		data_x.append(data_16[i])
	elif (j == 1):
		data_y.append(data_16[i])
	else:
		data_z.append(data_16[i])

if DEBUG:
	print(data_x)
	print(data_y)
	print(data_z)

data_handle = open(DATA_FILENAME, 'w')

for entry in data:
 	data_handle.write(str(ord(entry)) + '\n')

data_handle.close()
