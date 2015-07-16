#include "log.h"
#include <stdarg.h>
#include "../xprintf/xprintf.h"


void log_debug (			/* Put a formatted string to the default device */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
    va_list arp;
    if(LOG_LEVEL >= DEBUG_LEVEL){
    	va_start(arp, fmt);
        xprintf_for_log(fmt, arp);
    }
    va_end(arp);
}

void log_info (			/* Put a formatted string to the default device */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
    va_list arp;
    if(LOG_LEVEL >= INFO_LEVEL){
    	va_start(arp, fmt);
        xprintf_for_log(fmt, arp);
    }
    va_end(arp);
}

void log_notice (			/* Put a formatted string to the default device */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
    va_list arp;
    if(LOG_LEVEL >= NOTICE_LEVEL){
    	va_start(arp, fmt);
        xprintf_for_log(fmt, arp);
    }
    va_end(arp);
}

void log_warnign (			/* Put a formatted string to the default device */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
    va_list arp;
    if(LOG_LEVEL >= WARNING_LEVEL){
    	va_start(arp, fmt);
        xprintf_for_log(fmt, arp);
    }
    va_end(arp);
}

void log_error (			/* Put a formatted string to the default device */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
    va_list arp;
    if(LOG_LEVEL >= ERROR_LEVEL){
    	va_start(arp, fmt);
        xprintf_for_log(fmt, arp);
    }
    va_end(arp);
}
