import os
import psycopg2

conn = psycopg2.connect(
    host = "localhost",
    database = "newcollegefest",
    user = "postgres",
    password = "postgres")


cur = conn.cursor()

cur.execute('DROP TABLE IF EXISTS winners;')
cur.execute('DROP TABLE IF EXISTS volunteer;')
cur.execute('DROP TABLE IF EXISTS logisticpart;')
cur.execute('DROP TABLE IF EXISTS organiser_event;')
cur.execute('DROP TABLE IF EXISTS registrations;')
cur.execute('DROP TABLE IF EXISTS students;')
cur.execute('DROP TABLE IF EXISTS externalparticipants;')
cur.execute('DROP TABLE IF EXISTS organiser;')
cur.execute('DROP TABLE IF EXISTS users;')
cur.execute('DROP TABLE IF EXISTS events;')


cur.execute('''
    CREATE TABLE IF NOT EXISTS users (
        username VARCHAR PRIMARY KEY,
        fullname VARCHAR,
        email VARCHAR,
        phone VARCHAR,
        password VARCHAR,
        category VARCHAR
    );
''')

cur.execute('''
    CREATE TABLE IF NOT EXISTS students (
        username VARCHAR PRIMARY KEY,
        fullname VARCHAR,
        email VARCHAR,
        phone VARCHAR,
        password VARCHAR,
        category VARCHAR,
        FOREIGN KEY (username) REFERENCES users (username) 
        ON DELETE CASCADE
    );
''')

cur.execute('''
    CREATE TABLE IF NOT EXISTS externalparticipants (
        username VARCHAR PRIMARY KEY,
        fullname VARCHAR,
        email VARCHAR,
        phone VARCHAR,
        password VARCHAR,
        category VARCHAR,
        FOREIGN KEY (username) REFERENCES users (username) 
        ON DELETE CASCADE
    );
''')

cur.execute('''
    CREATE TABLE IF NOT EXISTS organiser (
        username VARCHAR PRIMARY KEY,
        fullname VARCHAR,
        email VARCHAR,
        phone VARCHAR,
        password VARCHAR,
        category VARCHAR,
        FOREIGN KEY (username) REFERENCES users(username)
        ON DELETE CASCADE
         
    );
''')

cur.execute('''
    INSERT INTO users (username, fullname, email, phone, password, category)
    VALUES (%s, %s, %s, %s, %s, %s)
''', ('admin', 'Admin', 'admin@gmail.com', '1234567890', 'YWRtaW4=', 'Admin'))

data = [
    ('21CS10041', 'Mayukha', 'abc@gmail.com', '123434324', 'YnQyMQ==', 'Student'),
    ('21CS10042', 'Harshith', 'def@gmail.com', '123434324', 'YnQyMQ==', 'Student'),
    ('21CS10043', 'John', 'ghi@gmail.com', '123434324', 'YnQyMQ==', 'Student'),
    ('21CS10044', 'Alice', 'jkl@gmail.com', '123434324', 'YnQyMQ==', 'Student'),
    ('21CS10045', 'Bob', 'mno@gmail.com', '123434324', 'YnQyMQ==', 'Student'),
    ('21CS10046', 'Emma', 'pqr@gmail.com', '123434324', 'YnQyMQ==', 'Student'),
    ('21CS10047', 'Oliver', 'stu@gmail.com', '123434324', 'YnQyMQ==', 'Student'),
    ('21CS10048', 'Sophia', 'vwx@gmail.com', '123434324', 'YnQyMQ==', 'Student'),
    ('21CS10049', 'Mia', 'yz@gmail.com', '123434324', 'YnQyMQ==', 'Student'),
    ('21CS10050', 'Liam', 'lmn@gmail.com', '123434324', 'YnQyMQ==', 'Student')
]


query = '''
    INSERT INTO users (username, fullname, email, phone, password, category) 
    VALUES (%s, %s, %s, %s, %s, %s)
'''
cur.executemany(query, data)

query = '''
    INSERT INTO students (username, fullname, email, phone, password, category) 
    VALUES (%s, %s, %s, %s, %s, %s)
'''
cur.executemany(query, data)


# Insert data into the "users" table
cur.execute('''
    INSERT INTO users (username, fullname, email, phone, password, category)
    VALUES ('OR1', 'Organiser', 'organiser@example.com', '1234567890', 'b3IyMQ==', 'Organiser')
''')

# Insert data into the "organiser" table
cur.execute('''
    INSERT INTO organiser (username, fullname, email, phone, password, category)
    VALUES ('OR1', 'Organiser', 'organiser@example.com', '1234567890', 'b3IyMQ==', 'Organiser')
''')


query = '''
    CREATE TABLE events (
        event_id VARCHAR PRIMARY KEY,
        event_name VARCHAR,
        date VARCHAR,
        description VARCHAR,
        event_type VARCHAR
    );
'''
cur.execute(query)


