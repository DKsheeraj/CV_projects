from flask import Flask, render_template, request, session
from flask import *
import psycopg2
import base64

app = Flask(__name__)
# Secret key for signing the session cookie. This is to ensure threat actors cannot modify contents of the session cookie.
app.secret_key = b'\x07\xed\xf7\x96h\xa3M\xd2\xad"GK}R^\xe3'

def get_db_connection():
    conn = psycopg2.connect(
    host = "localhost",
    database = "newcollegefest",
    user = "postgres",
    password = "postgres")

    return conn


@app.route('/', methods = ['GET', 'POST'])
def home():
    
    if session.get('loggedin'): 
        if session.get('userType') == "ExternalParticipant":
            return redirect(url_for('afterlogin'))
        elif session.get('userType') == "Student":
            return redirect(url_for('afterloginstudent'))
        elif session.get('userType') == "Organiser":
            return redirect(url_for('afterloginorg'))
        elif session.get('userType') == "Admin":
            return redirect(url_for('afterloginadmin'))
        else: pass

    if(request.method == 'POST'):
        name = request.form['name']
        email = request.form['email']
        message = request.form['message']

        # Open a file in write mode
        file_path = "./messages.txt"
        with open(file_path, "a") as file:
            # Write content to the file
            file.write("Name: " + name + "\n")
            file.write("Email: " + email + "\n")
            file.write("Message: "+ message  + "\n\n")

            

        # File automatically closed after exiting the 'with' block
        print("Content has been written to", file_path)




    session['loggedin'] = False
    return render_template('home.html')

@app.route('/login', methods = ['GET', 'POST'])
def login():
    if(request.method == 'POST'):

        username = request.form['username']
        password = request.form['password']
        password = base64.b64encode(password.encode('ascii')).decode()
        print(password)

        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute('SELECT users.fullname from users where username = %s', [username])
        db = cur.fetchall()

        if not db:
            msg = "Invalid username or password!"
            return render_template('login.html', msg = msg)

        fullname = db[0]
        # fullname.append(cur.fetchall())
        # print()
        # print(fullname)
        # fullname = fullname[0][0]
        # conn = get_db_connection()
        # cur = conn.cursor()

        cur.execute('SELECT * FROM users WHERE username = %s AND password = %s', (username, password))
        user = cur.fetchall()

        # print(user[0][0], user[0][1], user[0][2], user[0][3], user[0][4], user[0][5])

        if user:
            
            session['loggedin'] = True
            session['userType'] = user[0][5]
            session['username'] = username
            session["fullname"] = fullname

            if (user[0][5] == 'Student'):
                return redirect(url_for('afterloginstudent'))
            elif user[0][5] == 'ExternalParticipant':
                return redirect(url_for('afterlogin'))
            elif user[0][5] == 'Admin':
                return redirect(url_for('afterloginadmin'))
            elif user[0][5] == 'Organiser':
                return redirect(url_for('afterloginorg'))
        else:
            msg = "Invalid username and password!"
            return render_template('login.html', msg = msg)
        print(id, password)
    return render_template('login.html')

@app.route('/logout')
def logout():

    if not session.get('loggedin'):
        return redirect(url_for('login'))

    session.pop('userType')
    session.pop('username')
    session.pop('fullname')
    session['loggedin'] = False
    
    return redirect(url_for('login'))

@app.route('/afterlogin' , methods = ['GET', 'POST'])
def afterlogin():
    #username = request.args.get('username')
    #password = request.args.get('password')
    #fullname = request.args.get('fullname')
    
    #print(username, password)
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "ExternalParticipant":
        return redirect(url_for('logout'))
    
    if(request.method == 'POST'):
        name = request.form['name']
        email = request.form['email']
        message = request.form['message']

        # Open a file in write mode
        file_path = "./messages.txt"
        with open(file_path, "a") as file:
            # Write content to the file
            file.write("Name: " + name + "\n")
            file.write("Email: " + email + "\n")
            file.write("Message: "+ message  + "\n\n")

            

        # File automatically closed after exiting the 'with' block
        print("Content has been written to", file_path)


    
    return render_template('afterlogin.html')

@app.route('/afterloginstudent', methods = ['GET', 'POST'])
def afterloginstudent():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # print(username, password)
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Student":
        return redirect(url_for('logout'))
    
    if(request.method == 'POST'):
        name = request.form['name']
        email = request.form['email']
        message = request.form['message']

        # Open a file in write mode
        file_path = "./messages.txt"
        with open(file_path, "a") as file:
            # Write content to the file
            file.write("Name: " + name + "\n")
            file.write("Email: " + email + "\n")
            file.write("Message: "+ message  + "\n\n")

            

        # File automatically closed after exiting the 'with' block
        print("Content has been written to", file_path)


    
    return render_template('afterloginstudent.html')

