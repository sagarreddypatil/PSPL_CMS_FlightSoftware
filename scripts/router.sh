echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -A FORWARD -i enp0s13f0u3 -o wlan0 -j ACCEPT
iptables -A FORWARD -i wlan0 -o enp0s13f0u3 -m state --state ESTABLISHED,RELATED \
         -j ACCEPT
iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE

