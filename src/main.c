#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/queue.h>

#include "stat_instance.h"
#include "stat_plugin.h"

#include "plugin_net.h"

int
main(int argc, const char *argv[])
{
	struct stat_instance *s1, *s2;

	plugin_init();

	plugin_net_register();

	/*
	 * Now, some testing!
	 */
	s1 = stat_plugin_create("net");
	s2 = stat_plugin_create("net");

	printf("s1 id=%d, s2 id=%d\n", s1->instance_id, s2->instance_id);

	exit(0);
}
