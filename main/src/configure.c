/*
 * configure.c
 *
 *  Created on: Mar 29, 2022
 *      Author: T. Schmidt.
 */

#include <errno.h>
#include <unistd.h>
#include <mxml.h>
#include <limits.h>

#include "log.h"
#include "configure.h"
#include "my_alloc.h"

static Settings_P conf;
static mxml_node_t *xmltree;

bool xml2bool(mxml_node_t *node) {
	if(mxmlGetOpaque(node) == NULL) return false;
	return (strncmp( mxmlGetOpaque(node), "true", 5) == 0 ? true : false);
}

t_mqtt_modes xml2MqttModes(mxml_node_t *node) {
	if(mxmlGetOpaque(node) == NULL) return plain;
	if(!strncmp(mxmlGetOpaque(node), "google", 5)) {
		return google;
	}
	return plain;
}

t_variable_data_types str2DataType(const char *type) {
	if(!strncmp( type, "NUMBER", 5)) return NUMBER;
	if(!strncmp( type, "BOOL", 4)) return BOOL;
	return DAT_UNKNOWN;
}
t_variable_report_types str2ReportType(const char *type) {
	if(!strncmp( type, "ONDEBUG", 7)) return ONDEBUG;
	if(!strncmp( type, "INSTANT", 7)) return INSTANT;
	return REP_UNKNOWN;
}

static void toString() {
	logger(LOG_DEBUG,  \
			"\nModbus: \n" \
			"    Server      : %s\n" \
			"    Port        : %d\n" \
			"    Interval    : %d\n", \
			conf->modbussettings.server,
			conf->modbussettings.port,
			conf->modbussettings.interval);
	logger(LOG_DEBUG,  \
			"\nMQTT: \n" \
			"    Server      : %s\n" \
			"    Port        : %d\n" \
			"    TLS         : %s\n" \
			"    Mode        : %s\n", \
			conf->mqttsettings.server,
			conf->mqttsettings.port,
			(conf->mqttsettings.tls ? "true":"false"),
			MqttModes_STRING[conf->mqttsettings.mode]);

	t_variable_list_P nextListElem = conf->variablelist;
	while(nextListElem) {
		logger(LOG_DEBUG,  \
				"Variable: name=\"%s\" modbus-addr=\"%d\" type=\"%s\"report=\"%s\"",
				nextListElem->variable.name,
				nextListElem->variable.addr,
				VariableDataTypes_STRING[nextListElem->variable.datatype],
				VariableReportTypes_STRING[nextListElem->variable.reporttype]);
		nextListElem = nextListElem->next;
	}

}

int loadConfig(char *config_file, int verbose) {

	FILE *fp;
	char cwd[PATH_MAX];
	mxml_node_t *xml_node;
	char *buffer_ref;

	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		logger(LOG_INFO,"Current working dir: %s", cwd);
	} else {
		perror("getcwd() error");
		return ENOENT;
	}
	if( access( config_file, F_OK ) != 0 ) {
		logger(LOG_ALERT, "Not such config file: \"%s\"", config_file);
		return ENOENT; // not such file
	}
	conf = my_malloc(sizeof(Settings));
	conf->verbose = verbose;
	conf->variablelist = NULL;
	conf->toString = toString;

	logger(LOG_DEBUG, "Loading config file: \"%s\"", config_file);

	if(!(fp = fopen(config_file, "r"))) {
		 int errnum = errno;
		 logger(LOG_ERR, "Error opening config file \"%s\": %s, code %d",config_file, strerror( errnum ),errno);
		 return errnum;
	}
	xmltree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);

	fclose(fp);

	xml_node = mxmlFindPath(xmltree, "mmgw-config/modbus/server");
	conf->modbussettings.server = (xml_node == NULL) ? DEFAULT_MODBUS_SERVER : mxmlGetOpaque(xml_node);
	xml_node = mxmlFindPath(xmltree, "mmgw-config/modbus/port");
	conf->modbussettings.port = (xml_node == NULL) ? DEFAULT_MODBUS_PORT : atoi(mxmlGetOpaque(xml_node));
	xml_node = mxmlFindPath(xmltree, "mmgw-config/modbus/poll-interval");
	conf->modbussettings.interval = (xml_node == NULL) ? DEFAULT_MODBUS_INTERVAL : atoi(mxmlGetOpaque(xml_node));

	xml_node = mxmlFindPath(xmltree, "mmgw-config/mqtt/server");
	conf->mqttsettings.server = (xml_node == NULL) ? DEFAULT_MQTT_SERVER : mxmlGetOpaque(xml_node);
	xml_node = mxmlFindPath(xmltree, "mmgw-config/mqtt/tls");
	conf->mqttsettings.tls = (xml_node == NULL) ? false : xml2bool(xml_node);
	xml_node = mxmlFindPath(xmltree, "mmgw-config/mqtt/port");
	conf->mqttsettings.port = (xml_node == NULL) ? (conf->mqttsettings.tls ? DEFAULT_MQTTS_PORT : DEFAULT_MQTT_PORT ): atoi(mxmlGetOpaque(xml_node));
	xml_node = mxmlFindPath(xmltree, "mmgw-config/mqtt/mode");
	conf->mqttsettings.mode = (xml_node == NULL) ? DEFAULT_MQTT_MODE : xml2MqttModes(xml_node);

	mxml_node_t *iter_node;
	xml_node = mxmlFindPath(xmltree, "mmgw-config/variables/variable/");

	while(xml_node) {
		if(mxmlGetType(xml_node)  == MXML_ELEMENT) {
			logger(LOG_DEBUG, "Loading variable %s", mxmlElementGetAttr(xml_node, "name"));
			t_variable_list_P new_variable_element = my_malloc(sizeof(t_variable_list));

			new_variable_element->variable.name = mxmlElementGetAttr(xml_node, "name");
			new_variable_element->variable.addr = atoi(mxmlElementGetAttr(xml_node, "modbus-addr"));
			new_variable_element->variable.datatype = str2DataType(mxmlElementGetAttr(xml_node, "type"));
			new_variable_element->variable.reporttype = str2ReportType(mxmlElementGetAttr(xml_node, "report"));

			if(conf->variablelist == NULL) {
				new_variable_element->next = NULL;
				conf->variablelist = new_variable_element;
			} else {
				new_variable_element->next = conf->variablelist;
				conf->variablelist = new_variable_element;
			}
		}
		xml_node = mxmlWalkNext(xml_node, xmltree, MXML_DESCEND);
	}

	return 0;
}

Settings_P getConfig() {
	return conf;
}

void destroyConfig() {
	if(xmltree) mxmlDelete(xmltree);
	if(!conf) return;
	if(conf->variablelist) {
		while(conf->variablelist) {
			t_variable_list_P nextListElem = conf->variablelist->next;
			my_free(conf->variablelist);
			conf->variablelist = nextListElem;
		}
	}
	if(conf) my_free(conf);
}
