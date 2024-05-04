
# Test setup

Two Virtual Machines (VMs)

On Host PC, enable logging in without username by running the following commands for each VM
<pre>ssh-keygen</pre>
<pre>ssh-copy-id vm_username@vm_IP</pre>
<pre>ssh-copy-id vm_username@vm_IP</pre>

on Host PC, install SCP
<pre>cd /usr/local/lib/python3.8/dist-packages</pre>
<pre>sudo git clone https://github.com/jbardin/scp.py</pre>
<pre>mv scp.py scp</pre>

Install wireshark on your host PC
Install pyshark on your host PC



Clone the rdisc repository 
<pre>git clone https://github.com/juakali-networks/rdisc/tree/master></pre>

# Run tests

Navigate to the tests folder and in the python files, adapt the IP addresses and VM usernames and Passwords to match your own

Run the VM configuration script
<pre>cd tests</pre>
<pre>python3 configure_VMs.py</pre>

Run the tests
<pre>python3 router_solicit.py</pre>
<pre>python3 router_advert.py</pre>







