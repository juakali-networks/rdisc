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
 
        print "Starting Docker containers"
        self.__cmd = "docker run --name rdisc_source -itd rdisc_image sh"
        self.__docker_id1 = subprocess.check_output(self.__cmd)
        print("self.__docker_id1")
        print(self.__docker_id1)


        self.__cmd = "docker run --name rdisc_destination -itd rdisc_image sh"
       #  self.__docker_id2 = subprocess.check_output(self.__cmd).split()).strip()
        self.__docker_id2 = subprocess.check_output(self.__cmd)

        print("self.__docker_id2")

        self.__cmd = "docker exec -it rdisc_source sh"
        self.__docker_id3 = subprocess.check_output(self.__cmd)
        print("self.__docker_id3")

        self.__cmd = "docker exec -it rdisc_destination sh"
        self.__docker_id3 = subprocess.check_output(self.__cmd)
        print("self.__docker_id4")

        return True


    def kill_docker_instances(self):
        print("Killing all Docker containers")
        try:
            countainer_hashes = subprocess.check_output("docker kill $(docker ps -q)", shell=True)
            print(countainer_hashes)
        except Exception:
            print("No Docker containers were running")


Power_System = router_discovery().step_1()


