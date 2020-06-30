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
static char *pr_name(struct in_addr addr);
static unsigned short in_cksum(unsigned short *addr, int len);

/* Statics */
static int num_interfaces;
static struct interface *interfaces;
static int sendmcast(int s, char *packet, int packetlen, struct sockaddr_in *sin);

/*static int sendmcastif(int s, char *packet, int packetlen, struct sockaddr_in *sin, struct interface *ifp);
*/
int socketfd;		    /* Socket file descriptor */

/*struct sockaddr_in whereto; / Address to send to /

int setsockopt(int socket, int level, int option_name,
const void *option_value, socklen_t option_len);
*/

struct interface
{
	struct in_addr 	address;	/* Used to identify the interface */
	struct in_addr	localaddr;	/* Actual address if the interface */
	int 		preference;
	int		flags;
	struct in_addr	bcastaddr;
	struct in_addr	remoteaddr;
	struct in_addr	netmask;
	int		ifindex;
	char		name[IFNAMSIZ];
};





#endif /* _RDISC_H*/
