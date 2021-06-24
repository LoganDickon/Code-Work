import sqlite3

def show_all():
	# Connect to database
	con = sqlite3.connect("Login.db")
	# Create cursor
	c = con.cursor()
	c.execute("SELECT * FROM login")
	contents = c.fetchall()
	for item in contents:
		print(" |" + item[0] + "| |" + item[1] + "|")
	# Close connection
	con.close()

def create_account(account_id, password):
	con = sqlite3.connect("Login.db")
	c = con.cursor()
	c.execute("INSERT INTO customers VALUES (?,?)", (account_id, password))
	con.commit()
	con.close()