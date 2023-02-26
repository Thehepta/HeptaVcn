package socks5Client

import (
	"errors"
	"log"
	"net"
	"time"
)

// Client is socks5 client wrapper
type Client struct {
	Server   string
	UserName string
	Password string
	// On cmd UDP, let server control the tcp and udp connection relationship
	TCPConn       net.Conn
	UDPConn       net.Conn
	RemoteAddress net.Addr
	TCPTimeout    int
	UDPTimeout    int
	Dst           string
}

func (c *Client) Read(b []byte) (int, error) {
	//TODO implement me
	if c.UDPConn == nil {
		return c.TCPConn.Read(b)
	}
	n, err := c.UDPConn.Read(b)
	if err != nil {
		return 0, err
	}
	d, err := NewDatagramFromBytes(b[0:n])
	if err != nil {
		return 0, err
	}
	n = copy(b, d.Data)
	return n, nil

}

func (c *Client) Write(b []byte) (int, error) {
	if c.UDPConn == nil {
		return c.TCPConn.Write(b)
	}
	a, h, p, err := ParseAddress(c.Dst)
	if err != nil {
		return 0, err
	}
	if a == ATYPDomain {
		h = h[1:]
	}
	d := NewDatagram(a, h, p, b)
	b1 := d.Bytes()
	n, err := c.UDPConn.Write(b1)
	if err != nil {
		return 0, err
	}
	if len(b1) != n {
		return 0, errors.New("not write full")
	}
	return len(b), nil
}

func (c *Client) Close() error {
	if c.UDPConn == nil {
		return c.TCPConn.Close()
	}
	if c.TCPConn != nil {
		c.TCPConn.Close()
	}
	return c.UDPConn.Close()
}

func (c *Client) LocalAddr() net.Addr {
	if c.UDPConn == nil {
		return c.TCPConn.LocalAddr()
	}
	return c.UDPConn.LocalAddr()
}

func (c *Client) RemoteAddr() net.Addr {
	return c.RemoteAddress
}

func (c *Client) SetDeadline(t time.Time) error {
	if c.UDPConn == nil {
		return c.TCPConn.SetDeadline(t)
	}
	return c.UDPConn.SetDeadline(t)
}

func (c *Client) SetReadDeadline(t time.Time) error {
	if c.UDPConn == nil {
		return c.TCPConn.SetReadDeadline(t)
	}
	return c.UDPConn.SetReadDeadline(t)
}

func (c *Client) SetWriteDeadline(t time.Time) error {
	if c.UDPConn == nil {
		return c.TCPConn.SetWriteDeadline(t)
	}
	return c.UDPConn.SetWriteDeadline(t)
}

func NewClient(addr, username, password string, tcpTimeout, udpTimeout int) (*Client, error) {
	c := &Client{
		Server:     addr,
		UserName:   username,
		Password:   password,
		TCPTimeout: tcpTimeout,
		UDPTimeout: udpTimeout,
	}
	return c, nil
}

func (c *Client) Dial(network, addr string) (net.Conn, error) {
	return c.DialWithLocalAddr(network, "", addr, nil)
}

func (c *Client) DialWithLocalAddr(network, src string, dst string, remoteAddr net.Addr) (net.Conn, error) {
	c = &Client{
		Server:        c.Server,
		UserName:      c.UserName,
		Password:      c.Password,
		TCPTimeout:    c.TCPTimeout,
		UDPTimeout:    c.UDPTimeout,
		Dst:           dst,
		RemoteAddress: remoteAddr,
	}

	var err error
	if network == "tcp" {
		var laddr net.Addr
		if src != "" {
			laddr, err = net.ResolveTCPAddr("tcp", src)
			if err != nil {
				return nil, err
			}
		}
		if err := c.Negotiate(laddr); err != nil {
			return nil, err
		}
		a, h, p, err := ParseAddress(dst)
		if err != nil {
			return nil, err
		}
		if a == ATYPDomain {
			h = h[1:]
		}
		if _, err := c.Request(NewRequest(CmdConnect, a, h, p)); err != nil {
			return nil, err
		}
		return c, nil
	}
	if network == "udp" {
		var laddr net.Addr
		if src != "" {
			laddr, err = net.ResolveTCPAddr("tcp", src)
			if err != nil {
				return nil, err
			}
		}
		if err := c.Negotiate(laddr); err != nil {
			return nil, err
		}

		src = c.TCPConn.LocalAddr().String()
		a, h, p, err := ParseAddress(src)
		if err != nil {
			return nil, err
		}
		if a == ATYPDomain {
			h = h[1:]
		}
		rp, err := c.Request(NewRequest(CmdUDP, a, h, p))
		if err != nil {
			return nil, err
		}
		c.UDPConn, err = DialUDP("udp", src, rp.Address())
		if err != nil {
			return nil, err
		}
		if c.UDPTimeout != 0 {
			if err := c.UDPConn.SetDeadline(time.Now().Add(time.Duration(c.UDPTimeout) * time.Second)); err != nil {
				return nil, err
			}
		}
		return c, nil
	}
	return nil, errors.New("unsupport network")

}

