from cv2 import VideoCapture, CAP_DSHOW

class VideoCaptureWrapper(object):
    def __init__(self, *args, **kwargs):
        self.Cam = VideoCapture(*args, **kwargs)
    def __enter__(self):
        return self
    def __exit__(self, *args):
        self.Cam.release() #Release camera resource

def Hud_Picture():
    with VideoCaptureWrapper(0, CAP_DSHOW) as Hud_camera: #Context Manager to control the VideoCapture method
        _, H_Img = Hud_camera.Cam.read() #Read Camera Frame
    H_Array = H_Img.copy() #Fill array template with gauge camera rgb values
    print(H_Array) #Log to console
    return H_Array #Return Array so LabVIEW can grab it

if __name__ == "__main__": #Run if this is main .py file
    Hud_Picture() #Call camera function

