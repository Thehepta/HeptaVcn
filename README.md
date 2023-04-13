如果使用ip协议代理，需要注意mtu。
真实网卡的mtu一般是1500，这个数值代表一个真实的ip包的大小 = mtu(1500)-以太网层头

需要创建一个网卡配合使用，并配置iptable规则

sudo ip tuntap add tun111 mode tun
sudo ip addr add 10.0.0.2/30 dev tun111
sudo ip link set dev tun111 up
sudo iptables -t nat -A POSTROUTING -o enp89s0 -j MASQUERADE
 // iptables -t nat -A POSTROUTING -s 192.168.0.0/24 -j MASQUERADE  匹配来源地址IP/MASK，加叹号"!"表示除这个IP外。
sudo iptables -I FORWARD -i tun111 -o enp89s0 -j ACCEPT   
sudo iptables -I FORWARD -i enp89s0 -o tun111 -j ACCEPT
sudo ifconfig tun111 mtu 1400 up
