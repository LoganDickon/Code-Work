### Kivy Tutorial 8 ###

### Imports ###
from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
from kivy.lang import Builder

### Call Kivy File ###

Builder.load_file('KivyTutorial8.kv')

### Grid Class ###
class MyBoxLayout(Widget):
    pass

### Main Application Class ###
class MyApp(App):
    def build(self):
        return MyBoxLayout()

if __name__ == "__main__":
    MyApp().run()