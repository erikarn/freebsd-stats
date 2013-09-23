#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "stat_instance.h"
#include "stat_plugin.h"

#include "plugin_net.h"

int
main(int argc, const char *argv[])
{

	plugin_init();

	plugin_net_register();

	exit(0);
}
