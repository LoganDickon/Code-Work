# --Libraries--
import tkinter as tk
import tkinter.ttk as ttk
from tkinter import messagebox
import sqlite3 as sql
from windows import set_dpi_awareness
from PIL import ImageTk, Image
import subprocess
import cv2
import sys
import os

# --Powershell File Path
POWERSHELL_PATH = "powershell.exe"  # POWERSHELL EXE PATH
ps_script_path = "C:/Omni_Digital/Omni1/runasAdmin.ps1"  # YOUR POWERSHELL FILE PATH

# --Set High DPI--
set_dpi_awareness()

# --Disable Webcam--
class Utility:
	@staticmethod
	# --Run Powershell Sript in Adminstator Mode Method--
	def run_ftp_upload_powershell_script(script_path, *params):

		# --Add Powershell EXE and Execution Policy to Command Variable--
		commandline_options = [POWERSHELL_PATH, '-ExecutionPolicy', 'Unrestricted', script_path]  
		
		# --Loop For Each Parameter From Array--
		for param in params:
			
			# --Append Your Powershell Script--
			commandline_options.append("'" + param + "'")

		# --Run Powershell Scipt--
		process_result = subprocess.run(commandline_options, stdout = subprocess.PIPE, stderr = subprocess.PIPE, universal_newlines = True)

		# --Loop Until User Approves Adminstator Mode or Cancels 3 Times--
		loop_iter = 1
		while (process_result.stderr):
			if (loop_iter == 3):
				exit()
				break
			process_result = subprocess.run(commandline_options, stdout = subprocess.PIPE, stderr = subprocess.PIPE, universal_newlines = True)
			loop_iter += 1

		# --Check For Errors in Powershell Script--
		if process_result.returncode == 0:
			Message = "Success !"
		else:
			Message = "Error Occurred !"

		# --Return Code Given By Powershell--
		return Message

# --App Manager--
class OmniMain(tk.Tk):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)
		
		# --Window Setup--
		self.title("OMNICHECK")
		self.attributes('-fullscreen', True)
		self.after(0, self.focus_force)
		
		# --Obtain Screen Resolution--
		global width
		global height
		width = self.winfo_screenwidth()
		height = self.winfo_screenheight()
		print(width)
		print(height)

		# --Configure Styles--
		style = ttk.Style(self)
		style.theme_use("winnative")
		style.configure('background.TFrame', background='#ce2029')
		style.configure('login.TButton', font=('arial', 12), background='grey', foreground='red', relief='raised')
		style.configure("login.TLabel", foreground="red", background="white")
		style.configure('login.TFrame', background='white', borderwidth=1, relief="raised")
		style.configure('global.TButton', width=3.75, font=('arial', 16), relief='raised', background='#ce2029')
		style.configure('back.TFrame', background='light gray', borderwidth=1, relief="sunken")
		style.configure('back.TLabel', foreground='red', font=('arial', 18), background='light gray',)
		style.configure('selection.TFrame', background='dark gray', borderwidth=1, relief='raised')
		style.configure('selection.TButton', width=35, font=('arial', 20), relief='raised', foreground="#ce2029")
		style.configure('selection.TLabel', background="#ce2029", foreground='white', font=('arial', 25))
		style.configure('scba.TFrame', background='white', borderwidth=1, relief='raised')
		style.configure('scba1.TLabel', foreground="white", font=('arial', 50), background="#ce2029")
		style.configure('scba2.TLabel', foreground="#ce2029", font=('arial', 32), background="white")
		style.configure('scba1.TButton', width=3.75, font=('arial', 30), relief='raised', foreground="#ce2029", background='white')
		style.configure('scba2.TButton', width=3.75, font=('arial', 25), relief='raised', foreground="#ce2029", background='white')
		style.configure('scba_test.TFrame', background='#ce2029')
		
		# --Create Frame Container--
		container = ttk.Frame(self)
		container.grid(row=0, column=0)

		# --Layout All Frames Onto Screen--
		self.frames = dict()
		for FrameClass in (Login_Screen, Signup_Screen, Select_Screen, SCBA_Test_Screen):
			frame = FrameClass(container, self)
			self.frames[FrameClass] = frame
			frame.grid(row=0, column=0, sticky="NSEW")

		# --Raise Login_Screen Frame To Top--
		self.show_frame(Login_Screen)

		# --Minimize Button--
		minimize_Button = ttk.Button(self, text=' - ', command=self.iconify, style='global.TButton')
		minimize_Button.place(x=(width * .9595),y=(0))

		# --Exit App Button--
		quit_Button = ttk.Button(self, text=' X ', command=self.quit_app, style='global.TButton')
		quit_Button.place(x=(width * .9774),y=(0))

	# --Raise Selected Frame To Top
	def show_frame(self, container):
		frame = self.frames[container]
		frame.configure(style='background.TFrame', width=width, height=height)
		frame.tkraise()

	# --Exit App Method--
	def quit_app(self):
		yes_no = messagebox.askyesno("Quit?", "Are you sure?")
		if (yes_no == True):
			self.quit()

	# --Load Database--
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

		# Insert Default Account
		db_C.execute("""INSERT OR IGNORE INTO accounts (oid, username, password, fname, lname)
						values ('1', 'GODMODE', 'GODMODE', 'GODMODE', 'GODMODE')""")

		# Insert Default Account
		db_C.execute("""INSERT OR IGNORE INTO accounts (oid, username, password, fname, lname)
						values ('2', '', '', '', '')""")

		# Save & Exit database
		db.commit()
		db.close()