@app.route('/afterloginadmin', methods = ['GET', 'POST'])
def afterloginadmin():
    # username = request.args.get('username')
    # password = request.args.get('password')

    # print(username, password)
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Admin":
        return redirect(url_for('logout'))
    
    if(request.method == 'POST'):
        name = request.form['name']
        email = request.form['email']
        message = request.form['message']

        # Open a file in write mode
        file_path = "./messages.txt"
        with open(file_path, "a") as file:
            # Write content to the file
            file.write("Name: " + name + "\n")
            file.write("Email: " + email + "\n")
            file.write("Message: "+ message  + "\n\n")

            

        # File automatically closed after exiting the 'with' block
        print("Content has been written to", file_path)


    
    return render_template('afterloginadmin.html')



@app.route('/event', methods = ['GET', 'POST'])
def event():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    username = ''

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "ExternalParticipant":
        return redirect(url_for('logout'))

    # Login auth check
    if session.get('username'):
        
        print("event - ", session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    event_id1 = request.args.get('event_id1')
    event_id2 = request.args.get('event_id2')
    event_id3 = request.args.get('event_id3')
    print(event_id1)

    
    if event_id1:
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute("SELECT * FROM registrations WHERE event_id = %s AND username = %s", (event_id1, username))
        if cur.fetchone() is None:
            cur.execute('INSERT INTO registrations (username, event_id, category) VALUES (%s, %s, %s) ', (username, event_id1, 'ExternalParticipant'))
        
        conn.commit()
        cur.close()
        conn.close()

    if(event_id2):
        return redirect(url_for('winnerdisplay', event_id2 = event_id2))

    if(event_id3):
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute('DELETE FROM registrations WHERE event_id = %s AND username = %s', (event_id3, username))
        conn.commit()
    

    
    # print(username, password)
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('SELECT * FROM events order by event_id')
    events = cur.fetchall()

    cur.execute("SELECT event_id FROM registrations WHERE username = %s", (username, ))
    regevents = [row[0] for row in cur.fetchall()]

    cur.execute("SELECT event_id FROM winners WHERE gold IS NOT NULL OR silver IS NOT NULL OR bronze IS NOT NULL")
    blockevents = [row[0] for row in cur.fetchall()]

    print(regevents)

    if not cur.closed:
        cur.close()

    if not conn.closed:
        conn.close()

    length = len(events)

    return render_template('event.html', events = events, length = length, regevents = regevents, blocks = blockevents)


@app.route('/registration', methods = ['GET', 'POST'])
def registration():
    if(request.method == 'POST'):
        username = request.form['username']
        fullname = request.form['fullname']
        email = request.form['email']
        phone = request.form['phone']
        password = request.form['password']
        password = base64.b64encode(password.encode('ascii')).decode()
        confirm_password = request.form['confirm_password']
        confirm_password = base64.b64encode(confirm_password.encode('ascii')).decode()
        category = request.form['category']

        conn = get_db_connection()
        cur = conn.cursor()

        cur.execute('SELECT * FROM users WHERE username = %s', [username])
        
        if cur.fetchone() is not None:
            return redirect(url_for('login'))

        if password == confirm_password:
            cur.execute('INSERT INTO users (username, fullname, email, phone, password, category)'
                    'VALUES (%s, %s, %s, %s, %s, %s)',
                    (username, fullname, email, phone, password, category)
                    )
            
            conn.commit()

            if (category == 'Student'):
                cur.execute('INSERT INTO students (username, fullname, email, phone, password, category)'
                    'VALUES (%s, %s, %s, %s, %s, %s)',
                    (username, fullname, email, phone, password, category)
                    )
                
            elif (category == 'ExternalParticipant'):
                cur.execute('INSERT INTO externalparticipants (username, fullname, email, phone, password, category)'
                    'VALUES (%s, %s, %s, %s, %s, %s)',
                    (username, fullname, email, phone, password, category)
                    )
            elif (category == 'Organiser'):
                cur.execute('INSERT INTO organiser (username, fullname, email, phone, password, category)'
                    'VALUES (%s, %s, %s, %s, %s, %s)',
                    (username, fullname, email, phone, password, category)
                    )
                
               
            conn.commit()
            cur.close()
            conn.close()
            return redirect(url_for("login"))
        
        else:
            msg = "Password fields do not match!"
            return render_template('registration.html', msg = msg)
        
    return render_template('registration.html')

@app.route('/eventstudent', methods = ['GET', 'POST'])
def eventstudent():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Student":
        print("hit")
        return redirect(url_for('logout'))
    
    # Login auth check
    if session.get('username'):
        
        print("eventstudent - ", session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    event_id1 = request.args.get('event_id1')
    event_id2 = request.args.get('event_id2')

    if event_id1:
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute("SELECT * FROM registrations WHERE event_id = %s AND username = %s", (event_id1, username))
        if cur.fetchone() is None:
            cur.execute('INSERT INTO registrations (username, event_id, category) VALUES (%s, %s, %s) ', (username, event_id1, 'Student'))
            
            cur.execute("SELECT * from volunteer WHERE event_id = %s AND username = %s", (event_id1, username))
            if cur.fetchone() is not None:
                cur.execute("DELETE FROM volunteer WHERE event_id = %s AND username = %s", (event_id1, username))

        conn.commit()
        cur.close()
        conn.close()

    if(event_id2):
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute("SELECT * FROM volunteer WHERE event_id = %s AND username = %s", (event_id2, username))
        
        if cur.fetchone() is None:
            print(username, event_id2)
            cur.execute('INSERT INTO volunteer (username, event_id) VALUES (%s, %s) ', (username, event_id2))
            
            cur.execute("SELECT * from registrations WHERE event_id = %s AND username = %s", (event_id2, username))
            if cur.fetchone() is not None:
                cur.execute("DELETE FROM registrations WHERE event_id = %s AND username = %s", (event_id2, username))
        else:
            print("Already a volunteer")
        conn.commit()
        cur.close()
        conn.close()

    if(event_id3):
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute('DELETE FROM registrations WHERE event_id = %s AND username = %s', (event_id3, username))
        conn.commit()

    conn = get_db_connection()
    cur = conn.cursor()

    cur.execute('SELECT * FROM events order by event_id;')
    events = cur.fetchall()

    cur.execute("SELECT event_id FROM registrations WHERE username = %s", (username, ))
    regevents = [row[0] for row in cur.fetchall()]

    cur.execute("SELECT event_id FROM volunteer WHERE username = %s", (username, ))
    regvols = [row[0] for row in cur.fetchall()]

    cur.execute("SELECT event_id FROM winners WHERE gold IS NOT NULL OR silver IS NOT NULL OR bronze IS NOT NULL")
    blockevents = [row[0] for row in cur.fetchall()]
    # print(blockevents)

    if not cur.closed:
        cur.close()

    if not conn.closed:
        conn.close()

    length = len(events)

    return render_template('eventstudent.html', events = events, length = length, regevents = regevents, regvols = regvols, blocks = blockevents)

@app.route('/manageuser')
def manageuser():
    
    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Admin":
        return redirect(url_for('logout'))
    
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('SELECT * FROM students;')
    db = cur.fetchall()

    length = len(db)

    category = request.args.get('category')
    deluser = request.args.get('deluser')

    # print(deluser, type(deluser))

    if(category == 'Student'):
        cur.execute('SELECT * FROM students;')
        db = cur.fetchall()
        length = len(db)

    elif(category == 'ExternalParticipant'):
        cur.execute('SELECT * FROM externalparticipants;')
        db = cur.fetchall()
        length = len(db)
    elif(category == 'Organiser'):
        cur.execute('SELECT * FROM organiser;')
        db = cur.fetchall()
        length = len(db)


    types = 0
    if(deluser):
        cur.execute('SELECT * FROM students WHERE username = %s', [deluser])
        if cur.fetchone is not None:
            # cur.execute('DELETE FROM students WHERE username = %s', [deluser])
            cur.execute('DELETE FROM users WHERE username = %s', [deluser])
            conn.commit()
            types = 1
        cur.execute('SELECT * FROM externalparticipants WHERE username = %s', [deluser])
        if cur.fetchone() is not None:
            # cur.execute('DELETE FROM externalparticipants WHERE username = %s', [deluser])
            cur.execute('DELETE FROM users WHERE username = %s', [deluser])
            conn.commit()
            types = 2
        cur.execute('SELECT * FROM organiser WHERE username = %s', [deluser])
        if cur.fetchone() is not None:
            # cur.execute('DELETE FROM organiser WHERE username = %s', [deluser])
            cur.execute('DELETE FROM users WHERE username = %s', [deluser])
            conn.commit()
            types = 3

        if(types == 1):
            cur.execute('SELECT * FROM students;')
            db = cur.fetchall()
            length = len(db)
            return render_template('manageuser.html', db = db, length = length, cat = category)
        elif(types == 2):
            cur.execute('SELECT * FROM externalparticipants;')
            db = cur.fetchall()
            length = len(db)
            return render_template('manageuser.html', db = db, length = length, cat = category)
        elif(types == 3):
            cur.execute('SELECT * FROM organiser;')
            db = cur.fetchall()
            length = len(db)
            return render_template('manageuser.html', db = db, length = length, cat = category)
        else:
            cur.execute('SELECT * FROM students;')
            db = cur.fetchall()
            length = len(db)
            return render_template('manageuser.html', db = db, length = length, cat = category)
    
    if(types == 0) :
        return render_template('manageuser.html', db = db, length = length, cat = category)

@app.route('/adduser', methods = ['GET', 'POST'])
def adduser():

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Admin":
        return redirect(url_for('logout'))
    
    msg = None
    
    if(request.method == 'POST'):
        username = request.form['username']
        fullname = request.form['fullname']
        email = request.form['email']
        phone = request.form['phone']
        password = request.form['password']
        password = base64.b64encode(password.encode('ascii')).decode()
        category = request.form['category']

        conn = get_db_connection()
        cur = conn.cursor()

        cur.execute('SELECT * FROM users WHERE username = %s', [username])

        if cur.fetchone():
            return render_template('adduser.html', msg = "User already exists!")
        
        msg = "Successfully added user!"

        cur.execute('INSERT INTO users (username, fullname, email, phone, password, category)'
                    'VALUES (%s, %s, %s, %s, %s, %s)',
                    (username, fullname, email, phone, password, category)
                    )

        if(category == 'Student'):
            cur.execute('INSERT INTO students (username, fullname, email, phone, password, category)'
                    'VALUES (%s, %s, %s, %s, %s, %s)',
                    (username, fullname, email, phone, password, category)
                    )
        elif(category == 'ExternalParticipant'):
            cur.execute('INSERT INTO externalparticipants (username, fullname, email, phone, password, category)'
                    'VALUES (%s, %s, %s, %s, %s, %s)',
                    (username, fullname, email, phone, password, category)
                    )
        elif(category == 'Organiser'):
            cur.execute('INSERT INTO organiser (username, fullname, email, phone, password, category)'
                    'VALUES (%s, %s, %s, %s, %s, %s)',
                    (username, fullname, email, phone, password, category)
                    )
        conn.commit()

    
    return render_template('adduser.html', msg = msg)

@app.route('/manageevent')
def manageevent():

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Admin":
        return redirect(url_for('logout'))
    
    delevent = request.args.get('delevent')
    print(delevent)

    if(delevent):
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute('DELETE FROM events WHERE event_id = %s', [delevent])
        conn.commit()
    
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('SELECT * FROM events;')
    db = cur.fetchall()
    length = len(db)

    return render_template('manageevent.html', db = db, length = length)

@app.route('/addevent', methods = ['GET', 'POST'])
def addevent():

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Admin":
        return redirect(url_for('logout'))
    
    msg = None
    
    if(request.method == 'POST'):
        event_id = request.form['eventid']
        event_name = request.form['eventname']
        date = request.form['date']
        description = request.form['description']
        event_type = request.form['eventtype']

        conn = get_db_connection()
        cur = conn.cursor()

        cur.execute("SELECT * FROM events WHERE event_id = %s", [event_id])
        result = cur.fetchone()
        
        if result:
            return render_template('addevent.html', msg = "Event already exists!")
        
        msg = "Successfully added event!"

        cur.execute('INSERT INTO events (event_id, event_name, date, description, event_type)'
                    'VALUES (%s, %s, %s, %s, %s)',
                    (event_id, event_name, date, description, event_type)
                    )
        
        cur.execute('INSERT INTO winners (event_id, gold, silver, bronze)'
                    'VALUES (%s, %s, %s, %s)',
                    (event_id, None, None, None)
                    )
        conn.commit()

    return render_template('addevent.html', msg = msg)

@app.route('/afterloginorg', methods = ['GET', 'POST'])
def afterloginorg():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Organiser":
        return redirect(url_for('logout'))
    
    if(request.method == 'POST'):
        name = request.form['name']
        email = request.form['email']
        message = request.form['message']

        # Open a file in write mode
        file_path = "./messages.txt"
        with open(file_path, "a") as file:
            # Write content to the file
            file.write("Name: " + name + "\n")
            file.write("Email: " + email + "\n")
            file.write("Message: "+ message  + "\n\n")

            

        # File automatically closed after exiting the 'with' block
        print("Content has been written to", file_path)


    
    return render_template('afterloginorg.html')

@app.route('/infoorg', methods = ['GET', 'POST'])
def infoorg():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')
    eventid = request.args.get('eventid')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Organiser":
        return redirect(url_for('logout'))
    
    # Login auth check
    if session.get('username'):
        
        print(session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    if(eventid):
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute('SELECT * FROM organiser_event WHERE username = %s and event_id = %s', (username, eventid))
        if cur.fetchone() is None:
            cur.execute('INSERT INTO organiser_event (username, event_id) VALUES (%s, %s)', (username, eventid))
            conn.commit()
        else:
            print("Already an organiser")
        
        if not cur.closed:
            cur.close()
    
        if not conn.closed:
            conn.close()


    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('SELECT * FROM events;')
    db = cur.fetchall()

    cur.execute('SELECT * FROM organiser_event WHERE username = %s', (username, ))
    db1 = cur.fetchall()

    organiser = []

    if db1:
        for row in db1:
            organiser.append(row[2])

    length = len(db)

    if not cur.closed:
        cur.close()
    
    if not conn.closed:
        conn.close()

    #print(username, password)
    return render_template('infoorg.html', db = db, length = length, organiser = organiser)

@app.route('/winner', methods = ['GET', 'POST'])
def winner():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Organiser":
        return redirect(url_for('logout'))
    
    # Login auth check
    if session.get('username'):
        
        print(session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    eventid = request.args.get('eventid')
    goldw = request.args.get('goldw')
    silverw = request.args.get('silverw')
    bronzew = request.args.get('bronzew')

    print(eventid, goldw, silverw, bronzew)

    if(eventid):
        print(eventid, goldw, silverw, bronzew)
        if(goldw):
            conn = get_db_connection()
            cur = conn.cursor()
            cur.execute('''
                UPDATE winners
                SET gold = %s
                WHERE event_id = %s;
                        ''', [goldw, eventid])
            conn.commit()
        
        if(silverw):
            conn = get_db_connection()
            cur = conn.cursor()
            cur.execute('''
                UPDATE winners
                SET silver = %s
                WHERE event_id = %s;
                        ''', [silverw, eventid])
            conn.commit()
        
        if(bronzew):
            conn = get_db_connection()
            cur = conn.cursor()
            cur.execute('''
                UPDATE winners
                SET bronze = %s
                WHERE event_id = %s;
                        ''', [bronzew, eventid])
            
            conn.commit()
    
    # print("HERE")
    # print(username, password)
    # print(username, password, eventid)

    print(username)
    
    conn = get_db_connection()
    cur = conn.cursor()

    cur.execute('''
        SELECT events.event_id, events.event_name, users.username, winners.gold, winners.silver, winners.bronze
        FROM events
        JOIN organiser_event ON events.event_id = organiser_event.event_id
        JOIN registrations ON events.event_id = registrations.event_id
        JOIN users ON registrations.username = users.username
        JOIN winners ON events.event_id = winners.event_id
        WHERE organiser_event.username = %s
        ORDER BY events.event_id
    ''', [username])
    
    db = cur.fetchall()
    length = len(db)

    print(db)
    
    return render_template('winner.html', length = length, db = db)


@app.route('/volunteerinfo', methods = ['GET', 'POST'])
def volunteerinfo():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Organiser":
        return redirect(url_for('logout'))
    
    # Login auth check
    if session.get('username'):
        
        print(session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    #print(username)
    
    conn = get_db_connection()
    cur = conn.cursor()

    cur.execute('''
        SELECT events.event_id, events.event_name, users.username, users.fullname
        FROM events
        JOIN volunteer ON events.event_id = volunteer.event_id
        JOIN users ON volunteer.username = users.username   
    ''')

    db = cur.fetchall()
    length = len(db)

    print(db)
    
    return render_template('volunteerinfo.html', length = length, db = db)

@app.route('/winnerdisplay')
def winnerdisplay():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    event_id2 = request.args.get('event_id2')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "ExternalParticipant":
        return redirect(url_for('logout'))

    # Login auth check
    if session.get('username'):
        
        print(session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('''
        SELECT events.event_name, winners.gold, winners.silver, winners.bronze
        FROM events
        JOIN winners ON events.event_id = winners.event_id
        
        WHERE events.event_id = %s
    ''', [event_id2])

    db = cur.fetchall()

    print(db)

    cur.execute('''
        SELECT events.event_name, winners.gold, winners.silver, winners.bronze
        FROM events
        JOIN winners ON events.event_id = winners.event_id
        
        WHERE events.event_id = %s
    ''', [event_id2])

    if cur.fetchone() is None:
        golduser = "None"
        silveruser = "None"
        bronzeuser = "None"
        # dbano = [('None',)]
        cur.execute('''
            SELECT events.event_name
            FROM events
            where events.event_id = %s
                    ''', [event_id2])
        
        dbano = cur.fetchall()
        
    else:
        golduser = db[0][1]
        silveruser = db[0][2]
        bronzeuser = db[0][3]
        dbano = [(db[0][0],)]

    print(golduser)
    print(bronzeuser)

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [golduser])


    dbgold = cur.fetchall()

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [golduser])

    if cur.fetchone() is None:
        dbgold = [('Not yet declared',)]
    

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [silveruser])

    dbsilver = cur.fetchall()

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [silveruser])

    if cur.fetchone() is None:
        dbsilver = [('Not yet declared',)]

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [bronzeuser])

    # dbtem = cur.fetchall()

    # if cur.fetchone() is None:
    #     dbbronze = [('None',)]
    # else:
    #     dbbronze = dbtem

    dbbronze = cur.fetchall()

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [bronzeuser])
    
    if cur.fetchone() is None:
        print("hello")
        dbbronze = [('Not yet declared',)]

    print(dbgold, dbbronze)

    return render_template('winnerdisplay.html', db = dbano, dbgold = dbgold, dbsilver = dbsilver, dbbronze = dbbronze)

