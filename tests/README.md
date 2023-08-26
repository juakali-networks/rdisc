# 
# Prerequisites
#


Docker is installed
 







# Building the docker Image
<pre> 

cd tests
docker build  -t rdisc_src .
</pre>


## Run docker image

## i<pre>docker run -it rdisc_src</pre>



# Create docker network

<pre>docker network create -d bridge rdisc-network</pre>


# Create two containers and map them to the network.

<pre>docker ps</pre>
<pre>docker run -itd --network=rdisc-network rdisc sh</pre>
<pre>docker run -itd --network=rdisc-network rdisc sh</pre>



# open the containers in different terminals

<pre>docker exec -it CONTAINER_ID1 rdisc sh</pre>
<pre>docker exec -it CONTAINER_ID2 rdisc sh</pre>


# Open the solicitor in one container


<pre>
git clone https://github.com/juakali-networks/rdisc.git
cd rdisc/src
make
</pre>


# Open the route advertiser in the container


<pre>
git clone https://github.com/juakali-networks/rdisc.git
cd rdisc/src
make
</pre>


# In the solicitor
<pre>sudo ./rdisc -s</pre>


# In the advertiser

<pre>sudo ./rdisc -r</pre>




