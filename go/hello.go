package main
 
import "fmt"
import "time"
import "runtime"
import "math/rand"

func canirun() {
	 fmt.Println("can i run!")
}

func busy(f chan bool) {
	 runtime.LockOSThread()
     defer runtime.UnlockOSThread()
	 f <- true
	 fmt.Println("I am fucking!")

	 fmt.Println("Fuck Busy!")
	 go canirun()
     time.Sleep(100000000000)

     fmt.Println("Fuck OK!")	
	 //for {}
     fmt.Println("Fuck OVER!")	

}
 
func routine(name string, delay time.Duration) {
 
    t0 := time.Now()
    fmt.Println(name, " start at ", t0)
 
    time.Sleep(delay)
 
    t1 := time.Now()
    fmt.Println(name, " end at ", t1)
 
    fmt.Println(name, " lasted ", t1.Sub(t0))
}
 
func main() {
	fmt.Println(runtime.NumGoroutine())
	//runtime.GOMAXPROCS(1)
 
    //生成随机种子
    rand.Seed(time.Now().Unix())

	begin := make(chan bool)
	go busy(begin)
	fmt.Println(runtime.NumGoroutine())
	<- begin
	fmt.Println("is fucking!")

    var input string
    fmt.Scanln(&input)

    fmt.Println("done1")

    var name string
    for i:=0; i<3; i++{
        name = fmt.Sprintf("go_%02d", i) //生成ID
        //生成随机等待时间，从0-4秒
        go routine(name, time.Duration(rand.Intn(5)) * time.Second)
    }
 
	fmt.Println(runtime.NumGoroutine())
    //让主进程停住，不然主进程退了，goroutine也就退了

    fmt.Scanln(&input)
    fmt.Println("done")
	fmt.Println(runtime.NumGoroutine())
}