package tcpconn

// TODO LIST:
// select add timeout, expecial Client.Send


import (
	//"fmt"
	"log"
	"net"
	"reflect"
	"time"
	"encoding/binary"
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
func (self *Client) Send(conn_man *ConnectionManager, s string) {
	log.Println("Client.Send", self.conn_id, s)

	self.sendLock()
	defer self.sendUnLock()

	self.conn.SetWriteDeadline(time.Now().Add(time.Duration(5) * time.Second))
	a, err := self.conn.Write([]byte(s))
	log.Println("Client.Send Write rv", self.conn_id, self.client_id, a, err)

	if err != nil {
		log.Println("Client write error: ", self.conn_id, self.client_id, err)
		self.Close(conn_man)
		return
	}

}

// goroutine
func (self *Client) Recv(conn_man *ConnectionManager) {
	buffer := make([]byte, 2048)
	packBuff := make([]byte, 0)
	var bufLen uint64 = 0

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



		packBuff = append(packBuff, buffer[:bytesRead]...)
		bufLen += uint64(bytesRead)


	    log.Println("Client Recv: ", self.conn_id, self.client_id, bytesRead, packBuff, bufLen)

		for {
			if (bufLen > 0) {
			    pacLen, sz := binary.Uvarint(packBuff[:bufLen])
				if sz < 0 {
					log.Println("Client package head error: ", self.conn_id, self.client_id, packBuff[:bufLen])
					return
				} else if sz == 0 {
				    break
				}
				// must < 5K
				if pacLen > 1024 * 5 {
					log.Println("Client package too long error: ", self.conn_id, self.client_id, packBuff[:bufLen])
					return
				}

				apacLen := uint64(sz)+pacLen+1
				if bufLen >= apacLen {
				    pad := packBuff[apacLen-1]
					if pad != 0 {
						log.Println("Client package pad error: ", self.conn_id, self.client_id, packBuff[:bufLen])
					    return
					}
				    conn_man.Recv(self, string(packBuff[sz:apacLen-1]))
					packBuff = packBuff[apacLen:]
					bufLen -= apacLen
				} else {
					break
				}

			} else {
				break

			}

		}



		//log.Println("Client.Recv:", pack_len, reflect.TypeOf(pack_len))

		//conn_man.Recv(self, string(buffer[0:bytesRead]))
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
	data   string
}

type ConnectionManager struct {
	clients map[string]*Client

	addreq chan ClientAddReq
	delreq chan ClientDelReq

	sendbuf chan TransReq
	recvbuf chan TransRecv
}


func (self *ConnectionManager) addClient(conn net.Conn) {
	self.addreq <- ClientAddReq{conn}
}

func (self *ConnectionManager) DelClient(conn_id string) {
	self.delreq <- ClientDelReq{conn_id}
}

// goroutine
func (self *ConnectionManager) req() {

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
			//go cli.Send(self, "Conn OK:"+conn_id)

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
func (self *ConnectionManager) trans() {
	log.Println("Trans Start")
	for {
		select {
		case r := <-self.sendbuf:
			log.Println("ConnectionManager.Trans Send", r.client_id, r.conn_id, r.data)

		case r := <-self.recvbuf:
			log.Println("ConnectionManager.Trans Recv", r.client.client_id, r.client.conn_id, r.data)
			//go r.client.Send(self, "Recv:connid"+r.client.conn_id+" Data:"+r.data)
			go r.client.Send(self, r.data)

		}

	}

}


func (self *ConnectionManager) Loop(addr string) {
	go self.req()
	go self.trans()


	tcpAddr, error := net.ResolveTCPAddr("tcp", addr)
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
					self.addClient(connection)

				}
			}
		}
	}

}


func NewConnectionManager() *ConnectionManager {
	return &ConnectionManager {
		sendbuf: make(chan TransReq, 9),
		recvbuf: make(chan TransRecv, 9),

		addreq: make(chan ClientAddReq, 0),
		delreq: make(chan ClientDelReq, 0),
		clients: make(map[string]*Client),

	}

}

