/*
 * log.h
 *
 *  Created on: Jul 16, 2015
 *      Author: romang
 */

#ifndef LOG_H_
#define LOG_H_

enum {
    ERROR_LEVEL,
    WARNING_LEVEL,
    NOTICE_LEVEL,
    INFO_LEVEL,
    DEBUG_LEVEL
};

#define LOG_LEVEL DEBUG_LEVEL

void log_debug (const char* fmt, ...);
void log_info (const char* fmt, ...);
void log_notice (const char* fmt, ...);
void log_warning (const char* fmt, ...);
void log_error (const char* fmt, ...);
void log_enable(void);
void log_disable(void);

#endif /* LOG_H_ */
