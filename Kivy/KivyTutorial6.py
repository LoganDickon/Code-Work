### Kivy Tutorial 6 ###

### Imports ###
from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
from kivy.lang import Builder

### Call Kivy File ###

Builder.load_file('KivyTutorial6.kv')
# or Builder.load_string("""      """)

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
class MyApp(App):
    def build(self):
        return MyGridLayout()


if __name__ == "__main__":
    MyApp().run()