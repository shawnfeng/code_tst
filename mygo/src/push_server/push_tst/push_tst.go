package main

import (
	"fmt"
	"log"
	"net"
	"errors"
	"encoding/binary"
)

// ext lib
import (
	"code.google.com/p/go-uuid/uuid"
	"code.google.com/p/goprotobuf/proto"
)

// my lib
import (
	"push_server/pb"
	"push_server/util"

)


func Read(conn net.Conn) {
	buffer := make([]byte, 2048)
	for {
		bytesRead, error := conn.Read(buffer)
		if error != nil {
			log.Println("Client connection error: ", error)
			return
		}

		log.Println("Client Read", conn, bytesRead, buffer[:bytesRead])

	}

}

func pakSyn(conn net.Conn) {

	uuidgen := uuid.NewUUID()
	installid := uuidgen.String()

	syn := &pushproto.Talk{
		Type: pushproto.Talk_SYN.Enum(),
		Msgid: proto.Int32(17),
		Appid: proto.String("376083b4ee34c038f9cd90c5cff364e8"),
		Installid: proto.String(installid),
		Auth: proto.String("Fuck"),
		Clienttype: proto.String("Android"),
		Clientver: proto.String("1.0.0"),

	}


	data, err := proto.Marshal(syn)
	log.Println(data, err)
	if err != nil {
		log.Println("marshaling error: ", err)
	}


	packdata(conn, data)

}


func packdata(conn net.Conn, data []byte) {
	sendbuff := make([]byte, 0)
	// no pad
	var pacLen uint64 = uint64(len(data))
	buff := make([]byte, 20)
	rv := binary.PutUvarint(buff, pacLen)
	log.Println("Pack Len", rv, buff[:rv])


	sendbuff = append(sendbuff, buff[:rv]...)
	sendbuff = append(sendbuff, data...)

	sendbuff = append(sendbuff, 0)

	log.Println("Send Buff", buff[:rv], sendbuff)
	a, err := conn.Write(sendbuff)
	log.Println("Write rv", a, err)


}


func packtst(conn net.Conn, len int, pad byte) {
	sendbuff := make([]byte, 0)
	// no pad
	var pacLen uint64 = uint64(len)
	buff := make([]byte, 20)
	rv := binary.PutUvarint(buff, pacLen)
	log.Println("Pack Len", rv, buff[:rv])


	sendbuff = append(sendbuff, buff[:rv]...)

	for i := 0; i < len; i++ {
		sendbuff = append(sendbuff, byte(i))

	}

	sendbuff = append(sendbuff, pad)

	log.Println("Send Buff", buff[:rv], sendbuff)
	a, err := conn.Write(sendbuff)
	log.Println("Write rv", a, err)


}


func connect() (net.Conn, error) {
	return net.Dial("tcp", "127.0.0.1:9988")

}


// 仅读取一次，不考虑TCP的拆包，粘包问题
// 仅用于自动化测试检测
func ReadOnce(conn net.Conn) ([]byte, error) {
	buffer := make([]byte, 4096)
	bytesRead, error := conn.Read(buffer)


	if error != nil {
		return nil, error
	}


	// n == 0: buf too small
	// n  < 0: value larger than 64 bits (overflow)
    //         and -n is the number of bytes read

	pacLen, sz := binary.Uvarint(buffer[:bytesRead])
	if sz < 0 {
		return nil, errors.New("package head error")
	} else if sz == 0 {
		return nil, errors.New("package head small")
	}

	apacLen := uint64(sz)+pacLen+1

	pad := buffer[apacLen-1]
	if pad != 0 {
		return nil, errors.New("package pad error")
	}

	return buffer[sz:apacLen-1], nil

}


func tstErr(tstfun string, sb []byte, checkFun func(*pushproto.Talk) ) {
	conn, err := connect()
	if err != nil {
		util.LogError("%s ERROR:create connection error:%s", tstfun, err)
		return
	}

	defer conn.Close()

	//sb := util.PackdataPad(data, 1)

	ln, err := conn.Write(sb)
	if ln != len(sb) || err != nil {
		util.LogError("%s ERROR:send error:%s", tstfun, err)
		return
	}

	data, err := ReadOnce(conn)
	if err != nil {
		util.LogError("%s ERROR:read connection error:%s", tstfun, err)
		return
	}



	pb := &pushproto.Talk{}
	err = proto.Unmarshal(data, pb)
	if err != nil {
		util.LogError("%s ERROR:unmarshaling connection error:%s", tstfun, err)
		return
	}

	util.LogInfo("%s PROTO:%s", tstfun, pb)

	checkFun(pb)



}


// ----测试用例----
// 1. pad错误
func tstErrpad() {
	sb := util.PackdataPad([]byte("error pad"), 1)

	tstfun := "tstErrpad"
	tstErr(tstfun, sb, func (pb *pushproto.Talk) {
		pb_type := pb.GetType()
		if pb_type == pushproto.Talk_ERR {
			util.LogInfo("%s OK: msg:%s", tstfun, pb.GetExtdata())
		}
	})


}



// 2. 空数据包
func tstErrEmptyPack() {
	sb := util.Packdata([]byte(""))
	tstfun := "tstErrEmptyPack"
	tstErr(tstfun, sb, func (pb *pushproto.Talk) {
		pb_type := pb.GetType()
		if pb_type == pushproto.Talk_ERR {
			util.LogInfo("%s OK: msg:%s", tstfun, pb.GetExtdata())
		}
	})


}


// 3. 长度为1数据包
func tstErrOneSizePack() {
	sb := util.Packdata([]byte("1"))
	tstfun := "tstErrOneSizePack"
	tstErr(tstfun, sb, func (pb *pushproto.Talk) {
		pb_type := pb.GetType()
		if pb_type == pushproto.Talk_ERR {
			util.LogInfo("%s OK: msg:%s", tstfun, pb.GetExtdata())
		}
	})


}


// 4. 超长数据包

// 5. 拆包测试，拆开了发
// 6. 粘包测试，合并了发送


func main() {

	tstErrpad()
	tstErrEmptyPack()
	tstErrOneSizePack()
	//  tst pad err
	//conn := connect()
	//pakSyn(conn)
	//pakSyn(conn)
	var input string
	fmt.Scanln(&input)
	fmt.Println("done")


	return
    //packtst(conn, 300*1000000, 0)

/*
	packtst(conn, 0, 1)

	conn = connect()
	packtst(conn, 2, 1)


    // tst zero pak
	conn = connect()
	packtst(conn, 0, 0)

	// tst 1 pak
	packtst(conn, 1, 0)


	// tst 10 pak
	packtst(conn, 10, 0)


	// split pak
    packtst(conn, 300, 0)




	// > 5k
	//packtst(conn, 1024*5+10, 0)



*/
}
