#!/bin/bash
# Example Custom script for barometer protoype
# Prerequired applications:
#   - jq
#   - mail (configured for example with msmtp)

# Load config
source config.sh
# Request the data from api
json_data=$(curl -s -G --data-urlencode "device_id=$DEVICE_ID" --data-urlencode "num=1" "$HOST:$PORT/data/get")
# Parse data from json
device_id=$(echo $json_data | jq '.data[0].device_id')
temperature=$(echo $json_data | jq '.data[0].data.temperature')
pressure=$(echo $json_data | jq '.data[0].data.pressure')
# Send data via mail (mail has to be configured)  
printf "Neuste Messwerte vom Gerät $device_id:\nTemperatur: $temperature Celcius\nLuftdruck: $pressure Pascal" | mail -s "Neuste Messwerte" $EMAIL
