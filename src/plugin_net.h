#ifndef	__PLUGIN_NET_H__
#define	__PLUGIN_NET_H__

struct plugin_net_parent {
};

struct plugin_net_instance {
	char *netif;
	int row_id;	/* inside ifmib */

	uint64_t last_tx_pkts;
	uint64_t last_rx_pkts;
	uint64_t last_tx_bytes;
	uint64_t last_rx_bytes;
};

extern	int plugin_net_register(void);

#endif
