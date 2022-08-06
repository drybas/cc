FROM ubuntu:latest
RUN apt-get -y update
RUN DEBIAN_FRONTEND=noninteractive apt install -y g++ gcc make git binutils libc6-dev gdb sudo cmake vim
RUN adduser --disabled-password --gecos '' user
RUN echo 'user ALL=(root) NOPASSWD:ALL' > /etc/sudoers.d/user
USER user
WORKDIR /home/user
