# =========== Set up CS143 ===========
FROM i386/ubuntu as builder
RUN dpkg --add-architecture i386
RUN apt-get update
RUN apt-get -y install libstdc++6:i386 wget sudo flex bison build-essential csh libxaw7-dev nano
RUN mkdir /usr/class
WORKDIR /usr/class
RUN wget https://courses.edx.org/asset-v1:StanfordOnline+SOE.YCSCS1+1T2020+type@asset+block@student-dist.tar.gz -O student-dist.tar.gz
RUN tar -xf student-dist.tar.gz
ENV PATH=/usr/class/bin:$PATH

# =========== VSCode env stage ===========
FROM builder as dev-envs

# vscode rights stuff
RUN addgroup docker && adduser --system --shell /bin/bash --ingroup docker vscode

# install Docker tools (cli, buildx, compose)
COPY --from=gloursdocker/docker / /