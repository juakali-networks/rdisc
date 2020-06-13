/*
 * Rdisc for Ubuntu
 * Peter Wanawunga
 * Peter.Wanawunga@gmail.com
test
*/





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
	struct icmphdr *icp = (struct icmphdr *) ALLIGN(outpack);
	int packetlen;

	printK("Sending solicitations to %s\n", sin->sin_addr);
	icp->type = ICMP_ROUTERSOLICIT;
	icp->code = 0;
	icp->checksum = 0;
	icp->un.gateway = 0; /* Reserved */
	packetlen = 8;

	/* Compute ICMP checksum here */
	icmphr->checksum = ip_fast_checksum( (unsigned short *)icp, packetlen );

	
}
