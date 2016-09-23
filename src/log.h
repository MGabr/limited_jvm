/**
 * @file log.h
 * @author Markus Gabriel
 * @brief Macros for logging
 *
 * Macros for logging that consider the log level specified in settings.h.
 *
 * @see log_levels.h
 * @see settings.h
 */
#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#include "log_levels.h"
#include "settings.h"

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
	#define DEBUG(...) printf(__VA_ARGS__)
#else
	#define DEBUG(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
	#define INFO(...) printf(__VA_ARGS__)
#else
	#define INFO(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
	#define WARN(...) fprintf(stderr, __VA_ARGS__)
#else
	#define WARN(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
	#define ERROR(...) fprintf(stderr, __VA_ARGS__)
#else
	#define ERROR(...)
#endif

#endif