# --Login Screen--
class Login_Screen(ttk.Frame):
	def __init__(self, container, controller, **kwargs):
		super().__init__(container, **kwargs)
		self.controller = controller

		login_Frame = ttk.Frame(self, width=(width*.275), height=(height*.55), style='login.TFrame')
		login_Frame.place(x=(width*.3625), y=(height*.24))
		
		login_Label = ttk.Label(self, text='Sign In', font=('Arial', 32), style='login.TLabel')
		login_Label.place(x=(width*.457), y=(height*.31))

		self.login_user_Entry = ttk.Entry(self, font=('Arial', 16), justify="center")
		self.login_user_Entry.place(x=(width*.42), y=(height*.46))
		login_user_Label = ttk.Label(self, text='Username', font=('Arial', 16), style='login.TLabel')
		login_user_Label.place(x=(width*.466), y=(height*.42))

		self.login_pass_Entry = ttk.Entry(self, font=('Arial', 16), show='*', justify="center")
		self.login_pass_Entry.place(x=(width*.42), y=(height*.56))
		login_pass_Label = ttk.Label(self, text='Password', font=('Arial', 16), style='login.TLabel')
		login_pass_Label.place(x=(width*.467), y=(height*.52))

		signup_Button = ttk.Button(self, text='Sign Up', command=lambda: controller.show_frame(Signup_Screen), style='login.TButton')
		signup_Button.place(x=(width*.415), y=(height*.65))

		login_Button = ttk.Button(self, text='Login', command=self.login, style='login.TButton')
		login_Button.place(x=(width*.515), y=(height*.65))

	def login(self):
		# Grab Entry Block Information
		user = self.login_user_Entry.get()
		password = self.login_pass_Entry.get()

		# Connect To Login Database
		db = sql.connect('login.db')

		# Create Cursor
		db_C = db.cursor()

		db_C.execute("SELECT * FROM accounts WHERE username = ? AND password = ?", (user, password))
		row = db_C.fetchone()

		if row:
			print('signed in')
			self.controller.show_frame(Select_Screen)
		else: 
			print('try again')
			messagebox.showwarning('Try Again!', 'Incorrect username or password')

		db.commit()
		db.close()

