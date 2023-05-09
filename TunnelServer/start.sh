sudo ip rule add fwmark 1 table 100
sudo ip route add local 0.0.0.0/0 dev lo table 100

sudo ip tuntap add tun111 mode tun
sudo ip addr add 10.0.0.1/30 dev tun111
sudo ip link set dev tun111  mtu 1500 up

sudo iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 90 -j MARK --set-mark 1

iptables -A OUTPUT  -p tcp -m owner --uid-owner 10170 -j MARK --set-mark 1