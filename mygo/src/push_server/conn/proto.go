package connection

// base lib
import (
	"fmt"
	"time"
	"log"
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

func (self *Client) sendERR(errmsg string) {
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
	self.bussmsg[msgid] = ack_notify

	retry_intv := 2
	retry_time := 3

	go func() {

		defer delete(self.bussmsg, msgid)

		for i := 1; i <= retry_time+1; i++ {

			select {
			case <-ack_notify:
				util.LogInfo("client:%s recv ack msgid:%d", self, msgid)
				return

			case <-time.After(time.Second * time.Duration(retry_intv)):
				if i <= retry_time {
					util.LogInfo("client:%s retry msgid:%d times:%d", self, msgid, i)
					self.Send(pb)
				} else {
					// 最后一次发送已经超时
					util.LogInfo("client:%s send timeout msgid:%d", self, msgid)
					// 断开连接
					self.Close()

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

	self.Send(p)

}


func (self *Client) recvACK(pb *pushproto.Talk) {
	msgid := pb.GetAckmsgid()
	if v, ok := self.bussmsg[msgid]; ok {
		close(v)
	}
}


func (self *Client) recvSYN(pb *pushproto.Talk) {

	if self.state == State_ESTABLISHED  {
		// 已经建立了连接，当前状态是ESTABLISHED，可能是客户端没有收到synack
		// 重新回执synack
		util.LogWarn("conn: %s state: ESTABLISHED can not change SYN_RCVD", self.client_id)

		self.sendSYNACK(self.client_id)


	} else {
		appid := pb.GetAppid()
		installid := pb.GetInstallid()
		sec := "9b0319bc5c05055283cee2533abab270"


		h := sha1.Sum([]byte(appid+installid+sec))
		cli_id := fmt.Sprintf("%x", h)
		self.client_id = cli_id
		self.changeState(State_ESTABLISHED)
		self.manager.addClient(self)

		self.sendSYNACK(cli_id)

	}


}


func (self *Client) proto(data []byte) {
	pb := &pushproto.Talk{}
	err := proto.Unmarshal(data, pb)
	if err != nil {
		log.Println("unmarshaling error: ", err)
		self.errmsg = "package unmarshaling error"
		self.CloseErr()
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