# --Signup Screen--
class Signup_Screen(ttk.Frame):
	def __init__(self, container, controller, **kwargs):
		super().__init__(container, **kwargs)

		self.controller = controller

		# Draw main signup frame
		signup_Frame = ttk.Frame(self, width=(width*.45), height=(height*.8), style='login.TFrame')
		signup_Frame.place(x=(width*.275), y=(height*.1))

		# Draw "create new" label
		signup_Label = ttk.Label(self, text='Create New', font=('Arial', 32), style='login.TLabel')
		signup_Label.place(x=(width*.4275), y=(height*.1465))

		# Draw gray back frame for depth
		back_Frame = ttk.Frame(self, width=(width*.4), height=(height*.58), style='back.TFrame')
		back_Frame.place(x=(width*.3), y=(height*.25))

		# Draw first name entry box
		self.signup_fname_Entry = ttk.Entry(self, font=('arial', 16), justify="center")
		self.signup_fname_Entry.place(x=(width*.325), y=(height*.4))
		fname_Label = ttk.Label(self, text='First Name', style="back.TLabel")
		fname_Label.place(x=(width*.3675), y=(height*.355))

		# Draw username entry box
		self.signup_user_Entry = ttk.Entry(self, font=('arial', 16), justify="center")
		self.signup_user_Entry.place(x=(width*.325), y=(height*.525))
		user_Label = ttk.Label(self, text='Username', style="back.TLabel")
		user_Label.place(x=(width*.3685), y=(height*.48))
		user_req_Label = ttk.Label(self, text='* Must be at least 6 characters', background='light gray')
		user_req_Label.place(x=(width*.3525), y=(height*.565))

		# Draw admin key entry box
		self.signup_admin_Entry = ttk.Entry(self, font=('arial', 16), justify="center")
		self.signup_admin_Entry.place(x=(width*.325), y=(height*.65))
		admin_Label = ttk.Label(self, text='Admin Key', style="back.TLabel")
		admin_Label.place(x=(width*.366), y=(height*.605))

		# Draw last name entry box
		self.signup_lname_Entry = ttk.Entry(self, font=('arial', 16), justify="center")
		self.signup_lname_Entry.place(x=(width*.5125), y=(height*.4))
		lname_Label = ttk.Label(self, text='Last Name', style="back.TLabel")
		lname_Label.place(x=(width*.555), y=(height*.355))

		# Draw password entry box
		self.signup_pass_Entry = ttk.Entry(self, show='*', font=('arial', 16), justify="center")
		self.signup_pass_Entry.place(x=(width*.5125), y=(height*.525))
		pass_Label = ttk.Label(self, text='Enter Password', style="back.TLabel")
		pass_Label.place(x=(width*.5375), y=(height*.48))
		pass_req_Label = ttk.Label(self, text='* Must be at least 8 characters', background='light gray')
		pass_req_Label.place(x=(width*.54), y=(height*.565))

		# Draw re-enter password entry box
		self.signup_repass_Entry = ttk.Entry(self, show='*', font=('arial', 16), justify="center")
		self.signup_repass_Entry.place(x=(width*.5125), y=(height*.65))
		repass_Label = ttk.Label(self, text='Re-enter Password', style="back.TLabel")
		repass_Label.place(x=(width*.5265), y=(height*.605))
		repass_req_Label = ttk.Label(self, text='* Must be at least 8 characters', background='light gray')
		repass_req_Label.place(x=(width*.54), y=(height*.69))

		# Draw buttons
		clear_Button = ttk.Button(self, text='Clear', command=self.reset_signin, style='login.TButton')
		clear_Button.place(x=(width*.4125), y=(height*.75))

		signup_Button = ttk.Button(self, text='Create', command=self.signup, style='login.TButton')
		signup_Button.place(x=(width*.5175), y=(height*.75))

		back_Button = ttk.Button(self, text='Back', command=lambda: controller.show_frame(Login_Screen), style='login.TButton')
		back_Button.place(x=(width*.625), y=(height*.85))

	def signup(self):
		# Read Entry Box Data
		fname = self.signup_fname_Entry.get()
		lname = self.signup_lname_Entry.get()
		user = self.signup_user_Entry.get()
		password = self.signup_pass_Entry.get()
		repass = self.signup_repass_Entry.get()


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
			signup_fname_Entry.delete(0, "end")

		if (len(lname) > 20):
			print("lname: " + lname)
			exit = True
			messagebox.showwarning("Try Again!", "Last Name must be less than 20 characters")
			signup_lname_Entry.delete(0, "end")

		if (falpha == False):
			print(falpha)
			exit = True
			messagebox.showwarning("Try Again!", "First name must only be letters")
			self.signup_fname_Entry.delete(0, "end")

		if (lalpha == False):
			print(lalpha)
			exit = True
			messagebox.showwarning("Try Again!", "Last name must only be letters")
			self.signup_lname_Entry.delete(0, "end")

		if len(user) <= 6:
			print("user: " + user)
			exit = True
			messagebox.showwarning("Try Again!", "Username must have at least 6 characters")
			self.signup_user_Entry.delete(0, "end")
		
		if len(password) <= 8:
			print("pass: " + password)
			exit = True
			messagebox.showwarning("Try Again!", "Password must have at least 8 characters")
			self.signup_pass_Entry.delete(0, "end")

		if (password != repass):
			exit = True
			print("Passwords don't match")
			messagebox.showwarning("Try Again!", "Passwords don\'t match")
			self.signup_repass_Entry.delete(0, "end")

		if (db_C.fetchone()):
			exit = True
			print("found")
			messagebox.showwarning("Try Again!", "Account exists for the username already")
			self.signup_user_Entry.delete(0, "end")

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
	
	def reset_signin(self):
		self.signup_fname_Entry.delete(0, 'end')
		self.signup_lname_Entry.delete(0, 'end')
		self.signup_user_Entry.delete(0, 'end')
		self.signup_pass_Entry.delete(0, 'end')
		self.signup_admin_Entry.delete(0, 'end')
		self.signup_repass_Entry.delete(0, 'end')

