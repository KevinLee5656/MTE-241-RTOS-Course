/**
 * Event logger implementation.
 * @author your name
 */

#include "logger.h"

/** Used by loggerFmtTime() helper function. **/
char loggerTimeString[20];

/**
 * Convert time_t time to a textual representation.  Storage for the
 * returned string is statically allocated.
 * @param time time_t value to convert
 * @return pointer to a static null-terminated string representing date and time
 */
char *loggerFmtTime(time_t time) {
	strftime(loggerTimeString, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));
	return loggerTimeString;
}

/**
 * Your #include directives, typedefs, global variables and helper
 * functions here.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct event_t{
	LEVEL level;
	const char *message;
	time_t time;
	const char *fileName;
	int lineNumber;
} event_t;

const char* log_levels[4] = {"INFO", "WARNING", "ERROR", "SEVERE"};

event_t *log_record; // kevinlee - might want to initialize an empty array instead
bool multiple_inits = false;
int current_log_index = 0;
int last_log_index = 0;

/**
 * Complete the following functions.  Function documentation is found in
 * logger.h.
 */

bool loggerInit(size_t numLogs) {
	if(c_assert(numLogs>0)) {
		if(numLogs < current_log_index+1) {
			current_log_index = numLogs;
		}
		log_record = realloc(log_record, numLogs * sizeof(event_t));
		last_log_index = numLogs - 1;
		return true;
	}
	return false;
}

bool loggerLog(LEVEL level, time_t time, const char *fileName,
		int lineNumber, const char *message) {
	if(c_assert(level==INFO||level==WARNING||level==ERROR||level==SEVERE)&&c_assert(current_log_index<=last_log_index)) {
		log_record[current_log_index].level = level;
		log_record[current_log_index].message = message;
		log_record[current_log_index].time = time;
		log_record[current_log_index].fileName = fileName;
		log_record[current_log_index].lineNumber = lineNumber;
		current_log_index++;
		return true;
	}	
	return false;
}

void loggerPrintByLevel(LEVEL minLevel) {
	c_assert(minLevel==INFO||minLevel==WARNING||minLevel==ERROR||minLevel==SEVERE);
	for(int index=0; index<=last_log_index; index++) {
		if(log_record[index].level >= minLevel && log_record[index].lineNumber!=NULL) {
			printf("%s [%s] %s:%d %s \n", loggerFmtTime(log_record[index].time), log_levels[log_record[index].level], log_record[index].fileName, log_record[index].lineNumber, log_record[index].message);
		}
	}
}

void loggerPrintByMsg(const char *searchString) {
	c_assert(strlen(searchString) <= MAX_MSG_SIZE);
	for(int index=0; index<=last_log_index; index++) { // kevinlee - go through log record to find appropriate logs and print it
		if(strstr(log_record[index].message, searchString)) {
			printf("%s [%s] %s:%d %s \n", loggerFmtTime(log_record[index].time), log_levels[log_record[index].level], log_record[index].fileName, log_record[index].lineNumber, log_record[index].message);
		}
	}
}

