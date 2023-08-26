#This is a sample Image 
FROM ubuntu 
MAINTAINER demousr@gmail.com 

RUN apt-get update

# Install vim
RUN apt install vim -y

# Install python 3.8
# RUN apt install software-properties-common -y
# RUN add-apt-repository ppa:deadsnakes/ppa
# RUN apt install python3.8 -y

RUN apt-get install -y gcc
RUN apt-get install -y net-tools 
RUN apt-get install -y iputils-ping
RUN apt-get install -y tcpdump
RUN apt-get install -y git
RUN apt-get install -y make
CMD [“echo”,”Image created”] 
