package socks5Client

import (
	"bytes"
	"encoding/binary"
	"errors"
	"io"
	"log"
	"net"
	"strconv"
)

// Reply is the reply packet
type Reply struct {
	Ver  byte
	Rep  byte
	Rsv  byte // 0x00
	Atyp byte
	// CONNECT socks server's address which used to connect to dst addr
	// BIND ...
	// UDP socks server's address which used to connect to dst addr
	BndAddr []byte
	// CONNECT socks server's port which used to connect to dst addr
	// BIND ...
	// UDP socks server's port which used to connect to dst addr
	BndPort []byte // 2 bytes
}

// NegotiationReply is the negotiation reply packet
type NegotiationReply struct {
	Ver    byte
	Method byte
}

// Address return request address like ip:xx
func (r *Reply) Address() string {
	var s string
	if r.Atyp == ATYPDomain {
		s = bytes.NewBuffer(r.BndAddr[1:]).String()
	} else {
		s = net.IP(r.BndAddr).String()
	}
	p := strconv.Itoa(int(binary.BigEndian.Uint16(r.BndPort)))
	return net.JoinHostPort(s, p)
}

func NewNegotiationReplyFrom(r io.Reader) (*NegotiationReply, error) {
	bb := make([]byte, 2)
	if _, err := io.ReadFull(r, bb); err != nil {
		return nil, err
	}
	if bb[0] != Ver {
		return nil, errors.New("Invalid Version")
	}
	if Debug {
		log.Printf("Got NegotiationReply: %#v %#v\n", bb[0], bb[1])
	}
	return &NegotiationReply{
		Ver:    bb[0],
		Method: bb[1],
	}, nil
}

// NewReplyFrom read reply packet from server
func NewReplyFrom(r io.Reader) (*Reply, error) {
	bb := make([]byte, 4)
	if _, err := io.ReadFull(r, bb); err != nil {
		return nil, err
	}
	if bb[0] != Ver {
		return nil, ErrVersion
	}
	var addr []byte
	if bb[3] == ATYPIPv4 {
		addr = make([]byte, 4)
		if _, err := io.ReadFull(r, addr); err != nil {
			return nil, err
		}
	} else if bb[3] == ATYPIPv6 {
		addr = make([]byte, 16)
		if _, err := io.ReadFull(r, addr); err != nil {
			return nil, err
		}
	} else if bb[3] == ATYPDomain {
		dal := make([]byte, 1)
		if _, err := io.ReadFull(r, dal); err != nil {
			return nil, err
		}
		if dal[0] == 0 {
			return nil, ErrBadReply
		}
		addr = make([]byte, int(dal[0]))
		if _, err := io.ReadFull(r, addr); err != nil {
			return nil, err
		}
		addr = append(dal, addr...)
	} else {
		return nil, ErrBadReply
	}
	port := make([]byte, 2)
	if _, err := io.ReadFull(r, port); err != nil {
		return nil, err
	}
	if Debug {
		log.Printf("Got Reply: %#v %#v %#v %#v %#v %#v\n", bb[0], bb[1], bb[2], bb[3], addr, port)
	}
	return &Reply{
		Ver:     bb[0],
		Rep:     bb[1],
		Rsv:     bb[2],
		Atyp:    bb[3],
		BndAddr: addr,
		BndPort: port,
	}, nil
}
