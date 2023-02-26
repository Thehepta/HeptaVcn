package socks5Client

import (
	"io"
	"log"
)

// Request is the request packet
type Request struct {
	Ver     byte
	Cmd     byte
	Rsv     byte // 0x00
	Atyp    byte
	DstAddr []byte
	DstPort []byte // 2 bytes
}

// NegotiationRequest is the negotiation reqeust packet
type NegotiationRequest struct {
	Ver      byte
	NMethods byte
	Methods  []byte // 1-255 bytes
}

func NewRequest(cmd byte, atyp byte, dstaddr []byte, dstport []byte) *Request {
	if atyp == ATYPDomain {
		dstaddr = append([]byte{byte(len(dstaddr))}, dstaddr...)
	}
	return &Request{
		Ver:     Ver,
		Cmd:     cmd,
		Rsv:     0x00,
		Atyp:    atyp,
		DstAddr: dstaddr,
		DstPort: dstport,
	}
}

// WriteTo write request packet into server
func (r *Request) WriteTo(w io.Writer) (int64, error) {
	i, err := w.Write(append(append([]byte{r.Ver, r.Cmd, r.Rsv, r.Atyp}, r.DstAddr...), r.DstPort...))
	if err != nil {
		return 0, err
	}
	if Debug {
		log.Printf("Sent Request: %#v %#v %#v %#v %#v %#v\n", r.Ver, r.Cmd, r.Rsv, r.Atyp, r.DstAddr, r.DstPort)
	}
	return int64(i), nil
}

// WriteTo write negotiation request packet into server
func (r *NegotiationRequest) WriteTo(w io.Writer) (int64, error) {
	i, err := w.Write(append([]byte{r.Ver, r.NMethods}, r.Methods...))
	if err != nil {
		return 0, err
	}
	if Debug {
		log.Printf("Sent NegotiationRequest: %#v %#v %#v\n", r.Ver, r.NMethods, r.Methods)
	}
	return int64(i), nil
}

func NewNegotiationRequest(methods []byte) *NegotiationRequest {
	return &NegotiationRequest{
		Ver:      Ver,
		NMethods: byte(len(methods)),
		Methods:  methods,
	}
}
