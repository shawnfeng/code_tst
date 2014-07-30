package main

import (
	//"fmt"
	"log"
	"net"
	"push_server/tcpconn"

)


func main() {
	var conn_man tcpconn.ConnectionManager
	conn_man.Init()
	go conn_man.Req()
	go conn_man.Trans()

	service := ":9988"
	tcpAddr, error := net.ResolveTCPAddr("tcp", service)
	if error != nil {
		log.Println("Error: Could not resolve address")
	} else {
		netListen, error := net.Listen(tcpAddr.Network(), tcpAddr.String())
		if error != nil {
			log.Println(error)
		} else {
			defer netListen.Close()

			for {
				log.Println("Waiting for clients")
				connection, error := netListen.Accept()
				if error != nil {
					log.Println("Client error: ", error)
				} else {
					conn_man.AddClient(connection)

				}
			}
		}
	}
}
