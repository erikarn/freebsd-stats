#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

#include <sys/queue.h>

#include "stat_plugin.h"
#include "stat_instance.h"

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
}

void
plugin_finish(void)
{
}
