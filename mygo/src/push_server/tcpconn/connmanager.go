package tcpconn

// TODO LIST:
// select add timeout, expecial Client.Send

// base lib
import (
	"fmt"
	"log"
	"net"
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

