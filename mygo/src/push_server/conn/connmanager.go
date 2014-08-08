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
	"github.com/sdming/gosnow"
)

// my lib
import (
//	"push_server/pb"
	"push_server/util"

)


type ConnectionManager struct {
	clients map[string]*Client

	sf *gosnow.SnowFlake
}


func (self *ConnectionManager) addClient(cli *Client) {
	client_id := cli.client_id
	if v, ok := self.clients[client_id]; ok {
		v.errNotifyCLOSED("dup client add client")
		delete(self.clients, client_id)
		util.LogWarn("dup client add client_id %s", client_id)

	}
	self.clients[client_id] = cli
	util.LogInfo("Add %s %d", cli, len(self.clients))

}

func (self *ConnectionManager) delClient(client_id string, addr string) {

	if v, ok := self.clients[client_id]; ok {
		if v.remoteaddr == addr {
			delete(self.clients, client_id)
			util.LogInfo("Remove %s %d", v, len(self.clients))
		} else {
			util.LogWarn("delete client %s not same %s", v, addr)
		}

	} else {
		util.LogWarn("delete client_id %s not fond", client_id)
	}

}

func (self *ConnectionManager) Send(client_id string, ziptype int32, datatype int32, data []byte) {

	if v, ok := self.clients[client_id]; ok {
		v.SendBussiness(ziptype, datatype, data)

	} else {
		util.LogWarn("send client_id %s not fond", client_id)
	}


}




func (self *ConnectionManager) Msgid() (uint64, error) {
	return self.sf.Next()

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


	//go self.req()
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
	v, err := gosnow.Default()
	//v, err := gosnow.NewSnowFlake(100)
	if err != nil {
		panic("snowflake init error, msgid can not get!")
	}

	return &ConnectionManager {

		clients: make(map[string]*Client),

		sf: v,

	}

}

