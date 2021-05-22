from cv2 import VideoCapture, CAP_DSHOW
from numpy import zeros, uint8

class VideoCaptureWrapper(object):
    def __init__(self, *args, **kwargs):
        self.Cam = VideoCapture(*args, **kwargs)
    def __enter__(self):
        return self
    def __exit__(self, *args):
        self.Cam.release()

def Gauge_Picture():
    with VideoCaptureWrapper(1, CAP_DSHOW) as Gauge_camera:
        _, G_Img = Gauge_camera.Cam.read()
    G_Array = zeros(G_Img.shape, dtype=uint8) #Create an array template
    G_Array = G_Img.copy() #Fill array template with gauge camera rgb values
    print(G_Array)
    return G_Array

if __name__ == "__main__":
    Gauge_Picture()

