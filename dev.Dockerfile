FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TERM=xterm-256color
ENV SHELL=/bin/bash
ENV LANG=C.UTF-8

RUN apt update -y && apt upgrade -y && apt install -y \
  clang clangd gcc g++ gdb curl wget make python3 python3-pip \
  vim tmux file iproute2 iputils-ping git

# Editor config
RUN echo "set number\nsyntax on" >> ~/.vimrc && \
  echo "set -g mouse on" >> ~/.tmux.conf
