package main

import (
	   "fmt"
	   "runtime"
	   "time"
)

func busy() {
	 runtime.LockOSThread()
     defer runtime.UnlockOSThread()

     fmt.Println("BUSY RUN")
	 for {}

     //time.Sleep(1000 * 1000 * 1000 * 100)


}

func loop_show() {

	 for i:=0; ; i++ {
	      time.Sleep(1000 * 1000 * 1000)
          fmt.Println("SHOW:", i)
     }
}


func main() {
	//runtime.GOMAXPROCS(2)


    fmt.Println("GO BUSY")
	go busy()

    fmt.Println("GO LOOP SHOW")
	go loop_show()

    var input string
    fmt.Scanln(&input)
    fmt.Println("done")


}
