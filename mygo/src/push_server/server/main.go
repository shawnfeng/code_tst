package main

import (
	//"fmt"
//	"log"

	"push_server/conn"

)


func main() {
	conn_man := connection.NewConnectionManager()

	httpport := ":9091"
	connection.StartHttp(conn_man, httpport)

	service := ":9989"
	conn_man.Loop(service)

}

// 规整的log

// rediscluster
// 支持apply, 一次多个命令过去,并统一获得返回
// rediscluster 增删也采用req的方式进行

// package put in github
// 调整exported函数



// Client close show log if not map
// write once not enough

// num use const config,not direct use

// log level
// connmanager req/recv/send use one goroutine?
