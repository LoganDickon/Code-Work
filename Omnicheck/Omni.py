# Creating a LOGIN page

# Creating the App

from kivymd.app import MDApp
from kivy.lang import Builder
from kivy.uix.screenmanager import Screen, ScreenManager
from kivy.core.window import Window
from kivy.properties import ObjectProperty

class FirstScreen(Screen):
    pass

class SecondScreen(Screen):
    pass

class WindowManager(ScreenManager):
    pass

class Myapp(MDApp):
    
    def build(self):
        self.title = "OMNICheck"
        return Builder.load_file('Omni.kv')

if __name__ == "__main__":
    Myapp().run()