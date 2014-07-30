package main

import (
	   "fmt"
	   "time"

	   "github.com/samuel/go-zookeeper/zk"
)

func main() {
	 c, w, err := zk.Connect([]string{"127.0.0.1"}, time.Second) //*10)
	fmt.Println(c, w, err)
	return


	 if err != nil {
	 	panic(err)
		}
		children, stat, ch, err := c.ChildrenW("/")
		if err != nil {
		   panic(err)
		   }
		   fmt.Printf("%+v %+v\n", children, stat)
		   e := <-ch
		   fmt.Printf("%+v\n", e)
}