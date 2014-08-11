package connection

// base lib
import (
//	"fmt"
	"time"
//	"log"
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
	if *errmsg == "" {
		self.chgCLOSED()
	} else {
		self.errNotifyCLOSED(*errmsg)
	}
}

func (self *Client) errNotifyCLOSED(errmsg string) {
	fun := "Client.errNotifyCLOSED"
	//util.LogDebug("errmsg:%s", errmsg)
	errpb := &pushproto.Talk{
		Type: pushproto.Talk_ERR.Enum(),
		Extdata: []byte(errmsg),
	}

	util.LogDebug("%s errmsg:%s", fun, errpb)
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
	fun := "Client.sendBussRetry"
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
					util.LogInfo("%s client:%s recv ack msgid:%d", fun, self, msgid)
				} else {
					util.LogInfo("%s client:%s close not recv ack msgid:%d", fun, self, msgid)
				}
				return

			case <-time.After(time.Second * time.Duration(retry_intv)):
				if i <= retry_time {
					util.LogInfo("%s client:%s retry msgid:%d times:%d", fun, self, msgid, i)
					self.Send(pb)
				} else {
					// 最后一次发送已经超时
					util.LogInfo("%s client:%s send timeout msgid:%d", fun, self, msgid)
					// 断开连接
					self.chgCLOSED()
					return

				}


			}

			retry_intv = retry_intv << 1

		}




	}()

}

func (self *Client) SendBussiness(ziptype int32, datatype int32, data []byte) (uint64, string) {
	fun := "Client.SendBussiness"

	msgid, err := self.manager.Msgid()
	if err != nil {
		util.LogError("%s get msgid error:%s", fun, err)
		return 0, self.remoteaddr
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
		util.LogError("%s marshaling error: ", fun, err)
		return 0, self.remoteaddr
	}

	p := util.Packdata(spb)
	self.sendBussRetry(msgid, p)

	util.LogInfo("%s client:%s send msgid:%d", fun, self, msgid)
	self.Send(p)


	return msgid, self.remoteaddr
}


func (self *Client) recvACK(pb *pushproto.Talk) {
	fun := "Client.recvACK"

	msgid := pb.GetAckmsgid()

	c := self.getBussmsg(msgid)

	if c != nil {
		select {
		case c <-true:
			util.LogDebug("%s client:%s msgid:%d notify", fun, self, msgid)
		default:
			util.LogWarn("%s client:%s msgid:%d no wait notify", fun, self, msgid)
		}
	} else {
		util.LogWarn("%s client:%s msgid:%d not found", fun, self, msgid)
	}

}


func (self *Client) recvSYN(pb *pushproto.Talk) {
	self.chgESTABLISHED(pb)
}


func (self *Client) proto(data []byte) {
	fun := "Client.proto"
	pb := &pushproto.Talk{}
	err := proto.Unmarshal(data, pb)
	if err != nil {
		util.LogWarn("%s unmarshaling error: %s", fun, err)
		self.errNotifyCLOSED("package unmarshaling error")
		return
	}

	util.LogDebug("%s recv proto: %s", fun, pb)
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


