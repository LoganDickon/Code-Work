#===============================================================================================================================#

print('-------------------------------------------------')
print("Modified by Logan D on: 5/14/2021\n")

#===============================================================================================================================#

from timeit import default_timer #Library for runtime Timer
start = default_timer() #Start runtime Timer
from numpy import zeros, uint8 #Library for creating and manipulating arrays
from cv2 import VideoCapture, CAP_DSHOW #Library for webcam functionality
from threading import Thread #Library that allows multithreading

#===============================================================================================================================#

W, H = 640, 480 #Resolution of both camera's
cam_Num = 0 #Gauge camera's camera number
cam_Num_2 = 1 #Gauge camera's camera number

#===============================================================================================================================#

def HudCam(W, H, cam_Num): #Function to create an array from hud cam
    hud_Cam = VideoCapture(cam_Num, CAP_DSHOW) #Call hud cam
    #If Hud camera is not found
    if not hud_Cam.isOpened(): #If hud camera resource is not found
        raise RuntimeError('Error opening Hud Camera. Check USB port!') #Log error to console
    #Set resolution of camera to 720p
    #hud_Cam.set(3, W) #Set aspect ratio and resolution of x-Coordinate
    #hud_Cam.set(4, H) #Set aspect ratio and resolution of y-Coordinate
    #Read/Grab Hud camera frame
    rval, hud_Img = hud_Cam.read() #Read gauge camera frame
    hud_Cam.release() #Release gauge camera resource
    hud_Array = zeros(hud_Img.shape, dtype=uint8) #Create an array template
    hud_Array = hud_Img.copy() #Fill array template with gauge camera rgb values
    print("Hud Camera\'s resolution is:", hud_Array.shape) #Log resolution to console 
    #print(hud_Array) #Log hud camera's array to console
    
#===============================================================================================================================#

def GaugeCam(W, H, cam_Num_2): #Function to create an array from gauge cam
    gauge_Cam = VideoCapture(cam_Num_2, CAP_DSHOW) #Open gauge camera
    if not gauge_Cam.isOpened(): #If gauge camera is not found
        raise RuntimeError('Error opening Gauge Camera. Check USB port!') #Log error to console
    #Set resolution of camera to 720p
    #gauge_Cam.set(3, W) #Set aspect ratio and resolution of x-Coordinate
    #gauge_Cam.set(4, H) #Set aspect ratio and resolution of y-Coordinate
    rval, gauge_Img = gauge_Cam.read() #Read gauge camera frame
    gauge_Cam.release() #Release gauge camera resource
    gauge_Array = zeros(gauge_Img.shape, dtype=uint8) #Create an array template
    gauge_Array = gauge_Img.copy() #Fill array template with gauge camera rgb values
    print('-------------------------------------------------')
    print("Gauge Camera\'s resolution is:", gauge_Array.shape) #Log resolution to console
    print('-------------------------------------------------')
    print()
    
#===============================================================================================================================#

def main(start): #Function that runs both camera's
    hud_Thread = Thread(target = HudCam, args = (W, H, cam_Num)) #Assigns HudCam to run on it's own thread
    gauge_Thread = Thread(target = GaugeCam, args = (W, H, cam_Num_2)) #Assigns GaugeCam to run on it's own thread
    hud_Thread.start() #Starts running HudCam function in it's specified cpu thread
    gauge_Thread.start() #Starts running GaugeCam function in it's specified cpu thread
    hud_Thread.join() #When HudCam is finished release it's specified thread
    gauge_Thread.join() #When GaugeCam is finished release it's specified thread
    
#===============================================================================================================================#

main(start)
stop = default_timer() #Stop runtime Timer
print('-------------------------------------------------')
print('Time:', "{:.2f}".format(stop - start))
print('-------------------------------------------------')
    
#===============================================================================================================================#
