import time
import threading
from pprint import pprint
from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse
import json
from datetime import datetime

verbose = False
database_wrapper = None
hostname = None
port = None

api_webserver = None

# Parse a get query and return it as dict (e.g: device_id=DEVICE_1&num=5 => { "device_id": "DEVICE_1", "num": "5" })
def parse_get_query(query):
    args = { }
    if query == "":
        return args
    entries = query.split("&")
    for entry in entries:
        splitted = entry.split("=")
        # Nothing to do, because no data
        if len(splitted) < 1:
            continue
        # len 1 means no key value pair, instead only a key
        if len(splitted) == 1:
            key = splitted[0]
            args[key] = None
            continue
        # Key value pair existing
        if len(splitted) >= 2:
            key = splitted[0]
            # Unqote data so spaces etc are correctly
            value = urllib.parse.unquote(splitted[1])
            args[key] = value
            continue
    return args

class APIServerRequestHandler(BaseHTTPRequestHandler):
    # Handle Get requests
    def do_GET(self):
        if verbose:
            print("APIServer: Received GET request: " + self.path)
        # Remove get args from path
        parsed_url = urllib.parse.urlparse(self.path)
        path = parsed_url.path
        query = parsed_url.query
        args = parse_get_query(query)
        # Get args: device_id, num
        # /data/get?device_id=deviceid&num=5
        # ToDo: make device_id optional, return all when not device_id is given
        #       Optional: paging
        if path == "/data/get":
            # Validate args
            error = False
            error_msg = ""
            if not 'device_id' in args:
                error = True
                error_msg = "Required argument 'device_id' not set."
            if not error and 'num' in args:
                try:
                    int(args['num'])
                except:
                    error = True
                    error_msg = "Defined argument 'num' is not valid."
            if not error and 'since' in args:
                try:
                    datetime.strptime(args['since'], '%Y-%m-%dT%H:%M:%S')
                except ValueError:
                    error = True
                    error_msg = "Datetime format of argument 'since' is not valid. Valid format: 'YYYY-MM-DDTHH:mm:ss' (UTC)"
            
            # Return correct response when request was valid
            if not error:
                device_id = args['device_id']
                num = args.get('num', None)
                # Convert num
                if not num == None:
                    num = int(num)
                    if num == 0:
                        num = None
                since = args.get('since', None)
                # Get data from db
                db_data = database_wrapper.get_received_data(device_id, num, since)
                # Create response with loaded data
                response = {
                    'success': True,
                    'data': db_data
                }
                response_json = json.dumps(response)
                self.send_response(200)
                self.send_header("Content-type", "application/json")
                self.end_headers()
                self.wfile.write(bytes(response_json, encoding='utf8'))
            # Return error if given request is invalid
            else:
                response = {
                    'success': False,
                    'error': error_msg
                }
                response_json = json.dumps(response)
                # Error code: 400 Bad Request
                self.send_response(400)
                self.send_header("Content-type", "application/json")
                self.end_headers()
                self.wfile.write(bytes(response_json, encoding='utf8'))

def init(_database_wrapper, _hostname, _port, _verbose):
    global verbose, database_wrapper, hostname, port
    verbose = _verbose
    database_wrapper = _database_wrapper
    hostname = _hostname
    port = _port

def start():
    global api_webserver
    api_webserver = HTTPServer((hostname, port), APIServerRequestHandler)
    if verbose:
        print("API Webserver started at: http://%s:%s" % (hostname, port))
    # Start webserver in thread so it's not blocking
    threading.Thread(target=api_webserver.serve_forever, daemon=True).start()
   
def stop():
    if api_webserver is not None:
        api_webserver.server_close()
        if verbose:
            print("API Webserver stopped")

