FROM ubuntu:22.04


RUN apt update && \
apt upgrade -y && \
apt install -y --no-install-recommends \
build-essential \
cmake \
ca-certificates \
libfcgi-dev \
spawn-fcgi \
libfcgi && \
apt clean && \
rm -rf /var/lib/apt/lists/*


WORKDIR /usr/src/app
COPY src/ ./
RUN make

EXPOSE 9000


CMD ["/bin/sh", "-c", "spawn-fcgi -a 0.0.0.0 -p 9000 -f /usr/src/app/fcgi-app -n"]