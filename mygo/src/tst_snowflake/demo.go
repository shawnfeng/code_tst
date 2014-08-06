/*
github.com/twitter/snowflake in golang
*/

package main

import (
	"github.com/sdming/gosnow"
	"fmt"
)

func main() {

	v, err := gosnow.Default()
	fmt.Println(err)
	//v := gosnow.NewSnowFlake(100)
	for i := 0; i < 100; i++ {
		id, err := v.Next()
		fmt.Println(id, err)
	}
}
