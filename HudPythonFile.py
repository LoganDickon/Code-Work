from cv2 import VideoCapture, CAP_DSHOW
from numpy import zeros, uint8

class VideoCaptureWrapper(object):
    def __init__(self, *args, **kwargs):
        self.Cam = VideoCapture(*args, **kwargs)
    def __enter__(self):
        return self
    def __exit__(self, *args):
        self.Cam.release()

def Hud_Picture():
    with VideoCaptureWrapper(0, CAP_DSHOW) as Hud_camera:
        _, H_Img = Hud_camera.Cam.read()
    H_Array = zeros(H_Img.shape, dtype=uint8) #Create an array template
    H_Array = H_Img.copy() #Fill array template with gauge camera rgb values
    print(H_Array)
    return H_Array

if __name__ == "__main__":
    Hud_Picture()

