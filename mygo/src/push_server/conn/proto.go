package connection

// base lib
import (
//	"fmt"
	"time"
	"log"
	//"crypto/sha1"
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

// use defer
func (self *Client) deferErrNotifyCLOSED(errmsg *string) {
	self.errNotifyCLOSED(*errmsg)
}

func (self *Client) errNotifyCLOSED(errmsg string) {
	//util.LogDebug("errmsg:%s", errmsg)
	errpb := &pushproto.Talk{
		Type: pushproto.Talk_ERR.Enum(),
		Extdata: []byte(errmsg),
	}

	data, _ := proto.Marshal(errpb)
	self.SendClose(util.Packdata(data))

}



func (self *Client) sendSYNACK(client_id string) {
	synack := &pushproto.Talk{
		Type: pushproto.Talk_SYNACK.Enum(),
		Clientid: proto.String(client_id),
	}

	data, _ := proto.Marshal(synack)
	self.Send(util.Packdata(data))

}

func (self *Client) sendHEART() {
	synack := &pushproto.Talk{
		Type: pushproto.Talk_HEART.Enum(),
	}

	data, _ := proto.Marshal(synack)
	self.Send(util.Packdata(data))

}

func (self *Client) sendBussRetry(msgid uint64, pb []byte) {
	ack_notify := make(chan bool)

	self.addBussmsg(msgid, ack_notify)


	retry_intv := 2
	retry_time := 3

	go func() {
		defer self.rmBussmsg(msgid)

		for i := 1; i <= retry_time+1; i++ {

			select {
			case v := <-ack_notify:
				if v {
					util.LogInfo("client:%s recv ack msgid:%d", self, msgid)
				} else {
					util.LogInfo("client:%s close not recv ack msgid:%d", self, msgid)
				}
				return

			case <-time.After(time.Second * time.Duration(retry_intv)):
				if i <= retry_time {
					util.LogInfo("client:%s retry msgid:%d times:%d", self, msgid, i)
					self.Send(pb)
				} else {
					// 最后一次发送已经超时
					util.LogInfo("client:%s send timeout msgid:%d", self, msgid)
					// 断开连接
					self.chgCLOSED()
					return

				}


			}

			retry_intv = retry_intv << 1

		}




	}()

}

func (self *Client) SendBussiness(ziptype int32, datatype int32, data []byte) {
	msgid, err := self.manager.Msgid()
	if err != nil {
		util.LogError("SendBussiness get msgid error:%s", err)
		return
	}


	buss := &pushproto.Talk {
		Type: pushproto.Talk_BUSSINESS.Enum(),
		Msgid: proto.Uint64(msgid),
		Ziptype: proto.Int32(ziptype),
		Datatype: proto.Int32(datatype),
		Bussdata: data,
	}

	spb, err := proto.Marshal(buss)
	if err != nil {
		util.LogError("Bussiness marshaling error: ", err)
		return
	}

	p := util.Packdata(spb)
	self.sendBussRetry(msgid, p)

	util.LogInfo("client:%s send msgid:%d", self, msgid)
	self.Send(p)

}


func (self *Client) recvACK(pb *pushproto.Talk) {
	msgid := pb.GetAckmsgid()

	c := self.getBussmsg(msgid)

	if c != nil {
		select {
		case c <-true:
			util.LogDebug("recvACK client:%s msgid:%d notify", self, msgid)
		default:
			util.LogWarn("recvACK client:%s msgid:%d no wait notify", self, msgid)
		}
	}

}


func (self *Client) recvSYN(pb *pushproto.Talk) {
	self.chgESTABLISHED(pb)
}


func (self *Client) proto(data []byte) {
	pb := &pushproto.Talk{}
	err := proto.Unmarshal(data, pb)
	if err != nil {
		log.Println("unmarshaling error: ", err)
		self.errNotifyCLOSED("package unmarshaling error")
		return
	}

	util.LogDebug("recv proto: %s", pb)
	pb_type := pb.GetType()


	if pb_type == pushproto.Talk_SYN {
		self.recvSYN(pb)
	} else if pb_type == pushproto.Talk_ECHO {
		self.Send(util.Packdata(data))

	} else if pb_type == pushproto.Talk_HEART {
		self.sendHEART()

	} else if pb_type == pushproto.Talk_ACK {
		self.recvACK(pb)


	}


}


