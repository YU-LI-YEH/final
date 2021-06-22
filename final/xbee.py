import serial
import time

serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)

print("start")
while True:

    message = s.read(4)
    print(message.decode())
    
s.close()
