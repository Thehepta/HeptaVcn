package main

import (
	"bytes"
	"fmt"
	"net"
	"os"
)

func main1() {
	ln, err := net.Listen("tcp", "192.168.0.105:50000")
	checkError(err)
	for {
		conn, err := ln.Accept()
		checkError(err)
		go readFully(conn)

	}
}

func readFully1(conn net.Conn) (string, error) {
	fmt.Print("readFully start")

	readBytes := make([]byte, 1600)
	var buffer bytes.Buffer
	for {
		nlen, err := conn.Read(readBytes)
		if err != nil {
			return "", err
		}
		fmt.Printf("readFully continue%d\n", nlen)

		readByte := readBytes[0]
		if readByte == '\t' {
			break
		}
		buffer.WriteByte(readByte)
	}

	return buffer.String(), nil
}

func checkError1(err error) {
	if err != nil {
		fmt.Fprint(os.Stderr, "fatal error %s", err.Error())
		os.Exit(1)
	}
}
