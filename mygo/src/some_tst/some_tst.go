package main

import (
	"fmt"
)

func main() {
	var a interface{}
	//a = "hello"
	a = nil

	value, ok := a.(string)
	fmt.Println(value, ok)


	value = a.(string)
	fmt.Println(value)

}
