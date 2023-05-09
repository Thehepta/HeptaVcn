

ip rule add  priority 12000 iif tun111 table local_network
ip rule add  priority 13000 fwmark 0x0/0x20000 iif lo uidrange 0-99999 table 100
ip rule add  priority 13000 fwmark 0xc0089/0xcffff  table 100


ip rule add  priority 16000 fwmark 0x10089/0x1ffff iif lo uidrange 0-99999 table 100
ip rule add  priority 16000 fwmark 0x10089/0x1ffff iif lo uidrange 0-0 table 100

ip rule add  priority 17000 iif lo oif tun111 uidrange 0-99999  table 100

ip rule add  priority 26000 fwmark 0x0/0x10000 iif lo  table wlan0_local
ip rule add  priority 28000 fwmark 0x89/0xffff  table wlan0

ip route add proto static 10.0.0.0/30 dev tun111 table 100

ip route add proto kernel 10.0.0.0/30 dev tun111

ip tuntap add tun111 mode tun
ip addr add 10.0.0.2/30 dev tun111
ip link set dev tun111  mtu 1500 up

sudo iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 90 -j MARK --set-mark 1

iptables -A OUTPUT  -p tcp -m owner --uid-owner 10170 -j MARK --set-mark 1