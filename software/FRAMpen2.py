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
DATA_POS_ROT_FILENAME = 'pos_rot.dat'

DEBUG = False

AVERAGES_X = 30  # Number of samples for moving average
AVERAGES_Y = 30  #
AVERAGES_Z = 30  #

DECAY_VALUE_X = 0.00  # Intensity of decay
DECAY_VALUE_Y = 0.00  #
DECAY_VALUE_Z = 0.00  #

DISCRIMINATOR_X = 05.0  # Acceleration below the value
DISCRIMINATOR_Y = 05.0  # will be disregarded
DISCRIMINATOR_Z = 05.0  #

ZERO_X = 25  # Zero the velocity auf n sample with no acceleration
ZERO_Y = 25  #
ZERO_Z = 25  #

ROTATE = 0  # Rotate the x-, y-coordinates afterwards

##############################################
# No configuration needed beyond this point. #
##############################################

import serial  # PySerial
from math import cos, sin, pi  # Cosinus, sinus and PI for rotation
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
# raw_data = ['0']  # Disable receive for testing
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
#     # Shift 'high byte' back by multiply with 8
#     data_16.append(data_8[i] * 256 + data_8[i+1])
#     i += 2

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
# data_x = sample3_raw_x
# data_y = sample3_raw_y
# data_z = sample3_raw_z

# Output raw data
data_handle = open(DATA_RAW_FILENAME, 'w')

for i in range(len(data_x)):
    data_handle.write(str(data_x[i]) + ' ' +
                      str(data_y[i]) + ' ' +
                      str(data_z[i]) + '\n')

data_handle.close()


### Integrate acceleration ###################################################

acc_x = []  # Stores acceleration data without offset
acc_y = []  #
acc_z = []  #

# Moving averages
average_values_x = [data_x[0]] * AVERAGES_X
average_values_y = [data_y[0]] * AVERAGES_Y
average_values_z = [data_z[0]] * AVERAGES_Z

# Decay values
decay_x = 0.0
decay_y = 0.0
decay_z = 0.0

vel_x = [0] * len(data_x)  # Stores velocity data
vel_y = [0] * len(data_y)  #
vel_z = [0] * len(data_z)  #

pos_x = [0] * len(vel_x)  # Stores positional (distance) data
pos_y = [0] * len(vel_y)  #
pos_z = [0] * len(vel_z)  #

zero_counter_x = 0  # How many sequential acceleration samples are zero
zero_counter_y = 0  #
zero_counter_z = 0  #

# File handels
data_acc_handle = open(DATA_ACC_FILENAME, 'w')
data_vel_handle = open(DATA_VEL_FILENAME, 'w')
data_pos_handle = open(DATA_POS_FILENAME, 'w')


