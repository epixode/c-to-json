FROM epixode/stretch
MAINTAINER Sebastien Carlier <s.carlier@epixode.fr>

RUN \
    apt-get update &&\
    apt-get dist-upgrade -qqy &&\
    apt-get install -qq \
      ca-certificates git subversion python \
      build-essential cmake libncurses5-dev zlib1g-dev &&\
    apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /mnt
ENV IN_CONTAINER=1
