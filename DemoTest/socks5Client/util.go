package socks5Client

import (
	"encoding/binary"
	"net"
	"strconv"
)

var Resolve func(network string, addr string) (net.Addr, error) = func(network string, addr string) (net.Addr, error) {
	if network == "tcp" {
		return net.ResolveTCPAddr("tcp", addr)
	}
	return net.ResolveUDPAddr("udp", addr)
}

var DialTCP func(network string, laddr, raddr string) (net.Conn, error) = func(network string, laddr, raddr string) (net.Conn, error) {
	var la, ra *net.TCPAddr
	if laddr != "" {
		var err error
		la, err = net.ResolveTCPAddr(network, laddr)
		if err != nil {
			return nil, err
		}
	}
	a, err := Resolve(network, raddr)
	if err != nil {
		return nil, err
	}
	ra = a.(*net.TCPAddr)
	return net.DialTCP(network, la, ra)
}

var DialUDP func(network string, laddr, raddr string) (net.Conn, error) = func(network string, laddr, raddr string) (net.Conn, error) {
	var la, ra *net.UDPAddr
	if laddr != "" {
		var err error
		la, err = net.ResolveUDPAddr(network, laddr)
		if err != nil {
			return nil, err
		}
	}
	a, err := Resolve(network, raddr)
	if err != nil {
		return nil, err
	}
	ra = a.(*net.UDPAddr)
	return net.DialUDP(network, la, ra)
}

func ParseAddress(address string) (a byte, addr []byte, port []byte, err error) {
	var h, p string
	h, p, err = net.SplitHostPort(address)
	if err != nil {
		return
	}
	ip := net.ParseIP(h)
	if ip4 := ip.To4(); ip4 != nil {
		a = ATYPIPv4
		addr = []byte(ip4)
	} else if ip6 := ip.To16(); ip6 != nil {
		a = ATYPIPv6
		addr = []byte(ip6)
	} else {
		a = ATYPDomain
		addr = []byte{byte(len(h))}
		addr = append(addr, []byte(h)...)
	}
	i, _ := strconv.Atoi(p)
	port = make([]byte, 2)
	binary.BigEndian.PutUint16(port, uint16(i))
	return
}