# --Selection Screen--
class Select_Screen(ttk.Frame):
	def __init__(self, container, controller, **kwargs):
		super().__init__(container, **kwargs)

		self.controller = controller

		# --Sign Out Button--
		signout_Button = ttk.Button(self, text=' Sign Out ', command=self.sign_out, style='global.TButton')
		signout_Button.place(x=(width * .8905),y=(0))

		# --SCBA Widgets--
		selection_SCBA_Frame = ttk.Frame(self, width=(width * .4), height=(height * .4), style='selection.TFrame')
		selection_SCBA_Frame.place(x=(width * .075), y=(height * .075))
		self.image("C:/Omni_Digital/scba.jpg", .09, .1)
		selection_SCBA_Button = ttk.Button(self, text="Open-Circuit SCBA (FIRE)",style="selection.TButton", command=lambda: controller.show_frame(SCBA_Test_Screen))
		selection_SCBA_Button.place(x=(width * .1065), y=(height * .415))
		
		# --PAPR Widgets--
		selection_PAPR_Frame = ttk.Frame(self, width=(width * .4), height=(height * .4), style='selection.TFrame')
		selection_PAPR_Frame.place(x=(width * .525), y=(height * .075))
		self.image("C:/Omni_Digital/PAPR.jpg", .54, .1)
		selection_PAPR_Button = ttk.Button(self, text="Particulate Filters Only (PAPR)", style="selection.TButton")
		selection_PAPR_Button.place(x=(width * .557), y=(height * .415))

		# --C_SCBA Widgets--
		selection_CSCBA_Frame = ttk.Frame(self, width=(width * .4), height=(height * .4), style='selection.TFrame')
		selection_CSCBA_Frame.place(x=(width * .075), y=(height * .525))
		self.image("C:/Omni_Digital/c_scba.jpg", .09, .55)
		selection_CSCBA_Button = ttk.Button(self, text="Closed-Circuit SCBA", style="selection.TButton")
		selection_CSCBA_Button.place(x=(width * .1065), y=(height * .865))
		
		# --CHEM_BIO Widgets--
		selection_CHEM_Frame = ttk.Frame(self, width=(width * .4), height=(height * .4), style='selection.TFrame')
		selection_CHEM_Frame.place(x=(width * .525), y=(height * .525))
		self.image("C:/Omni_Digital/c_scba.jpg", .54, .55)
		selection_CHEM_Button = ttk.Button(self, text="Chemical Cartridges Vapors and/or Gases", style="selection.TButton")
		selection_CHEM_Button.place(x=(width * .557), y=(height * .865))

		select_Label = ttk.Label(self, text='Select Device', style='selection.TLabel')
		select_Label.place(x=(width*.447), y=(height*.0175))

	# --Image Placement Method--
	def image(self, path, x_axis, y_axis):
		image = Image.open(str(path))
		photo = ImageTk.PhotoImage(image.resize((700, 370), Image.ANTIALIAS))
		label = ttk.Label(self, image=photo)
		label.image = photo
		label.place(x=(width * float(x_axis)), y=(height * float(y_axis)))

		# --Sign Out Method--
	def sign_out(self):
		yes_no = messagebox.askyesno("Sign Out?", "Warning all unsaved work will be LOST!")
		if (yes_no == True):
			python = sys.executable
			os.execl(python, python, * sys.argv)

