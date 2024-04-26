
import subprocess
import re
from subprocess import PIPE
import pyshark
import os
import time
import paramiko
from scp.scp import SCPClient
import threading
# from scp.SCPClient import SCPClient

class router_discovery():

    def __init__(self):

        # Configs. Change your settings here
        self._pwd = "lubuntu"
        self._user_name = "lubuntu"

        self._ip1 = "192.168.0.34"
        self._ip2 = "192.168.0.240"
    
  
        self._router_advertisement_msg_type = 1
        self._all_host_mcast_addr = "224.0.0.1"
        self._icmp_ra_type = "9"
        self._icmp_ra_code = "0"
        self._dest_addr = self._ip1
    
        self._file = 'agent_adv.pcap'
        self._local_path = '/home/dancer/rdisc/tests/results'

 
    def step_1(self):

        subprocess.run(["rm results/agent_adv.pcap"], shell=True, capture_output=False)

        print("Send router solicitation message\n")

        vm_user = "%s@%s" % (self._user_name, self._ip2)
        try:
            vm2_process = subprocess.Popen(['ssh','-tt', vm_user, "echo '%s' | sudo -S  ./rdisc/src/rdisc -s" % self._pwd],
                                   stdin=subprocess.PIPE, 
                                   stdout = subprocess.PIPE,
                                   universal_newlines=True,
                                bufsize=0)
            vm2_process.communicate()
            vm2_process.kill()
        
        except Exception as err:
            print("Connecting to VM with IP %s failed with error %s" % (self._ip2, err))
            return False
    
        time.sleep(10)


        return True

    def step_2(self):

        # Create threads for each command
        thread1 = threading.Thread(target=self.capture_packet)
        thread2 = threading.Thread(target=self.run_router_advert)
        
        # Start both threads
        thread1.start()
        time.sleep(5)
        thread2.start()

        # Wait for both threads to finish
        thread1.join()
        thread2.join()

        print("Both commands completed")


        state = self.read_packet_header()

        if state is True:
            print("Test Passed")
        else:
            print("Test Failed")

        
        # self.clean_up()

        return state


    def read_packet_header(self):
        """
        Check IP packet header
        """
        state = list()


        ssh = self.createSSHClient(self._ip2, 22, self._user_name, self._pwd)
        scp = SCPClient(ssh.get_transport())
        scp.get(remote_path=self._file, local_path=self._local_path)
        scp.close()

        vm_user = "%s@%s" % (self._user_name, self._ip2)

        ma_process = subprocess.Popen(['ssh','-tt', vm_user, "echo '%s' | sudo -S rm advert.pcap\n" % self._pwd],
                                    stdin=subprocess.PIPE,
                                    stdout = subprocess.PIPE,
                                    universal_newlines=True,
                                    bufsize=0)
        ma_process.communicate()

        ma_process.kill()

        # read pcap file and read packet fields
        pcap_file = pyshark.FileCapture('/home/dancer/rdisc/tests/results/advert.pcap')
        
        try:

            aa_process = subprocess.Popen(['ssh','-tt', vm_user, "echo '%s' | sudo -S  ./mip/src/mip -m" % self._pwd],
                                    stdin=subprocess.PIPE, 
                                    stdout = subprocess.PIPE,
                                    universal_newlines=True,
                                bufsize=0)
            
            aa_process.communicate()
            aa_process.kill()
            
        except Exception as err:
            print("Connecting to Foriegn Agent VM with IP %s failed with error %s" % (self._ip1, err))
            return False

        print("Mobile Node sending Registration Reply Packet to Foreign Adent\n")

        time.sleep(5)

        vm_user = "%s@%s" % (self._user_name, self._ip2)

        try:
            ma_process = subprocess.Popen(['ssh','-tt', vm_user, "echo %s | sudo -S  ./mip/src/mip -r" % self._pwd],
                                   stdin=subprocess.PIPE, 
                                   stdout = subprocess.PIPE,
                                   universal_newlines=True,
                                bufsize=0)

            ma_process.communicate()
            ma_process.kill()

        except Exception as err:
            print("Connecting to Mobile Agent VM with IP %s failed with error %s" % (self._ip2, err))
            return False

        state = self.check_packet_header()

        if state is True:
            print("Test Passed")
        else:
            print("Test Failed")

        # self.clean_up()

        return state


    def check_packet_header(self):
        """
        Check IP packet header
        """
        state = list()

        print("\nCapturing wireshark pcap packet")

        vm_user = "%s@%s" % (self._user_name, self._ip2)

        try:
            ma_process = subprocess.Popen(['ssh','-tt', vm_user, "echo '%s' | sudo -S  tcpdump -i enp0s3 icmp -c 1 -w agent_adv.pcap\n" % self._pwd],
                                    stdin=subprocess.PIPE,
                                    stdout = subprocess.PIPE,
                                    universal_newlines=True,
                                bufsize=0)
            ma_process.communicate()
            ma_process.kill()

        except Exception as err:
             print("Connecting to Mobile Agent VM with IP %s failed with error %s" % (self._ip2, err))
             return False
    
        print("\nEnd of capturing wireshark pcap packet")

        # username = "%s" % self._pwd
        ssh = self.createSSHClient(self._ip2, 22, self._pwd, self._pwd)
        scp = SCPClient(ssh.get_transport())
        scp.get(remote_path=self._file, local_path=self._local_path)
        scp.close()
    
        vm_user = "%s@%s" % (self._user_name, self._ip2)

        ma_process = subprocess.Popen(['ssh','-tt', vm_user, "echo %s | sudo -S rm agent_adv.pcap\n" % self._pwd],
                                    stdin=subprocess.PIPE,
                                    stdout = subprocess.PIPE,
                                    universal_newlines=True,
                                    bufsize=0)
        ma_process.communicate()

        ma_process.kill()


        # read pcap file and read packet fields
        pcap_file = pyshark.FileCapture('/home/dancer/mip/tests/Results/agent_adv.pcap')

        try:
            for packet in pcap_file:

           #     tos_hex_value = int(packet.layers[1].dsfield, 16)
                dst_addr = packet.layers[1].dst
                icmp_type = packet.layers[2].type
                icmp_code = packet.layers[2].code
                icmp_router_addr = packet.layers[2].router_address


                if dst_addr == self._all_host_mcast_addr:
                    print("\nRouter sent router advertisement message to host on the all host multicast IP address %s as expected\n" % dst_addr)
                    state.append(True)
                else:
                    print("\nRouter advertisement message is not sent to the all host multicast IP address %s but to destination address %s\n" % (self._all_host_mcast_addr, dst_addr))
                    state.append(False)

                if icmp_type == self._icmp_ra_type:
                    print("\nRouter advertisement message is sent with correct ICMP type number %s\n" % icmp_type)
                    state.append(True)
                else:
                    print("\nRouter advertisement message is sent with wrong ICMP type number %s and not type number %s\n" % (self._icmp_ra_type, icmp_type))
                    state.append(False)


                if icmp_code == self._icmp_ra_code:
                    print("\nRouter advertisement message is sent with correct ICMP code %s\n" % icmp_code)
                    state.append(True)
                else:
                    print("\nRouter advertisement message is sent with wrong ICMP code %s and not code %s\n" % (self._icmp_ra_code, icmp_type))
                    state.append(False)

            if icmp_router_addr == self._ip1:
                    print("\nRouter advertisement message sent with the proper router address\n" % icmp_router_addr)
                    state.append(True)
            else:
                    print("\nRouter advertisement message is not sent with the proper router address %s\n" % (self._ip1, icmp_router_addr))
                    state.append(False)


        except Exception as err:
            print("Failed to  captured packet with error %s" % err)
            state.append(False)
        return all(state) if state else False


    def createSSHClient(self, server, port, user, password):
        client = paramiko.SSHClient()
        client.load_system_host_keys()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        client.connect(server, port, user, password)
        return client

    def run_router_advert(self):
    
        print("\nSend Router Advertisement multicast packet\n")
       
        vm_user = "%s@%s" % (self._user_name, self._ip1)
        try:
            vm1_process = subprocess.Popen(['ssh','-tt', vm_user, "echo '%s' | sudo -S  ./rdisc/src/rdisc -r" % self._pwd],
                                    stdin=subprocess.PIPE, 
                                    stdout = subprocess.PIPE,
                                    universal_newlines=True,
                                bufsize=0)
            vm1_process.communicate()
            vm1_process.kill()
            
        except Exception as err:
            print("Connecting to VM with IP %s failed with error %s" % (self._ip1, err))
            return False
        
        return True


    def capture_packet(self):

        print("\nCapturing wireshark pcap packet")

        vm_user = "%s@%s" % (self._user_name, self._ip2)
        try:
            vm2_process = subprocess.Popen(['ssh','-tt', vm_user, "echo '%s' | sudo -S  tcpdump -i enp0s3 icmp -c 1 -w agent_adv.pcap\n" % self._pwd],
                                    stdin=subprocess.PIPE,
                                    stdout = subprocess.PIPE,
                                    universal_newlines=True,
                                bufsize=0)
            vm2_process.communicate()
            vm2_process.kill()

        except Exception as err:
             print("Connecting to Mobile Agent VM with IP %s failed with error %s" % (self._ip1, err))
             return False

        print("\nEnd of capturing wireshark pcap packet")

        return True

    def clean_up(self):
        """
        Reboot VMs
        """

        vm1_user = "%s@%s" % (self._user_name, self._ip1)
        vm1_process = subprocess.Popen(['ssh','-tt', vm1_user, "echo '%s' | sudo -S  reboot\n" % self._pwd],
                                    stdin=subprocess.PIPE,
                                    stdout = subprocess.PIPE,
                                    universal_newlines=True,
                                bufsize=0)

        vm2_user = "%s@%s" % (self._user_name, self._ip2)
        vm2_process = subprocess.Popen(['ssh','-tt', vm2_user, "echo '%s' | sudo -S  reboot\n" % self._pwd],
                                    stdin=subprocess.PIPE,
                                    stdout = subprocess.PIPE,
                                    universal_newlines=True,
                                bufsize=0)



        vm1_process.communicate()
        vm2_process.communicate()


        vm1_process.kill()
        vm2_process.kill()

        print("Wait 120s for VMs to reboot")
        time.sleep(120)
        print("VMs are fully rebooted")

        return True

router_discovery().step_1()
router_discovery().step_2()





 












