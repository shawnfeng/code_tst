package main

import (
	"fmt"
	"encoding/binary"

)


func main() {

	 var u uint64 = 256 * 256
	 //u = 0
	 buff := make([]byte, 15)
	 rv := binary.PutUvarint(buff, u)
	 fmt.Println(u, rv, buff)
	 fmt.Printf("%b\n", u)

	 for i := 0; i < rv; i++ {
	 	 fmt.Printf("%b ", buff[i])

	 }

	 fmt.Printf("\n")

	 fmt.Println("Uvarint")
	 //	rv0 uint64 rv1 read size/error check
	 rv0, rv1 := binary.Uvarint(buff)
	 fmt.Println(rv0, rv1)



	 fmt.Println("Uvarint buff small")
	 rv0, rv1 = binary.Uvarint(buff[:1])
	 fmt.Println(rv0, rv1)


	 fmt.Println("Uvarint over flow")
	 for i := 0; i < 15; i++ {
	 	 buff[i] = 128
	 }
	 buff[14] = 1;

	 rv0, rv1 = binary.Uvarint(buff)
	 fmt.Println(rv0, rv1)

	 // test slice
	 fmt.Println(buff, buff[14:], buff[15:])
	 // buff[16:] panic
}