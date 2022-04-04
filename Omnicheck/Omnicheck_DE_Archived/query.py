import sqlite3

db = sqlite3.connect('login.db')
cursor = db.cursor()	
cursor.execute("""CREATE TABLE IF NOT EXISTS accounts (
		username text,
		password text,
		fname text,
		lname text
		)""")
cursor.execute("SELECT rowid, * FROM accounts")
print(cursor.fetchall())
db.commit()
db.close()