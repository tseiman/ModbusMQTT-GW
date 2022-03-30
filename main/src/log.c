#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>

#include "log.h"
#include "configure.h"


// void bootLoggerLog(int loglevel, const char *format, ...);


static Settings *conf = NULL;

void openLog(Settings * settings) {
    conf = settings;
    logger(LOG_DEBUG,"Set loglevel to (%d)", (conf ? conf->verbose : 7));

}


void loggerLog(int loglevel, const char *format, ...) {
    char buffer[512];
    va_list arglist;
    va_start(arglist, format);
    vsnprintf( buffer, 512, format,arglist );
    va_end(arglist);

    if ( (conf ? conf->verbose : 7) >= loglevel) {
	    printf("%s\n",buffer);
    }
}

