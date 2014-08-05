package connection

// TODO LIST:
// select add timeout, expecial Client.Send

// base lib
import (
//	"fmt"
	"log"
	"net"
//	"crypto/sha1"
)

// ext lib
import (
	//"code.google.com/p/go-uuid/uuid"
//	"code.google.com/p/goprotobuf/proto"
)

// my lib
import (
//	"push_server/pb"
	"push_server/util"

)


type ClientAddReq struct {
	cli *Client
}

type ClientDelReq struct {
	client_id string
	addr string
}

type TransSend struct {
	client_id   string
	ziptype int32
	datatype int32
	data      []byte
}

//type TransRecv struct {
//	client *Client
//	data   []byte
//}

type ConnectionManager struct {
	clients map[string]*Client

	addreq chan ClientAddReq
	delreq chan ClientDelReq

	sendbuf chan TransSend
//	recvbuf chan TransRecv
}


func (self *ConnectionManager) addClient(cli *Client) {
	self.addreq <- ClientAddReq{cli}
}

func (self *ConnectionManager) DelClient(client_id string, addr string) {
	self.delreq <- ClientDelReq{client_id, addr}
}

func (self *ConnectionManager) Send(client_id string, ziptype int32, datatype int32, data []byte) {
	self.sendbuf <- TransSend{client_id, ziptype, datatype, data}
}



// goroutine
func (self *ConnectionManager) req() {

	log.Println("Req Start")
	for {
		select {
		case r := <-self.addreq:
			client_id := r.cli.client_id
			if v, ok := self.clients[client_id]; ok {
				v.errmsg = "dup client add client"
				v.CloseErr()
				util.LogWarn("dup client add client_id %s", client_id)

			}
			self.clients[client_id] = r.cli
			util.LogInfo("Add %s %d %s", client_id, len(self.clients), r.cli.remoteaddr)


		case r := <-self.delreq:
			client_id := r.client_id
			// The delete function doesn't return anything, and will do nothing if the specified key doesn't exist.

			if v, ok := self.clients[r.client_id]; ok {
				if v.remoteaddr == r.addr {
					delete(self.clients, client_id)
					util.LogInfo("Remove %s %d %s", r.client_id, len(self.clients), v.remoteaddr)
				} else {
					util.LogWarn("delete client %s not same %s", v, r.addr)
				}

			} else {
				util.LogWarn("delete client_id %s not fond", r.client_id)
			}




		case r := <-self.sendbuf:
			log.Println(self.clients)
			if v, ok := self.clients[r.client_id]; ok {
				v.SendBussiness(r.ziptype, r.datatype, r.data)

			} else {
				util.LogWarn("send client_id %s not fond", r.client_id)
			}


		}

	}

}

//func (self *ConnectionManager) Recv(cli *Client, data []byte) {
//	self.recvbuf <- TransRecv{cli, data}
//}



// goroutine
func (self *ConnectionManager) trans() {
	log.Println("Trans Start")
	for {
		select {
		case r := <-self.sendbuf:
			log.Println("ConnectionManager.Trans Send", r.client_id, r.data)

//		case r := <-self.recvbuf:
//			log.Println("ConnectionManager.Trans Recv", r.client.client_id, r.data)
			//uuidgen := uuid.NewUUID()
			//client_id := uuidgen.String()

			//go r.client.Send(self, r.data)
			//self.Proto(&r)

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
	//go self.trans()

	for {
		util.LogInfo("Waiting for clients")
		connection, error := netListen.Accept()
		if error != nil {
			log.Println("Client error: ", error)
		} else {
			NewClient(self, connection)
		}
	}

}


func NewConnectionManager() *ConnectionManager {
	return &ConnectionManager {
		sendbuf: make(chan TransSend, 9),
//		recvbuf: make(chan TransRecv, 9),

		addreq: make(chan ClientAddReq, 0),
		delreq: make(chan ClientDelReq, 0),
		clients: make(map[string]*Client),

	}

}

