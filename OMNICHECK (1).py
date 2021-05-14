"""

Created on  5/1/2021
Last edited: 5/12/2021

Author: Logan Dickon, ATORLABS

"""
from threading import Thread #Library for assigning code to different cpu threads to run in parallel rather than sequentially
import os #Library for editing system files directly
import cv2 #Library for image & video processing
import numpy as np

def _HudCam_(): #Define Hud Camera Function

        #Call Hud camera resource
        hud_Cam = cv2.VideoCapture(0, cv2.CAP_DSHOW) 

        #If Hud camera is not found
        if not hud_Cam.isOpened():
                #Log error to console
                raise RuntimeError('Error opening Hud Camera.')
        
        #Set resolution of camera to 720p
        hud_Cam.set(3, 1280)
        hud_Cam.set(4, 720)

        #Read/Grab Hud camera frame
        rval, hud_Img = hud_Cam.read()
        snapshot = np.zeros(hud_Img.shape, dtype=np.uint8)
        print(snapshot)
        #Initialize file name
        hud_name = ('C:/Users/Betty/Downloads/HudPicture-0.jpg') 

        #Write Hud camera frame to file name
        cv2.imwrite(hud_name, hud_Img) 

        #Initialize rename hud jpg variable
        old_file1 = os.path.join("C:/Users/Betty/Downloads", "HudPicture-0.jpg") 

        #Initialize rename hud jpg variable
        new_file1 = os.path.join("C:/Users/Betty/Downloads", "HudPicture0.jpg") 

        #If a file exists under the renamed files path
        if (os.path.exists('C:/Users/Betty/Downloads/HudPicture0.jpg')):

                #Delete the file in the path
                os.remove('C:/Users/Betty/Downloads/HudPicture0.jpg')

        #Rename file so labview knows its not being written to
        os.rename(old_file1, new_file1) 

        #Log to console
        print("Hud Picture written!") 

        #Release camera resource to clear memory
        hud_Cam.release() 

def _GaugeCam_(): #Define Gauge Camera Function

        #Call Gauge camera resource
        gauge_Cam = cv2.VideoCapture(1, cv2.CAP_DSHOW) 

        #If gauge camera is not found
        if not gauge_Cam.isOpened():
                #Log error to console
                raise RuntimeError('Error opening Gauge Camera.')
        
        #Set resolution of camera to 720p
        gauge_Cam.set(3, 1280) 
        gauge_Cam.set(4, 720)

        #Read/Grab Gauge camera frame
        rval, gauge_Img = gauge_Cam.read() 

        #Initialize file name
        gauge_name = ('C:/Users/Betty/Downloads/GaugePicture-0.jpg') 

        #Write Gauge camera frame to file name
        cv2.imwrite(gauge_name, gauge_Img) 

        #Initialize rename gauge .jpg variable
        old_file2 = os.path.join("C:/Users/Betty/Downloads", "GaugePicture-0.jpg") 

        #Initialize rename gauge .jpg variable
        new_file2 = os.path.join("C:/Users/Betty/Downloads", "GaugePicture0.jpg") 

        #If a file exists under the renamed files path
        if (os.path.exists('C:/Users/Betty/Downloads/GaugePicture0.jpg')): 

                #Delete the file in the path
                os.remove('C:/Users/Betty/Downloads/GaugePicture0.jpg')

        #Rename file so labview knows its not being written to
        os.rename(old_file2, new_file2) 

        #Log to console
        print("Gauge Picture written!") 

        #Release camera resource to clear memory
        gauge_Cam.release() 

def main():

        #Assigns hud camera function to its own cpu thread
        t1= Thread(target = _HudCam_).start()
        #Assigns guage camera function to its own cpu thread
        t2= Thread(target = _GaugeCam_).start()
        #Starts hud function in its own cpu thread
        #t1.start()
        #Starts gauge function in its own cpu thread
        #t2.start()
        #Rejoin threads to run sequentially
        t1.join()
        t2.join()

#https://github.com/nrsyed/computer-vision/blob/master/get_video_pixel/get_video_pixel.py