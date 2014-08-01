package tcpconn

// TODO LIST:
// select add timeout, expecial Client.Send

// base lib
import (
	"log"
	"net"
	"reflect"
	"time"
	"encoding/binary"
)

// ext lib
import (
	//"code.google.com/p/go-uuid/uuid"
)

// my lib
import (


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

