#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/queue.h>

#include "stat_instance.h"
#include "stat_plugin.h"

#include "plugin_net.h"
#include "plugin_pmc.h"

int
main(int argc, const char *argv[])
{
	struct stat_instance *s1, *s2, *s3, *s4;

	plugin_init();

	plugin_net_register();
	plugin_pmc_register();

	/*
	 * Now, some testing!
	 */
	s1 = stat_plugin_create("net");
	s2 = stat_plugin_create("net");
	s3 = stat_plugin_create("net");
	s4 = stat_plugin_create("pmc");

	printf("s1 id=%d, s2 id=%d, s3 id=%d, s4 id=%d\n",
	    s1->instance_id, s2->instance_id, s3->instance_id, s4->instance_id);

	/* Next, do setup on all three */
	stat_plugin_config(s1, "lagg0");
	stat_plugin_config(s2, "ix0");
	stat_plugin_config(s3, "ix1");
	stat_plugin_config(s4, "-s instructions");

	while (1) {
		plugin_fetch_all();
		sleep(1);
	}

	exit(0);
}
