package main

import "fmt"

/*
#include <stdio.h>
extern void ACFunction();
*/
import "C"

//export AGoFunction
func AGoFunction() {
        fmt.Println("IN GO AGoFunction()")
}

func main() {
        C.ACFunction()
}

