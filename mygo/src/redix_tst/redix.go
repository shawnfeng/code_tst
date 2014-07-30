// example program

package main

import (
//	"fmt"
	"log"
//	"errors"
	"github.com/fzzy/radix/redis"
//	"os"
	"time"
//	"reflect"
)


type RedisEntry struct {
	client *redis.Client
	m chan bool

}

func (self *RedisEntry) lock() {
	self.m <- true
}

func (self *RedisEntry) unlock() {
	<-self.m
}


func (self *RedisEntry) Cmd(args []interface{}) *redis.Reply {
	self.lock()
	defer self.unlock()

	value := args[0].(string)

	return self.client.Cmd(value, args[1:]...)

}



///////
type RedisPool struct {
	rds map[string] *RedisEntry
}

func (self *RedisPool) getConn(addr string) (*RedisEntry, error) {
	if c, ok := self.rds[addr]; ok {
		return c, nil

	} else {
		log.Printf("add conn addr:%s\n", addr)
		c, err := redis.DialTimeout("tcp", addr, time.Duration(10)*time.Second)
		if err != nil {
			return nil, err
		}

		en := &RedisEntry{c, make(chan bool, 1)}
		self.rds[addr] = en
		return en, nil
	}

}

func (self *RedisPool) Cmd(multi_args map[string][]interface{}) map[string]*redis.Reply {
	rv := make(map[string]*redis.Reply)
	for k, v := range multi_args {
		c, err := self.getConn(k)
		if err != nil {
			log.Printf("get conn addr:%s err:%s\n", k, err)
			continue
		}

		rv[k] = c.Cmd(v)

	}

	return rv

}

func NewRedisPool() *RedisPool {
	return &RedisPool{make(map[string] *RedisEntry)}
}


//////////


func main() {
	pool := NewRedisPool()
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
