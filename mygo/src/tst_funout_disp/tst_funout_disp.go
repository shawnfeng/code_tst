package main

import (
	   "fmt"
	   "time"
	   "runtime"

)


func cus(nm string, q <-chan int) {

	 for s := range q {
	 	 fmt.Println(nm, ":", s)

	 }

}

func fac() (chan int){
	 rv := make(chan int, 20)
	 go func () {
	 	 for i := 0;; i++ {
		 	 //fmt.Println("Create:", i)
	 	 	 rv <- i
			 time.Sleep(1000 * 1000 * 10)
	     }

	 }()

	 return rv

}


func main() {
	 runtime.GOMAXPROCS(2)
	 pause := make(chan bool)
	 rv := fac()

	 for i := 0; i < 100; i++ {
	 	 go cus(fmt.Sprintf("Cus%d:", i), rv)

	 }


	 <-pause

}