@app.route('/logisticspart', methods = ['GET', 'POST'])
def logisticspart():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "ExternalParticipant":
        return redirect(url_for('logout'))

    # Login auth check
    if session.get('username'):
        
        print(session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('''SELECT logisticpart.hall, logisticpart.roomnum, logisticpart.foodtype 
                FROM logisticpart
                WHERE logisticpart.username = %s
                ''', [username])
    db = cur.fetchall()

    cur.execute('''SELECT logisticpart.hall, logisticpart.roomnum, logisticpart.foodtype 
                FROM logisticpart
                WHERE logisticpart.username = %s
                ''', [username])
    
    if cur.fetchone() is None:
        db = [('None', 'None', 'None')]

    length = len(db)
    
    return render_template('logisticspart.html', db = db, length = length)


@app.route('/logisticorg')
def logisticorg():

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Organiser":
        return redirect(url_for('logout'))
    
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('''
        SELECT users.username, users.fullname, logisticpart.hall, logisticpart.roomnum, logisticpart.foodtype
                FROM users
                LEFT OUTER JOIN logisticpart ON users.username = logisticpart.username
                WHERE logisticpart.username IS NOT NULL''')
    db = cur.fetchall()
    length = len(db)

    return render_template('logisticorg.html', db = db, length = length)


@app.route('/changeacco')
def changeacco():

    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "ExternalParticipant":
        return redirect(url_for('logout'))

    # Login auth check
    if session.get('username'):
        
        print(session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    hall = request.args.get('hall')
    roomnum = request.args.get('roomnum')
    foodtype = request.args.get('foodtype')

    if hall:
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute('''
            UPDATE logisticpart
            SET foodtype = NULL, username = NULL
            WHERE username = %s;
                    ''', [username])
        conn.commit()
        cur.execute('''
            UPDATE logisticpart
            SET foodtype = %s, username = %s
            WHERE hall = %s and roomnum = %s;
                    ''', [foodtype, username, hall, roomnum])
        conn.commit()

        
    
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('''SELECT logisticpart.hall, logisticpart.roomnum
                FROM logisticpart 
                WHERE logisticpart.username IS NULL
                ORDER BY logisticpart.hall, logisticpart.roomnum;
                ''')
    db = cur.fetchall()
    length = len(db)

    return render_template('changeacco.html', db = db, length = length)

@app.route('/accodetailsofall')
def accodetailsofall():

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Admin":
        return redirect(url_for('logout'))
    
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('''
        SELECT users.username, users.fullname, logisticpart.hall, logisticpart.roomnum, logisticpart.foodtype
                FROM users
                LEFT OUTER JOIN logisticpart ON users.username = logisticpart.username
                WHERE logisticpart.username IS NOT NULL''')
    db = cur.fetchall()
    length = len(db)

    return render_template('accodetailsofall.html', db = db, length = length)

@app.route('/winnerdisplaystudent')
def winnerdisplaystudent():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Student":
        return redirect(url_for('logout'))

    # Login auth check
    if session.get('username'):
        
        print(session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    event_id2 = request.args.get('event_id3')

    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('''
        SELECT events.event_name, winners.gold, winners.silver, winners.bronze
        FROM events
        JOIN winners ON events.event_id = winners.event_id
        
        WHERE events.event_id = %s
    ''', [event_id2])
    
    db = cur.fetchall()

    print(db)

    cur.execute('''
        SELECT events.event_name, winners.gold, winners.silver, winners.bronze
        FROM events
        JOIN winners ON events.event_id = winners.event_id
        
        WHERE events.event_id = %s
    ''', [event_id2])


    if cur.fetchone() is None:
        golduser = "None"
        silveruser = "None"
        bronzeuser = "None"
        # dbano = [('None',)]
        cur.execute('''
            SELECT events.event_name
            FROM events
            where events.event_id = %s
                    ''', [event_id2])
        
        dbano = cur.fetchall()
        
    else:
        golduser = db[0][1]
        silveruser = db[0][2]
        bronzeuser = db[0][3]
        dbano = [(db[0][0],)]

    print(golduser)
    print(bronzeuser)

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [golduser])

    dbgold = cur.fetchall()

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [golduser])

    if cur.fetchone() is None:
        dbgold = [('Not yet declared',)]
    
    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [silveruser])

    dbsilver = cur.fetchall()
    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [silveruser])

    if cur.fetchone() is None:
        dbsilver = [('Not yet declared',)]

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [bronzeuser])

    # dbtem = cur.fetchall()

    # if cur.fetchone() is None:
    #     dbbronze = [('None',)]
    # else:
    #     dbbronze = dbtem

    dbbronze = cur.fetchall()

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [bronzeuser])
    
    if cur.fetchone() is None:
        print("hello")
        dbbronze = [('Not yet declared',)]

    print(dbgold, dbbronze)


    return render_template('winnerdisplaystudent.html', db = dbano, dbgold = dbgold, dbsilver = dbsilver, dbbronze = dbbronze)

