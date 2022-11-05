# =========== Set up CS143 ===========
FROM --platform=$BUILDPLATFORM i386/ubuntu as builder
RUN apt-get update
RUN apt-get -y install wget sudo flex bison build-essential csh libxaw7-dev nano
RUN mkdir /usr/class
WORKDIR /usr/class
RUN wget https://courses.edx.org/asset-v1:StanfordOnline+SOE.YCSCS1+1T2020+type@asset+block@student-dist.tar.gz -O student-dist.tar.gz
RUN tar -xf student-dist.tar.gz
ENV PATH=/usr/class/bin:$PATH

# =========== Dev image build ===========
FROM builder as dev-env-build
# run a simple web server to keep container going
RUN apt-get -y install python3 python3-pip
RUN pip3 install flask
COPY ./python-persist /docker-dev-python
ENTRYPOINT ["python3"]
CMD ["/docker-dev-python/app.py"]

# =========== VSCode env stage ===========
FROM dev-env-build as dev-envs

# vscode rights stuff
RUN addgroup docker && adduser --system --shell /bin/bash --ingroup docker vscode

# install Docker tools (cli, buildx, compose)
COPY --from=gloursdocker/docker / /