/*
 * modbus.c
 *
 *  Created on: Apr 7, 2022
 *      Author: T. Schmidt.
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <modbus/modbus.h>
#include <errno.h>

#include "modbus_runner.h"
#include "log.h"
#include "configure.h"

static Settings *conf = NULL;
pthread_t thread_modbuspoll_id = 0;
static bool run = false;
static modbus_t *mb;

void *modbusPollLoop(void *vargp);



int connectModbus(Settings *settings) {
	conf = settings;
	logger(LOG_DEBUG,"Open Modbus connection to %s:%d", conf->modbussettings.server, conf->modbussettings.port);
	run = true;
	pthread_create(&thread_modbuspoll_id, NULL, modbusPollLoop, NULL);
	return 0;
}


void waitForModbusThread() {
	pthread_join(thread_modbuspoll_id, NULL);
}


void *modbusPollLoop(void *vargp) {
	logger(LOG_DEBUG,"Enter Modbus thread, runtime variable is %d", run);
	struct timespec ts;
	long msec = conf->modbussettings.interval;

	if (msec < 0) {
		logger(LOG_ERR,"Enter Modbus interval timer must be larger than 0 - given was %d", msec);
		errno = EINVAL;
		return NULL;
	}
	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;

	mb = modbus_new_tcp(conf->modbussettings.server, conf->modbussettings.port);
	if (mb == NULL) {
	    logger(LOG_ERR, "Unable to allocate libmodbus context");
	    return NULL;
	}
	if (modbus_connect(mb) == -1) {
	    logger(LOG_ERR, "Modbus connection failed: %s", modbus_strerror(errno));
	    modbus_free(mb);
	    return NULL;
	}


	while(run) {
		nanosleep(&ts, &ts);
		t_variable_list_P nextListElem = conf->variablelist;
		while(nextListElem) {

			if(nextListElem->variable.datatype == BOOL) {
				 uint8_t dest;
				int ret = modbus_read_bits(mb,nextListElem->variable.addr, 1, &dest);
				logger(LOG_DEBUG, "Bool Variable: name=\"%s\" modbus-addr=\"%d\" type=\"%s\"report=\"%s\" = %d",
						nextListElem->variable.name,
						nextListElem->variable.addr,
						VariableDataTypes_STRING[nextListElem->variable.datatype],
						VariableReportTypes_STRING[nextListElem->variable.reporttype],
						dest );
			}



			nextListElem = nextListElem->next;
		}

	}
	logger(LOG_DEBUG,"Exiting Modbus thread");


	modbus_close(mb);
	modbus_free(mb);

	pthread_exit(0);
	return NULL;
}



void disconnectModbus() {
	logger(LOG_DEBUG,"Close Modbus connection");
	run = false;
}
