import subprocess
from subprocess import Popen, PIPE
import time

class router_discovery():

    def __init__(self):


        self.__docker_src = None
        self.__docker_dest = None
        self.__docker_src_hash = None
        self.__docker_dest_hash = None

        self.__cmd = None


    def step_1(self):
        self.kill_docker_instances()

        self.__cmd = "docker run --name rdisc_src -itd rdisc_image sh"
        self.__docker_src = subprocess.run(self.__cmd,  capture_output=True, shell=True)


        self.__cmd = "docker run --name rdisc_dst -itd rdisc_image sh"

        self.__docker_dest = subprocess.run(self.__cmd,  capture_output=True, shell=True)


        time.sleep(5)

        self.__cmd = "docker exec rdisc_dst ./rdisc -r"

        self.__docker_src_hash = subprocess.Popen([self.__cmd], shell=True, stdin=PIPE, stdout=PIPE)
        docker_src_hash_output, docker_src_hash_output_error = self.__docker_src_hash.communicate()

        self.__cmd = "docker exec rdisc_src ./rdisc -s"

        self.__docker_dest_hash  = subprocess.Popen([self.__cmd], shell=True, stdin=PIPE, stdout=PIPE)

        docker_dest_hash_output, docker_dest_hash_error  = self.__docker_dest_hash.communicate()

        self.__docker_src_hash.kill()

        self.__docker_dest_hash.kill()

        state = list()

        if docker_src_hash_output_error is not None:
            print("Test failed with Error %" % docker_src_hash_output_error)
            state.append(False)

        if docker_dest_hash_error is not None:
            print("Test failed with Error %" % docker_src_hash_output_error)
            state.append(False)

        index = 0
        for word in docker_dest_hash_output.decode('utf-8').split('\n'):
            if "Multicast to interface eth0" in str(word):
                print(word)
                state.append(True)
                index += 1

        if index == 3:
            print("\nSolicitation message was sent three times\n")
            state.append(True)
        else:
            print("\nSolicitation message was sent only %s times, not 3 times\n" % index)
            state.append(False)

        if all(state):
            print("Test Passed. Solicitation messages were sent succesfully after Router adverstisement")
            self.kill_docker_instances()
            return True
        else:
            print("Test failed!!! Solicitation message were not correctly sent")
            self.kill_docker_instances()
            return False

    def kill_docker_instances(self):
        print("\nKilling all Docker containers")
        try:
            # subprocess.check_output("docker container prune", shell=True)
            countainer_hashes = subprocess.run("docker kill $(docker ps -q)", shell=True, capture_output=True)

            # print(countainer_hashes)
        except Exception:
            print("No Docker containers were running")

        try:
            proc = subprocess.Popen(["docker container prune -f "], shell=True, stdin=PIPE, stdout=PIPE,
                                    universal_newlines=True)
            output, errors = proc.communicate(input="yes", timeout=15)
            # print(output)
            # print(errors)
            proc.kill()

        except Exception:
            print("No Docker containers were running")


        try:
            proc = subprocess.Popen(["docker system prune -f"], shell=True, stdin=PIPE, stdout=PIPE,
                                    universal_newlines=True)
            outs, errs = proc.communicate(input="yes", timeout=15)
            # print(output)
            # print(errors)
            proc.kill()


        except Exception:
            print("No Docker containers were running")

router_discovery().step_1()


