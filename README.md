# 
# ICMPv4 Router Solicitation Protocol and ICMPv4 Router Adverisement Protocol
#

This is the linux implementation of the IICMPv4 Router Solicitation Protocol and ICMPv4 Router Adverisement Protocol (RFC1256) using the C programming language. 

The ICMP Router Soicitation Message is sent by a computer host to any routers that may exist on a local area network requesting them to advertise their presence on the network. This message is sent not only at bootup of the computer but also periodically and is useful in that the computer administrator does not need to add IP routes manually to the host computer.


The ICMP Router Adverisement Message is sent by a router (or routing computer)  to announce that its IP address as available for routing.  THe ICMP Router Adverisement Message is sent on receiving the ICMP Router Soicitation Message.


# Usage
git clone https://github.com/Peters-Lab/rdisc.git


cd rdisc/src

make

sudo ./rdisc -s

Send three solicitation messages to LAN (using the all-routers.mcast.net (224.0.0.2) asking if there is a router(224.0.0.2)). 


sudo ./rdisc -r

Send a router advertisement message either periodically or in response to a router solicitation message.


sudo ./rdisc -s 192.168.0.10

Send solicitation messages to a specific address. This is also broadcast on the LAN.

######################   
# Linux PC 1         #  
#                    # 
#                    # 
# sudo ./rdisc -s    #    
#                    # 
#                    #                              
######################   




