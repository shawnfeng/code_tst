// Code generated by protoc-gen-go.
// source: push.proto
// DO NOT EDIT!

/*
Package pushproto is a generated protocol buffer package.

It is generated from these files:
	push.proto

It has these top-level messages:
	Talk
*/
package pushproto

import proto "code.google.com/p/goprotobuf/proto"
import math "math"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = math.Inf

type Talk_ProType int32

const (
	Talk_SYN       Talk_ProType = 1
	Talk_SYNACK    Talk_ProType = 2
	Talk_RESET     Talk_ProType = 3
	Talk_REROUTE   Talk_ProType = 4
	Talk_HEART     Talk_ProType = 5
	Talk_ACK       Talk_ProType = 6
	Talk_BUSSINESS Talk_ProType = 7
	Talk_ERR       Talk_ProType = 100
	Talk_ECHO      Talk_ProType = 101
)

var Talk_ProType_name = map[int32]string{
	1:   "SYN",
	2:   "SYNACK",
	3:   "RESET",
	4:   "REROUTE",
	5:   "HEART",
	6:   "ACK",
	7:   "BUSSINESS",
	100: "ERR",
	101: "ECHO",
}
var Talk_ProType_value = map[string]int32{
	"SYN":       1,
	"SYNACK":    2,
	"RESET":     3,
	"REROUTE":   4,
	"HEART":     5,
	"ACK":       6,
	"BUSSINESS": 7,
	"ERR":       100,
	"ECHO":      101,
}

func (x Talk_ProType) Enum() *Talk_ProType {
	p := new(Talk_ProType)
	*p = x
	return p
}
func (x Talk_ProType) String() string {
	return proto.EnumName(Talk_ProType_name, int32(x))
}
func (x *Talk_ProType) UnmarshalJSON(data []byte) error {
	value, err := proto.UnmarshalJSONEnum(Talk_ProType_value, data, "Talk_ProType")
	if err != nil {
		return err
	}
	*x = Talk_ProType(value)
	return nil
}

type Talk struct {
	Type     *Talk_ProType `protobuf:"varint,1,req,enum=pushproto.Talk_ProType" json:"Type,omitempty"`
	Clientid *string       `protobuf:"bytes,2,opt" json:"Clientid,omitempty"`
	Msgid    *int32        `protobuf:"varint,3,opt" json:"Msgid,omitempty"`
	Ackmsgid *int32        `protobuf:"varint,4,opt" json:"Ackmsgid,omitempty"`
	// syn ext
	Auth       *string `protobuf:"bytes,1001,opt" json:"Auth,omitempty"`
	Appid      *string `protobuf:"bytes,1002,opt" json:"Appid,omitempty"`
	Installid  *string `protobuf:"bytes,1003,opt" json:"Installid,omitempty"`
	Clienttype *string `protobuf:"bytes,1004,opt" json:"Clienttype,omitempty"`
	Clientver  *string `protobuf:"bytes,1005,opt" json:"Clientver,omitempty"`
	// bussiness ext
	Ziptype          *int32 `protobuf:"varint,1500,opt" json:"Ziptype,omitempty"`
	Datatype         *int32 `protobuf:"varint,1501,opt" json:"Datatype,omitempty"`
	Bussdata         []byte `protobuf:"bytes,1502,opt" json:"Bussdata,omitempty"`
	Extdata          []byte `protobuf:"bytes,2000,opt" json:"Extdata,omitempty"`
	XXX_unrecognized []byte `json:"-"`
}

func (m *Talk) Reset()         { *m = Talk{} }
func (m *Talk) String() string { return proto.CompactTextString(m) }
func (*Talk) ProtoMessage()    {}

func (m *Talk) GetType() Talk_ProType {
	if m != nil && m.Type != nil {
		return *m.Type
	}
	return Talk_SYN
}

func (m *Talk) GetClientid() string {
	if m != nil && m.Clientid != nil {
		return *m.Clientid
	}
	return ""
}

func (m *Talk) GetMsgid() int32 {
	if m != nil && m.Msgid != nil {
		return *m.Msgid
	}
	return 0
}

func (m *Talk) GetAckmsgid() int32 {
	if m != nil && m.Ackmsgid != nil {
		return *m.Ackmsgid
	}
	return 0
}

func (m *Talk) GetAuth() string {
	if m != nil && m.Auth != nil {
		return *m.Auth
	}
	return ""
}

func (m *Talk) GetAppid() string {
	if m != nil && m.Appid != nil {
		return *m.Appid
	}
	return ""
}

func (m *Talk) GetInstallid() string {
	if m != nil && m.Installid != nil {
		return *m.Installid
	}
	return ""
}

func (m *Talk) GetClienttype() string {
	if m != nil && m.Clienttype != nil {
		return *m.Clienttype
	}
	return ""
}

func (m *Talk) GetClientver() string {
	if m != nil && m.Clientver != nil {
		return *m.Clientver
	}
	return ""
}

func (m *Talk) GetZiptype() int32 {
	if m != nil && m.Ziptype != nil {
		return *m.Ziptype
	}
	return 0
}

func (m *Talk) GetDatatype() int32 {
	if m != nil && m.Datatype != nil {
		return *m.Datatype
	}
	return 0
}

func (m *Talk) GetBussdata() []byte {
	if m != nil {
		return m.Bussdata
	}
	return nil
}

func (m *Talk) GetExtdata() []byte {
	if m != nil {
		return m.Extdata
	}
	return nil
}

func init() {
	proto.RegisterEnum("pushproto.Talk_ProType", Talk_ProType_name, Talk_ProType_value)
}
