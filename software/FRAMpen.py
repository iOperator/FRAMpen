#!/usr/bin/env python

"""FRAMpen.py: Reads and converts data from the FRAMpen.
http://processors.wiki.ti.com/index.php/FRAMpen
"""

__author__ = "Max Groening, Thomas Bendel"

#################
# Configuration #
#################

MAX_BYTES = 6666 #7164  # Maximum number of bytes to read
BAUDRATE = 9600
PORT = 2  # Windows ports start from '0'(=COM1), Linux: e.g. '/dev/ttyS1'
TIMEOUT = 30  # Timeout in seconds

DATA_RAW_FILENAME = 'raw.dat'
DATA_ACC_FILENAME = 'acc.dat'
DATA_VEL_FILENAME = 'vel.dat'
DATA_POS_FILENAME = 'pos.dat'
DEBUG = False

AVERAGES_X = 10  # Number of samples for moving average
AVERAGES_Y = 10  #
AVERAGES_Z = 10  #


##############################################
# No configuration needed beyond this point. #
##############################################

import serial  # PySerial
from test import *  # Test samples

### Get the data via serial connection #######################################

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

# Join to (decimal) 'bytes' back into one word
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

for i in range(len(data_16[0::3])):
	data_x.append(data_16[0::3][i])
 	data_y.append(data_16[1::3][i])
 	data_z.append(data_16[2::3][i])

if DEBUG:
	print(data_x)
 	print(data_y)
 	print(data_z)

# # Load test samples
# data_x = sample1_raw_x
# data_y = sample1_raw_y
# data_z = sample1_raw_z

# Output raw data
data_handle = open(DATA_RAW_FILENAME, 'w')

for i in range(len(data_x)):
	data_handle.write(str(data_x[i]) + ' ' +
			  str(data_y[i]) + ' ' +
			  str(data_z[i]) + '\n')

data_handle.close()


### Integrate acceleration ###################################################

# Remove offset
x_offset_0 = data_x[0]
y_offset_0 = data_y[0]
z_offset_0 = data_z[0]
acc_x = []  # Stores acceleration data without offset
acc_y = []  #
acc_z = []  #

# # Moving average offset
# # 3rd order
# x_offset_3rd = [0] * len(data_x)
# y_offset_3rd = [0] * len(data_y)
# z_offset_3rd = [0] * len(data_z)
# x_offset_3rd[0] = data_x[0]
# y_offset_3rd[0] = data_y[0]
# z_offset_3rd[0] = data_z[0]
# for i in range(1, len(data_x) - 1):
# 	x_offset_3rd[i] = (data_x[i-1] + data_x[i] + data_x[i+1]) / 3
# 	y_offset_3rd[i] = (data_y[i-1] + data_y[i] + data_y[i+1]) / 3
# 	z_offset_3rd[i] = (data_z[i-1] + data_z[i] + data_z[i+1]) / 3
# x_offset_3rd[-1] = (data_x[-2] + data_x[-1] + data_x[-1]) / 3
# y_offset_3rd[-1] = (data_y[-2] + data_y[-1] + data_y[-1]) / 3
# z_offset_3rd[-1] = (data_z[-2] + data_z[-1] + data_z[-1]) / 3

# for i in range(len(data_x)):
# 	acc_x.append(data_x[i] - x_offset_3rd[i])
# 	acc_y.append(data_y[i] - y_offset_3rd[i])
# 	acc_z.append(data_z[i] - z_offset_3rd[i])

# Moving average
average_values_x = [data_x[0]] * AVERAGES_X
average_values_y = [data_y[0]] * AVERAGES_Y
average_values_z = [data_z[0]] * AVERAGES_Z

for i in range(len(data_x)):
	# right ship 'average_values', insert current sample at first position
	average_values_x = [data_x[i]] + average_values_x[:-1]
	average_values_y = [data_y[i]] + average_values_y[:-1]
	average_values_z = [data_z[i]] + average_values_z[:-1]
	acc_x.append(data_x[i] - sum(average_values_x) / float(AVERAGES_X))
	acc_y.append(data_y[i] - sum(average_values_y) / float(AVERAGES_Y))
	acc_z.append(data_z[i] - sum(average_values_z) / float(AVERAGES_Z))

# # Static offset
# for datum in data_x:
# 	acc_x.append(datum - x_offset_0)
# for datum in data_y:
# 	acc_y.append(datum - y_offset_0)
# for datum in data_z:
# 	acc_z.append(datum - z_offset_0)

if DEBUG:
	print(acc_x)
	print(acc_y)
	print(acc_z)


# Output acceleration data
data_handle = open(DATA_ACC_FILENAME, 'w')

for i in range(len(acc_x)):
	data_handle.write(str(acc_x[i]) + ' ' +
			  str(acc_y[i]) + ' ' +
			  str(acc_z[i]) + '\n')

data_handle.close()

# Integrate acceleration into velocity
vel_x = [0] * len(acc_x)  # Stores velocity data
vel_y = [0] * len(acc_y)  #
vel_z = [0] * len(acc_z)  #
for i in range(1, len(acc_x)):
	vel_x[i] = vel_x[i-1] - acc_x[i]
	vel_y[i] = vel_y[i-1] - acc_y[i]
	vel_z[i] = vel_z[i-1] - acc_z[i]

if DEBUG:
	print(vel_x)
	print(vel_y)
	print(vel_z)

# Output velocity data
data_handle = open(DATA_VEL_FILENAME, 'w')

for i in range(len(vel_x)):
	data_handle.write(str(vel_x[i]) + ' ' +
			  str(vel_y[i]) + ' ' +
			  str(vel_z[i]) + '\n')

data_handle.close()

# Integrate velocity into distance
pos_x = [0] * len(vel_x)
pos_y = [0] * len(vel_y)
pos_z = [0] * len(vel_z)
for i in range(1, len(vel_x)):
	pos_x[i] = pos_x[i-1] - vel_x[i]
	pos_y[i] = pos_y[i-1] - vel_y[i]
	pos_z[i] = pos_z[i-1] - vel_z[i]

if DEBUG:
	print(pos_x)
	print(pos_y)
	print(pos_z)

# Output distance data
data_handle = open(DATA_POS_FILENAME, 'w')

for i in range(len(pos_x)):
	data_handle.write(str(pos_x[i]) + ' ' +
			  str(pos_y[i]) + ' ' +
			  str(pos_z[i]) + '\n')

data_handle.close()

