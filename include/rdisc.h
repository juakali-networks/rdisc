/*
 * rdisc.h
 */

#ifndef _RDISC_H
#define _RDISC_H

/* include here*/


#define ALL_HOSTS_ADDRESS		"224.0.0.1"
#define ALL_ROUTERS_ADDRESS		"224.0.0.2"

/* Codes for Router Discovery*/
#define ICMP_ROUTERSOLICIT          10       /* Router Solicitation  */


#define	MAXPACKET	4096	/* max packet size */			

/* Router constants */
#define	MAX_INITIAL_ADVERT_INTERVAL	16
#define	MAX_INITIAL_ADVERTISEMENTS  	3
#define	MAX_RESPONSE_DELAY		2	/* Not used */

/* Host constants */
#define MAX_SOLICITATIONS 		3
#define SOLICITATION_INTERVAL 		3
#define MAX_SOLICITATION_DELAY		1	/* Not used */

#define INELIGIBLE_PREF			0x80000000	/* Maximum negative */

#define MAX_ADV_INT 600


/* Common variables */
int verbose = 0;
int debug = 0;
int trace = 0;
int solicit = 0;
int ntransmitted = 0;
int nreceived = 0;
int forever = 0;	/* Never give up on host. If 0 defer fork until
			 * first response.
			 */

#ifdef RDISC_SERVER
/* Router variables */
int responder;
int max_adv_int = MAX_ADV_INT;
int min_adv_int;
int lifetime;
int initial_advert_interval = MAX_INITIAL_ADVERT_INTERVAL;
int initial_advertisements = MAX_INITIAL_ADVERTISEMENTS;
int preference = 0;		/* Setable with -p option */
#endif

/* Host variables */
int max_solicitations = MAX_SOLICITATIONS;
unsigned int solicitation_interval = SOLICITATION_INTERVAL;
int best_preference = 1;  	/* Set to record only the router(s) with the
				   best preference in the kernel. Not set
				   puts all routes in the kernel. */

struct sockaddr_in whereto;/* Address to send to */

static void solicitor(struct sockaddr_in *sin);
static char *pr_name(struct in_addr addr);
static unsigned short in_cksum(unsigned short *addr, int len);

static void graceful_finish(void);
static void finish(void);
static void timer(void);
static void initifs(void);

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
