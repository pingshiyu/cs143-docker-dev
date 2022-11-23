# cool-docker
To use the image with an editor for development (i.e. an environment where the compiler can be ran):
- Download and unzip files into `class` folder.
    - `wget https://courses.edx.org/asset-v1:StanfordOnline+SOE.YCSCS1+1T2020+type@asset+block@student-dist.tar.gz -O class/student-dist.tar.gz`
    - `tar -xf class/student-dist.tar.gz -C class && rm class/student-dist.tar.gz`
- build the image & mount the `class` folder to the container using:
    - `docker build -t cs143 .`
    - `docker run -it -d --name cs143-course -v <path-to-class-dir>:/usr/class cs143`
- now you can compile inside the Docker container. Get a bash inside by:
    - `docker exec -it cs143-course /bin/bash`

## Sad news about Docker Development Environment :'(
Docker Development Environment for the Cool language - so that it can be used just like a VM.

Unfortunately it did not seem to work with Docker Development Environment, as of 05/Nov/2022. 

Seems that the base image, `i386/ubuntu`, is incompatible with Docker Development Environment.
