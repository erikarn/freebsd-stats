#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
get_ifmib_row_byname(const char *name)
{
	uint32_t ifcount;
	int ret, i;
	unsigned long l;
	struct ifmibdata ifmd;

	l = sizeof(ifcount);

	ret = sysctlbyname("net.link.generic.system.ifcount",
	    (void *) &ifcount, &l, NULL, 0);

	if (ret < 0) {
		warn("%s: sysctl", __func__);
		return (-1);
	}

	fprintf(stderr, "%s: ifcount=%d\n", __func__, ifcount);

	for (i = 1; i <= ifcount; i++) {
		bzero(&ifmd, sizeof(ifmd));
		if (get_ifmib_general(i, &ifmd) < 0)
			continue;
		fprintf(stderr, "%s: row=%d, ifname=%s\n",
		    __func__,
		    i,
		    ifmd.ifmd_name);
		if (strlen(ifmd.ifmd_name) == strlen(name) &&
		    strcmp(name, ifmd.ifmd_name) == 0)
			return (i);
	}

	/* Not found */
	return (-1);
}

static int
plugin_net_fetch(struct stat_instance *instance)
{
	struct ifmibdata ifmd;
	struct plugin_net_instance *ni = instance->state;

#if 0
	fprintf(stderr, "%s: id=%d, called\n",
	    __func__,
	    instance->instance_id);
#endif

	/*
	 * Don't fetch data if we're not yet ready
	 */
	if (instance->is_ready == 0)
		return (-1);

	bzero(&ifmd, sizeof(ifmd));
	if (get_ifmib_general(ni->row_id, &ifmd) < 0)
		return (-1);

	/* XXX for now, print it out */
	printf("%s: %s: pkt.in=%lu, pkt.out=%lu, bytes.in=%lu, bytes.out=%lu\n",
	    __func__,
	    ni->netif,
	    ifmd.ifmd_data.ifi_ipackets,
	    ifmd.ifmd_data.ifi_opackets,
	    ifmd.ifmd_data.ifi_ibytes,
	    ifmd.ifmd_data.ifi_obytes);

	return (0);
}

static int
plugin_net_config(struct stat_instance *instance, const char *config)
{
	struct plugin_net_instance *n = instance->state;

	/* XXX hack - do setup now; we should defer this lateR */
	n->row_id = get_ifmib_row_byname(config);

	if (n->row_id < 0) {
		fprintf(stderr, "%s: unknown interface '%s'\n",
		    __func__,
		    config);
		return(-1);
	}

	n->netif = strdup(config);
	instance->is_ready = 1;

	return (0);
}


static int
plugin_net_create_instance(struct stat_plugin *plugin,
    struct stat_instance *instance)
{
	struct plugin_net_instance *n;

	n = calloc(1, sizeof(*n));
	if (n == NULL) {
		warn("%s: calloc", __func__);
		return (-1);
	}

	/* Not ready */
	n->netif = NULL;
	n->row_id = -1;

	/* XXX methodize this! */
	instance->is_ready = 0;
	instance->state = n;
	instance->stat_fetch = plugin_net_fetch;
	instance->stat_config = plugin_net_config;

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
