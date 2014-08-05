package tcpconn

// base lib
import (
	"fmt"
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



func (self *Client) SendBussiness(msgid int64, ziptype int32, datatype int32, data []byte) {
	buss := &pushproto.Talk {
		Type: pushproto.Talk_BUSSINESS.Enum(),
		Msgid: proto.Int64(msgid),
		Ziptype: proto.Int32(ziptype),
		Datatype: proto.Int32(datatype),
		Bussdata: data,
	}

	data, err := proto.Marshal(buss)
	if err != nil {
		util.LogError("Bussiness marshaling error: ", err)
		return
	}

	self.Send(util.Packdata(data))

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

	}


}


