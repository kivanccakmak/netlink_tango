#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>

#define PROGRAM_NAME "debug"

#define _WHITE   "\033[37;1m"
#define _BLUE    "\033[34;1m"
#define _RED     "\033[31;1m"
#define _GREEN   "\033[32;1m"
#define _YELLOW  "\033[33;2m"
#define _NORM    "\033[0m"

enum debug_level {
	debug_level_silent = -1,
	debug_level_error = 0,
	debug_level_warning,
	debug_level_info,
	debug_level_debug,
	debug_level_trace,
};

#define dbg_printf(level, fmt, ...) do{ \
		if (debug_get_level() >= level) { \
			fprintf(stdout, fmt, ##__VA_ARGS__); \
		} \
} while(0)

#define tracef(fmt, ...) do{ \
		dbg_printf(debug_level_trace, PROGRAM_NAME"-trace: %s(%d): " fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define debugf(fmt, ...) do{ \
		dbg_printf(debug_level_debug, PROGRAM_NAME"-debug: %s(%d): " fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define infof(fmt, ...) do{ \
		dbg_printf(debug_level_info, PROGRAM_NAME"-info: %s(%d): " fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define warningf(fmt, ...) do{ \
		dbg_printf(debug_level_warning, _YELLOW PROGRAM_NAME"-warning: %s(%d): " fmt _NORM "\n", \
				__func__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define errorf(fmt, ...) do{ \
		dbg_printf(debug_level_error, _RED PROGRAM_NAME"-error: %s(%d): " fmt "\n  %s" _NORM "\n",	\
				__func__, __LINE__, ##__VA_ARGS__, __FILE__); \
} while(0)

enum debug_level debug_get_level(void);
void debug_set_level(enum debug_level level);
enum debug_level debug_level_from_string(const char *string);
#endif
