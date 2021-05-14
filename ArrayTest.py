"""
Created on Wed May 12 08:33:18 2021

@author: Logan D
"""

import numpy as np 
import cv2
from threading import Thread
import cProfile, pstats, io

def Profile(fnc):
    ###A decorator that uses cProfile to profile a function###
    def inner(*args, **kwargs):
        pr = cProfile.Profile()
        pr.enable()
        retval = fnc(*args, **kwargs)
        pr.disable()
        s = io.StringIO()
        sortby = 'cumulative'
        ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
        ps.print_stats()
        print(s.getvalue())
        return retval
    return inner

def HudCam():
    cam_Num = 0
    hud_Cam = cv2.VideoCapture(cam_Num, cv2.CAP_DSHOW) 
    #If Hud camera is not found
    if not hud_Cam.isOpened():
        #Log error to console
        raise RuntimeError('Error opening Hud Camera. Check USB port!')
    #Set resolution of camera to 720p
    W, H = 1280, 720
    hud_Cam.set(cv2.CAP_PROP_FRAME_WIDTH, W)
    hud_Cam.set(cv2.CAP_PROP_FRAME_HEIGHT, H)
    #Read/Grab Hud camera frame
    rval, hud_Img = hud_Cam.read()
    hud_Cam.release()
    hud_Array = np.zeros(hud_Img.shape, dtype=np.uint8)
    hud_Array = hud_Img.copy()
    print(type(hud_Array))
    print(hud_Array.shape)
    print(hud_Array)

def GaugeCam():
    cam_Num = 1
    hud_Cam = cv2.VideoCapture(cam_Num, cv2.CAP_DSHOW) 
    #If Hud camera is not found
    if not hud_Cam.isOpened():
        #Log error to console
        raise RuntimeError('Error opening Hud Camera. Check USB port!')
    #Set resolution of camera to 720p
    W, H = 1280, 720
    hud_Cam.set(cv2.CAP_PROP_FRAME_WIDTH, W)
    hud_Cam.set(cv2.CAP_PROP_FRAME_HEIGHT, H)
    #Read/Grab Hud camera frame
    rval, hud_Img = hud_Cam.read()
    hud_Cam.release()
    hud_Array = np.zeros(hud_Img.shape, dtype=np.uint8)
    hud_Array = hud_Img.copy()
    print(type(hud_Array))
    print(hud_Array.shape)
    print(hud_Array)


def main():
    t1 = Thread(target = HudCam)
    t2 = Thread(target = GaugeCam)
    t1.start()
    t2.start()
    t1.join()
    t2.join()
main()