# --SCBA Screen--
class SCBA_Test_Screen(ttk.Frame):
	def __init__(self, container, controller, **kwargs):
		super().__init__(container, **kwargs)

		self.controller = controller

		signout_Button = ttk.Button(self, text=' Sign Out ', command=self.sign_out, style='global.TButton')
		signout_Button.place(x=(width * .8905),y=(0))

		self.cam1 = 0
		self.cam2 = 1

		scba_Label = ttk.Label(self, text='Camera Setup', style='scba1.TLabel')
		scba_Label.place(x=(width * .36), y=(height * .0725))

		hud_Frame = ttk.Frame(self, width=(width * .35), height=(height * .55), style='scba.TFrame')
		hud_Frame.place(x=(width * .1), y=(height * .225))
		hud_Label = ttk.Label(self, text='Hud Camera', style='scba2.TLabel')
		hud_Label.place(x=(width * .1965), y=(height * .7))
		
		gauge_Frame = ttk.Frame(self, width=(width* .35), height=(height * .55), style='scba.TFrame')
		gauge_Frame.place(x=(width * .55), y=(height * .225))
		gauge_Label = ttk.Label(self, text='Gauge Camera', style='scba2.TLabel')
		gauge_Label.place(x=(width * .63), y=(height * .7))

		self.cam1_label = ttk.Label(self, relief='sunken')
		self.cam1_label.place(x=(width * .1075),y=(height *.24))
		self.cam2_label = ttk.Label(self, relief='sunken')
		self.cam2_label.place(x=(width * .5575),y=(height *.24))

		cam_start = True
		while (cam_start == True):
			try:
				self.cap = cv2.VideoCapture(self.cam1, cv2.CAP_DSHOW)
				self.cap2 = cv2.VideoCapture(self.cam2, cv2.CAP_DSHOW)
				self.cap_read = self.cap.read()[0]
				self.cap2_read = self.cap.read()[0]

				if ((self.cap_read == True) and (self.cap2_read == True)):
					self.show_cam1()
					self.show_cam2()
					cam_start = False
					
				if ((self.cap_read == False) or (self.cap2_read == False)):
					self.cap.release()
					self.cap2.release()
					cam_start = messagebox.askretrycancel("Warning!", "Missing Camera Peripherals (Check Connections)")
					
			except:
				print("error")

		swap_Button = ttk.Button(self, text=' Swap Cameras ', command=self.swap_camera, style='scba1.TButton')
		swap_Button.place(x=(width * .40095),y=(height *.85))

		next_Button = ttk.Button(self, text=' Next ', style='scba2.TButton', command=self.next)
		next_Button.place(x=(width * .925),y=(height *.925))

		but = ttk.Button(self, text='Take Picture', command=self.picture)
		but.pack()

	def next(self):
		for widget in self.winfo_children():
			widget.destroy()
			signout_Button = ttk.Button(self, text=' Sign Out ', command=self.sign_out, style='global.TButton')
			signout_Button.place(x=(width * .8905), y=(0))


	def picture(self):
		_, frame = self.cap.read()
		_, frame2 = self.cap2.read()
		frame = cv2.flip(frame, 1)
		frame2 = cv2.flip(frame2, 1)
		out = cv2.imwrite('C:/Omni_Digital/Pics/hud.jpeg', frame)
		out2 = cv2.imwrite('C:/Omni_Digital/Pics/gauge.jpeg', frame2)
		print("SNAP!")

	def swap_camera(self):
		self.cap.release()
		self.cap2.release()

		if (self.cam1 == 0):
			self.cam1 = 1
			self.cam2 = 0

			self.cap = cv2.VideoCapture(self.cam1, cv2.CAP_DSHOW)
			self.cap2 = cv2.VideoCapture(self.cam2, cv2.CAP_DSHOW)

			print('Cam1: ', self.cam1)
			print('Cam2: ', self.cam2)

			return self.cap, self.cap2, self.cam1, self.cam2

		if (self.cam1 == 1):
			self.cam1 = 0
			self.cam2 = 1

			self.cap = cv2.VideoCapture(self.cam1, cv2.CAP_DSHOW)
			self.cap2 = cv2.VideoCapture(self.cam2, cv2.CAP_DSHOW)
			
			print('Cam1: ', self.cam1)
			print('Cam2: ', self.cam2)
			
			return self.cap, self.cap2, self.cam1, self.cam2

	def show_cam1(self):
		_, frame = self.cap.read()
		frame = cv2.flip(frame, 1)
		cv2image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
		img = Image.fromarray(cv2image)
		imgtk = ImageTk.PhotoImage(image=img)
		self.cam1_label.configure(image=imgtk)
		self.cam1_label._image_cache = imgtk
		self.cam1_label.after(50, self.show_cam1)

	def show_cam2(self):
		_, frame = self.cap2.read()
		frame = cv2.flip(frame, 1)
		cv2image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
		img = Image.fromarray(cv2image)
		imgtk = ImageTk.PhotoImage(image=img)
		self.cam2_label.configure(image=imgtk)
		self.cam2_label._image_cache = imgtk
		self.cam2_label.after(50, self.show_cam2)

	# --Sign Out Method--
	def sign_out(self):
		yes_no = messagebox.askyesno("Sign Out?", "Warning all unsaved work will be LOST!")
		if (yes_no == True):
			python = sys.executable
			os.execl(python, python, * sys.argv)

# --Main Method--
def main():
	#Utility.run_ftp_upload_powershell_script(POWERSHELL_PATH, ps_script_path)
	root = OmniMain()
	OmniMain.db_start()
	root.mainloop()

if __name__ == "__main__":
	main()