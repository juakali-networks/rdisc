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

#define MAXIFS 32

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

#define TIMER_INTERVAL 	3
#define GETIFCONF_TIMER	30

#define ALLIGN(ptr)	(ptr)
struct sockaddr_in whereto;/* Address to send to */
struct table *table;

static void solicitor(struct sockaddr_in *sin);
static void prusage(void);
static char *pr_name(struct in_addr addr);
static void pr_pack(char *buf, int cc, struct sockaddr_in *from);

static char *pr_type(int t);
static unsigned short in_cksum(unsigned short *addr, int len);
static void record_router(struct in_addr router, int preference, int ttl);
static void add_route(struct in_addr addr);
static void del_route(struct in_addr addr);
static void rtioctl(struct in_addr addr, int op);

static void init(void);
static void graceful_finish(void);
static void finish(void);
static void timer(void);
static void initifs(void);
static void do_fork(void);
static void initlog(void);
static void logmsg(int const prio, char const *const fmt, ...);
static int logging = 0;
static void logperror(char *str);
static int join(int sock, struct sockaddr_in *sin);
static void signal_setup(int signo, void (*handler)(void));

static int left_until_advertise;
static void age_table(int time);
static void record_router(struct in_addr router, int preference, int ttl);

static void discard_table(void);

static int num_interfaces;
static struct interface *interfaces;
static int interfaces_size;			/* Number of elements in interfaces */

static int support_multicast(void);
static int is_directly_connected(struct in_addr in);
static int max_preference(void);

static int sendmcast(int s, char *packet, int packetlen, struct sockaddr_in *sin);
static int sendmcastif(int s, char *packet, int packetlen, struct sockaddr_in *sin, struct interface *ifp);
static int sendbcast(int s, char *packet, int packetlen);
static int sendbcastif(int s, char *packet, int packetlen, struct interface *ifp);

static __inline__ int ismulticast(struct sockaddr_in *sin)
{
	return IN_CLASSD(ntohl(sin->sin_addr.s_addr));
}

static __inline__ int isbroadcast(struct sockaddr_in *sin)
{
        return (sin->sin_addr.s_addr == INADDR_BROADCAST);
}


int socketfd;		    /* Socket file descriptor */

/*struct sockaddr_in whereto; / Address to send to /

int setsockopt(int socket, int level, int option_name,
const void *option_value, socklen_t option_len);
*/
struct table *find_router(struct in_addr addr);


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

/*
 * TABLES
 */
struct table {
	struct in_addr	router;
	int		preference;
	int		remaining_time;
	int		in_kernel;
	struct table	*next;
};


#if defined(__GLIBC__) && __GLIBC__ < 2
/* For router advertisement */
struct icmp_ra
{
	unsigned char	icmp_type;		/* type of message, see below */
	unsigned char	icmp_code;		/* type sub code */
	unsigned short	icmp_cksum;		/* ones complement cksum of struct */
	unsigned char	icmp_num_addrs;
	unsigned char	icmp_wpa;		/* Words per address */
	short 	icmp_lifetime;
};

struct icmp_ra_addr
{
	uint32_t	ira_addr;
	uint32_t	ira_preference;
};
#else
#define icmp_ra icmp
#endif



#endif /* _RDISC_H*/
