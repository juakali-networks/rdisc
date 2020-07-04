/*
 * Rdisc for Ubuntu
 * Peter Wanawunga
 * Peter.Wanawunga@gmail.com
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <net/if.h>
/* #include <asm-generic/socket.h>
#include <asm-generic/types.h>

#include <uapi/linux/icmp.h>
#include <asm/checksum.h> 
#include <net/sock.h> */
#include "rdisc.h"

/*
 * 			M A I N
 */
char    *sendaddress, *recvaddress;


int main(int argc, char **argv) 
{

    	/* solicitor();*/
	/*test*/
	printf("Testing the main function...\n");

	struct sockaddr_in from = { 0 };
	char **av = argv;
	struct sockaddr_in *to = &whereto;
	struct sockaddr_in joinaddr;
	sigset_t sset, sset_empty;
#ifdef RDISC_SERVER
	int val;

	atexit(close_stdout);
	min_adv_int =( max_adv_int * 3 / 4);
	lifetime = (3*max_adv_int);
#endif

	argc--, av++;
	while (argc > 0 && *av[0] == '-') {
		while (*++av[0]) {
			switch (*av[0]) {
			case 'd':
				debug = 1;
				break;
			case 't':
				trace = 1;
				break;
			case 'v':
				verbose++;
				break;
			case 's':
				solicit = 1;
				break;
#ifdef RDISC_SERVER
			case 'r':
				responder = 1;
				break;
#endif
			case 'a':
				best_preference = 0;
				break;
			case 'b':
				best_preference = 1;
				break;
			case 'f':
				forever = 1;
				break;
			case 'V':
				printf(IPUTILS_VERSION("rdisc"));
				printf("Compiled %s ENABLE_RDISC_SERVER.\n",
#ifdef RDISC_SERVER
						"with"
#else
						"without"
#endif
				);
				exit(0);
#ifdef RDISC_SERVER
			case 'T':
				argc--, av++;
				if (argc != 0) {
					val = strtol(av[0], (char **)NULL, 0);
					if (val < 4 || val > 1800)
						error(1, 0, "Bad Max Advertisement Interval: %d",
							     val);
					max_adv_int = val;
					min_adv_int =( max_adv_int * 3 / 4);
					lifetime = (3*max_adv_int);
				} else {
					prusage();
					/* NOTREACHED*/
				}
				goto next;
			case 'p':
				argc--, av++;
				if (argc != 0) {
					val = strtol(av[0], (char **)NULL, 0);
					preference = val;
				} else {
					prusage();
					/* NOTREACHED*/
				}
				goto next;
#endif
			default:
				prusage();
				/* NOTREACHED*/
			}
		}
#ifdef RDISC_SERVER
next:
#endif
		argc--, av++;
	}
	if( argc < 1)  {
		if (support_multicast()) {
			sendaddress = ALL_ROUTERS_ADDRESS;
#ifdef RDISC_SERVER
			if (responder)
				sendaddress = ALL_HOSTS_ADDRESS;
#endif
		} else
			sendaddress = "255.255.255.255";
	} else {
		sendaddress = av[0];
		argc--;
	}

	if (argc < 1) {
		if (support_multicast()) {
			recvaddress = ALL_HOSTS_ADDRESS;
#ifdef RDISC_SERVER
			if (responder)
				recvaddress = ALL_ROUTERS_ADDRESS;
#endif
		} else
			recvaddress = "255.255.255.255";
	} else {
		recvaddress = av[0];
		argc--;
	}
	if (argc != 0) {
		error(0, 0, "Extra parameters");
		prusage();
		/* NOTREACHED */
	}

#ifdef RDISC_SERVER
	if (solicit && responder) {
		prusage();
		/* NOTREACHED */
	}
#endif

	if (!(solicit && !forever)) {
		do_fork();
/*
 * Added the next line to stop forking a second time
 * Fraser Gardiner - Sun Microsystems Australia
 */
		forever = 1;
	}

	memset( (char *)&whereto, 0, sizeof(struct sockaddr_in) );
	to->sin_family = AF_INET;
	to->sin_addr.s_addr = inet_addr(sendaddress);

	memset( (char *)&joinaddr, 0, sizeof(struct sockaddr_in) );
	joinaddr.sin_family = AF_INET;
	joinaddr.sin_addr.s_addr = inet_addr(recvaddress);

#ifdef RDISC_SERVER
	if (responder)
		iputils_srand();
#endif

	if ((socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		logperror("socket");
		exit(5);
	}

	setlinebuf( stdout );

	signal_setup(SIGINT, finish );
	signal_setup(SIGTERM, graceful_finish );
	signal_setup(SIGHUP, initifs );
	signal_setup(SIGALRM, timer );

	sigemptyset(&sset);
	sigemptyset(&sset_empty);
	sigaddset(&sset, SIGALRM);
	sigaddset(&sset, SIGHUP);
	sigaddset(&sset, SIGTERM);
	sigaddset(&sset, SIGINT);

	init();
	if (join(socketfd, &joinaddr) < 0) {
		logmsg(LOG_ERR, "Failed joining addresses\n");
		exit (2);
	}

	timer();	/* start things going */

	for (;;) {
		unsigned char	packet[MAXPACKET];
		int len = sizeof (packet);
		socklen_t fromlen = sizeof (from);
		int cc;

		cc=recvfrom(socketfd, (char *)packet, len, 0,
			    (struct sockaddr *)&from, &fromlen);
		if (cc<0) {
			if (errno == EINTR)
				continue;
			logperror("recvfrom");
			continue;
		}

		sigprocmask(SIG_SETMASK, &sset, NULL);
		pr_pack( (char *)packet, cc, &from );
		sigprocmask(SIG_SETMASK, &sset_empty, NULL);
	}
	/*NOTREACHED*/
}
/*
 * 			S O L I C I T O R
 *
 * Compose and transmit an ICMP ROUTER SOLICITATION REQUEST packet.
 * The IP packet will be added on by the kernel.
 */
void
solicitor(struct sockaddr_in *sin)
{
	static unsigned char outpack[MAXPACKET];
	struct icmphdr *icmph;
	int packetlen, i;

	printf("Sending solicitations to %s\n", pr_name(sin->sin_addr));
	icmph->type = ICMP_ROUTERSOLICIT;
	icmph->code = 0;
	icmph->checksum = 0;
	icmph->un.gateway = 0; /* Reserved */
	packetlen = 8;

	/* Compute ICMP checksum here */
        icmph->checksum = in_cksum((unsigned short *)icmph, packetlen);

	i = sendmcast(socketfd, (char *)outpack, packetlen, sin);

	if( i < 0 || i != packetlen )  {
		if( i<0 ) {
		    printf("solicitor:sendto");
		}
		/*printk("wrote %s %d chars, ret=%d\n", sendaddress, packetlen, i );*/
	}
	
}

int sendmcast(int socket, char *packet, int packetlen, struct sockaddr_in *sin)
{
	int i, cc;

	for (i = 0; i < num_interfaces; i++) {
		if ((interfaces[i].flags & (IFF_BROADCAST|IFF_POINTOPOINT|IFF_MULTICAST)) == 0)
			continue;
	       cc = sendmcastif(socket, packet, packetlen, sin, &interfaces[i]);
		if (cc!= packetlen) {
			return (cc);
		}
	}
	return (packetlen);
}


int sendmcastif(int socket, char *packet, int packetlen, struct sockaddr_in *sin,
	    struct interface *ifp)
	{
	int cc;
	struct ip_mreqn mreqn;

	memset(&mreqn, 0, sizeof(mreqn));
	mreqn.imr_ifindex = ifp->ifindex;
	mreqn.imr_address = ifp->localaddr;
	printf("Multicast to interface %s, %s\n", ifp->name, pr_name(mreqn.imr_address));

	if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_IF,
		       (char *)&mreqn,
		       sizeof(mreqn)) < 0) {
				printf("setsockopt (IP_MULTICAST_IF): Cannot send multicast packet over interface %s, %s\n", ifp->name, pr_name(mreqn.imr_address));
		return (-1);

	}
	cc = sendto(socket, packet, packetlen, 0,
		    (struct sockaddr *)sin, sizeof (struct sockaddr));
	if (cc!= packetlen) {
		printf("sendmcast: Cannot send multicast packet over interface %s, %s\n",
		       ifp->name, pr_name(mreqn.imr_address));
	}
	return (cc);
}



