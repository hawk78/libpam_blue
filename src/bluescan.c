/*
** Copyright (C) 2003 Hagen Paul Pfeifer <hagen@jauu.net>
**
** $Id: bluescan.c 19 2004-04-21 18:24:46Z pfeifer $
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#if HAVE_CONFIG_H
#include  <config.h>
#endif /* HAVE_CONFIG_H */

#include "global.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#include <termios.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#define	false		-1
#define	true		1

#ifdef DEBUG
static inline void
show_debug(char *msg)
{
	fprintf(stderr,"bluescan: %s\n",msg);
}
#else
static inline void show_debug(char *msg) {}
#endif /* DEBUG */

/*
** derived form bluez-utils/tools/l2ping.c
** tim - tim@spacetim.de
*/

/* Improved connection timeout 
** David De Sousa - davidesousa@gmail.com
*/

int bluescan
(struct entrybuff *enbfptr)
{
	struct sockaddr_l2 addr;
	socklen_t optlen;
	unsigned char *buf;
	char str[18];
	int i, sk, lost;
	int size    = 44;
	int ident   = 200;
	int timeout;
	bdaddr_t bdaddr;
	long skctl; // Socket Flags
	/* Setting variables for select to work. */
	fd_set skset; 
	struct timeval tv;
	int lon, retval;

	bacpy(&bdaddr, BDADDR_ANY);

	if ( (enbfptr->timeout < 3) || (enbfptr->timeout > 15))
		timeout = DEFAULTTIMEOUT;
	else
		timeout = enbfptr->timeout;


	buf = malloc(L2CAP_CMD_HDR_SIZE + size);
	if (!buf) {
		show_debug("Can't allocate buffer");
		return(false);
	}

	/* Create socket */
	sk = socket(PF_BLUETOOTH, SOCK_RAW, BTPROTO_L2CAP);
	if (sk < 0) {
		show_debug("Can't create socket");
		free(buf);
		return(false);
	}

	/* Bind to local address */
	memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	bacpy(&addr.l2_bdaddr, &bdaddr);

	if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		show_debug("Can't bind socket");
		close(sk);
		free(buf);
		return(false);
	}

	/* Set non-blocking state for the socket. DDS */
        skctl = fcntl(sk, F_GETFL, NULL);
        skctl |= O_NONBLOCK;
        fcntl(sk, F_SETFL, skctl);
				

	/* Connect to remote device */
	memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	str2ba(enbfptr->btmac, &addr.l2_bdaddr);

	/* Connecting with timeout. */
	if (connect(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		if (errno == EINPROGRESS) {
			tv.tv_sec = timeout; 
		        tv.tv_usec = 0;
			FD_ZERO(&skset); 
		        FD_SET(sk, &skset);
			if (select(sk+1, NULL, &skset, NULL, &tv) > 0) { 
				lon = sizeof(int); 
				getsockopt(sk, SOL_SOCKET, SO_ERROR, (void*)(&retval), &lon); 
				if (retval)
					show_debug ("Error Connecting!!!");
				else
					show_debug ("Connected!!!");
        		} 
			else {
				show_debug ("Connection Timeout [bluetooth device up?]");
				retval = 1;
			}
		}
		else {
			show_debug ("Error Connecting");
			retval = 1;
		}
		
		if (retval) {
	                close(sk);
        	        free(buf);
                	return(false);
		}
	}

	/* Set socket to original mode (blocking). */

	skctl = fcntl(sk, F_GETFL, NULL); 
	skctl &= (~O_NONBLOCK); 
	fcntl(sk, F_SETFL, skctl); 

	/* Get local address */
	memset(&addr, 0, sizeof(addr));
	optlen = sizeof(addr);

	if (getsockname(sk, (struct sockaddr *) &addr, &optlen) < 0) {
		show_debug("Can't get local address");
		close(sk);
		free(buf);
		return(false);
	}

	ba2str(&addr.l2_bdaddr, str);

	/* Initialize buffer */
	for (i = 0; i < size; i++)
		buf[L2CAP_CMD_HDR_SIZE + i] = (i % 40) + 'A';

	l2cap_cmd_hdr *cmd = (l2cap_cmd_hdr *) buf;

	/* Build command header */
	cmd->code  = L2CAP_ECHO_REQ;
	cmd->ident = ident;
	cmd->len   = htobs(size);

	/* Send Echo Request */
	if (send(sk, buf, L2CAP_CMD_HDR_SIZE + size, 0) <= 0) {
		show_debug("Send failed");
		close(sk);
		free(buf);
		return(false);
	}

	/* Wait for Echo Response */
	lost = 0;
	while (1) {
		struct pollfd pf[1];
		int err;

		pf[0].fd = sk;
		pf[0].events = POLLIN;

		if ((err = poll(pf, 1, timeout * 1000)) < 0) {
			show_debug("Poll failed");
			close(sk);
			free(buf);
			return(false);
		}

		if (!err) {
			lost = 1;
			break;
		}

		if ((err = recv(sk, buf, L2CAP_CMD_HDR_SIZE + size, 0)) < 0) {
			show_debug("Recv failed");
			close(sk);
			free(buf);
			return(false);
		}

		if (!err){
			show_debug("Disconnected\n");
			close(sk);
			free(buf);
			return(false);
		}

		cmd->len = btohs(cmd->len);

		if (cmd->ident != ident)
			continue;

		if (cmd->code == L2CAP_ECHO_RSP)
			break;
		if (cmd->code == L2CAP_COMMAND_REJ) {
			show_debug("Peer doesn't support Echo packets\n");
			close(sk);
			free(buf);
			return(false);
		}

	}
	show_debug("pam_blue successful!");
	close(sk);
	free(buf);
	return(true);
}
