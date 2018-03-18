ifdef CONFIG_TIMER_CHECKS
CFLAGS += -DCONFIG_TIMER_CHECKS
endif

SOURCES = ../timer.c ../arch/$(ARCH)/timer.c

SOURCES += eth.c if.c arp.c ip.c
SOURCES += tr_chksum.c ../sys/chksum.c pkt-mempool.c route.c

CFLAGS += -DCONFIG_PKT_NB_MAX=$(CONFIG_PKT_NB_MAX)
CFLAGS += -DCONFIG_PKT_SIZE=$(CONFIG_PKT_SIZE)

ifdef CONFIG_IP_TTL
CFLAGS += -DCONFIG_IP_TTL=$(CONFIG_IP_TTL)
endif

ifdef CONFIG_ICMP
SOURCES += icmp.c
CFLAGS += -DCONFIG_ICMP
endif

ifdef CONFIG_UDP
SOURCES += udp.c
CFLAGS += -DCONFIG_UDP
endif

ifdef CONFIG_DNS
ifeq ($(CONFIG_UDP),)
$(error CONFIG_UDP is required for DNS)
endif
ifeq ($(CONFIG_EVENT),)
$(error CONFIG_EVENT is required for DNS)
endif
SOURCES += dns.c
CFLAGS += -DCONFIG_DNS
endif

ifdef CONFIG_TCP
SOURCES += tcp.c
CFLAGS += -DCONFIG_TCP
ifdef CONFIG_TCP_CLIENT
CFLAGS += -DCONFIG_TCP_CLIENT
endif
CFLAGS += -DCONFIG_TCP_MAX_CONNS=$(CONFIG_TCP_MAX_CONNS)
endif
ifdef CONFIG_TCP_RETRANSMIT
CFLAGS += -DCONFIG_TCP_RETRANSMIT
ifdef CONFIG_TCP_RETRANSMIT_TIMEOUT
CFLAGS += -DCONFIG_TCP_RETRANSMIT_TIMEOUT=$(CONFIG_TCP_RETRANSMIT_TIMEOUT)
else
CFLAGS += -DCONFIG_TCP_RETRANSMIT_TIMEOUT=3000
endif
endif

ifdef CONFIG_ARP_TABLE_SIZE
CFLAGS += -DCONFIG_ARP_TABLE_SIZE=$(CONFIG_ARP_TABLE_SIZE)
endif

ifdef CONFIG_ARP_EXPIRY
CFLAGS += -DCONFIG_ARP_EXPIRY
endif

ifeq "$(or $(CONFIG_UDP), $(CONFIG_TCP))" "y"
SOURCES += socket.c
CFLAGS += -DCONFIG_TRANSPORT_MAX_HT=$(CONFIG_TRANSPORT_MAX_HT)
CFLAGS += -DCONFIG_TCP_SYN_TABLE_SIZE=$(CONFIG_TCP_SYN_TABLE_SIZE)
CFLAGS += -DCONFIG_EPHEMERAL_PORT_START=$(CONFIG_EPHEMERAL_PORT_START)
CFLAGS += -DCONFIG_EPHEMERAL_PORT_END=$(CONFIG_EPHEMERAL_PORT_END)
endif

ifdef CONFIG_HT_STORAGE
CFLAGS += -DCONFIG_HT_STORAGE
ifeq ($(CONFIG_MAX_SOCK_HT_SIZE),)
$(error CONFIG_MAX_SOCK_HT_SIZE not set)
endif
CFLAGS += -DCONFIG_MAX_SOCK_HT_SIZE=$(CONFIG_MAX_SOCK_HT_SIZE)
SOURCES += ../sys/hash-tables.c
endif

ifdef CONFIG_BSD_COMPAT
CFLAGS += -DCONFIG_BSD_COMPAT
SOURCES += ../sys/errno.c
endif

ifdef CONFIG_EVENT
CFLAGS += -DCONFIG_EVENT
endif

ifdef CONFIG_SWEN
SOURCES += swen.c
CFLAGS += -DCONFIG_SWEN
ifdef CONFIG_RF_RECEIVER
CFLAGS += -DCONFIG_RF_RECEIVER
ifdef CONFIG_RF_GENERIC_COMMANDS
CFLAGS += -DCONFIG_RF_GENERIC_COMMANDS
endif
endif
ifdef CONFIG_RF_SENDER
CFLAGS += -DCONFIG_RF_SENDER
endif
endif

ifdef TEST
CFLAGS += -DTEST
SOURCES += tests.c
endif
