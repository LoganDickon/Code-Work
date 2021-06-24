# Creating a LOGIN page

from kivymd.app import MDApp
from kivy.lang import Builder
from kivy.uix.screenmanager import Screen, ScreenManager
from kivy.core.window import Window
from kivy.properties import ObjectProperty
from kivymd.uix.button import MDFlatButton
from kivymd.uix.dialog import MDDialog
from win32api import GetSystemMetrics
import logindb

class WindowManager(ScreenManager):
    pass

class Log_in(Screen):
        
    def login(self):
        logindb.show_all()
    
class Sign_up(Screen):
    pass

class BuildApp(MDApp):

    def Fullscreen(self):
        if Window.fullscreen == False:
            Window.size = (GetSystemMetrics(0), GetSystemMetrics(1))
            Window.fullscreen = True

    def build(self):
        self.title = "OMNICheck"
        self.Fullscreen()
        return Builder.load_file('Omni.kv')

    dialog = None
    def show_alert_dialog(self):
        if not self.dialog:
            self.dialog = MDDialog(
                title="Close Program?",
                text="WARNING! You will lose all unsaved progress!",
                buttons=[
                    MDFlatButton(
                        text="CANCEL", text_color=self.theme_cls.primary_color, on_release=self.close_dialog
                    ),
                    MDFlatButton(
                        text="EXIT", text_color=self.theme_cls.primary_color, on_release=self.close_app
                    ),
                ],
            )
        self.dialog.open()
    
    def close_dialog(self, obj):
        self.dialog.dismiss()
    
    def close_app(self, obj):
        exit()

if __name__ == "__main__":
    BuildApp().run()