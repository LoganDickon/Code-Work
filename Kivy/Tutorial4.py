### Kivy Tutorial 4 ###

### Imports ###
from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.gridlayout import GridLayout
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button


### Grid Class ###
class MyGridLayout(GridLayout):

    ### Initialize Infinite Keywords ###

    def __init__(self, **kwargs):
        ### Call GridLayout Constructor ###
        super(MyGridLayout, self).__init__(**kwargs)
        ### Number of Columns in main-grid ###
        self.cols = 1
        self.row_force_default = True
        self.row_default_height = 85
        self.col_force_default = True
        self.col_default_width = 100

        ### Create Widgets Functions ###
        def sub_grid():

            ### Create Sub-Grid ###
            self.top_grid = GridLayout(row_force_default=True,
                                       row_default_height=40,
                                       col_force_default=True,
                                       col_default_width=100)
            ### Number of Columns in sub-grid ###
            self.top_grid.cols = 2

        def name_label():
            ### Add Text Widgets ###
            self.top_grid.add_widget(Label(text="Name: "))
            ### Add Input Widget ###
            self.name = TextInput(multiline=True)
            self.top_grid.add_widget(self.name)

        def pizza_label():
            ### Add Text Widgets ###
            self.top_grid.add_widget(Label(text="Favorite Pizza: "))
            ### Add Input Widget ###
            self.pizza = TextInput(multiline=False)
            self.top_grid.add_widget(self.pizza)

        def color_label():
            ### Add Text Widgets ###
            self.top_grid.add_widget(Label(text="Favorite Color: "))
            ### Add Input Widget ###
            self.color = TextInput(multiline=False)
            self.top_grid.add_widget(self.color)

        ### Call Widgets ###
        sub_grid()
        name_label()
        pizza_label()
        color_label()

        ### Add top_grid Widget ###
        self.add_widget(self.top_grid)

        ### Create Submit Widget ###
        self.submit = Button(text="Submit", font_size=32, size_hint_y=None, height=50, size_hint_x=None, width=200)
        ### Add Button Functionality ###
        self.submit.bind(on_press=self.button_press)
        ### Add Submit Widget ###
        self.add_widget(self.submit)

    ### Button Functionality ###
    def button_press(self, instance):
        ### Initialize Local Variables ###
        name = self.name.text
        pizza = self.pizza.text
        color = self.color.text
        ### Add Label Widget ###
        self.add_widget(Label(text=f'Hello {name}, you like {pizza} pizza, and your favorite color is {color}!'))
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
