import serial
import time

index = 0
total = 0
i = 0
avg = 0

# Serial Connection
time.sleep(.5)
ser = serial.Serial(
        port="COM2", baudrate = 115200,
        parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS, timeout=1)

# Write ASCII Commands To TSI 5300 Flow Sensor
ser.write(b'?\r\n') # Ask Sensor if it is getting a signal (Returns "OK")
ser.write(b'SUS\r\n') # Set Flow type to SLPM (Returns "OK")
ser.write(b'SG0\r\n') # Set Flow Gas to Air (Returns "OK")
ser.write(b'SSR0005\r\n') # Set Sample Rate to 5ms (Returns "OK")
ser.write(b'LPZ\r\n') # Zero Low Pressure Sensor

# Read serial output to remove all 'OK's from buffer
while (i <= 4):
    ser.readline() # Read one line of serial and discard it
    i += 1

# Loop infinite flow readings until Flow is <= to 85 LPM
while (avg <= 85):
    index = 0
    total = 0

    ser.write(b'DAFxxxxx0005\r\n') # Read 5 sensor Flow Reading
    ser.readline() # Read one line of serial data and discard it
    byte = ser.readline() # Read one line of serial data and store it
    string = byte.decode('utf-8') # Convert from BYTE to STRING
    array = string.split(',') # Convert from STRING to STRING ARRAY
    
    # Convert each element of the ARRAY to FLOAT then add them together
    for data in array:
        index += 1 
        data = float(data)
        total += data

    avg = total / index # Find the average Flow in LPM
    print(avg)
    time.sleep(1)

ser.close()