for i in range(len(data_x)):
    # Remove offset

    # right ship 'average_values', insert current sample at first position
    average_values_x = [data_x[i]] + average_values_x[:-1]
    average_values_y = [data_y[i]] + average_values_y[:-1]
    average_values_z = [data_z[i]] + average_values_z[:-1]

    tmp_x = data_x[i] - sum(average_values_x) / float(AVERAGES_X)
    tmp_y = data_y[i] - sum(average_values_y) / float(AVERAGES_Y)
    tmp_z = data_z[i] - sum(average_values_z) / float(AVERAGES_Z)

    # Filter small values (discriminator)
    if (abs(tmp_x) > DISCRIMINATOR_X):
        acc_x.append(data_x[i] - sum(average_values_x) / float(AVERAGES_X) - decay_x)
    else:
        acc_x.append(0)

    if (abs(tmp_y) > DISCRIMINATOR_Y):
        acc_y.append(data_y[i] - sum(average_values_y) / float(AVERAGES_Y) - decay_y)
    else:
        acc_y.append(0)

    if (abs(tmp_z) > DISCRIMINATOR_Z):
        acc_z.append(data_z[i] - sum(average_values_z) / float(AVERAGES_Z) - decay_z)
    else:
        acc_z.append(0)

    # Count how many sequential samples are zero
    if (acc_x[i] == 0):
        zero_counter_x += 1
    else:
        zero_counter_x = 0

    if (acc_y[i] == 0):
        zero_counter_y += 1
    else:
        zero_counter_y = 0

    if (acc_z[i] == 0):
        zero_counter_z += 1
    else:
        zero_counter_z = 0

    if DEBUG:
	print(acc_x)
	print(acc_y)
	print(acc_z)

    # Output acceleration data
    data_acc_handle.write(str(acc_x[i]) + ' ' +
                          str(acc_y[i]) + ' ' +
                          str(acc_z[i]) + '\n')

    # Integrate acceleration into velocity
    # vel_x[i] = vel_x[i-1] - acc_x[i]
    # vel_y[i] = vel_y[i-1] - acc_y[i]
    # vel_z[i] = vel_z[i-1] - acc_z[i]
    if (zero_counter_x >= ZERO_X):
        vel_x[i] = 0
        vel_x[i-1] = 0
    else:
        vel_x[i] = vel_x[i-1] + acc_x[i-1] + ((acc_x[i] - acc_x[i-1]) / 2)

    if (zero_counter_y >= ZERO_Y):
        vel_y[i] = 0
        vel_y[i-1] = 0
    else:
        vel_y[i] = vel_y[i-1] + acc_y[i-1] + ((acc_y[i] - acc_y[i-1]) / 2)

    if (zero_counter_z >= ZERO_Z):
        vel_z[i] = 0
        vel_z[i-1] = 0
    else:
        vel_z[i] = vel_z[i-1] + acc_z[i-1] + ((acc_z[i] - acc_z[i-1]) / 2)

    if DEBUG:
	print(vel_x)
	print(vel_y)
	print(vel_z)

    # Output velocity data
    data_vel_handle.write(str(vel_x[i]) + ' ' +
			  str(vel_y[i]) + ' ' +
			  str(vel_z[i]) + '\n')

    # Decay
    if (vel_x[i] > 0):
        decay_x -= DECAY_VALUE_X
    else:
        decay_x += DECAY_VALUE_X

    if (vel_y[i] > 0):
        decay_y -= DECAY_VALUE_Y
    else:
        decay_y += DECAY_VALUE_Y

    if (vel_z[i] > 0):
        decay_z -= DECAY_VALUE_Z
    else:
        decay_z += DECAY_VALUE_Z


    # Integrate velocity into distance
    # pos_x[i] = pos_x[i-1] - vel_x[i]
    # pos_y[i] = pos_y[i-1] - vel_y[i]
    # pos_z[i] = pos_z[i-1] - vel_z[i]
    pos_x[i] = pos_x[i-1] + vel_x[i-1] + ((vel_x[i] - vel_x[i-1]) / 2)
    pos_y[i] = pos_y[i-1] + vel_y[i-1] + ((vel_y[i] - vel_y[i-1]) / 2)
    pos_z[i] = pos_z[i-1] + vel_z[i-1] + ((vel_z[i] - vel_z[i-1]) / 2)

    if DEBUG:
	print(pos_x)
	print(pos_y)
	print(pos_z)

    # Output distance data
    data_pos_handle.write(str(pos_x[i]) + ' ' +
                          str(pos_y[i]) + ' ' +
                          str(pos_z[i]) + '\n')


# Close file handles
data_acc_handle.close()
data_vel_handle.close()
data_pos_handle.close()

# Perform rotation
data_handle = open(DATA_POS_ROT_FILENAME, 'w')

angle = ROTATE / 360.0 * 2 * pi
for i in range(len(pos_x)):
    x_tmp = pos_x[i] * cos(angle) + pos_y[i] * sin(angle)
    y_tmp = -pos_x[i] * sin(angle) + pos_y[i] * cos(angle)
    data_handle.write(str(x_tmp) + ' ' +
                      str(y_tmp) + ' ' +
                      str(data_z[i]) + '\n')
data_handle.close()
