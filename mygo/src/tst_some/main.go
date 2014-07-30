package main

import (
	   "fmt"
	   "time"
)

func loop_show(r string) {

	 for i:=0;; i++ {
	      time.Sleep(1000 * 1000 * 1000 * 1)
          fmt.Println("SHOW:", r, i)

     }
}



func main() {
	go loop_show("GO 1:")
	go loop_show("GO 2:")


    var input string
    fmt.Scanln(&input)
    fmt.Println("done")

}
