#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

#include <sys/queue.h>

#include "stat_plugin.h"
#include "stat_instance.h"

TAILQ_HEAD(, stat_plugin) plugins_list;
TAILQ_HEAD(, stat_instance) instances_list;
static uint32_t plugin_current_id;

uint32_t
plugin_instance_get_next_id(void)
{

	return (plugin_current_id++);
}

static struct stat_plugin *
stat_plugin_lookup_by_name(const char *name)
{
	struct stat_plugin *p;

	TAILQ_FOREACH(p, &plugins_list, node) {
		if (strlen(name) == strlen(p->name)
		    && strcmp(name, p->name) == 0) {
			return p;
		}
	}
	return (NULL);
}

int
stat_plugin_register(char *name, stat_plugin_create_func *create_func,
    void *state)
{
	struct stat_plugin *p;

	/* Does a plugin match this particular name? */
	if (stat_plugin_lookup_by_name(name) != NULL) {
		fprintf(stderr, "%s: duplicate plugin name (%s)\n",
		    __func__,
		    name);
		return (-1);
	}

	/* Add it to the plugin list */
	p = calloc(1, sizeof(*p));
	if (p == NULL) {
		warn("%s: calloc", __func__);
		return (-1);
	}

	p->state = state;
	p->name = strdup(name);
	p->stat_plugin_create = create_func;

	/* Add to the end of the list */
	TAILQ_INSERT_TAIL(&plugins_list, p, node);

	return (0);
}

struct stat_instance *
stat_plugin_create(char *name)
{
	struct stat_plugin *p;
	struct stat_instance *n;

	/* See if we have a plugin of this id */
	p = stat_plugin_lookup_by_name(name);
	if (p == NULL)
		return (NULL);

	n = calloc(1, sizeof(*n));
	if (n == NULL) {
		warn("%s: calloc", __func__);
		return (NULL);
	}

	if (p->stat_plugin_create(p, n) < 0) {
		free(n);
		return (NULL);
	}

	n->name = strdup(name);

	TAILQ_INSERT_TAIL(&instances_list, n, node);
	n->instance_id = plugin_instance_get_next_id();

	return (n);
}

int
stat_plugin_config(struct stat_instance *n, const char *config)
{

	return (n->stat_config(n, config));
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

void
plugin_fetch_all(void)
{
	struct stat_instance *n;

	TAILQ_FOREACH(n, &instances_list, node) {
		n->stat_fetch(n);
	}
}

