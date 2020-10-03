#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>
#include <tonc_types.h>
#include <string.h>
#include <stdio.h>

#include "ent.h"

#define DEBUG 1

#define REG_DEBUG_ENABLE 	(vu16*)0x4FFF780
#define REG_DEBUG_FLAGS 	(vu16*)0x4FFF700
#define REG_DEBUG_STRING 	(char*)0x4FFF600

typedef enum log_level_e {
	LOG_LEVEL_FATAL = 0, LOG_LEVEL_ERROR, LOG_LEVEL_WARN, 
	LOG_LEVEL_INFO, LOG_LEVEL_DEBUG
} log_level_e;

void init_debug();

inline static void write_to_log(log_level_e level, const char* ptr,...) {
	va_list args;
	va_start(args, ptr);
	vsnprintf(REG_DEBUG_STRING, 0x100, ptr, args);
	va_end(args);
	*REG_DEBUG_FLAGS = (u16)level | 0x100;
}


#endif