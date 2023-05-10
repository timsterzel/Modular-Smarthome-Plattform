#!/bin/bash
# Example Custom script for barometer protoype
# Prerequired applications:
#   - jq
#   - mail (configured for example with msmtp)

echo "Event Skript start"
# Load config
# Script is started from CentralDecive, so the absolute path for loading config is necessary
# As the config is inside the folder of the script, the absolute path (root path) to the script is needed
ROOT_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
source "$ROOT_PATH/config.sh"

# Parse Args
device_id=""
time_received_utc=""
data=""
parse_device_id=False
parse_data=false
parse_time_received_utc=false
for arg in "$@"
do
    # Decide what data to parse
    if [ "$arg" = "--device_id" ] ; then
        parse_device_id=true
    elif [ "$arg" = "--time_received_utc" ] ; then
        parse_time_received_utc=true
    elif [ "$arg" = "--data" ] ; then
        parse_data=true
    # Parse data
    elif [ "$parse_device_id" = true ] ; then
        device_id="$arg"
        parse_device_id=false
    elif [ "$parse_time_received_utc" = true ] ; then
        time_received_utc="$arg"
        parse_time_received_utc=false
    elif [ "$parse_data" = true ] ; then
        data="$arg"
        parse_data=false
    fi
done
# Parse json data (key 'event')
is_event=$(echo $data | jq '.event')
# Send email if event
if [ "$is_event" = "true" ]; then
    # Send data via mail (mail has to be configured)  
    printf "Event vom Gerät '$device_id' um $time_received_utc UTC aufgetreten" | mail -s "Event von $device_id" $EMAIL
fi