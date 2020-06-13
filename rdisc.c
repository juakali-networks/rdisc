/*
 * Rdisc for Ubuntu
 * Peter Wanawunga
 * Peter.Wanawunga@gmail.com
*/


#include <uapi/linux/icmp.h>
#include <uapi/linux/in.h>
#include <uapi/linux/if.h>
#include <asm/checksum.h> 

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
	int packetlen;

	printk("Sending solicitations to %s\n", sin->sin_addr);
	icmph->type = ICMP_ROUTERSOLICIT;
	icmph->code = 0;
	icmph->checksum = 0;
	icmph->un.gateway = 0; /* Reserved */
	packetlen = 8;

	/* Compute ICMP checksum here */
        icmph->checksum = ip_fast_csum( (unsigned short *)icmph, packetlen);

	/* i = sendmcast(socketfd, (char *)outpack, packetlen, sin);*/

	
}
