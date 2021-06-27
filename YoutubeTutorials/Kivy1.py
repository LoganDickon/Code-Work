from kivymd.app import MDApp
from kivy.lang import Builder
from kivy.uix.screenmanager import Screen, ScreenManager

class WindowManager(ScreenManager):
	pass

class AppScreen1(Screen):
	pass

class BuildApp(MDApp):
	
	def build(self):
		self.title = "Kivy Tutorial 1"
		return Builder.load_file('KivyTutorial1.kv')

if __name__ == "__main__":
	BuildApp().run()
