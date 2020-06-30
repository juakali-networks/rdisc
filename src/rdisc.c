/*
 * Rdisc for Ubuntu
 * Peter Wanawunga
 * Peter.Wanawunga@gmail.com
*/
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <net/if.h>
/*  <linux/inet.h> 
#include <uapi/linux/icmp.h>
#include <asm/checksum.h> 
#include <net/sock.h> */
#include "rdisc.h"

int main() {
    /* solicitor();*/
	/*test*/
   return (0);
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

