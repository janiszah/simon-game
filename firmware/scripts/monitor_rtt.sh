#!/bin/sh

GIT_ROOT=$(git rev-parse --show-toplevel) # Get project root directory
CONFIG="interface/stlink.cfg -f target/stm32g0x.cfg"       # OpenOCD config
MAP_FILE="output.map"                   # Map file name
PORT=4450                                 # RTT port
HELP=0

# Get RTT RAM addres from MAP file
# RTT_ADDRESS=$(awk '/_SEGGER_RTT/{i++}i==2{print $1; exit}' "${GIT_ROOT}/firmware/build/${MAP_FILE}")
RTT_ADDRESS=0x20000000

# Parse arguments
for arg in "$@"
do
    case $arg in
        -p|--port)
        PORT="$2"
        shift
        shift
        ;;
        -c|--config)
        CONFIG="$2"
        shift
        shift
        ;;
        --m|--map)
        MAP_FILE="$2"
        shift
        shift
        ;;
        -h|--help)
        HELP=1
        shift
        ;;
    esac
done

show_help() {
    echo "RTT Monitor\n"
    echo "Usage: rtt_monitor.sh [OPTION]\n"
    echo "  -p, --port        : Telent port for RTT channel"
    echo "  -c, --config      : OpenOCD config file"
    echo "  -m, --map         : Firmware .map file"
    echo "  -h, --help        : Displays this help"
    echo "\nScript requires OpenOCD 0.11.0!"
}

if [ $HELP -eq 1 ]; then
    show_help
    exit 0
fi

openocd -f ${CONFIG} -c "init" -c "rtt setup ${RTT_ADDRESS} 0x8000 \"SEGGER RTT\"" -c "rtt start" -c "rtt server start ${PORT} 0" &
openocd_pid=$!

res=1
while [ $res -eq 1 ]; do
    sleep 1
    echo "Trying to connect to RTT..."
    telnet 127.0.0.1 ${PORT}
    res=$?
    telnet_pid=$!
done

echo "Kill OpenOCD ${openocd_pid}"
kill $openocd_pid