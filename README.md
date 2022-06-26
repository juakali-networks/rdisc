# 
# ICMPv4 Router Solicitation Protocol and ICMPv4 Router Advertisement Protocol
#

This is the linux implementation of the ICMPv4 Router Solicitation Protocol and the ICMPv4 Router Advertisement Protocol (RFC1256) using the C programming language. 

The ICMP Router Solicitation Message is sent by a computer host to any routers (or routing computers) that may exist on a local area network requesting them to advertise their presence on the network. This message should be sent not only at bootup of the computer but also periodically and is useful in that the computer administrator does not need to add IP routes manually to the host computer.


The ICMP Router Advertisement Message is sent by a router (or routing computer) to announce that its IP address is available for routing.  The ICMP Router Adverisement Message is also sent when the router (or routing computer) receives the ICMP Router Soicitation Message.


# Building
<pre> git clone https://github.com/Peters-Lab/rdisc.git


cd rdisc/src

make</pre>


# Usage

<pre>sudo ./rdisc -s</pre>



Send three solicitation messages to LAN (using the all-routers.mcast.net (224.0.0.2) asking if there is a router(224.0.0.2)). 


<pre>sudo ./rdisc -r</pre>


Send a router advertisement message either periodically or in response to a router solicitation message.


<pre>sudo ./rdisc -s 192.168.0.10</pre>


Send solicitation messages to a specific address. This is also broadcast on the LAN.

# Example

Linux PC 1 and Linux PC 2 all on the same LAN

On Linux PC 1. run
<pre>sudo ./rdisc -s</pre>


On Linux PC 1. run

<pre>sudo ./rdisc -r</pre>




