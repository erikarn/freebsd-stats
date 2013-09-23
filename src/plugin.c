#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

#include <sys/queue.h>

#include "stat_plugin.h"
#include "stat_instance.h"

TAILQ_HEAD(, stat_plugins) plugins_list;
TAILQ_HEAD(, stat_instance) instances_list;

int
stat_plugin_register(char *name, stat_plugin_create_func *create_func,
    void *state)
{
	struct stat_plugin *p;

	/* Does a plugin match this particular name? */
	/* Yes, error out */

	/* Add it to the plugin list */

	return (0);
}

void
plugin_init(void)
{
	TAILQ_INIT(&plugins_list);
	TAILQ_INIT(&instances_list);
}

void
plugin_finish(void)
{
}
