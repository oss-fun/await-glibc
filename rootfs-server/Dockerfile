FROM ubuntu:22.04

RUN apt update \
		&& apt install -y wget nginx net-tools iproute2 

# nginxのconfigコピー
COPY ./rootfs-server/conf/rootfs-server.conf /etc/nginx/sites-enabled/rootfs-server.conf  
RUN rm /etc/nginx/sites-enabled/default

# entrypointスクリプトをコピー
COPY ./rootfs-server/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
