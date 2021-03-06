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

#include <stdio.h>
#include <stdint.h>
#include <sys/chksum.h>
#include "ip.h"

static void ip_chksum(void)
{
	uint8_t data[] = {
		0x45, 0x00, 0x00, 0x54, 0xc6, 0x5c, 0x40, 0x00, 0x40, 0x01,
		0xed, 0xa4, 0xc0, 0xa8, 0x02, 0xa3, 0xac, 0xd9, 0x16, 0x83, 0x08, 0x00,
		0xfd, 0x94, 0x03, 0xdd, 0x00, 0x01, 0xa6, 0xb7, 0x78, 0x59, 0x00, 0x00,
		0x00, 0x00, 0x14, 0xa9, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11,
		0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
		0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
		0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
		0x36, 0x37
	};
	ip_hdr_t *ip = (ip_hdr_t *)data;
	int i;

	for (i = 0; i < sizeof(ip_hdr_t); i++) {
		printf("0x%02X ", data[i]);
	}
	puts("");
	printf("ip cksum:0x%X\n", ip->chksum);
	printf("ip chsum computed: 0x%X\n", cksum(ip, ip->hl * 4));
}

static void random_data_chksum(void)
{
	uint8_t data[] = { 0x69, 0xaa, 0x06, 0x0, 0x0, 0x62, 0x6c, 0x61, 0x62, 0x6c, 0x61 };
	uint16_t csum = cksum(data, sizeof(data));

	printf("chksum: 0x%X\n", csum);
}

int main(int argc, char **argv)
{
	ip_chksum();
	random_data_chksum();
	return 0;
}