@app.route('/winnerdisplayorg')
def winnerdisplayorg():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Organiser":
        return redirect(url_for('logout'))

    # Login auth check
    if session.get('username'):
        
        print(session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    event_id2 = request.args.get('event_id2')

    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('''
        SELECT events.event_name, winners.gold, winners.silver, winners.bronze
        FROM events
        JOIN winners ON events.event_id = winners.event_id
        
        WHERE events.event_id = %s
    ''', [event_id2])
  
    db = cur.fetchall()

    print(db)

    cur.execute('''
        SELECT events.event_name, winners.gold, winners.silver, winners.bronze
        FROM events
        JOIN winners ON events.event_id = winners.event_id
        
        WHERE events.event_id = %s
    ''', [event_id2])


    if cur.fetchone() is None:
        golduser = "None"
        silveruser = "None"
        bronzeuser = "None"
        # dbano = [('None',)]
        cur.execute('''
            SELECT events.event_name
            FROM events
            where events.event_id = %s
                    ''', [event_id2])
        
        dbano = cur.fetchall()
        
    else:
        golduser = db[0][1]
        silveruser = db[0][2]
        bronzeuser = db[0][3]
        dbano = [(db[0][0],)]

    print(golduser)
    print(bronzeuser)

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [golduser])


    dbgold = cur.fetchall()

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [golduser])

    if cur.fetchone() is None:
        dbgold = [('Not yet declared',)]
    

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [silveruser])


    dbsilver = cur.fetchall()

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [silveruser])

    if cur.fetchone() is None:
        dbsilver = [('Not yet declared',)]

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [bronzeuser])

    # dbtem = cur.fetchall()

    # if cur.fetchone() is None:
    #     dbbronze = [('None',)]
    # else:
    #     dbbronze = dbtem

    dbbronze = cur.fetchall()

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [bronzeuser])
    
    if cur.fetchone() is None:
        print("hello")
        dbbronze = [('Not yet declared',)]

    print(dbgold, dbbronze)


    return render_template('winnerdisplayorg.html', db = dbano, dbgold = dbgold, dbsilver = dbsilver, dbbronze = dbbronze)

