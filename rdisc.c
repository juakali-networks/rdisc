/*
 * Rdisc for Ubuntu
 * Peter Wanawunga
 * Peter.Wanawunga@gmail.com
*/

#include <linux/inet.h>
#include <uapi/linux/icmp.h>
#include <uapi/linux/in.h>
#include <uapi/linux/if.h>
#include <asm/checksum.h> 
#include <net/sock.h>
#include "rdisc.h"

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

	printk(KERN_INFO "Sending solicitations to %p\n", sin->sin_addr);
	icmph->type = ICMP_ROUTERSOLICIT;
	icmph->code = 0;
	icmph->checksum = 0;
	icmph->un.gateway = 0; /* Reserved */
	packetlen = 8;

	/* Compute ICMP checksum here */
        icmph->checksum = ip_fast_csum( (unsigned short *)icmph, packetlen);

	i = sendmcast(socketfd, (char *)outpack, packetlen, sin);

	if( i < 0 || i != packetlen )  {
		if( i<0 ) {
		    printk(KERN_WARNING "solicitor:sendto");
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

int
sendmcastif(int socket, char *packet, int packetlen, struct sockaddr_in *sin,
	    struct interface *ifp)
{

	struct ip_mreqn mreqn;

	memset(&mreqn, 0, sizeof(mreqn));
	mreqn.imr_ifindex = ifp->ifindex;
	mreqn.imr_address = ifp->localaddr;
	printk(KERN_DEBUG, "Multicast to interface %s, %p\n",
			 ifp->name, mreqn.imr_address);
	if (ip_setsockopt(socket, IPPROTO_IP, IP_MULTICAST_IF,
		       (char *)&mreqn,
		       sizeof(mreqn)) < 0) {
				printk(KERN_ERR, "ip_setsockopt (IP_MULTICAST_IF): 
						Cannot send multicast packet over interface %s, %p\n",
		       ifp->name, mreqn.imr_address);
		return (-1);
}




