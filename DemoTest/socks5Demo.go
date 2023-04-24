package main

import (
	"bytes"
	"fmt"
	"io"
	"net"
	"os"
	"os/signal"
	"syscall"
)

func main() {

	//client, _ := socks5Client.NewClient("127.0.0.1:8889", "", "", 300, 300)
	//conn, err := client.Dial("tcp", "www.baidu.com:80")
	//checkError(err)
	//_, err = conn.Write([]byte("HEAD / HTTP/1.0\r\n\r\n"))
	//checkError(err)
	//
	//// 通过连接对象提供的 Read 方法读取所有响应数据
	//result, err := readFully(conn)
	//checkError(err)
	//
	//// 打印响应数据
	//fmt.Println(string(result))

	sigCh := make(chan os.Signal, 1)
	signal.Notify(sigCh, syscall.SIGINT, syscall.SIGTERM)
	<-sigCh
}

func readFully(conn net.Conn) ([]byte, error) {
	// 读取所有响应数据后主动关闭连接
	defer conn.Close()

	result := bytes.NewBuffer(nil)
	var buf [512]byte
	for {
		n, err := conn.Read(buf[0:])
		result.Write(buf[0:n])
		if err != nil {
			if err == io.EOF {
				break
			}
			return nil, err
		}
	}
	return result.Bytes(), nil
}

func checkError(err error) {
	if err != nil {
		fmt.Fprintf(os.Stderr, "Fatal error: %s", err.Error())
		os.Exit(1)
	}
}
