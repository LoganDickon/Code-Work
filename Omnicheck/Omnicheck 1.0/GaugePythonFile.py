# Import Opencv and Numpy
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
    print(G_Img)
    return G_Img

if __name__ == "__main__":
    Gauge_Picture()

