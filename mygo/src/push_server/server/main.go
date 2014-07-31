package main

import (
	//"fmt"
//	"log"

	"push_server/tcpconn"

)


func main() {

	conn_man := tcpconn.NewConnectionManager()

	service := ":9988"
	conn_man.Loop(service)

}


// rediscluster
// 支持apply, 一次多个命令过去,并统一获得返回
// rediscluster 增删也采用req的方式进行

// package put in github
// 调整exported函数


