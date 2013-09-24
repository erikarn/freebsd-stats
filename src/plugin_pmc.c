#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include <sys/types.h>
#include <sys/stdint.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/queue.h>
#include <sys/param.h>
#include <sys/cpuset.h>

#include <assert.h>

#include <pmc.h>

#include <net/if.h>
#include <net/if_mib.h>

#include "stat_instance.h"
#include "stat_plugin.h"

//#include "pmcstat.h"

#include "plugin_pmc.h"



void
plugin_pmc_print_counters(struct stat_instance *instance)
{
	struct plugin_pmc_instance *n = instance->state;
	struct pmcstat_ev *ev;
	pmc_value_t value;
	int c;

	TAILQ_FOREACH(ev, &n->ev_list, ev_next) {

		if (pmc_read(ev->ev_pmcid, &value) < 0) {
			warn("%s: can't read PMC '%s'", __func__, ev->ev_name);
			continue;
		}

		/* First, header */
		c = PMC_IS_SYSTEM_MODE(ev->ev_mode) ? 's' : 'p';
		if (c == 's')
			(void) fprintf(stdout, "s/%02d/%s=",
			    ev->ev_cpu,
			    ev->ev_name);
		else
			(void) fprintf(stdout, "p/%s=",
			    ev->ev_name);

		/* Then, data */
		(void) fprintf(stdout, "%ju\n",
		    (uintmax_t) ev->ev_cumulative ? value :
		       (value - ev->ev_saved));

		/* Cumulative? Save counter value */
		if (ev->ev_cumulative == 0)
			ev->ev_saved = value;
	}
}

/*
 * Start all active pmcs.
 */
static int
plugin_pmc_start_pmcs(struct stat_instance *instance)
{
	struct plugin_pmc_instance *n = instance->state;
	struct pmcstat_ev *ev;

	TAILQ_FOREACH(ev, &n->ev_list, ev_next) {
	    assert(ev->ev_pmcid != PMC_ID_INVALID);
	    if (pmc_start(ev->ev_pmcid) < 0) {
	        warn("ERROR: Cannot start pmc 0x%x \"%s\"",
		    ev->ev_pmcid, ev->ev_name);
//		pmcstat_cleanup();
		return (-1);
	    }
	}
	return (0);
}

/*
 * Allocate PMCs for the given list of PMC events.
 */
static int
plugin_pmc_alloc_pmcs(struct stat_instance *instance)
{
	struct plugin_pmc_instance *n = instance->state;
	struct pmcstat_ev *ev;

	TAILQ_FOREACH(ev, &n->ev_list, ev_next) {
		fprintf(stderr,
		    "%s: attempting: ev_spec=%s, ev_mode=%d, ev_flags=0x%x ev_cpu=%d\n",
		    __func__,
		    ev->ev_spec,
		    (int) ev->ev_mode,
		    ev->ev_flags,
		    ev->ev_cpu);
		if (pmc_allocate(ev->ev_spec, ev->ev_mode,
		    ev->ev_flags, ev->ev_cpu, &ev->ev_pmcid) < 0) {
			warn("%s: ERROR: Cannot allocate %s-mode pmc with specification \"%s\"",
			    __func__,
			    PMC_IS_SYSTEM_MODE(ev->ev_mode) ?
			    "system" : "process", ev->ev_spec);
			/* XXX cleanup? */
			return (-1);
		}
	}
	return (0);
}

/*
 * Allocate a new PMC.
 */
static struct pmcstat_ev *
plugin_pmc_alloc_event(struct stat_instance *instance, enum pmc_mode ev_mode,
    uint32_t ev_flags, uint32_t ev_cpu, const char *ev_spec)
{
	struct pmcstat_ev *ev;
	int c;

	ev = calloc(1, sizeof(*ev));
	if (ev == NULL) {
		warn("%s: calloc", __func__);
		return (NULL);
	}

	ev->ev_mode = ev_mode;
	ev->ev_flags = ev_flags;
	ev->ev_cpu = ev_cpu;
	ev->ev_spec = strdup(ev_spec);
	ev->ev_pmcid = PMC_ID_INVALID;

	/* Extract event name */
	c = strcspn(ev_spec, ", \t");
	ev->ev_name = malloc(c + 1);
	(void) strncpy(ev->ev_name, ev_spec, c);
	*(ev->ev_name + c) = '\0';

	/* Don't add it to the event list yet */
	return (ev);
}

static int
plugin_pmc_fetch(struct stat_instance *instance)
{

	plugin_pmc_print_counters(instance);
	return (0);
}

static int
plugin_pmc_config(struct stat_instance *instance, const char *config)
{
	struct pmcstat_ev *ev;
	struct plugin_pmc_instance *n = instance->state;

	/* Allocate on CPU 0 only for now */
	ev = plugin_pmc_alloc_event(instance, PMC_MODE_SC, 0, 0, "instructions");
	if (ev == NULL)
		return (-1);

	/* Add to the list */
	TAILQ_INSERT_TAIL(&n->ev_list, ev, ev_next);

	/* XXX Do other CPU events if required */

	/* XXX TODO: split the config from the startup method */

	/* Completed? Allocate PMCs */
	if (plugin_pmc_alloc_pmcs(instance) < 0)
		return (-1);

	/* Start PMCs */
	if (plugin_pmc_start_pmcs(instance) < 0)
		return (-1);

	return (0);
}


static int
plugin_pmc_create_instance(struct stat_plugin *plugin,
    struct stat_instance *instance)
{
	struct plugin_pmc_instance *n;

	n = calloc(1, sizeof(*n));
	if (n == NULL) {
		warn("%s: calloc", __func__);
		return (-1);
	}

	TAILQ_INIT(&n->ev_list);

	/* XXX methodize this! */
	instance->is_ready = 0;
	instance->state = n;
	instance->stat_fetch = plugin_pmc_fetch;
	instance->stat_config = plugin_pmc_config;

	/* Everything's ok for now */
	return (0);
}

static int
plugin_pmc_init_cpumask(struct plugin_pmc_parent *parent)
{
	int hcpu, ncpu;
	size_t dummy;

	CPU_ZERO(&parent->cpumask);

	/*
	 * The initial CPU mask specifies all non-halted CPUS in the
	 * system.
	 */
	dummy = sizeof(int);
	if (sysctlbyname("hw.ncpu", &ncpu, &dummy, NULL, 0) < 0) {
		warn("%s: ERROR: Cannot determine the number of CPUs",
		     __func__);
		return (-1);
	}
	for (hcpu = 0; hcpu < ncpu; hcpu++)
		CPU_SET(hcpu, &parent->cpumask);

	parent->ncpu = ncpu;
	return (0);
}

int
plugin_pmc_register(void)
{
	struct plugin_pmc_parent * parent_state;

	fprintf(stderr, "%s: registering\n", __func__);

	parent_state = calloc(1, sizeof(*parent_state));
	if (parent_state == NULL) {
		warn("%s: malloc", __func__);
		goto error;
	}

	if (stat_plugin_register("pmc", plugin_pmc_create_instance,
	    parent_state) < 0) {
		goto error;
	}

	/*
	 * Get number of active CPUs.
	 */
	if (plugin_pmc_init_cpumask(parent_state) < 0)
		goto error;

	/*
	 * Initialise the pmc framework.
	 */
	if (pmc_init() < 0) {
		warn("%s: pmc_init() failed", __func__);
		goto error;
	}

	return (0);

error:
	if (parent_state)
		free(parent_state);
	return (-1);
}