data = [
    ('101', '2024-03-15', 'The Battle of the Bands is a competitive event where different musical groups showcase their talents in front of an audience and judges. Bands of various genres compete for prizes and recognition, making it an exciting platform for emerging artists to gain exposure and launch their careers.', 'Battle of the Bands', 'MegaEvent'),
    ('102', '2024-04-02', 'A Tech Expo is an event where companies display their latest technological innovations. Attendees can explore new products, network with industry professionals, and learn about emerging trends in technology.','Tech Expo', 'Technology'),
    ('103', '2024-05-08', 'Dance Fusion integrates diverse movement styles, such as ballet, hip-hop, and contemporary, into seamless routines that emphasize fluidity and expression. Performances often feature multicultural influences and experimental techniques, captivating audiences with its fusion of tradition and innovation.','Dance Fusion', 'Dance'),
    ('104', '2024-06-20', 'The Literary Symposium is a gathering of writers, scholars, and literature enthusiasts to discuss and celebrate literary works and ideas. The event may include panel discussions, readings, workshops, and book signings, providing opportunities for engagement and exchange among participants. It aims to promote literacy, appreciation for literature, and intellectual discourse within the literary community.','Literary Symposium', 'Literature'),
    ('105', '2024-07-12', 'The Culinary Challenge brings together chefs and food enthusiasts to showcase their culinary skills and creativity. Participants compete in various cooking challenges, presenting their dishes to judges for evaluation. It celebrates gastronomy, diverse cuisines, and the artistry of cooking.','Culinary Challenge', 'Culinary'),
    ('106', '2024-08-05', 'The Exhibition is a showcase of art, innovation, and creativity, featuring works from local and international artists. Attendees can explore a wide range of artistic mediums, from paintings and sculptures to digital art and installations. The event fosters cultural exchange, appreciation for the arts, and inspiration for both artists and visitors alike.','Exhibition', 'MegaEvent'),
    ('107', '2024-09-18', 'The Hackathon is an intense, collaborative event where programmers, designers, and innovators come together to solve problems and create innovative solutions within a limited timeframe. Participants work in teams to develop software, applications, or prototypes, often focusing on specific themes or challenges. It\'s a platform for creativity, learning, and innovation, with the potential to produce groundbreaking ideas and technologies.','Hackathon', 'MegaEvent'),
    ('108', '2024-10-25', 'Stand-up Comedy Night is an evening of laughter and entertainment, featuring performances by stand-up comedians. Audiences can enjoy a lineup of comedic talent, with comedians delivering humorous anecdotes, observations, and jokes on various topics. It\'s a chance to unwind, share laughs, and appreciate the comedic craft in a lively and social atmosphere.','Stand-up Comedy Night', 'Comedy')
]

query = '''
    INSERT INTO events (event_id, date, description, event_name, event_type) 
    VALUES (%s, %s, %s, %s, %s)
'''
cur.executemany(query, data)


query = '''
    CREATE TABLE IF NOT EXISTS registrations (
        participant_id serial,
        username VARCHAR,
        event_id VARCHAR,
        category VARCHAR,
        PRIMARY KEY(username, event_id),
        FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE,
        FOREIGN KEY (event_id) REFERENCES events(event_id) ON DELETE CASCADE
    );
'''
cur.execute(query)

data = [
    ('21CS10041', '101', 'Student'),
    ('21CS10042', '102', 'Student'),
    ('21CS10043', '103', 'Student'),
    ('21CS10044', '104', 'Student'),
    ('21CS10045', '105', 'Student')
]

query = '''
    INSERT INTO registrations (username, event_id, category)
    VALUES (%s, %s, %s)
'''
cur.executemany(query, data)


query = '''
    CREATE TABLE IF NOT EXISTS organiser_event (
        organiser_id serial,
        username VARCHAR,
        event_id VARCHAR,
        PRIMARY KEY(username, event_id),
        FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE,
        FOREIGN KEY (event_id) REFERENCES events(event_id) ON DELETE CASCADE
    );
'''
cur.execute(query)

query = '''
    CREATE TABLE IF NOT EXISTS logisticpart (
        place_id serial,
        hall VARCHAR,
        roomnum VARCHAR,
        username VARCHAR,
        foodtype VARCHAR,
        PRIMARY KEY(hall, roomnum),
        FOREIGN KEY (username) REFERENCES users(username) ON DELETE SET NULL
    );
'''
cur.execute(query)

data = [
    ('VS','101', None, None),
    ('VS', '102', None, None),
    ('VS', '103', None, None),
    ('VS', '104', None, None),
    ('VS', '105', None, None),
    ('VS', '106', None, None),
    ('VS', '107', None, None),
    ('VS', '108', None, None),
    ('VS', '109', None, None),
    ('VS', '110', None, None),
    ('NS', '101', None, None),
    ('NS', '102', None, None),
    ('NS', '103', None, None),
    ('NS', '104', None, None),
    ('NS', '105', None, None),
    ('NS', '106', None, None),
    ('NS', '107', None, None),
    ('NS', '108', None, None),
    ('NS', '109', None, None),
    ('NS', '110', None, None),
    ('CS', '101', None, None),
    ('CS', '102', None, None),
    ('CS', '103', None, None),
    ('CS', '104', None, None),
    ('CS', '105', None, None),
    ('CS', '106', None, None),
    ('CS', '107', None, None),
    ('CS', '108', None, None),
    ('CS', '109', None, None),
    ('CS', '110', None, None)
]

