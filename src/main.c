#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/queue.h>

#include <sys/param.h> /* for plugin_pmc.h */
#include <sys/cpuset.h>	/* for plugin_pmc.h */
#include <pmc.h>	/* for plugin_pmc.h> */

#include "stat_instance.h"
#include "stat_plugin.h"

#include "plugin_net.h"
#include "plugin_pmc.h"

int
main(int argc, char *argv[])
{
	struct stat_instance *s1;
	int i;

	plugin_init();

	plugin_net_register();
	plugin_pmc_register();

	/* Loop over, populating things.. */
	for (i = 1; argc - i >= 2; i += 2) {
		s1 = stat_plugin_create(argv[i]);
		if (s1 == NULL)
			exit(127);
		stat_plugin_config(s1, argv[i+1]);
	}

	while (1) {
		plugin_fetch_all();
		sleep(1);
	}

	exit(0);
}
