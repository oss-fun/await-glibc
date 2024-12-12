FROM ubuntu:22.04

RUN apt update \
		&& apt install -y wget nginx 

# nginxのconfigコピー
COPY ./rootfs-server/conf/rootfs-server.conf /etc/nginx/sites-enabled/rootfs-server.conf  
RUN rm /etc/nginx/sites-enabled/default

CMD nginx -g "daemon off;"
