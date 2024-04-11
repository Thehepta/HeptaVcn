

### HeptaVcn (virtual capturp net)

这是一个抓门用于配合进行vpn抓包的工具，在基础的socks代理和http代理的基础上，并拓展了类似pptp协议这样的ip包转发功能。

### 工作原理
通过在android创建vpn,将流泪转发到pc端代理器上（burpsite charles fiddler）,android上使用的是tun2socks开源库


### 提供功能
+ 白名单模式：单个app代理，多个app代理
+ 黑名单模式：禁止某个app使用代理，禁止某几个app使用代理
+ socks代理
+ http代理
+ ip转发（pptp的简单版）


### 使用ip转发功能所需要的(ip-reflector)
如果使用ip协议代理需要配置网卡，并使用我提供的linux pc 上的程序运行，让手机连接上linux  pc程序
+ linux 网卡创建
```
sudo ip tuntap add tun111 mode tun
sudo ip addr add 10.0.0.1/30 dev tun111
sudo ip link set dev tun111 up
sudo iptables -t nat -A POSTROUTING -o enp89s0 -j MASQUERADE
 // iptables -t nat -A POSTROUTING -s 192.168.0.0/24 -j MASQUERADE  匹配来源地址IP/MASK，加叹号"!"表示除这个IP外。
sudo iptables -I FORWARD -i tun111 -o enp89s0 -j ACCEPT
sudo iptables -I FORWARD -i enp89s0 -o tun111 -j ACCEPT
sudo ifconfig tun111 mtu 1400 up

```
+ linux pc 程序运行在同一个局域网（好像需要root，编译Tunnel_server.cpp）

如果使用ip协议代理，需要注意mtu。

真实网卡的mtu一般是1500，这个数值代表一个真实的ip包的大小 = mtu(1500)-以太网层头

需要在linux电脑上创建一个网卡配合使用，并配置iptable规则


手机上连接到本地地址就可以了

假如你修改了tun网卡的配置，比如ip地址，你需要确保手机上虚拟网卡的地址，和电脑上网卡的地址在一个网段。


### MoveCertificate
这个工具好像有点问题，需要手工想证书的路径复制到系统证书路径


### 测试
提供了一个app测试程序，会向百度发送https请求,用于对https证书配置环境进行测试


然后直接抓pc端的网卡就可以了。
# 致谢
https://github.com/xjasonlyu/tun2socks