query = '''
    INSERT INTO logisticpart (hall, roomnum, username, foodtype) 
    VALUES (%s, %s, %s, %s)
'''

cur.executemany(query, data)


query = '''
    CREATE TABLE IF NOT EXISTS volunteer (
        volunteer_id serial PRIMARY KEY,
        username VARCHAR,
        event_id VARCHAR,
        FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE,
        FOREIGN KEY (event_id) REFERENCES events(event_id) ON DELETE CASCADE
    );
'''
cur.execute(query)


query = '''
    CREATE TABLE IF NOT EXISTS winners (
        event_id VARCHAR,
        gold VARCHAR,
        silver VARCHAR,
        bronze VARCHAR,
        PRIMARY KEY(event_id),
        FOREIGN KEY (event_id) REFERENCES events(event_id) ON DELETE CASCADE,
        FOREIGN KEY (gold) REFERENCES users(username) ON DELETE SET NULL,
        FOREIGN KEY (silver) REFERENCES users(username) ON DELETE SET NULL,
        FOREIGN KEY (bronze) REFERENCES users(username) ON DELETE SET NULL
    );
'''
cur.execute(query)

create_trigger_sql = """
    CREATE OR REPLACE FUNCTION update_medals_trigger()
    RETURNS TRIGGER AS $$
    BEGIN
        IF NEW.gold = OLD.silver THEN
            NEW.silver = NULL;
            RETURN NEW;
        END IF;
        IF NEW.gold = OLD.bronze THEN
            NEW.bronze = NULL;
            RETURN NEW;
        END IF;
        IF NEW.silver = OLD.gold THEN
            NEW.gold = NULL;
            RETURN NEW;
        END IF;
        IF NEW.silver = OLD.bronze THEN
            NEW.bronze = NULL;
            RETURN NEW;
        END IF;
        IF NEW.bronze = OLD.gold THEN
            NEW.gold = NULL;
            RETURN NEW;
        END IF;
        IF NEW.bronze = OLD.silver THEN
            NEW.silver = NULL;
            RETURN NEW;
        END IF;
        RETURN NEW;
    END;
    $$ LANGUAGE plpgsql;
    CREATE TRIGGER update_medals_trigger
    BEFORE INSERT OR UPDATE ON winners
    FOR EACH ROW EXECUTE FUNCTION update_medals_trigger();
    """
cur.execute(create_trigger_sql)

data = [('101', None, None, None),
        ('102', None, None, None),
        ('103', None, None, None),
        ('104', None, None, None),
        ('105', None, None, None),
        ('106', None, None, None),
        ('107', None, None, None),
        ('108', None, None, None)
       ]

query = '''
    INSERT INTO winners (event_id, gold, silver, bronze) 
    VALUES (%s, %s, %s, %s)
'''

cur.executemany(query, data)

data = [
    ('IITD1', 'Kavya', 'abc@iitd.ac.in', '12334525', 'ZXgyMQ==', 'ExternalParticipant'),
    ('IITD2', 'John', 'john@example.com', '1234567890', 'ZXgyMQ==', 'ExternalParticipant'),
    ('IITD3', 'Alice', 'alice@example.com', '1234567890', 'ZXgyMQ==', 'ExternalParticipant'),
    ('IITD4', 'Bob', 'bob@example.com', '1234567890', 'ZXgyMQ==', 'ExternalParticipant'),
    ('IITD5', 'Emma', 'emma@example.com', '1234567890', 'ZXgyMQ==', 'ExternalParticipant'),
    ('IITD6', 'Oliver', 'oliver@example.com', '1234567890', 'ZXgyMQ==', 'ExternalParticipant'),
    ('IITD7', 'Sophia', 'sophia@example.com', '1234567890', 'ZXgyMQ==', 'ExternalParticipant'),
    ('IITD8', 'Mia', 'mia@example.com', '1234567890', 'ZXgyMQ==', 'ExternalParticipant'),
    ('IITD9', 'Liam', 'liam@example.com', '1234567890', 'ZXgyMQ==', 'ExternalParticipant'),
    ('IITD10', 'Noah', 'noah@example.com', '1234567890', 'ZXgyMQ==', 'ExternalParticipant')
]

query = '''
    INSERT INTO users (username, fullname, email, phone, password, category)
    VALUES (%s, %s, %s, %s, %s, %s)
'''
cur.executemany(query, data)

query = '''
    INSERT INTO externalparticipants (username, fullname, email, phone, password, category)
    VALUES (%s, %s, %s, %s, %s, %s)
'''
cur.executemany(query, data)




conn.commit()
cur.close()
conn.close()

