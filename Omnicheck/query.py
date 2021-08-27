import sqlite3

db = sqlite3.connect('Omni_Login.db')
	
db.execute('CREATE TABLE IF NOT EXISTS login(username TEXT, password TEXT, fullname TEXT )')
cursor = db.cursor()
db.execute("DELETE FROM login WHERE rowid = 2")
cursor.execute("SELECT rowid, * FROM login")
print(cursor.fetchall())
db.commit()
db.close()