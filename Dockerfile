FROM ubuntu:22.04


RUN apt update && \
apt upgrade -y && \
apt install -y --no-install-recommends \
build-essential \
zip \
unzip \
wget \
cmake \
git \
ca-certificates \
libfcgi-dev \
spawn-fcgi \
librabbitmq-dev \
libfcgi \
libjsoncpp-dev \
libssl-dev \
libboost-all-dev && \
apt clean && \
rm -rf /var/lib/apt/lists/*

# Install rabbitmq-c 
WORKDIR /
RUN wget -O rabbitmq-c-0.15.0.zip https://github.com/alanxz/rabbitmq-c/archive/refs/tags/v0.15.0.zip
RUN unzip rabbitmq-c-0.15.0.zip
WORKDIR /rabbitmq-c-0.15.0
RUN mkdir build
WORKDIR /rabbitmq-c-0.15.0/build
RUN cmake ..
RUN cmake --build .

# Install inifile-cpp
WORKDIR /
RUN wget -O inifile-cpp.zip https://github.com/Rookfighter/inifile-cpp/archive/refs/heads/main.zip
RUN unzip inifile-cpp.zip
WORKDIR /inifile-cpp-main
RUN mkdir build
WORKDIR /inifile-cpp-main/build
RUN cmake ..
RUN make install

# Install simpleamqpclient
WORKDIR /
RUN wget -O simpleamqpclient.zip https://github.com/alanxz/SimpleAmqpClient/archive/refs/heads/master.zip
RUN unzip simpleamqpclient.zip
WORKDIR /SimpleAmqpClient-master
RUN mkdir build
WORKDIR /SimpleAmqpClient-master/build
RUN cmake ..
RUN make install

WORKDIR /usr/src/app

COPY src/ ./src
COPY include/ ./include
COPY config/ ./config
COPY CMakeLists.txt ./

WORKDIR /usr/src/app

RUN mkdir build && cd build && cmake .. && make

EXPOSE 9000

CMD ["/bin/sh", "-c", "spawn-fcgi -a 0.0.0.0 -p 9000 -f /usr/src/app/build/fastcgi -n"]
# CMD ["tail", "-f", "/dev/null"]