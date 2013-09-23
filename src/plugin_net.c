#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/queue.h>

#include <net/if.h>
#include <net/if_mib.h>

#include "stat_instance.h"
#include "stat_plugin.h"

#include "plugin_net.h"

static int
get_ifmib_general(int row, struct ifmibdata *ifmd)
{
	int name[6];
	size_t len;

	name[0] = CTL_NET;
	name[1] = PF_LINK;
	name[2] = NETLINK_GENERIC;
	name[3] = IFMIB_IFDATA;
	name[4] = row;
	name[5] = IFDATA_GENERAL;

	len = sizeof(*ifmd);

	return sysctl(name, 6, ifmd, &len, (void *)0, 0);
}

static int
plugin_net_create_instance(struct stat_plugin *plugin,
    struct stat_instance *instance)
{

	/* Everything's ok for now */
	return (0);
}

int
plugin_net_register(void)
{
	struct plugin_net_parent * parent_state;

	fprintf(stderr, "%s: registering\n", __func__);

	parent_state = calloc(1, sizeof(*parent_state));
	if (parent_state == NULL) {
		warn("%s: malloc", __func__);
		return (-1);
	}

	if (stat_plugin_register("net", plugin_net_create_instance,
	    parent_state) < 0) {
		free(parent_state);
		return (-1);
	}

	return (0);
}
