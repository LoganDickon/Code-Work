from tkinter import *

root = Tk()

def thing():
	my_label.config(text="You Clicked!")

login_btn = PhotoImage(file='C:/Omni_Digital/yo.png')

but = Button(root, image=login_btn, command=thing, borderwidth=0)
but.pack(pady=20)

my_label = Label(root, text='')
my_label.pack(pady=20)

mainloop()