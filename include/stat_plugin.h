#ifndef	__STAT_PLUGIN_H__
#define	__STAT_PLUGIN_H__

struct stat_plugin;
struct stat_instance;

typedef	struct stat_instance * stat_plugin_create_func(struct stat_plugin *);

struct stat_plugin {
	char *name;
	void *state;

	stat_plugin_create_func * stat_plugin_create;
};

extern	int stat_plugin_register(char *name,
	    stat_plugin_create_func *create_func, void *state);

extern	struct stat_instance * stat_plugin_create(char *name);

extern	void plugin_init();
extern	void plugin_finish();

#endif	/* __STAT_PLUGIN_H__ */
