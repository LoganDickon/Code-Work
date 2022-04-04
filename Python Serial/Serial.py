import serial
import time
ser=serial.Serial(port='COM2',baudrate=9600)
ser.open()
while True:
    print("try")
    time.sleep(10)
    s=ser.read(100) #reading up to 100 bytes
    print(s)
ser.close()