/*
 * configure.h
 *
 *  Created on: Mar 29, 2022
 *      Author: tseiman
 */

#ifndef MAIN_INC_CONFIGURE_H_
#define MAIN_INC_CONFIGURE_H_

#define DEFAULT_MODBUS_SERVER 		"127.0.0.1"
#define DEFAULT_MODBUS_PORT 		502
#define DEFAULT_MODBUS_INTERVAL		10000
#define DEFAULT_MQTT_SERVER 		"127.0.0.1"
#define DEFAULT_MQTT_PORT	 		1883
#define DEFAULT_MQTTS_PORT	 		8883
#define DEFAULT_MQTT_MODE			plain

typedef enum { false, true } bool;

typedef struct ModbusSettings {
  const char *server;
  int port;
  unsigned int interval;
} t_modbus_settings;

typedef enum MqttModes {
	plain,
	google
} t_mqtt_modes;
static const char *MqttModes_STRING[] = {
    "plain", "google",
};


typedef struct MqttSettings {
  const char *server;
  int port;
  bool tls;
  t_mqtt_modes mode;
} t_mqtt_settings;

typedef enum VariableDataTypes {
	BOOL,
	INT,
	DAT_UNKNOWN
} t_variable_data_types;

static const char *VariableDataTypes_STRING[] = {
    "BOOL", "INT", "DAT_UNKNOWN"
};

typedef enum VariableReportTypes {
	ONDEBUG,
	INSTANT,
	REP_UNKNOWN
} t_variable_report_types;

static const char *VariableReportTypes_STRING[] = {
    "ONDEBUG", "INSTANT", "REP_UNKNOWN"
};


typedef struct Variable {
  const char *name;
  int addr;
  t_variable_data_types datatype;
  t_variable_report_types reporttype;
} t_variable;


typedef struct VariableList {
	t_variable variable;
	struct VariableList *next;
} t_variable_list;
typedef t_variable_list *t_variable_list_P;


typedef struct Settings {
	int verbose;
	t_modbus_settings modbussettings;
	t_mqtt_settings mqttsettings;
	t_variable_list_P variablelist;
	void (*toString)(void);
} Settings;
typedef Settings *Settings_P;

int loadConfig(char *config_file, int verbose);
static void toString();
Settings_P getConfig();
void destroyConfig();


#endif /* MAIN_INC_CONFIGURE_H_ */
