import os
import subprocess


class router_discovery():

    def __init__(self):


        self.__docker_id1 = None
        self.__docker_id2 = None
        self.__docker_id3 = None
        self.__docker_id4 = None



        self.__cmd = None

       # docker run --name rdisc_source -itd rdisc_image sh
       # docker run --name rdisc_destination -itd rdisc_image sh


       #  docker exec -it rdisc_source ./rdisc -r
       #  docker exec -it rdisc_destination ./rdisc -s

    def step_1(self):
        self.kill_docker_instances()
 
        print("Starting Docker containers")
        test = subprocess.check_output("pwd")
        #         self.__docker_id1 = subprocess.check_output(self.__cmd.format(self.__system_vlan_interface, self.__ip1, mod_id).split()).strip()
        self.__cmd = "docker run --name rdisc_src -itd rdisc_image sh"
        self.__docker_id1 = subprocess.check_output(self.__cmd.format().split()).strip()

        print(self.__docker_id1)


        self.__cmd = "docker run --name rdisc_dst -itd rdisc_image sh"

        self.__docker_id2 = subprocess.check_output(self.__cmd.format().split()).strip()


        print(self.__docker_id2)

        #      self.__cmd = "docker exec -it rdisc_source9 sh"
        self.__cmd = "docker exec -it rdisc_dst ./rdisc -r"
        self.__docker_id3 = subprocess.check_output(self.__cmd.format().split()).strip()
        print(self.__docker_id3)

        print("aaaa")
        self.__cmd = "docker exec -it rdisc_src ./rdisc -s"
        self.__docker_id4 = subprocess.check_output(self.__cmd.format().split()).strip()
        print("ccc")
        print(self.__docker_id4)
        print("bbb")
        return True


    def kill_docker_instances(self):
        print("Killing all Docker containers")
        try:
            # subprocess.check_output("docker container prune", shell=True)
            countainer_hashes = subprocess.check_output("docker kill $(docker ps -q)", shell=True)
            print(countainer_hashes)
        except Exception:
            print("No Docker containers were running")
        print "Killing Docker containers"
        #  os.system("docker kill {}".format(self.__docker_id1))
        # os.system("docker kill {}".format(self.__docker_id2))
        # print "Killing Docker containers"
        # os.system("docker kill {}".format(self.__docker_id3))
        # os.system("docker kill {}".format(self.__docker_id4))
        # All existing SNMP Trap configurations are deleted from IDU


router_discovery().step_1()


