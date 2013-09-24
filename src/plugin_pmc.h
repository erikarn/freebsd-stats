#ifndef	__PLUGIN_PMC_H__
#define	__PLUGIN_PMC_H__

struct pmcstat_ev {
	TAILQ_ENTRY(pmcstat_ev) ev_next;
	int             ev_count; /* associated count if in sampling mode */
	uint32_t        ev_cpu;   /* cpus for this event */
	int             ev_cumulative;  /* show cumulative counts */
	int             ev_flags; /* PMC_F_* */
	int             ev_fieldskip;   /* #leading spaces */
	int             ev_fieldwidth;  /* print width */
	enum pmc_mode   ev_mode;  /* desired mode */
	char           *ev_name;  /* (derived) event name */
	pmc_id_t        ev_pmcid; /* allocated ID */
	pmc_value_t     ev_saved; /* for incremental counts */
	char           *ev_spec;  /* event specification */
};

struct plugin_pmc_parent {
	cpuset_t cpumask;
	int ncpu;
};

struct plugin_pmc_instance {
	TAILQ_HEAD(, pmcstat_ev) ev_list;
};

extern	int plugin_pmc_register(void);

#endif
