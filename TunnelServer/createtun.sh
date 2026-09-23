
TUN=tun111

# ---------- 动态探测当前上网网卡 ----------
WAN_IF=$(ip route get 8.8.8.8 | awk '{for(i=1;i<=NF;i++) if($i=="dev"){print $(i+1); exit}}')
[ -n "$WAN_IF" ] || { echo "无法探测上网网卡"; exit 1; }
echo "当前上网网卡: $WAN_IF"

# ---------- 创建 TUN ----------
sudo ip tuntap add $TUN mode tun
sudo ip addr add 10.0.0.1/30 dev $TUN
sudo ip link set dev $TUN mtu 1400 up

# ---------- NAT + 转发（出口用动态变量） ----------
sudo iptables -t nat -A POSTROUTING -o "$WAN_IF" -j MASQUERADE
sudo iptables -I FORWARD -i $TUN      -o "$WAN_IF" -j ACCEPT
sudo iptables -I FORWARD -i "$WAN_IF" -o $TUN      -j ACCEPT

echo "配置完成，出口网卡: $WAN_IF"