/*
 *			P R _ N A M E
 *
 * Return a string name for the given IP address.
 */
char *pr_name(struct in_addr addr)
{
	struct sockaddr_in sin = { .sin_family = AF_INET, .sin_addr = addr };
	char hnamebuf[NI_MAXHOST] = "";
	static char buf[sizeof(hnamebuf) + INET6_ADDRSTRLEN + sizeof(" ()")];

	getnameinfo((struct sockaddr *) &sin, sizeof sin, hnamebuf, sizeof hnamebuf, NULL, 0, 0);
	snprintf(buf, sizeof buf, "%s (%s)", hnamebuf, inet_ntoa(addr));
	return(buf);
}

/*
 *			I N _ C K S U M
 *
 * Checksum routine for Internet Protocol family headers (C Version)
 *
 */
#if BYTE_ORDER == LITTLE_ENDIAN
# define ODDBYTE(v)	(v)
#elif BYTE_ORDER == BIG_ENDIAN
# define ODDBYTE(v)	((unsigned short)(v) << 8)
#else
# define ODDBYTE(v)	htons((unsigned short)(v) << 8)
#endif

unsigned short in_cksum(unsigned short *addr, int len)
{
	int nleft = len;
	unsigned short *w = addr;
	unsigned short answer;
	int sum = 0;

	/*
	 *  Our algorithm is simple, using a 32 bit accumulator (sum),
	 *  we add sequential 16 bit words to it, and at the end, fold
	 *  back all the carry bits from the top 16 bits into the lower
	 *  16 bits.
	 */
	while( nleft > 1 )  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if( nleft == 1 )
		sum += ODDBYTE(*(unsigned char *)w);	/* le16toh() may be unavailable on old systems */

	/*
	 * add back carry outs from top 16 bits to low 16 bits
	 */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return (answer);
}

