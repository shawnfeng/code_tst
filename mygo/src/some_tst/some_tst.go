package main

import (
	"fmt"
	"time"
)

type ts struct {
	a string
	b string

}

func tst_assert() {

	// TST: type assert
	var a interface{}
	//a = "hello"
	a = nil

	value, ok := a.(string)
	fmt.Println(value, ok)


	//value = a.(string) //crash
	//fmt.Println(value)


}

func tst_struct_init() {

	// TST: struct init

	a := ts{"hello", "world", }
	fmt.Println(a)



	b := ts{
		a: "hello",
		b: "world",
	}
	fmt.Println(b)


	b1 := ts{

		b: "world",
		a: "hello",
	}
	fmt.Println(b1)



	c := ts{
		a: "hello",
	}
	fmt.Println(c)


	d := ts{
		b: "world",
	}
	fmt.Println(d)

	// map
	commits := map[string]int{
		"rsc": 3711,
		"r":   2138,
		"gri": 1908,
		"adg": 912,
	}

	fmt.Println(commits)


}

func tst_const() {
	type Talk_ProType int32

	const (
		Talk_SYN       Talk_ProType = 1
		Talk_SYNACK    Talk_ProType = 2
	)

	a := Talk_SYN
	fmt.Println(a)

}


// 测试重复关闭channel，引起崩溃
func tst_double_close_channel() {
	c := make(chan bool)

	close(c)
	close(c)

}


// 测试map 清空
func map_clear() {
	m := make(map[string] string)

	m["A"] = "a"
	m["B"] = "b"
	m["C"] = "c"

	fmt.Println(m)

	for k, v := range(m) {

		fmt.Println(m)
		fmt.Println("delete", k, v)
		delete(m, k)
		fmt.Println(m)
	}

}

// 测试已经写的chann，别的go中关闭会崩溃么
// 答案是会崩溃
func chan_read_close() {

	c := make(chan bool)

	go func() {
		c<-true
		fmt.Println("been send")
	}()


	time.Sleep(time.Second * time.Duration(2))

	close(c)


	time.Sleep(time.Second * time.Duration(2))

}

// channel send no block test 1
func chan_send_noblock() {
	c := make(chan bool)

	go func() {
		r := <-c
		fmt.Println("read", r)
	}()


	// 如果没有下面的等待，下面就是noblock了
	// 因为没有来得及启动。。
	time.Sleep(time.Second * time.Duration(2))


	select {
	case c <-true:
		fmt.Println("send ok")

	default:
		fmt.Println("noblock")

	}


	time.Sleep(time.Second * time.Duration(2))

}


// channel send no block test 2
func chan_send_noblock2() {
	c := make(chan bool)

	go func() {
		r := <-c
		fmt.Println("read", r)
	}()

	// Nanosecond  Duration = 1
	// Microsecond          = 1000 * Nanosecond
	// Millisecond          = 1000 * Microsecond
	// Second               = 1000 * Millisecond
	// Minute               = 60 * Second
	// Hour                 = 60 * Minute
	select {
	case c <-true:
		fmt.Println("send ok")




	case <-time.After(time.Microsecond * time.Duration(1)):
		fmt.Println("noblock")

	}


	time.Sleep(time.Second * time.Duration(2))

}


func main() {
	//tst_assert()
	//tst_struct_init()
	//tst_const()
	//tst_double_close_channel()

	//map_clear()

	//chan_read_close()
	chan_send_noblock2()
}

