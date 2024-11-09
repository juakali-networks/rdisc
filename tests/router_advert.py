
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

class router_advert_test():

    def __init__(self):

        # Configs. Change your settings here
        self._pwd = "lubuntu"
        self._user_name = "lubuntu"

        self._ip1 = "192.168.0.33"
        self._ip2 = "192.168.0.242"
        self._local_path = '/home/dancer/rdisc/tests/results'
        ### Settings #####
  
        self._router_advertisement_msg_type = 1
        self._all_host_mcast_addr = "224.0.0.1"
        self._icmp_ra_type = "9"
        self._icmp_ra_code = "0"
        self._dest_addr = self._ip1
    
        self._file_name = 'router_advert.pcap'
        self._file_path = "%s/%s" % (self._local_path, self._file_name )

    def step_1(self):
        
        subprocess.run(["rm %s" % self._file_path], shell=True, capture_output=False)

        print("Router solicitation message sent by host\n")
        
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
    
        time.sleep(5)


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

        
        self.clean_up()

        return state

    def read_packet_header(self):
        """
        Check IP packet header
        """
        state = list()

        # username = "%s" % self._pwd
        ssh = self.createSSHClient(self._ip2, 22, self._pwd, self._pwd)
        scp = SCPClient(ssh.get_transport())
        scp.get(remote_path=self._file_name, local_path=self._local_path)
        scp.close()
    
        vm_user = "%s@%s" % (self._user_name, self._ip2)

        vm2_process = subprocess.Popen(['ssh','-tt', vm_user, "echo %s | sudo -S rm router_advert.pcap\n" % self._pwd],
                                    stdin=subprocess.PIPE,
                                    stdout = subprocess.PIPE,
                                    universal_newlines=True,
                                    bufsize=0)
        vm2_process.communicate()

        vm2_process.kill()


        # read pcap file and read packet fields
        pcap_file = pyshark.FileCapture(self._file_path)

        try:
            for packet in pcap_file:

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
                    print("\nRouter advertisement message is sent with wrong ICMP code %s and not code %s\n" % (self._icmp_ra_code, icmp_code))
                    state.append(False)

                if icmp_router_addr == self._ip1:
                    print("\nRouter advertisement message sent with the correct router address %s\n" % icmp_router_addr)
                    state.append(True)
                else:
                    print("\nRouter advertisement message is not sent with the wrong router address %s and not the expected router address %s\n" % (icmp_router_addr, self._ip1))
                    state.append(False)

        except Exception as err:
            print("Failed to read captured packet with error %s" % err)
            state.append(False)
        return all(state) if state else False


    def createSSHClient(self, server, port, user, password):
        client = paramiko.SSHClient()
        client.load_system_host_keys()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        client.connect(server, port, user, password)
        return client

    def run_router_advert(self):
    
        print("\nRouter sends router advertisement multicast packet\n")
       
        vm_user = "%s@%s" % (self._user_name, self._ip1)
        try:
            vm1_process = subprocess.Popen(['ssh','-tt', vm_user, "echo '%s' | sudo -S ./rdisc/src/rdisc -r" % self._pwd],
                                    stdin=subprocess.PIPE, 
                                    stdout = subprocess.PIPE,
                                    universal_newlines=True,
                                bufsize=0)
            vm1_process.communicate()
            vm1_process.kill()
            
        except Exception as err:
            print("Connecting to Virtual Machine with IP %s failed with error %s" % (self._ip1, err))
            return False
        
        return True


    def capture_packet(self):

        print("\nCapturing wireshark pcap packet")

        vm_user = "%s@%s" % (self._user_name, self._ip2)
        try:
            vm2_process = subprocess.Popen(['ssh','-tt', vm_user, "echo '%s' | sudo -S  tcpdump -i enp0s3 icmp and src %s -c 1 -w router_advert.pcap\n" % (self._pwd, self._ip1)],
                                    stdin=subprocess.PIPE,
                                    stdout = subprocess.PIPE,
                                    universal_newlines=True,
                                bufsize=0)
            vm2_process.communicate(timeout=40)
            vm2_process.kill()

        except Exception as err:
             print("Connecting to Virtual Machine with IP %s failed with error %s" % (self._ip1, err))
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

router_advert_test().step_1()
router_advert_test().step_2()





 












