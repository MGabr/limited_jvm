/**
 * @file settings.h
 * @author Markus Gabriel
 * @brief Adjustable settings for the virtual machine
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include "log_levels.h" // for LOG_LEVEL_* constants

/**
 * Only the messages with a log level equal to or larger than the current log
 * level are printed.
 */
#define LOG_LEVEL LOG_LEVEL_ERROR

/**
 * If _LOG_INSTRS_ is defined, all executed vm instructions are logged.
 * This only works if _TESTMODE_ is also defined.
 * Note that this should only be used for manual tests, as the additional output
 * will break tests that check the output.
 */
//#define _LOG_INSTRS_

/**
 * If _TESTMODE_ is defined, the vm terminates after a specified number of
 * instructions and saves its state in a global variable. see testmode.c
 * Comment this out during normal execution of the vm to prevent performance
 * degradation.
 */
//#define _TESTMODE_

#endif
