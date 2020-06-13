/*
 * rdisc.h
 */

#ifndef _RDISC_H
#define _RDISC_H

/* include here*/

/* Codes for Router Discovery*/
#define ICMP_ROUTERSOLICIT          10       /* Router Solicitation  */


#define	MAXPACKET	4096	/* max packet size */			

static void solicitor(struct sockaddr_in *sin);

/* Statics */
static int num_interfaces;
static struct interface *interfaces;


int socketfd;		    /* Socket file descriptor */
struct sockaddr_in whereto; /* Address to send to */






#endif /* _RDISC_H*/
