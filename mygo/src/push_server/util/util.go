package util

import (
	"log"
	"encoding/binary"
)

func LogDebug(format string, v ...interface{}) {
	log.Printf("[DEBUG] "+format, v...)
}

func LogInfo(format string, v ...interface{}) {
	log.Printf("[INFO] "+format, v...)
}

func LogWarn(format string, v ...interface{}) {
	log.Printf("[WARN] "+format, v...)
}

func LogError(format string, v ...interface{}) {
	log.Printf("[ERROR] "+format, v...)
}

func LogFatal(format string, v ...interface{}) {
	log.Printf("[FATAL] "+format, v...)
}


func PackdataPad(data []byte, pad byte) []byte {
	sendbuff := make([]byte, 0)
	// no pad
	var pacLen uint64 = uint64(len(data))
	buff := make([]byte, 20)
	rv := binary.PutUvarint(buff, pacLen)

	sendbuff = append(sendbuff, buff[:rv]...) // len
	sendbuff = append(sendbuff, data...) //data
	sendbuff = append(sendbuff, pad) //pad

	return sendbuff

}

func Packdata(data []byte) []byte {
	return PackdataPad(data, 0)
}



