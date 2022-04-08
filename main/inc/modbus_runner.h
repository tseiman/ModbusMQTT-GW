/*
 * modbus.h
 *
 *  Created on: Apr 7, 2022
 *      Author: tseiman
 */
#include "configure.h"

#ifndef MAIN_INC_MODBUS_H_
#define MAIN_INC_MODBUS_H_




int connectModbus(Settings *settings);
void waitForModbusThread();
void disconnectModbus();



#endif /* MAIN_INC_MODBUS_H_ */
