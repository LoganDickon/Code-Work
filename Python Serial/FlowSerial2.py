import time
import serial



# Opening the COM2 for Serial communication
def OpenPort():
    global ser
    COMPORT = 'COM2'

    try:
        ser = serial.Serial(
            port=COMPORT, baudrate = 38400,
            parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS, timeout=1
            )
        
        if (ser.isOpen() == True):
            print('DATA CONNECTION ESTABLISHED: ' + COMPORT)

    except:
        print("DATA CONNECTION NOT-ESTABLISHED: " + COMPORT)
        print("Check Connections & Try Again")

def ReadPort():
    ser.write(b'?\r\n')
    ser.write(b'SUS\r\n')
    ser.write(b'SG0\r\n')
    ser.write(b'SSR0010\r\n')
    ser.write(b'DAFxx0001\r\n')
    output = ser.read(50)
    print(output)

if __name__=="__main__":
    OpenPort()
    ReadPort()