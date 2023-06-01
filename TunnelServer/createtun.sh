
sudo ip tuntap add tun111 mode tun
sudo ip addr add 10.0.0.1/30 dev tun111
sudo ip link set dev tun111  mtu 1400 up

sudo iptables -t nat -A POSTROUTING -o enp89s0 -j MASQUERADE
sudo iptables -I FORWARD -i tun111 -o enp89s0 -j ACCEPT
sudo iptables -I FORWARD -i enp89s0 -o tun111 -j ACCEPT