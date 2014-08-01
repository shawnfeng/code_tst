package util

import (
	"log"
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


