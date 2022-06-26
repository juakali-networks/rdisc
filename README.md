# rdisc
ICMPv4 Router Solicitation Protocol and ICMPv4 Router Adverisement Protocol
#

I am involved in another open source project whereby I
maintain a git repository of the linux implementation of the ICMP router
solicitation protocol (part of RFC1256). I wrote the linux implementation
of the protocol using the C programming language. The ICMP Router So-
licitation Message is sent by a computer host to any routers that may exist
on a local area network requesting them to advertise their presence on the
network. This message is sent not only at bootup of the computer but also
periodically and is useful in that the computer administrator does not need
to add IP routes manually to the host computer.
