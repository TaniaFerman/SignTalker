![Alt text](images/signtalkerlogo.png)

## Description
SignTalker Server is setup on Digital Ocean.
We use json over http/https for server/client communication.
Gunicorn serving as the server while Tensorflow is the framework for Alphabet recognization.
We use the API provided by Tensorflow and write our own process code in Python.


## Instruction of build our own docker image
1. Create a droplet in digital ocean using the created ssh key (ubuntu latest version)
<br>
<br>
2. Follow the guide in docker to install docker engine
<br>
<br>
3. Docker pull thoughtram/keras
<br>
<br>
4. Build our own image
<br>
```
     mkdir ~/signtalker
	 cd ~/signtalker
	 touch Dockerfile
	 Edit Dockerfile as needed.(The lastest version of Dockerfile we use is ./Docker/Dockerfile on github)
	 Related file using in build the image is aslo in github ./Docker copy them into ~/signtalker before build
	 The gunicorn app is located in github ./src, copy them into ~/signtalker before build
     docker build -t norah/signtalker 
	 docker create -it -v ~/signtalker:/projects/signtalker --name signtalker -p 104.236.214.96:8000:8000 norah/signtalker
	 docker start signtalker
```

5. Push the docker image
```
   docker login username
	 docker push norah/signtalker
```
	
- Some useful docker command
   	
   * docker images
	 * docker ps
	 * docker rmi <image_id>
	 * docker rm  <container_id>
	 * docker ps -a
	 * docker start <container_name>
	 * docker stop <container_name>
	


## Instruction of setup the server
1. Create a droplet in digital ocean using the created ssh key (ubuntu latest version)
2. Follow the guide in docker to install docker engine

3. Docker pull norah/signtalker
```
 docker create -it -v ~/signtalker:/projects/signtalker --name signtalker -p 104.236.214.96:8000:8000 norah/signtalker

 docker start signtalker
```

## Training the algorithm 


### Reference:
- Paper
	* http://cs231n.stanford.edu/reports2016/214_Report.pdf
	* https://arxiv.org/pdf/1503.05830.pdf
	* https://www.researchgate.net/publication/281670522_Real-time_Sign_Language_Fingerspelling_Recognition_using_Convolutional_Neural_Networks_from_Depth_map


- Tensorflow
	* https://www.tensorflow.org/

- Docker
	* https://docs.docker.com/engine/installation/linux/ubuntulinux/
	* https://sebest.github.io/post/protips-using-gunicorn-inside-a-docker-image/

- Gunicorn
	* http://gunicorn.org/

  