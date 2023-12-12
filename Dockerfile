FROM debian:latest

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get update && apt-get install make
RUN apt install -y clang
CMD ["/bin/bash"]