@app.route('/winnerdisplayadmin')
def winnerdisplayadmin():
    event_id2 = request.args.get('event_id2')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Admin":
        return redirect(url_for('logout'))
    
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute('''
        SELECT events.event_name, winners.gold, winners.silver, winners.bronze
        FROM events
        JOIN winners ON events.event_id = winners.event_id
        
        WHERE events.event_id = %s
    ''', [event_id2])

    db = cur.fetchall()

    print(db)

    cur.execute('''
        SELECT events.event_name, winners.gold, winners.silver, winners.bronze
        FROM events
        JOIN winners ON events.event_id = winners.event_id
        
        WHERE events.event_id = %s
    ''', [event_id2])


    if cur.fetchone() is None:
        golduser = "None"
        silveruser = "None"
        bronzeuser = "None"
        # dbano = [('None',)]
        cur.execute('''
            SELECT events.event_name
            FROM events
            where events.event_id = %s
                    ''', [event_id2])
        
        dbano = cur.fetchall()
        
    else:
        golduser = db[0][1]
        silveruser = db[0][2]
        bronzeuser = db[0][3]
        dbano = [(db[0][0],)]

    print(golduser)
    print(bronzeuser)

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [golduser])


    dbgold = cur.fetchall()

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [golduser])

    if cur.fetchone() is None:
        dbgold = [('Not yet declared',)]
    

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [silveruser])


    dbsilver = cur.fetchall()

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [silveruser])

    if cur.fetchone() is None:
        dbsilver = [('Not yet declared',)]

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [bronzeuser])

    # dbtem = cur.fetchall()

    # if cur.fetchone() is None:
    #     dbbronze = [('None',)]
    # else:
    #     dbbronze = dbtem

    dbbronze = cur.fetchall()

    cur.execute('''
        SELECT users.fullname
        FROM users
        WHERE users.username = %s
    ''', [bronzeuser])
    
    if cur.fetchone() is None:
        print("hello")
        dbbronze = [('Not yet declared',)]

    print(dbgold, dbbronze)

    return render_template('winnerdisplayadmin.html', db = dbano, dbgold = dbgold, dbsilver = dbsilver, dbbronze = dbbronze)


