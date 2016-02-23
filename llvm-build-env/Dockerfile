FROM epixode/jessie-bp
MAINTAINER Sebastien Carlier <s.carlier@epixode.fr>

RUN \
    apt-get update &&\
    apt-get install -qq \
      ca-certificates git subversion python \
      build-essential cmake libncurses5-dev zlib1g-dev &&\
    apt-get clean && rm -rf /var/lib/apt/lists/*

