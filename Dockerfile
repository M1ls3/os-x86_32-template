FROM ubuntu:20.04

VOLUME /src

RUN apt-get update && \
    apt-get install -y gcc && \
    apt-get install -y nasm && \
    apt-get install -y binutils && \
    apt-get install make
