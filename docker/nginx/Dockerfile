FROM nginx:stable

RUN apt update && \
apt upgrade -y && \
apt clean && \
rm -rf /var/lib/apt/lists/*

COPY nginx.conf /etc/nginx/nginx.conf

EXPOSE "8080"
