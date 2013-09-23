#ifndef	__STAT_INSTANCE_H__
#define	__STAT_INSTANCE_H__

struct stat_plugin;

struct stat_instance {
	struct stat_plugin *parent;
	char *name;
	void *state;
	TAILQ_ENTRY(stat_instance) node;
};

#endif
