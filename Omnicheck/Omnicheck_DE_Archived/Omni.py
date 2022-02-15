

# ---------------#
# - App Imports  #
# ---------------#

from kivymd.app import MDApp
from kivy.lang import Builder
from kivy.uix.screenmanager import Screen, ScreenManager
from kivy.core.window import Window
from kivy.properties import ObjectProperty
from kivymd.uix.button import MDFlatButton
from kivymd.uix.dialog import MDDialog
from win32api import GetSystemMetrics
import sqlite3


# ----------------------#
# - ScreenManager class #
# ----------------------#


class WindowManager(ScreenManager):
    def __init__(self, **kwargs):
        super(WindowManager, self).__init__(**kwargs)


#---------------------- # ----------- #
#-- Login screen class --- 1st Screen #
#---------------------- # ----------- #


class Log_in(Screen):
    login_dialog = None
    def close_login_dialog(self, obj):
        self.login_dialog.dismiss()

    def login(self):
        user = self.ids.accountid.text
        password = self.ids.passwordid.text
        
        db = sqlite3.connect('Omni_Login.db')
        db.execute('CREATE TABLE IF NOT EXISTS login(username TEXT, password TEXT)')
        cursor = db.cursor()
        cursor.execute("SELECT * FROM login WHERE username = ? AND password = ?", (user, password))
        row = cursor.fetchone()
        
        if row:
            print('signed in')
            self.ids.accountid.text = ""
            self.ids.passwordid.text = ""
            self.manager.current = 'third'
        else:
            print('Nope!')
            if not self.login_dialog:
                self.login_dialog = MDDialog(
                    title="Login Unsuccessful!",
                    text="Check your username and/or password and try again!",
                    buttons=[
                        MDFlatButton(
                            text="OK", on_release = self.close_login_dialog
                        ),
                    ],
                )
            self.login_dialog.open()
        db.commit()
        db.close()


# ---------------------- # ----------- #
# - Signup screen class --- 2nd Screen #
# ---------------------- # ----------- #


class Sign_up(Screen):
    signin_dialog = None
    
    def close_signin_dialog(self, obj):
        self.signin_dialog.dismiss()
    
    def go_home(self, obj):
        self.signin_dialog.dismiss()
        self.manager.current = 'first'
    
    def signup(self):
        newuser = self.ids.newaccount.text
        newpassword = self.ids.newpassword.text
        newfullname = self.ids.newfullname.text
        db = sqlite3.connect('Omni_Login.db')
        db.execute('CREATE TABLE IF NOT EXISTS login(username TEXT, password TEXT, fullname TEXT )')
        cursor = db.cursor()
        db.execute("INSERT INTO login(username, password, fullname) VALUES(?,?,?)", [newuser, newpassword, newfullname])
        cursor.execute("SELECT rowid, * FROM login")
        new_row = cursor.fetchone()
        if new_row:
            print('Created New!')
            self.ids.newaccount.text = ""
            self.ids.newpassword.text = ""
            self.ids.newfullname.text = ""
            if not self.signin_dialog:
                self.signin_dialog = MDDialog(
                    title="Account Created!",
                    text="Return to Homepage to Log in!",
                    buttons=[
                        MDFlatButton(
                            text="Home", on_release = self.go_home
                        ),
                        MDFlatButton(
                            text="OK", on_release = self.close_signin_dialog
                        ),
                    ],
                )
            self.signin_dialog.open()
        db.commit()
        db.close()

# -------------------- # ----------- #
# - Home screen class --- 3rd Screen #
# -------------------- # ----------- #


class Home(Screen):
    pass


# -----------------#
# - Main app class #
# -----------------#


class BuildApp(MDApp):
    dialog = None
    def Fullscreen(self):
        if Window.fullscreen == True:
            Window.fullscreen = False
        else:
            Window.size = (GetSystemMetrics(0), GetSystemMetrics(1))
            Window.fullscreen = True

    def build(self):
        self.title = "OMNICheck"
        self.Fullscreen()
        return Builder.load_file('Omni.kv')

    def exit_dialog(self):
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
