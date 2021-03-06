#ifndef	__STAT_PLUGIN_H__
#define	__STAT_PLUGIN_H__

struct stat_plugin;
struct stat_instance;

typedef	int stat_plugin_create_func(struct stat_plugin *,
	    struct stat_instance *);

struct stat_plugin {
	char *name;
	void *state;

	TAILQ_ENTRY(stat_plugin) node;
	stat_plugin_create_func * stat_plugin_create;
};

extern	int stat_plugin_register(char *name,
	    stat_plugin_create_func *create_func, void *state);

extern	struct stat_instance * stat_plugin_create(char *name);

extern	void plugin_init(void);
extern	void plugin_finish(void);
extern	uint32_t plugin_instance_get_next_id(void);
extern	void plugin_fetch_all(void);
extern	int stat_plugin_config(struct stat_instance *, const char *);

#endif	/* __STAT_PLUGIN_H__ */
