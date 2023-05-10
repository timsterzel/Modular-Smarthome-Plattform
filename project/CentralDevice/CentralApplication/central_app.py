# https://pythonbasics.org/webserver/
import os
import time
import threading
import queue
import json
import subprocess
from datetime import datetime
from pprint import pprint
from http.server import BaseHTTPRequestHandler, HTTPServer
import atexit
from pathlib import Path
import config.config as config
from core.databasewrapper import DatabaseWrapper
import core.apiserver as APIServer

SCRIPT_ABSOLUTE_PATH = os.path.dirname(os.path.realpath(__file__))
# Store data under home dir under 'SMCentralApplication'
DATA_ABSOLUTE_PATH = str(Path.home()) + "/.CentralApplication/data"
DATABASE_PATH = DATA_ABSOLUTE_PATH + "/data.db"
MODULE_PATH = "modules"

# Create data path
Path(DATA_ABSOLUTE_PATH).mkdir(parents=True, exist_ok=True)

# Use thread safe queue to write data into sqlite database later (in a safe way)
received_data_queue = queue.Queue()
database_wrapper = DatabaseWrapper(DATABASE_PATH)

# Event called by communication modules after data was received
# Depending on communication module implementation
# Attention: when called from Threads, the call is also happening on that thread (not on main thread!)
def on_data_received(device_id, data):
    received_time_utc = datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%S")
    # Call custom script when assigned
    if device_id in config.REGISTERED_SCRIPT_EVENT_CALLS:
        data_json = json.dumps(data)
        script_to_call = config.REGISTERED_SCRIPT_EVENT_CALLS[device_id]
        if config.VERBOSE:
            print("Call custom script: " + script_to_call)
        process_args = [
            script_to_call,
            '--device_id', device_id,
            '--time_received_utc', received_time_utc,
            '--data', data_json
        ]
        try:
            # Start the custom script as subprocess (in own thread, so it is not blocking)
            subprocess.Popen(process_args)
        except Exception:
            print("ERROR: Can not start subprocess: " + script_to_call)
    # Store recevied data in queue
    received_data_queue.put({'device_id': device_id, 'time_utc': received_time_utc, 'data' : data })

def exit_application():
    global APIServer
    print("Stop...")
    APIServer.stop()
    for module in loaded_communication_modules:
        module.stop()
# Call exit function when programm gets closed or is terminated
atexit.register(exit_application)

loaded_communication_modules = []
# Init and load communication modules
for module_name in config.COMMUNICATION_MODULES_TO_LOAD:
    if config.VERBOSE:
        print("Load module: " + module_name)
    module_script_path = MODULE_PATH + ".communication." + module_name + ".main"
    loaded_module = __import__(module_script_path, fromlist=[''])
    loaded_module.init(on_data_received)
    loaded_communication_modules.append(loaded_module)

# Run communication modules
for module in loaded_communication_modules:
    thread = threading.Thread(target=module.run())
    thread.daemon = True
    thread.start()

# Start API Server
APIServer.init(database_wrapper, config.HOSTNAME, config.PORT, config.VERBOSE)
APIServer.start()

running = True
while(running):
    # Get received data (FIFO) and store them in database
    while not received_data_queue.empty():
        received_data_entry = received_data_queue.get()
        database_wrapper.add_received_data(received_data_entry['device_id'], received_data_entry['time_utc'], received_data_entry['data'])
    # Sleep for 10ms to prevent unnecessary high cpu resources
    # Time Critical events are handled in own threads anyway which are not affected by sleep
    time.sleep(0.01)
exit_application()