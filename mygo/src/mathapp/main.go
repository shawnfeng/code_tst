package main

/*
#cgo CFLAGS: -I .
#cgo LDFLAGS: -L . -lfuck

#include "fuck.h"
*/
import "C"



/*
#include <stdio.h>
*/
import "C"
import (
	   "fmt"
	   "runtime"
	   "time"
	   "mymath"
)

func canirun() {
	 C.tid()
	 fmt.Println("can i run!")

}


func busy(f chan bool) {
	 runtime.LockOSThread()
     defer runtime.UnlockOSThread()


	 f <- true
	 fmt.Println("I am fucking!")

	 fmt.Println("Fuck Busy!")
	 C.tid()
	 go canirun()

	 for {}

	 for i:=0; ; i++ {
	 	 time.Sleep(1000*1000*1000*3)
     	 C.tid()
         fmt.Println("Fuck OK!", i)
     }

}

func loop_show() {

	 for i:=0; ; i++ {
	      time.Sleep(1000 * 1000 * 1)
        	 C.tid()
          fmt.Println("SHOW:", i)

     }
}


func PrintHello() {
	 C.puts(C.CString("Hello, world\n"))
	 C.fuck()
	 C.tid()
}

func main() {
	 fmt.Printf("Hello, world.  Sqrt(2) = %v\n", mymath.Sqrt(2))
	 PrintHello()

	runtime.GOMAXPROCS(2)


	begin := make(chan bool)
	go busy(begin)
	fmt.Println(runtime.NumGoroutine())
	<- begin
	fmt.Println("is fucking!")
	go loop_show()


    var input string
    fmt.Scanln(&input)
    fmt.Println("done")

}