func (c *Client) Negotiate(laddr net.Addr) error {
	src := ""
	if laddr != nil {
		src = laddr.String()
	}
	var err error
	c.TCPConn, err = DialTCP("tcp", src, c.Server)
	if err != nil {
		return err
	}
	if c.TCPTimeout != 0 {
		if err := c.TCPConn.SetDeadline(time.Now().Add(time.Duration(c.TCPTimeout) * time.Second)); err != nil {
			return err
		}
	}
	m := MethodNone
	if c.UserName != "" && c.Password != "" {
		m = MethodUsernamePassword
	}
	rq := NewNegotiationRequest([]byte{m})
	if _, err := rq.WriteTo(c.TCPConn); err != nil {
		return err
	}
	rp, err := NewNegotiationReplyFrom(c.TCPConn)
	if err != nil {
		return err
	}

	if rp.Method != m {
		return errors.New("Unsupport method")
	}
	// 用户名和密码请求
	if m == MethodUsernamePassword {
		//urq := NewUserPassNegotiationRequest([]byte(c.UserName), []byte(c.Password))
		//if _, err := urq.WriteTo(c.TCPConn); err != nil {
		//	return err
		//}
		//urp, err := NewUserPassNegotiationReplyFrom(c.TCPConn)
		//if err != nil {
		//	return err
		//}
		//if urp.Status != UserPassStatusSuccess {
		//	return ErrUserPassAuth
		//}
	}
	return nil
}

func NewDatagramFromBytes(bb []byte) (*Datagram, error) {
	n := len(bb)
	minl := 4
	if n < minl {
		return nil, ErrBadRequest
	}
	var addr []byte
	if bb[3] == ATYPIPv4 {
		minl += 4
		if n < minl {
			return nil, ErrBadRequest
		}
		addr = bb[minl-4 : minl]
	} else if bb[3] == ATYPIPv6 {
		minl += 16
		if n < minl {
			return nil, ErrBadRequest
		}
		addr = bb[minl-16 : minl]
	} else if bb[3] == ATYPDomain {
		minl += 1
		if n < minl {
			return nil, ErrBadRequest
		}
		l := bb[4]
		if l == 0 {
			return nil, ErrBadRequest
		}
		minl += int(l)
		if n < minl {
			return nil, ErrBadRequest
		}
		addr = bb[minl-int(l) : minl]
		addr = append([]byte{l}, addr...)
	} else {
		return nil, ErrBadRequest
	}
	minl += 2
	if n <= minl {
		return nil, ErrBadRequest
	}
	port := bb[minl-2 : minl]
	data := bb[minl:]
	d := &Datagram{
		Rsv:     bb[0:2],
		Frag:    bb[2],
		Atyp:    bb[3],
		DstAddr: addr,
		DstPort: port,
		Data:    data,
	}
	if Debug {
		log.Printf("Got Datagram. data: %#v %#v %#v %#v %#v %#v datagram address: %#v\n", d.Rsv, d.Frag, d.Atyp, d.DstAddr, d.DstPort, d.Data, d.Address())
	}
	return d, nil
}

func (c *Client) Request(r *Request) (*Reply, error) {
	if _, err := r.WriteTo(c.TCPConn); err != nil {
		return nil, err
	}
	rp, err := NewReplyFrom(c.TCPConn)
	if err != nil {
		return nil, err
	}
	if rp.Rep != RepSuccess {
		return nil, errors.New("Host unreachable")
	}
	return rp, nil
}
