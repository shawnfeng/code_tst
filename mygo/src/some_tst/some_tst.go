package main

import (
	"fmt"
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

func main() {
	//tst_assert()
	tst_struct_init()
}
