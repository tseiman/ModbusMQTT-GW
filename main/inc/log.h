/*
 * log.h
 *
 *  Created on: Mar 30, 2022
 *      Author: tseiman
 */
#include <string.h>
#include "configure.h"

#ifndef MAIN_INC_LOG_H_
#define MAIN_INC_LOG_H_


#define        LOG_EMERG        0        /* system is unusable */
#define        LOG_ALERT        1        /* action must be taken immediately */
#define        LOG_CRIT         2        /* critical conditions */
#define        LOG_ERR          3        /* error conditions */
#define        LOG_WARNING      4        /* warning conditions */
#define        LOG_NOTICE       5        /* normal but significant condition */
#define        LOG_INFO        	6        /* informational */
#define        LOG_DEBUG        7        /* debug-level messages */

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define logger(loglevel, format, args...) 				\
	loggerLog(loglevel,"%s::%s(%d)> " format,  __FILENAME__,	\
		 __func__, __LINE__, ##args )

void openLog(Settings * settings);
void loggerLog(int loglevel, const char *format, ...);


#endif /* MAIN_INC_LOG_H_ */
