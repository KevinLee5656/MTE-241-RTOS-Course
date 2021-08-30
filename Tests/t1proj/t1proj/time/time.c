/**
 * uVision5 does not include an implementation of the time() function from
 * <time.h>.  This is a kludge to supply a fake time.
 *
 * @author Andrew Morton
 */

#include <time.h>

/**
 * Reports a time and increments by 1 second with each invocation.
 */
time_t time(time_t *time) {
	static time_t t = 1621364331;
	return t++;
}

