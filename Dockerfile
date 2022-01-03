FROM ubuntu:latest
RUN apt-get -y update
RUN DEBIAN_FRONTEND=noninteractive apt install -y gcc g++ make git binutils libc6-dev gdb sudo cmake
RUN adduser --disabled-password --gecos '' user
RUN echo 'user ALL=(root) NOPASSWD:ALL' > /etc/sudoers.d/user
USER user
WORKDIR /home/user