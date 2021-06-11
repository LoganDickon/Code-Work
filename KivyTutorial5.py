### Kivy Tutorial 5 ###

### Imports ###
from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.gridlayout import GridLayout
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty


### Grid Class ###
class MyGridLayout(Widget):

    ### Assign Variables From .kv File
    name = ObjectProperty(None)
    pizza = ObjectProperty(None)
    color = ObjectProperty(None)

    ### Button Functionality ###
    def press(self):
        ### Initialize Local Variables ###
        name = self.name.text
        pizza = self.pizza.text
        color = self.color.text
        ### Add Label Widget ###
        #self.add_widget(Label(text=f'Hello {name}, you like {pizza} pizza, and your favorite color is {color}!'))
        print(f'Hello {name}, you like {pizza} pizza, and your favorite color is {color}!')
        ### Clear Input Boxes ###
        self.name.text = ""
        self.pizza.text = ""
        self.color.text = ""


### Main Application Class ###
class KivyTutorial5(App):
    def build(self):
        return MyGridLayout()


if __name__ == "__main__":
    KivyTutorial5().run()