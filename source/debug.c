#include "debug.h"

void init_debug() {
	*REG_DEBUG_ENABLE = 0xC0DE;
}