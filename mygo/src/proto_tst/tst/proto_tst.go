package main

import (
	"log"

	"code.google.com/p/goprotobuf/proto"
	"proto_tst/p/example"
)

func main() {
	test := &example.Test{
		Label: proto.String("hello"),
		Type:  proto.Int32(17),
		Optionalgroup: &example.Test_OptionalGroup{
			RequiredField: proto.String("good bye"),
		},
	}
	log.Println(test)
	data, err := proto.Marshal(test)
	log.Println(data, err)
	if err != nil {
		log.Fatal("marshaling error: ", err)
	}
	newTest := &example.Test{}
	err = proto.Unmarshal(data, newTest)
	if err != nil {
		log.Fatal("unmarshaling error: ", err)
	}
	// Now test and newTest contain the same data.
	log.Println(test.GetLabel(), newTest.GetLabel())
	if test.GetLabel() != newTest.GetLabel() {
		log.Fatalf("data mismatch %q != %q", test.GetLabel(), newTest.GetLabel())
	}
	// etc.
}
