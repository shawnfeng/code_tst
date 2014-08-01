package tcpconn

// TODO LIST:
// select add timeout, expecial Client.Send

// base lib
import (
	"fmt"
	"log"
	"net"
	"reflect"
	"time"
	"encoding/binary"
	"crypto/sha1"
)

// ext lib
import (
	//"code.google.com/p/go-uuid/uuid"
	"code.google.com/p/goprotobuf/proto"
)

// my lib
import (
	"push_server/pb"
	"push_server/util"

)

type Client struct {
	client_id   string
	conn      net.Conn
	sending   chan bool
}

func (self *Client) Close(conn_man *ConnectionManager) {
	conn_man.DelClient(self.client_id)
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
	log.Println("Client.Send", self.client_id, s)

	self.sendLock()
	defer self.sendUnLock()

	self.conn.SetWriteDeadline(time.Now().Add(time.Duration(5) * time.Second))
	a, err := self.conn.Write([]byte(s))
	log.Println("Client.Send Write rv", self.client_id, a, err)

	if err != nil {
		log.Println("Client write error: ", self.client_id, err)
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
			log.Println("Client connection error: ", self.client_id, error)
			return
		}



		packBuff = append(packBuff, buffer[:bytesRead]...)
		bufLen += uint64(bytesRead)


	    log.Println("Client Recv: ", self.client_id, bytesRead, packBuff, bufLen)

		for {
			if (bufLen > 0) {
			    pacLen, sz := binary.Uvarint(packBuff[:bufLen])
				if sz < 0 {
					log.Println("Client package head error: ", self.client_id, packBuff[:bufLen])
					return
				} else if sz == 0 {
				    break
				}
				// must < 5K
				if pacLen > 1024 * 5 {
					log.Println("Client package too long error: ", self.client_id, packBuff[:bufLen])
					return
				}

				apacLen := uint64(sz)+pacLen+1
				if bufLen >= apacLen {
				    pad := packBuff[apacLen-1]
					if pad != 0 {
						log.Println("Client package pad error: ", self.client_id, packBuff[:bufLen])
					    return
					}
				    conn_man.Recv(self, packBuff[sz:apacLen-1])
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
		//log.Println("Receive:", self.client_id, self.client_id, string(buffer[0:bytesRead]))
		//a, err := conn.Write(buffer[0:bytesRead])
		//log.Println("Write rv", self.client_id, self.client_id, a, err)
	}

}

type ClientAddReq struct {
	cli *Client
}

type ClientDelReq struct {
	client_id string
}

type TransReq struct {
	client_id   string
	data      []byte
}

type TransRecv struct {
	client *Client
	data   []byte
}

type ConnectionManager struct {
	clients map[string]*Client

	addreq chan ClientAddReq
	delreq chan ClientDelReq

	sendbuf chan TransReq
	recvbuf chan TransRecv
}


func (self *ConnectionManager) addClient(cli *Client) {
	self.addreq <- ClientAddReq{cli}
}

func (self *ConnectionManager) DelClient(client_id string) {
	self.delreq <- ClientDelReq{client_id}
}

// goroutine
func (self *ConnectionManager) req() {

	log.Println("Req Start")
	for {
		select {
		case r := <-self.addreq:
			client_id := r.cli.client_id
			self.clients[client_id] = r.cli
			log.Println("Add", client_id, len(self.clients))

		case r := <-self.delreq:
			client_id := r.client_id
			// The delete function doesn't return anything, and will do nothing if the specified key doesn't exist.
			delete(self.clients, client_id)
			log.Println("Remove", r.client_id, len(self.clients))

		}

	}

}

func (self *ConnectionManager) Recv(cli *Client, data []byte) {
	self.recvbuf <- TransRecv{cli, data}
}


func (self *ConnectionManager) Proto(r *TransRecv) {
	pb := &pushproto.Talk{}
	err := proto.Unmarshal(r.data, pb)
	if err != nil {
		log.Println("unmarshaling error: ", err)
		r.client.Close(self)
	}

	util.LogDebug("recv proto: %s", pb)
	pb_type := pb.GetType()
	if pb_type == pushproto.Talk_SYN {
		if r.client.client_id != "NULL" {
			// 已经建立了连接，当前状态是ESTABLISHED，不是TCP_READY
			// 直接丢弃该协议
			util.LogWarn("conn: %s state: ESTABLISHED can not change SYN_RCVD", r.client.client_id)

		} else {
			appid := pb.GetAppid()
			installid := pb.GetInstallid()
			sec := "9b0319bc5c05055283cee2533abab270"


			h := sha1.Sum([]byte(appid+installid+sec))
			cli_id := fmt.Sprintf("%x", h)
			r.client.client_id = cli_id

			self.addClient(r.client)
		}

	}

}


// goroutine
func (self *ConnectionManager) trans() {
	log.Println("Trans Start")
	for {
		select {
		case r := <-self.sendbuf:
			log.Println("ConnectionManager.Trans Send", r.client_id, r.data)

		case r := <-self.recvbuf:
			log.Println("ConnectionManager.Trans Recv", r.client.client_id, r.data)
			//uuidgen := uuid.NewUUID()
			//client_id := uuidgen.String()

			//go r.client.Send(self, r.data)
			self.Proto(&r)

		}

	}

}


func (self *ConnectionManager) Loop(addr string) {
	tcpAddr, error := net.ResolveTCPAddr("tcp", addr)
	if error != nil {
		log.Println("Error: Could not resolve address")
		return
	}


	netListen, error := net.Listen(tcpAddr.Network(), tcpAddr.String())
	if error != nil {
		log.Println(error)
		return
	}
	defer netListen.Close()


	go self.req()
	go self.trans()

	for {
		util.LogInfo("Waiting for clients")
		connection, error := netListen.Accept()
		if error != nil {
			log.Println("Client error: ", error)
		} else {
			cli := &Client{
				client_id: "NULL",
				conn: connection,
				sending: make(chan bool, 1),
			}
			go cli.Recv(self)
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

