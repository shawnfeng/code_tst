package main
import (
    //"fmt"
	"time"
    "github.com/alphazero/Go-Redis"
    "log"
	"reflect"
    //"strconv"
)

// map [string] chan atomic count

type ClientEntry struct {
	client redis.Client
	lock chan bool

}

type RedisPool struct {
	clients map[string] *ClientEntry


}


func main() {
    // 连接Redis服务器 127.0.0.1:6379
    spec := redis.DefaultSpec().Host("127.0.0.1").Port(9600)
	log.Println(spec)
    client, e := redis.NewSynchClientWithSpec(spec)
	log.Println(reflect.TypeOf(client)) // *redis.syncClient


    // 是否连接出错
    if e != nil {
        log.Println("error on connect redis server")
        return
    }
	
	time.Sleep(1000 * 1000 * 1000 * 10)
	client.Quit()
    // 取值并输出
    err := client.Set("fuck", []byte("beauty"))
    if err != nil {
        log.Println(err)
       return
    }

	//log.Println(value)


}
