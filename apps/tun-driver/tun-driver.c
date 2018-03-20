/*
  sudo ip link set up dev tap0 && sudo ip a a 1.1.2.1/24 dev tap0 && sudo ip link set tap0 address 54:52:00:02:00:40 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <poll.h>

#ifdef CONFIG_USE_CAPABILITIES
#include <sys/capability.h>
#define STRING(e) #e
#endif

#include <timer.h>
#include <net/eth.h>
#include <net/route.h>
#define SOCKLEN_DEFINED
#include <net/socket.h>
#undef SOCKLEN_DEFINED
#include <net_apps/net_apps.h>
#include <net/pkt-mempool.h>

#include <linux/if.h>
#include <linux/if_tun.h>

struct pollfd tun_fds[1];

pkt_t *recv(void)
{
	return 0;
}

uint16_t send(const buf_t *out)
{
	ssize_t nwrite;

	if (out->len == 0)
		return 0;
	nwrite = write(tun_fds[0].fd, out->data, out->len);
	if (nwrite < 0) {
		if (errno != EAGAIN) {
			fprintf(stderr, "tun device is not up (%m)\n");
			exit(EXIT_FAILURE);
		}
		return 0;
	}
	return nwrite;
}

iface_t iface = {
	.flags = IF_UP|IF_RUNNING,
	.mac_addr = { 0x54, 0x52, 0x00, 0x02, 0x00, 0x41 },
	.ip4_addr = { 1,1,2,2 },
	.ip4_mask = { 255, 255, 255, 0 },
	.send = &send,
};

static void tun_alloc(char *dev)
{
	struct ifreq ifr;

	assert(dev != NULL);
	if ((tun_fds[0].fd = open("/dev/net/tun", O_RDWR)) < 0) {
		fprintf(stderr , "%s: can't open tun device (%m)\n", __func__);
		exit(EXIT_FAILURE);
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
	strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	if (ioctl(tun_fds[0].fd, TUNSETIFF, (void *) &ifr) < 0) {
		fprintf(stderr , "%s: can't ioctl tun device (%m)\n", __func__);
		exit(EXIT_FAILURE);
	}
	strncpy(dev, ifr.ifr_name, IFNAMSIZ);
	tun_fds[0].events = POLLIN;
}

static pkt_t *tun_receive_pkt(void)
{
	pkt_t *pkt;
	uint8_t buf[2048];
	ssize_t nread;

	if (poll(tun_fds, 1, -1) < 0) {
		if (errno == EINTR)
			return NULL;
		fprintf(stderr, "can't poll on tun fd (%m (%d))\n", errno);
		return NULL;
	}
	if ((tun_fds[0].revents & POLLIN) == 0)
		return NULL;

	nread = read(tun_fds[0].fd, buf, sizeof(buf));
	if (nread < 0 && errno == EAGAIN) {
		return NULL;
	}

	if (nread < 0) {
		if (errno == EAGAIN)
			return NULL;
		fprintf(stderr , "%s: reading tun device failed (%m)\n",
			__func__);
		exit(EXIT_FAILURE);
	}
	if (nread == 0)
		return NULL;
#if 0
	printf("read: %ld\n", nread);
	for (i = 0; i < nread; i++) {
		printf(" 0x%X", buf[i]);
	}
	puts("");
#endif
	/* Writes have higher priority.
	 * Don't allocate new packets if there are queued TX pkts.
	 */

	if ((pkt = pkt_alloc()) == NULL) {
		fprintf(stderr, "can't alloc a packet\n");
		return NULL;
	}

	if (buf_add(&pkt->buf, buf, nread) < 0) {
		pkt_free(pkt);
		return NULL;
	}
	return pkt;
}

