#ifndef	__STAT_INSTANCE_H__
#define	__STAT_INSTANCE_H__

struct stat_plugin;
struct stat_instance;

typedef int stat_fetch_func(struct stat_instance *);

struct stat_instance {
	struct stat_plugin *parent;
	char *name;
	void *state;
	uint32_t instance_id;
	TAILQ_ENTRY(stat_instance) node;

	stat_fetch_func *stat_fetch;
};

#endif
