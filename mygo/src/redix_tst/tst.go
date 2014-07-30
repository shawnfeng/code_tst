// example program

package main

import (

	"log"
	"rediscluster"
)




func main() {
	pool := rediscluster.NewRedisPool()
	cmd0 := []interface{}{"echo", "Hello world A0!"}
	cmd1 := []interface{}{"echo", "Hello world B0!"}
	cmd2 := []interface{}{"echo", "Hello world C0!"}

	//log.Println(reflect.TypeOf(cmd0[0]))

	mcmd := make(map[string][]interface{})
	mcmd["127.0.0.1:9600"] = cmd0
	mcmd["127.0.0.1:9601"] = cmd1
	mcmd["127.0.0.1:9602"] = cmd2

	log.Println(mcmd)
	rp := pool.Cmd(mcmd)

	log.Println(rp)





}

// 支持apply, 一次多个命令过去,并统一获得返回
