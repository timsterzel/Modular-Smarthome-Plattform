import time
import threading
from pprint import pprint
from http.server import BaseHTTPRequestHandler, HTTPServer
import json
# Import local config
from .config import config

MODULE_NAME = "ESP-WiFi-Communicator"
on_data_received_callback = None

class CentralDeviceServer(BaseHTTPRequestHandler):
    # Handle POST requests
    def do_POST(self):
        if config.VERBOSE:
            print("Received POST request: " + self.path)
        if (self.path == "/receiver"):
            content_length = int(self.headers['Content-Length']) 
            post_data_str = self.rfile.read(content_length)
            received_data = json.loads(post_data_str)
            # Send response to client
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(bytes("<html>Received</html>", "utf-8"))
            if config.VERBOSE:
                print("##############################")
                print("Received POST Data:")
                pprint(received_data)
                print("##############################")
            # Handle received data
            device_id = received_data['device_id']
            # Remove device id from repsonse as it's no real data
            received_data.pop('device_id')
            on_data_received_callback(device_id, received_data) # DEBUG

webserver = None


# Must to be implemented!
def init(_data_received_callback):
    global on_data_received_callback
    if config.VERBOSE:
        print("Init module: " + MODULE_NAME)
    on_data_received_callback = _data_received_callback

# This method should not block main thread and webservers etc have to run in background
# Must to be implemented!
def run():
    global webserver
    if config.VERBOSE:
        print("Run module: " + MODULE_NAME)
    webserver = HTTPServer((config.HOSTNAME, config.PORT), CentralDeviceServer)
    if config.VERBOSE:
        print("ESP Communication Webserver started at: http://%s:%s" % (config.HOSTNAME, config.PORT))
    # Start webserver in thread so it's not blocking
    threading.Thread(target=webserver.serve_forever, daemon=True).start()

# Must to be implemented!
def stop():
    global webserver
    if config.VERBOSE:
        print("Stop module: " + MODULE_NAME)
    if webserver is not None:
        webserver.server_close()
        if config.VERBOSE:
            print("ESP Communication Webserver stopped")
