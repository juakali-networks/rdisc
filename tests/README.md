# 
# Test setup
#


Two Virtual Machines (VM).
Install wireshark on your host PC
Clone the rdisc repository <https://github.com/juakali-networks/rdisc/tree/master>

Navigate to the tests folder and run the VM configuration script
<pre>cd tests</pre>
<pre>python3 configure_VMs.py</pre>

Run the tests
<pre>python3 router_solicit.py</pre>
<pre>python3 router_advert.py</pre>







