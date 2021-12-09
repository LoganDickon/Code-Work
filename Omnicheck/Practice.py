###################################
########### Libraries #############
###################################
								###
from tkinter import *			###
from tkinter.ttk import *		###
from tkinter import messagebox	###
import sqlite3 as sql			###
from PIL import ImageTk, Image	###
								###
###################################
###################################

###############
# QUIT METHOD #
###############

def quit():
	yes_no = messagebox.askyesno("Quit?", "Are you sure?")
	
	if (yes_no == True):
		root.quit()

########################
# SELECT_SCREEN METHOD #
########################

def select_page():
	# Remove Previous Screen
	hide_screens()
	select_screen_Frame.pack()

#######################
# HIDE_SCREENS METHOD #
#######################

def hide_screens():
	# Destroy Children of Each Frame
	for widget in login_screen_Frame.winfo_children():
		widget.destroy()
	for widget in signup_screen_Frame.winfo_children():
		widget.destroy()

	login_screen_Frame.pack_forget()
	signup_screen_Frame

#######################
# RESET_SIGNIN METHOD #
#######################

def reset_sigin():
	signup_fname_Entry.delete(0, END)
	signup_lname_Entry.delete(0, END)
	signup_user_Entry.delete(0, END)
	signup_pass_Entry.delete(0, END)
	signup_admin_Entry.delete(0, END)
	signup_repass_Entry.delete(0, END)

#################
# SIGNUP METHOD #
#################

def signup():
	# Read Entry Box Data
	fname = signup_fname_Entry.get()
	lname = signup_lname_Entry.get()
	user = signup_user_Entry.get()
	password = signup_pass_Entry.get()
	repass = signup_repass_Entry.get()


	# Connect To Login Database
	db = sql.connect('login.db')
	db_C = db.cursor()
	exit = False
	falpha = fname.isalpha()
	lalpha = lname.isalpha()
	db_C.execute("SELECT oid, username, password FROM accounts WHERE username = ? AND password = ?", [user, password])

	if (len(fname) > 20):
		print("fname: " + fname)
		exit = True
		messagebox.showerror("Try Again!", "First name must be less than 20 characters")
		signup_fname_Entry.delete(0, END)

	if (len(lname) > 20):
		print("lname: " + lname)
		exit = True
		messagebox.showwarning("Try Again!", "Last Name must be less than 20 characters")
		signup_lname_Entry.delete(0, END)

	if (falpha == False):
		print(falpha)
		exit = True
		messagebox.showwarning("Try Again!", "First name must only be letters")
		signup_fname_Entry.delete(0, END)

	if (lalpha == False):
		print(lalpha)
		exit = True
		messagebox.showwarning("Try Again!", "Last name must only be letters")
		signup_lname_Entry.delete(0, END)

	if len(user) <= 6:
		print("user: " + user)
		exit = True
		messagebox.showwarning("Try Again!", "Username must have at least 6 characters")
		signup_user_Entry.delete(0, END)
	
	if len(password) <= 8:
		print("pass: " + password)
		exit = True
		messagebox.showwarning("Try Again!", "Password must have at least 8 characters")
		signup_pass_Entry.delete(0, END)

	if (password != repass):
		exit = True
		print("Passwords don't match")
		messagebox.showwarning("Try Again!", "Passwords don\'t match")
		signup_repass_Entry.delete(0, END)

	if (db_C.fetchone()):
		exit = True
		print("found")
		messagebox.showwarning("Try Again!", "Account exists for the username already")

	if (exit == True):
		db.close()
		return

	# Insert and query
	db_C.execute("INSERT INTO accounts(username, password, fname, lname) VALUES(?,?,?,?)", [user, password, fname, lname])
	db_C.execute("SELECT oid, username FROM accounts WHERE username = ?", [user])

	row = db_C.fetchall()
	if row:
		print("New Login Created")
		print(row)
		reset_sigin()
		db.commit()
		db.close()

######################
# SIGNUP_PAGE METHOD #
######################

