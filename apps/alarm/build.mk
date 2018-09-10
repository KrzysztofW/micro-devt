ifdef CONFIG_SCHEDULER_MAX_TASKS
CFLAGS += -DCONFIG_SCHEDULER_MAX_TASKS=$(CONFIG_SCHEDULER_MAX_TASKS)
ifdef CONFIG_SCHEDULER_TASK_WATER_MARK
CFLAGS += -DCONFIG_SCHEDULER_TASK_WATER_MARK=$(CONFIG_SCHEDULER_TASK_WATER_MARK)
endif
endif

ifdef CONFIG_USART0
CFLAGS += -DCONFIG_USART0
ifdef CONFIG_USART0_SPEED
CFLAGS += -DCONFIG_USART0_SPEED=$(CONFIG_USART0_SPEED)
else
CFLAGS += -DCONFIG_USART0_SPEED=57600
endif
endif

ifdef CONFIG_USART1
CFLAGS += -DCONFIG_USART1
ifdef CONFIG_USART1_SPEED
CFLAGS += -DCONFIG_USART1_SPEED=$(CONFIG_USART1_SPEED)
else
CFLAGS += -DCONFIG_USART1_SPEED=57600
endif
endif

ifdef CONFIG_RND_SEED
CFLAGS += -DCONFIG_RND_SEED
SOURCES += ../../sys/random.c
endif

ifeq "$(or $(CONFIG_RF_RECEIVER), $(CONFIG_RF_SENDER))" "y"
SOURCES += ../../drivers/rf.c rf.c module.c
endif
ifdef CONFIG_RF_RECEIVER
CFLAGS += -DCONFIG_RF_RECEIVER
ifdef CONFIG_RF_GENERIC_COMMANDS
CFLAGS += -DCONFIG_RF_GENERIC_COMMANDS
endif
endif
ifdef CONFIG_RF_SENDER
CFLAGS += -DCONFIG_RF_SENDER
endif

ifdef CONFIG_RF_RECEIVER
CFLAGS += -DCONFIG_RF_RECEIVER
endif

ifdef CONFIG_RF_CHECKS
CFLAGS += -DCONFIG_RF_CHECKS
endif

ifdef CONFIG_AVR_SIMU
SOURCES += $(ARCH_DIR)/$(ARCH)/simu.c
CFLAGS += -DCONFIG_AVR_SIMU -DCONFIG_AVR_SIMU_PATH=$(CONFIG_AVR_SIMU_PATH)
endif

ifdef CONFIG_TIMER_CHECKS
CFLAGS += -DCONFIG_TIMER_CHECKS
endif

ifdef CONFIG_ETHERNET
CFLAGS += -DCONFIG_ETHERNET
endif

ifdef CONFIG_IP
CFLAGS += -DCONFIG_IP
endif

ifdef CONFIG_UDP
CFLAGS += -DCONFIG_UDP
endif

ifdef CONFIG_DNS
CFLAGS += -DCONFIG_DNS
endif

ifdef CONFIG_TCP
CFLAGS += -DCONFIG_TCP
ifdef CONFIG_TCP_CLIENT
CFLAGS += -DCONFIG_TCP_CLIENT
endif
endif

ifdef CONFIG_BSD_COMPAT
CFLAGS += -DCONFIG_BSD_COMPAT
endif

ifdef CONFIG_EVENT
CFLAGS += -DCONFIG_EVENT
endif

ifdef CONFIG_GSM_SIM900
SOURCES += ../../drivers/gsm-at.c gsm.c
CFLAGS += -DCONFIG_GSM_SIM900
endif

ifdef CONFIG_NETWORKING
CFLAGS += -DCONFIG_NETWORKING
SOURCES += network.c
endif
ifdef CONFIG_PKT_NB_MAX
CFLAGS += -DCONFIG_PKT_NB_MAX=$(CONFIG_PKT_NB_MAX)
CFLAGS += -DCONFIG_PKT_SIZE=$(CONFIG_PKT_SIZE)
CFLAGS += -DCONFIG_PKT_DRIVER_NB_MAX=$(CONFIG_PKT_DRIVER_NB_MAX)
endif
ifdef CONFIG_SWEN_ROLLING_CODES
CFLAGS += -DCONFIG_SWEN_ROLLING_CODES
endif
ifdef CONFIG_SWEN_L3
CFLAGS += -DCONFIG_SWEN_L3
endif
