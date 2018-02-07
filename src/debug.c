#include <stdlib.h>
#include <strings.h>

#include "debug.h"

enum debug_level g_debug_level = debug_level_trace;

enum debug_level debug_get_level (void)
{
	return g_debug_level;
}

void debug_set_level (enum debug_level level)
{
	g_debug_level = level;
}

enum debug_level debug_level_from_string (const char *string)
{
	if (string == NULL) {
		return debug_level_error;
	}
	if (strcasecmp(string, "error") == 0) {
		return debug_level_error;
	}
	if (strcasecmp(string, "warning") == 0) {
		return debug_level_warning;
	}
	if (strcasecmp(string, "info") == 0) {
		return debug_level_info;
	}
	if (strcasecmp(string, "debug") == 0) {
		return debug_level_debug;
	}
	if (strcasecmp(string, "trace") == 0) {
		return debug_level_trace;
	}
	return debug_level_error;
}