def signup_page():
	# Remove Previous Screen
	hide_screens()
	signup_screen_Frame.pack()

	# Draw main signup frame
	signup_Frame = Frame(signup_screen_Frame, width=(width*.45), height=(height*.8), style='login.TFrame')
	signup_Frame.place(x=(width*.285), y=(height*.1))

	# Draw "create new" label
	signup_Label = Label(signup_screen_Frame, text='Create New', font=('Arial', 32), style='login.TLabel')
	signup_Label.place(x=(width*.450), y=(height*.15))

	# Draw gray back frame for depth
	back_Frame = Frame(signup_screen_Frame, width=(width*.4), height=(height*.58), style='back.TFrame')
	back_Frame.place(x=(width*.31), y=(height*.25))

	# Draw first name entry box
	global signup_fname_Entry
	signup_fname_Entry = Entry(signup_screen_Frame, font=('arial', 20), justify=CENTER)
	signup_fname_Entry.place(x=(width*.335), y=(height*.4))
	fname_Label = Label(signup_screen_Frame, text='First Name', style="back.TLabel")
	fname_Label.place(x=(width*.385), y=(height*.37))

	# Draw username entry box
	global signup_user_Entry
	signup_user_Entry = Entry(signup_screen_Frame, font=('arial', 20), justify=CENTER)
	signup_user_Entry.place(x=(width*.335), y=(height*.525))
	user_Label = Label(signup_screen_Frame, text='Username', style="back.TLabel")
	user_Label.place(x=(width*.386), y=(height*.4925))
	user_req_Label = Label(signup_screen_Frame, text='* Must be at least 6 characters', background='light gray')
	user_req_Label.place(x=(width*.3725), y=(height*.565))

	# Draw admin key entry box
	global signup_admin_Entry
	signup_admin_Entry = Entry(signup_screen_Frame, font=('arial', 20), justify=CENTER)
	signup_admin_Entry.place(x=(width*.335), y=(height*.65))
	admin_Label = Label(signup_screen_Frame, text='Admin Key', style="back.TLabel")
	admin_Label.place(x=(width*.385), y=(height*.62))

	# Draw last name entry box
	global signup_lname_Entry
	signup_lname_Entry = Entry(signup_screen_Frame, font=('arial', 20), justify=CENTER)
	signup_lname_Entry.place(x=(width*.525), y=(height*.4))
	lname_Label = Label(signup_screen_Frame, text='Last Name', style="back.TLabel")
	lname_Label.place(x=(width*.572), y=(height*.37))

	# Draw password entry box
	global signup_pass_Entry
	signup_pass_Entry = Entry(signup_screen_Frame, show='*', font=('arial', 20), justify=CENTER)
	signup_pass_Entry.place(x=(width*.525), y=(height*.525))
	pass_Label = Label(signup_screen_Frame, text='Enter Password', style="back.TLabel")
	pass_Label.place(x=(width*.558), y=(height*.495))
	pass_req_Label = Label(signup_screen_Frame, text='* Must be at least 8 characters', background='light gray')
	pass_req_Label.place(x=(width*.5625), y=(height*.565))

	# Draw re-enter password entry box
	global signup_repass_Entry
	signup_repass_Entry = Entry(signup_screen_Frame, show='*', font=('arial', 20), justify=CENTER)
	signup_repass_Entry.place(x=(width*.525), y=(height*.65))
	repass_Label = Label(signup_screen_Frame, text='Re-enter Password', style="back.TLabel")
	repass_Label.place(x=(width*.55), y=(height*.62))
	repass_req_Label = Label(signup_screen_Frame, text='* Must be at least 8 characters', background='light gray')
	repass_req_Label.place(x=(width*.5625), y=(height*.69))

	# Draw buttons
	signup_Button = Button(signup_screen_Frame, text='Create', command=signup, style='login.TButton')
	signup_Button.place(x=(width*.525), y=(height*.75))

	clear_Button = Button(signup_screen_Frame, text='Clear', command=reset_sigin, style='login.TButton')
	clear_Button.place(x=(width*.425), y=(height*.75))

	back_Button = Button(signup_screen_Frame, text='Back', command=Main_Window, style='login.TButton')
	back_Button.place(x=(width*.6425), y=(height*.85))

################
# LOGIN METHOD #
################