@app.route('/profilechangestudent', methods = ['GET', 'POST'])
def profilechangestudent():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Student":
        return redirect(url_for('logout'))
    
    # Login auth check
    if session.get('username'):
        
        print(session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    if(request.method == 'POST'):

        emailid = request.form['emailid']
        phone = request.form['phone']
        newpass = request.form['newpass']
        newpass = base64.b64encode(newpass.encode('ascii'))
        curpass = request.form['curpass']
        curpass = base64.b64encode(curpass.encode('ascii'))

        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute('''\
            UPDATE users 
            SET email = %s, 
                phone = %s, 
                password = %s 
            WHERE username = %s 
                AND password = %s
                AND category = 'Student'
                ''', 
            (emailid, phone, newpass, username, curpass))
        
        # if (curpass == password):
        #     password = newpass

        cur.execute('''\
            UPDATE students 
            SET email = %s, 
                phone = %s, 
                password = %s 
            WHERE username = %s 
                AND password = %s''', 
            (emailid, phone, newpass, username, curpass))
        
        
        
        conn.commit()

        if not cur.closed:
            cur.close()
        if not conn.closed:
            conn.close()

        return redirect(url_for('afterloginstudent'))
    
    else:
        conn = get_db_connection()
        cur = conn.cursor()

        cur.execute("SELECT email, phone FROM users WHERE username = %s", (username, ))
        userdb = cur.fetchall()[0]

        if not cur.closed:
            cur.close()
        if not conn.closed:
            conn.close()
    
        return render_template('profilechangestudent.html', userdb = userdb)

@app.route('/profilechange', methods = ['GET', 'POST'])
def profilechange():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "ExternalParticipant":
        return redirect(url_for('logout'))

    # Login auth check
    if session.get('username'):
        
        print(session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    if(request.method == 'POST'):

        emailid = request.form['emailid']
        phone = request.form['phone']
        newpass = request.form['newpass']
        newpass = base64.b64encode(newpass.encode('ascii'))
        curpass = request.form['curpass']
        curpass = base64.b64encode(curpass.encode('ascii'))

        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute('''\
            UPDATE users 
            SET email = %s, 
                phone = %s, 
                password = %s 
            WHERE username = %s 
                AND password = %s
                AND category = 'ExternalParticipant'
                    ''', 
            (emailid, phone, newpass, username, curpass))
        
        # if(curpass == password):
        #     password = newpass


        cur.execute('''\
            UPDATE externalparticipants 
            SET email = %s, 
                phone = %s, 
                password = %s 
            WHERE username = %s 
                AND password = %s''', 
            (emailid, phone, newpass, username, curpass))
        
        conn.commit()

        return redirect(url_for('afterlogin'))
    
    else:
        conn = get_db_connection()
        cur = conn.cursor()

        cur.execute("SELECT email, phone FROM users WHERE username = %s", (username, ))
        userdb = cur.fetchall()[0]

        if not cur.closed:
            cur.close()
        if not conn.closed:
            conn.close()
    
        return render_template('profilechange.html', userdb = userdb)

@app.route('/profilechangeorg', methods = ['GET', 'POST'])
def profilechangeorg():
    # username = request.args.get('username')
    # password = request.args.get('password')
    # fullname = request.args.get('fullname')

    # Session check
    if not session.get('loggedin'):
        return redirect(url_for('login'))
    
    if session.get('userType') != "Organiser":
        return redirect(url_for('logout'))

    # Login auth check
    if session.get('username'):
        
        print(session.get('username'))
        username = session.get('username')

    else:
        msg = "Unauthorized access!"
        print(msg)
        return redirect(url_for('login'))

    # Auth check finishes

    if(request.method == 'POST'):

        emailid = request.form['emailid']
        phone = request.form['phone']
        newpass = request.form['newpass']
        newpass = base64.b64encode(newpass.encode('ascii'))
        curpass = request.form['curpass']
        curpass = base64.b64encode(curpass.encode('ascii'))

        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute('''\
            UPDATE users 
            SET email = %s, 
                phone = %s, 
                password = %s 
            WHERE username = %s 
                AND password = %s
                AND category = 'Organiser'
                ''', 
            (emailid, phone, newpass, username, curpass))
        
        # if(curpass == password):
        #     password = newpass

        cur.execute('''\
            UPDATE organiser 
            SET email = %s, 
                phone = %s, 
                password = %s 
            WHERE username = %s 
                AND password = %s''', 
            (emailid, phone, newpass, username, curpass))
        
        conn.commit()

        # return render_template('afterloginstudent.html', fullname = fullname, username = username, password = password)
        return redirect(url_for('afterloginorg'))

    else:
        conn = get_db_connection()
        cur = conn.cursor()

        cur.execute("SELECT email, phone FROM users WHERE username = %s", (username, ))
        userdb = cur.fetchall()[0]

        if not cur.closed:
            cur.close()
        if not conn.closed:
            conn.close()

        return render_template('profilechangeorg.html', userdb = userdb)


if __name__ == '__main__':
    app.run(debug=True, port = 5001)