int main(int argc, char *argv[])
{
	char dev[IFNAMSIZ+1];
#ifdef CONFIG_USE_CAPABILITIES
	cap_t caps;
	cap_value_t cap = CAP_NET_ADMIN;
	const char *capname = STRING(CAP_NET_ADMIN);
	cap_flag_value_t cap_effective;
	cap_flag_value_t cap_inheritable;
	cap_flag_value_t cap_permitted;
#endif

	memset(dev, 0, sizeof(dev));
	if (argc > 1)
		strncpy(dev, argv[1], sizeof(dev) - 1);

#ifdef CONFIG_USE_CAPABILITIES
	if ((caps = cap_get_proc()) == NULL) {
		fprintf(stderr , "%s: can't get capabilities (%m)\n", __func__);
		exit(EXIT_FAILURE);
	}

	/* Check that we have the required capabilities */
	/* At this point we only require CAP_NET_ADMIN to be permitted, */
	/* not effective as we will be enabling it later. */
	if (cap_get_flag(caps, cap, CAP_PERMITTED, &cap_permitted) < 0) {
		fprintf(stderr , "%s: can't get capabilities PERMITTED flag (%m)\n",
			__func__);
		exit(EXIT_FAILURE);
	}

	if (cap_get_flag(caps, cap, CAP_EFFECTIVE, &cap_effective) < 0) {
		fprintf(stderr , "%s: can't get capabilities EFFECTIVE flag (%m)\n",
			__func__);
		exit(EXIT_FAILURE);
	}
	if (cap_get_flag(caps, cap, CAP_INHERITABLE, &cap_inheritable) < 0) {
		fprintf(stderr , "%s: can't get capabilities INHERITABLE flag (%m)\n",
			__func__);
		exit(EXIT_FAILURE);
	}
	if (!cap_permitted) {
		fprintf(stderr, "%s not permitted, exiting\n", capname);
		exit(0);
	}

	/* And retain only what we require */
	if (cap_clear(caps) < 0) {
		fprintf(stderr , "%s: can't clear capabilities (%m)\n",
			__func__);
		exit(EXIT_FAILURE);
	}
	/* We must leave it permitted */
	if (cap_set_flag(caps, CAP_PERMITTED, 1, &cap, CAP_SET) < 0) {
		fprintf(stderr , "%s: can't set capabilities PERMITTED flag (%m)\n",
			__func__);
		exit(EXIT_FAILURE);
	}
	/* but also make it effective */
	if (cap_set_flag(caps, CAP_EFFECTIVE, 1, &cap, CAP_SET) < 0) {
		fprintf(stderr , "%s: can't set capabilities EFFECTIVE flag (%m)\n",
			__func__);
		exit(EXIT_FAILURE);
	}
	if (cap_set_proc(caps) < 0) {
		fprintf(stderr , "%s: can't set capabilities (%m)\n",
			__func__);
		exit(EXIT_FAILURE);
	}

#endif
	tun_alloc(dev);
	if (tun_fds[0].fd < 0)
		exit(0);

	/* wait for system to be initialized */
	puts("run in a terminal:");
	puts("sudo ip link set up dev tap0 && sudo ip a a 1.1.2.1/24 dev tap0");
	sleep(5);

#ifdef CONFIG_USE_CAPABILITIES
	/* And before anything else, clear all our capabilities */
	if (cap_clear(caps) < 0 || cap_set_proc(caps) < 0 || cap_free(caps) < 0) {
		fprintf(stderr , "%s: can't free capabilities (%m)\n",
			__func__);
		exit(EXIT_FAILURE);
	}
#endif
	if (if_init(&iface, &send, &recv) < 0) {
		fprintf(stderr, "can't init interface\n");
		return -1;
	}
	if (pkt_mempool_init() < 0) {
		fprintf(stderr, "can't initialize pkt pool\n");
		return -1;
	}

	if (fcntl(tun_fds[0].fd, F_SETFL, O_NONBLOCK) < 0) {
		fprintf(stderr, "can't set non blocking tcp socket (%m)\n");
		return -1;
	}

	timer_subsystem_init();

#ifdef CONFIG_TIMER_CHECKS
	timer_checks();
#endif

	socket_init();
	dft_route.iface = &iface;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	dft_route.ip = 0x01020101;
#endif
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	dft_route.ip = 0x01010201;
#endif

#ifdef CONFIG_UDP
	if (udp_init() < 0) {
		fprintf(stderr, "can't init udp sockets\n");
		return -1;
	}
#endif
#ifdef CONFIG_TCP
	if (tcp_init() < 0) {
		fprintf(stderr, "can't init tcp sockets\n");
		return -1;
	}
#endif
#ifdef CONFIG_DNS
	if (dns_resolver_init() < 0) {
		fprintf(stderr, "can't init dns resolver\n");
		return -1;
	}
#endif

	while (1) {
		pkt_t *pkt = tun_receive_pkt();

		if (pkt)
			eth_input(pkt, &iface);
#if defined(CONFIG_TCP) && !defined(CONFIG_EVENT)
		udp_app();
#endif
#if defined(CONFIG_TCP) && !defined(CONFIG_EVENT)
		tcp_app();
#endif
	}
	return 0;
}