def login():
	# Grab Entry Block Information
	user = login_user_Entry.get()
	password = login_pass_Entry.get()

	# Connect To Login Database
	db = sql.connect('login.db')

	# Create Cursor
	db_C = db.cursor()

	db_C.execute("SELECT * FROM accounts WHERE username = ? AND password = ?", (user, password))
	row = db_C.fetchone()

	if row:
		print('signed in')
		select_page()
	else: 
		print('try again')
		messagebox.showwarning('Try Again!', 'Incorrect username or password')

	db.commit()
	db.close()

#####################
# LOGIN_PAGE METHOD #
#####################

def login_page():
	# Login Page
	login_screen_Frame.pack()

	login_Frame = Frame(login_screen_Frame, width=(width*.275), height=(height*.55), style='login.TFrame')
	login_Frame.place(x=(width*.37), y=(height*.25))
	
	login_Label = Label(login_screen_Frame, text='Sign In', font=('Arial', 32), style='login.TLabel')
	login_Label.place(x=(width*.474), y=(height*.32))

	global login_user_Entry
	login_user_Entry = Entry(login_screen_Frame, font=('Arial', 18), justify=CENTER)
	login_user_Entry.place(x=(width*.44), y=(height*.45))
	login_user_Label = Label(login_screen_Frame, text='Username', font=('Arial', 16), style='login.TLabel')
	login_user_Label.place(x=(width*.48), y=(height*.42))

	global login_pass_Entry
	login_pass_Entry = Entry(login_screen_Frame, font=('Arial', 18), show='*', justify=CENTER)
	login_pass_Entry.place(x=(width*.44), y=(height*.55))
	login_pass_Label = Label(login_screen_Frame, text='Password', font=('Arial', 16), style='login.TLabel')
	login_pass_Label.place(x=(width*.48), y=(height*.52))

	login_Button = Button(login_screen_Frame, text='Login', command=login, style='login.TButton')
	login_Button.place(x=(width*.525), y=(height*.65))

	signup_Button = Button(login_screen_Frame, text='Sign Up', command=signup_page, style='login.TButton')
	signup_Button.place(x=(width*.425), y=(height*.65))

######################
# MAIN_WINDOW METHOD #
######################

def Main_Window():
	# Root Window Setup
	global root
	global width
	global height
	global login_screen_Frame
	global signup_screen_Frame
	global select_screen_Frame

	root = Tk()
	width = root.winfo_screenwidth()
	height = root.winfo_screenheight()
	root.title("OMNICheck")
	root.state('zoomed')
	root.attributes('-fullscreen', True)

	# Configure Styles
	style = Style(root)
	style.configure('login.TButton', font=('arial', 14), background='white', foreground='red')
	style.configure('exit.TButton', width=3.75, font=('arial', 18))
	style.configure('minimize.TButton', width=3.75, font=('arial', 18))
	style.configure("login.TLabel", foreground="red", background="white")
	style.configure('login.TFrame', background='white')
	style.configure('background.TFrame', background='#ce2029')
	style.configure('back.TFrame', background='light gray')
	style.configure('back.TLabel', background='light gray', foreground='red', font=('arial', 18))

	login_screen_Frame = Frame(root, width=width, height=height, style='background.TFrame')
	signup_screen_Frame = Frame(root, width=width, height=height, style='background.TFrame')
	select_screen_Frame = Frame(root, width=width, height=height, style='background.TFrame')

	quit_Button = Button(root, text=' X ', command=quit, style='exit.TButton')
	quit_Button.place(x=(width*.9795),y=(0))
	minimize_Button = Button(root, text=' - ', command=root.iconify, style='minimize.TButton')
	minimize_Button.place(x=(width*.9628),y=(0))

	# Login Page Method
	login_page()

###################
# DB_START METHOD #
###################

def db_start():
	# Connect To Login Database
	db = sql.connect('login.db')

	# Create Cursor
	db_C = db.cursor()

	# Create Table
	db_C.execute("""CREATE TABLE IF NOT EXISTS accounts (
		username text,
		password text,
		fname text,
		lname text
		)""")

	db.commit()
	db.close()

###############
# MAIN METHOD #
###############

def main():
	db_start()
	Main_Window()
	root.mainloop()

main()