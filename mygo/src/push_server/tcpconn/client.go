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
	"code.google.com/p/go-uuid/uuid"
)

// my lib
import (
	"push_server/util"

)

const (
	State_CLOSE       string = "CLOSE"
	State_TCP_READY   string = "TCP_READY"
	State_SYN_RCVD    string = "SYN_RCVD"
	State_ESTABLISHED string = "ESTABLISHED"

)


type Client struct {
	state       string // CLOSE TCP_READY SYN_RCVD ESTABLISHED
	client_id   string // CLOSE TCP_READY SYN_RCVD is tmp id
	conn        net.Conn
	sending     chan bool

	errmsg      string

	manager     *ConnectionManager
}

func NewClient(m *ConnectionManager, c net.Conn) *Client {
	// 分配一个临时的编号，方便问题查询定位
	uuidgen := uuid.NewUUID()
	tmp_client_id := uuidgen.String()
	h := sha1.Sum([]byte(tmp_client_id))
	tmp_client_id = fmt.Sprintf("tmp/%x", h)

	var cli *Client = &Client {
		state: State_CLOSE,
		client_id: tmp_client_id,
		conn: c,
		sending: make(chan bool, 1),
		errmsg: "",
		manager: m,
	}

	cli.changeState(State_TCP_READY)
	//util.LogInfo("new client state:%s id:%s", cli.state, tmp_client_id)

	go cli.Recv()

	return cli

}

func (self *Client) Close() {
	util.LogInfo("close client state:%s id:%s", self.state, self.client_id)

	if self.state == State_ESTABLISHED {
		self.manager.DelClient(self.client_id)
	}
	if err := self.conn.Close(); err != nil {
		util.LogWarn("Close net.Conn err: %s", err)
	}
	self.changeState(State_CLOSE)

}



func (self *Client) CloseErr() {
	util.LogDebug("CloseErr errmsg:%s", self.errmsg)
	if self.errmsg != "" {
		self.sendERR(self.errmsg)
	} else {
		self.Close()
	}

}



//func (self *Client) setClientid(s string) bool {
//}

func (self *Client) isState(s string) bool {
	return s == self.state

}

func (self *Client) changeState(s string) {
	if s != State_CLOSE && s != State_TCP_READY && s !=	State_SYN_RCVD && s != State_ESTABLISHED {
		util.LogError("error change client id:%s old:%s new:%s", self.client_id, self.state, s)
	} else {
		old := self.state
		self.state = s
		util.LogInfo("change client id:%s old:%s new:%s", self.client_id, old, s)
	}


}

func (self *Client) sendLock() {
	self.sending <- true
}

func (self *Client) sendUnLock() {
	<-self.sending
}

func (self *Client) Send(s []byte) {
	go self.sendData(s, false)

}

func (self *Client) SendClose(s []byte) {
	go self.sendData(s, true)

}


// goroutine
func (self *Client) sendData(s []byte, isclose bool) {
	util.LogDebug("sendData %s %d", s, isclose)
	self.sendLock()
	defer self.sendUnLock()

	self.conn.SetWriteDeadline(time.Now().Add(time.Duration(5) * time.Second))
	a, err := self.conn.Write(s)
	log.Println("Client.Send Write rv", self.client_id, a, err)

	if err != nil {
		log.Println("Client write error: ", self.client_id, err)
		self.Close()
		return
	}

	if isclose {
		self.Close()
	}

}

// goroutine
func (self *Client) Recv() {
	buffer := make([]byte, 2048)
	packBuff := make([]byte, 0)
	var bufLen uint64 = 0

	conn := self.conn
	log.Println(reflect.TypeOf(conn))


	defer self.CloseErr()

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

				util.LogDebug("Client Recv pacLen %d", pacLen)
				// must < 5K
				if pacLen > 1024 * 5 {
					log.Println("Client package too long error: ", self.client_id, packBuff[:bufLen])
					self.errmsg = "package too long"
					return
				} else if pacLen == 0 {
					self.errmsg = "package len 0"
					return
				} 

				apacLen := uint64(sz)+pacLen+1
				if bufLen >= apacLen {
				    pad := packBuff[apacLen-1]
					if pad != 0 {
						log.Println("Client package pad error: ", self.client_id, packBuff[:bufLen])
						self.errmsg = "package pad error"
					    return
					}
				    self.proto(packBuff[sz:apacLen-1])
					packBuff = packBuff[apacLen:]
					bufLen -= apacLen
				} else {
					break
				}

			} else {
				break

			}

		}

	}

}

