RUN=15 # active low, pull low to reset
BOOTSEL=14 # active low, pull low to bootsel

# configure pins as output
raspi-gpio set $RUN op
raspi-gpio set $BOOTSEL op

# Base state
raspi-gpio set $RUN dh
raspi-gpio set $BOOTSEL dh

# Reset the board
raspi-gpio set $RUN dl
sleep 0.1
raspi-gpio set $RUN dh
sleep 0.1

