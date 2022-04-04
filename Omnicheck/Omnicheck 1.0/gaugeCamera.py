# Import Opencv
from cv2 import VideoCapture, CAP_DSHOW 

class VideoCaptureWrapper(object):
    def __init__(self, *args, **kwargs):
        self.Cam = VideoCapture(*args, **kwargs)
    def __enter__(self):
        return self
    def __exit__(self, *args):
        self.Cam.release() #Release camera resource

def Gauge_Picture():
    with VideoCaptureWrapper(1, CAP_DSHOW) as Gauge_camera: #Context Manager to control the VideoCapture method
        _, G_Img = Gauge_camera.Cam.read() #Read Camera Frame
    #print(G_Img) #Log to console
    return G_Img #Return Array so LabVIEW can grab it

if __name__ == "__main__": #Run if this is main .py file
    Gauge_Picture() #Call camera function