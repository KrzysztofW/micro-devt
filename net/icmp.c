/*
 * microdevt - Microcontroller Development Toolkit
 *
 * Copyright (c) 2017, Krzysztof Witek
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "LICENSE".
 *
*/

#include <sys/chksum.h>
#include "icmp.h"
#include "eth.h"
#include "ip.h"
#if defined(CONFIG_UDP) && defined(CONFIG_EVENT)
#include "udp.h"
#include "socket.h"
#endif

int
icmp_output(pkt_t *out, iface_t *iface, int type, int code, uint16_t id,
	    uint16_t seq, const buf_t *id_data, uint16_t ip_flags)
{
	icmp_hdr_t *icmp_hdr;

	icmp_hdr = btod(out);
	icmp_hdr->type = type;
	icmp_hdr->code = code;
	icmp_hdr->cksum = 0;
	icmp_hdr->id = id;
	icmp_hdr->seq = seq;
	/* XXX check if out->data does not point to one byte more than it should */
	pkt_adj(out, (int)sizeof(icmp_hdr_t));
	buf_addbuf(&out->buf, id_data);
	pkt_adj(out, -(int)sizeof(icmp_hdr_t));
	icmp_hdr->cksum = cksum(icmp_hdr, sizeof(icmp_hdr_t) + id_data->len);
	pkt_adj(out, -(int)sizeof(ip_hdr_t));
	return ip_output(out, iface, ip_flags);
}

void icmp_input(pkt_t *pkt, iface_t *iface)
{
	icmp_hdr_t *icmp_hdr;
	ip_hdr_t *ip = btod(pkt);
	ip_hdr_t *ip2;
	buf_t id_data;
	pkt_t *out;

	/* XXX make sure pkt_adj is the same in all *_output() functions */
	pkt_adj(pkt, ip->hl * 4);

	icmp_hdr = btod(pkt);
	pkt_adj(pkt, sizeof(icmp_hdr_t));
	buf_init(&id_data, icmp_hdr->id_data, pkt_len(pkt));

	switch (icmp_hdr->type) {
	case ICMP_ECHO:
		if ((out = pkt_alloc()) == NULL) {
			/* inc stats */
			pkt_free(pkt);
			return;
		}
		pkt_adj(out, (int)sizeof(eth_hdr_t));
		ip2 = btod(out);
		ip2->dst = ip->src;
		ip2->src = ip->dst;
		ip2->p = IPPROTO_ICMP;
		/* XXX adj */
		pkt_adj(out, (int)sizeof(ip_hdr_t));
		icmp_output(out, iface, ICMP_ECHOREPLY, 0, icmp_hdr->id,
			    icmp_hdr->seq, &id_data, 0);
		break;
	case ICMP_ECHOREPLY:
		break;

#if defined(CONFIG_UDP) && defined(CONFIG_EVENT)
	case ICMP_UNREACH_PORT:
		ip2 = (ip_hdr_t *)&icmp_hdr->id;
		if (ip2->p == IPPROTO_UDP) {
			udp_hdr_t *udp_hdr;
			sock_info_t *sock_info;

			udp_hdr =  (udp_hdr_t *)((uint8_t *)ip2 + sizeof(ip_hdr_t));
			sock_info = udpport2sockinfo(udp_hdr->dst_port);
			if (sock_info)
				event_schedule_event(&sock_info->event,
						     EV_ERROR);
		}
		break;
#endif
	default:
		/* unsupported type */
		/* inc stats */
		break;
	}
	pkt_free(pkt);
}
