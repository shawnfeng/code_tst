package main
// TODO LIST:
// slice array string diff
// select add timeout, expecial Client.Send

import (
	//"fmt"
	"log"
	"net"
	"reflect"
	"time"
)

import (
	"code.google.com/p/go-uuid/uuid"
)

type Client struct {
	conn_id   string
	client_id string
	conn      net.Conn
	sending   chan bool
}

func (self *Client) Close(conn_man *ConnectionManager) {
	conn_man.DelClient(self.conn_id)
	self.conn.Close()

}

func (self *Client) sendLock() {
	self.sending <- true

	log.Println("Client.sendLock", len(self.sending))
}

func (self *Client) sendUnLock() {
	<-self.sending
	log.Println("Client.UnsendLock", len(self.sending))
}

// goroutine
func (self *Client) Send(s string) {
	log.Println("Client.Send", self.conn_id, s)

	self.sendLock()
	defer self.sendUnLock()
	a, err := self.conn.Write([]byte(s))
	log.Println("Client.Send Write rv", self.conn_id, self.client_id, a, err)

}

// goroutine
func (self *Client) Recv(conn_man *ConnectionManager) {
	buffer := make([]byte, 1024)
	conn := self.conn
	log.Println(reflect.TypeOf(conn))

	defer self.Close(conn_man)

	for {
		conn.SetReadDeadline(time.Now().Add(time.Duration(20) * time.Second))
		bytesRead, error := conn.Read(buffer)
		if error != nil {
			log.Println("Client connection error: ", self.conn_id, self.client_id, error)
			return
		}

		conn_man.Recv(self, string(buffer[0:bytesRead]))
		//log.Println("Receive:", self.conn_id, self.client_id, string(buffer[0:bytesRead]))
		//a, err := conn.Write(buffer[0:bytesRead])
		//log.Println("Write rv", self.conn_id, self.client_id, a, err)
	}

}

type ClientAddReq struct {
	conn net.Conn
}

type ClientDelReq struct {
	conn_id string
}

type TransReq struct {
	conn_id   string
	client_id string
	data      string
}

type TransRecv struct {
	client *Client
	data      string
}


type ConnectionManager struct {
	clients map[string]*Client

	addreq chan ClientAddReq
	delreq chan ClientDelReq

	sendbuf chan TransReq
	recvbuf chan TransRecv
}

func (self *ConnectionManager) Init() {
	self.sendbuf = make(chan TransReq, 9)
	self.recvbuf = make(chan TransRecv, 9)

	self.addreq = make(chan ClientAddReq, 0)
	self.delreq = make(chan ClientDelReq, 0)
	self.clients = make(map[string]*Client)

}

func (self *ConnectionManager) AddClient(conn net.Conn) {
	self.addreq <- ClientAddReq{conn}
}

func (self *ConnectionManager) DelClient(conn_id string) {
	self.delreq <- ClientDelReq{conn_id}
}

// goroutine
func (self *ConnectionManager) Req() {

	log.Println("Req Start")
	for {
		select {
		case r := <-self.addreq:
			uuidgen := uuid.NewUUID()
			conn_id := uuidgen.String()
			cli := &Client{conn_id, "", r.conn, make(chan bool, 1)}
			self.clients[conn_id] = cli
			log.Println("Add", conn_id, len(self.clients))
			go cli.Recv(self)
			go cli.Send("Conn OK:" + conn_id)

		case r := <-self.delreq:
			conn_id := r.conn_id
			delete(self.clients, conn_id)
			log.Println("Remove", r.conn_id, len(self.clients))

		}

	}

}

func (self *ConnectionManager) Recv(cli *Client, data string) {
	self.recvbuf <- TransRecv{cli, data}
}

// goroutine
func (self *ConnectionManager) Trans() {
	log.Println("Trans Start")
	for {
		select {
		case r := <-self.sendbuf:
			log.Println("ConnectionManager.Trans Send", r.client_id, r.conn_id, r.data)

		case r := <-self.recvbuf:
			log.Println("ConnectionManager.Trans Recv", r.client.client_id, r.client.conn_id, r.data)
			go r.client.Send("Recv:connid" + r.client.conn_id + " Data:" + r.data)

		}

	}

}

func main() {
	var conn_man ConnectionManager
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
