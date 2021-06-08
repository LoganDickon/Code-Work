### Kivy Tutorial 4 ###

### Imports ###
from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.gridlayout import GridLayout
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button

class MyGridLayout(GridLayout):
    # Initialize infinite keywords
    def __init__(self, **kwargs):
        # Call GridLayout Constructor
        super(MyGridLayout, self).__init__(**kwargs)
        # Set Columns
        self.cols = 1
        
        # Create Sub-Grid
        self.top_grid = GridLayout()
        self.top_grid.cols = 2
        
        # Add Text Widgets
        self.top_grid.add_widget(Label(text="Name: ", size_hint_y = None, height=50, size_hint_x = None, width=200))
        # Add Input Widget
        self.name = TextInput(multiline=True, size_hint_y = None, height=50, size_hint_x = None, width=200)
        self.top_grid.add_widget(self.name)
    
        # Add Text Widgets
        self.top_grid.add_widget(Label(text="Favorite Pizza: "))
        # Add Input Widget
        self.pizza = TextInput(multiline=False)
        self.top_grid.add_widget(self.pizza)
    
        # Add Text Widgets
        self.top_grid.add_widget(Label(text="Favorite Color: "))
        # Add Input Widget
        self.color = TextInput(multiline=False)
        self.top_grid.add_widget(self.color)
    
        # Add top_grid Widget
        self.add_widget(self.top_grid)
    
        # Create Submit Widget
        self.submit = Button(text="Submit", font_size=32, size_hint_y = None, height=50, size_hint_x = None, width=200)
        # Add Button Functionality
        self.submit.bind(on_press=self.button_Press)
        # Add Sumbit Widget 
        self.add_widget(self.submit)
        
        
        
    def button_Press(self, instance):
        # Initialize Local Variables
        name = self.name.text
        pizza = self.pizza.text
        color = self.color.text
        # Add Label Widget
        self.add_widget(Label(text=f'Hello {name}, you like {pizza} pizza, and your favorite color is {color}!'))
        # Clear Input Boxes
        self.name.text = ""
        self.pizza.text = ""
        self.color.text = ""
        
class MyApp(App):
    def build(self):
        return MyGridLayout()

if __name__ == "__main__":
    MyApp().run()