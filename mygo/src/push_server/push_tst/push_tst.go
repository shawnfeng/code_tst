package main

import (
	"fmt"
	"log"
	"net"
	"encoding/binary"
)

// ext lib
import (
	"code.google.com/p/goprotobuf/proto"
)

// my lib
import (
	"push_server/pb"

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

	syn := &pushproto.Talk{
		Type: pushproto.Talk_SYN.Enum(),
		Msgid: proto.Int32(17),
		Auth: proto.String("Fuck"),
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


func connect() net.Conn {
	conn, err := net.Dial("tcp", "127.0.0.1:9988")
	if err != nil {
		log.Println("Conn err:", conn, err)
	}

	go Read(conn)

	return conn
}



func main() {

	//  tst pad err
	conn := connect()
	pakSyn(conn)
	var input string
	fmt.Scanln(&input)
	fmt.Println("done")


	return
    //packtst(conn, 300*1000000, 0)


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




}
