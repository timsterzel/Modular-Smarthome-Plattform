import json
import sqlite3
from pprint import pprint

class DatabaseWrapper:
    def __init__(self, db_file):
        # Set check_same_thread=False, so connection can be used in multiple threads at the same time
        self.db_connection = sqlite3.connect(db_file, check_same_thread=False)
        # Use sqlite3.Row to access row data by column name
        self.db_connection.row_factory = sqlite3.Row
        self.create_table_received_data_if_not_exists()

    def __del__(self):
        self.db_connection.close()
    
    def get_connection(self):
        return self.db_con

    # General helper methods
    def is_table_existing(self, table_name):
        # Get count of tables with given name
        sql = """
            SELECT count(name) FROM sqlite_master WHERE type='table' AND name=?
        """
        cursor = self.db_connection.cursor()
        values = [table_name]
        cursor.execute(sql, values)
        # If count is >= 1 table exists
        if cursor.fetchone()[0] >= 1:
            self.db_connection.commit()
            return True
        self.db_connection.commit()
        return False
    
    def execute_single_insert(self, sql, values = ()):
        cursor = self.db_connection.cursor()
        cursor.execute(sql, values)
        id = cursor.lastrowid
        self.db_connection.commit()
        cursor.close()
        return id

    def execute_single_select(self, sql, values = ()):
        cursor = self.db_connection.cursor()
        cursor.execute(sql, values)
        data = cursor.fetchone()
        self.db_connection.commit()
        cursor.close()
        return data

    def execute_single_update(self, sql, values = ()):
        cursor = self.db_connection.cursor()
        cursor.execute(sql, values)
        id = cursor.lastrowid
        self.db_connection.commit()
        cursor.close()
        return id

    def execute_multi_select(self, sql, values = ()):
        cursor = self.db_connection.cursor()
        cursor.execute(sql, values)
        rows = cursor.fetchall()
        self.db_connection.commit()
        cursor.close()
        return rows

    def execute_single_delete(self, sql, values= ()):
        cursor = self.db_connection.cursor()
        cursor.execute(sql, values)
        self.db_connection.commit()
        cursor.close()

    # Specifric methods
    def create_table_received_data_if_not_exists(self):
        self.db_connection.execute("""
            CREATE TABLE IF NOT EXISTS received_data (
                id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                device_id TEXT,
                data_json TEXT,
                time_received_utc TEXT
            );
        """)

    # Public used methods
    # Time format: "2020-01-01T15:30:36" (YYYY-MM-DDTHH:MM:SS)
    # Data has to be string (json is recommended)
    def add_received_data(self, device_id, time_received_utc, data):
        # Data is stored as json string
        data_json = json.dumps(data)
        sql = """
            INSERT INTO received_data (
                device_id, data_json, time_received_utc
            )
            VALUES (
                  ?, ?, ?
            )
        """
        values = (device_id, data_json, time_received_utc)
        return self.execute_single_insert(sql, values)
    
    # Returns the newest num received data entries from db
    # If no num is set return all
    # datetime_utc_since format: YYYY-MM-DDTHH:mm:ss in utc
    #   Returns only entries after (or same time) specified time 
    def get_received_data(self, device_id, num = None, datetime_utc_since = None):
        #sql = "SELECT device_id, data_json, time_received_utc FROM received_data WHERE device_id = ? ORDER BY id DESC"
        # Basic sql request
        sql = "SELECT device_id, data_json, time_received_utc FROM received_data WHERE device_id = ?"
        values = [device_id]
        # Append sql for comparing datetime if set
        if not datetime_utc_since == None:
            sql += " AND time_received_utc >= ?"
            values.append(datetime_utc_since)
        # Order data
        sql += " ORDER BY id DESC"
        # Append sql for limiting request if a limit is set
        if not num == None:
            # Take care it is a real number, to prevent any sql injections when using the number in sql statement directly
            num_save = 0
            try:
                num_save = int(num)
            except:
                num_save = 0
            sql += " LIMIT " + str(num_save) + ""

        data = self.execute_multi_select(sql, values)
        data_parsed = []
        for entry in data:
            entry_parsed = { }
            entry_parsed['device_id'] = entry['device_id']
            entry_parsed['time_received_utc'] = entry['time_received_utc']
            entry_parsed['data'] = json.loads(entry['data_json'])
            data_parsed.append(entry_parsed)
        return data_parsed