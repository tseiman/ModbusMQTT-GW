

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <getopt.h>

#include "configure.h"
#include "log.h"


static void printHelp() {
	printf("\n");
	printf("ModbusMqtt-GW help:\n\n");
	printf(" [--help              , -h         ] : Optional, prints this help message\n\n");
	printf(" [--verbose=<level>   , -v <level> ] : Optional, prints out some debug information\n");
	printf("                                       if there are any. Per default set to max.\n");
	printf("                                       can have value from 0 (no logging) to\n");
	printf("                                       7 maximum logging\n\n");
	printf(" [--configfile=<file> , -c <file>  ] : Optional, gives path and filename of\n");
	printf("                                       configuration file.\n\n");
	printf("\n");
	exit(0);
}


int main (int argc, char *argv[] ) {


	int ret, c;
	char *config = NULL;
	int verbose = -1;
	int dontdaemonize = -1;
	int forceunlock = -1;

	opterr = 0;

	while (1) {
		static struct option long_options[] = {
				{"help"		, no_argument		, 0, 'h'},
				{"verbose"		, required_argument	, 0, 'v'},
				{"configfile"	, required_argument	, 0, 'c'},
				{ 0			, 0			, 0,  0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "hv:c:", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
		case 'v':
			verbose = atoi(optarg);
			if(verbose > 7) verbose = 7;
			if(verbose < 0) verbose = 0;
			break;
		case 'c':
			config = optarg;
			break;
		case 0:
		case 'h':
		case '?':
			printHelp();
			break;

		default:
			printHelp();
			break;
			/*    abort(); */
		}
	}



	if ((ret = loadConfig(config ? config : "/etc/mmgw-config.xml"))) {
		destroyConfig();
		return ret;
	}
	openLog(getConfig());

	if (verbose >= 0) getConfig()->verbose = verbose;


	getConfig()->toString();
//	daemonRun(getConf(), argc, argv);


	destroyConfig();


	return 0;
}
