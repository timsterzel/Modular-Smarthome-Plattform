# Documentation

## REST API
With the REST API, data can be retrieved with GET request. The response is in JSON Format.  
**Url:**  
```
/data/get
```
**Required parameters:**
- ```device_id```  

**Optional parametes:**
- ```num```   
Limit data. "0" for no limit
- ```since```  
Only retrieve entries newer or equal given date in UTC Time.  
Format: YYYY-MM-DDTHH:MM:SS

**Response:**
```
{
  "success": true,
  "data": [
    {
      "device_id": "MyDevice1",
      "time_received_utc": "2022-07-09T18:54:33",
      "data": {
        [...]
      }
    },
    [...]
  ]
}
```
Example calls:  
```
/data/get?device_id=MyDevice1&num=5
/data/get?device_id=MyDevice1&since=2022-06-09T15:00:00
```

## Adding of new Communication Modules
Communication modules has to be places inside the ```modules/communication/``` dir. The Central Application will automatically include and start implemented modules when they match the required format.  

## Creation of new Communication Modules
To Create an own communication module it has to follow some rules. The module needs a script ```main.py``` inside the modules root directory. Inside the script the following functions have to be implemenetd:  
```
# Called on initializing
def init(_data_received_callback):
    ...

# Called to start the module
# This function should not block main thread
def run():
    ....

# Called when modules is stopped
def stop():
    ....
```
The init function gets the callable ```_data_received_callback``` function as argument. And has to be called from the module whenever the module received data. The original function looks like this:  
```
on_data_received(device_id, data)
```
The Central Application will then take care for the received data and call defined events etc.
