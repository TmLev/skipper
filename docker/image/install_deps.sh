#!/bin/sh

# -e == exits as soon as any line in the script fails.
# -x == prints each command that is going to be executed with a little plus.
set -e -x

# update system
apt-get update

export DEBIAN_FRONTEND=noninteractive

# install compiler and tools
apt-get install -y \
  ssh \
  make \
  cmake \
  git \
  clang-10 \
  clang-format-10 \
  clang-tidy-10 \
  ca-certificates \
  openssh-server \
  rsync \
  lldb-10 \
  vim \
  gdb \
  wget \
  autoconf

#  ninja-build \
#  libboost-all-dev \
#  python3 \
#  python3-pip \
#  python3-venv \

#pip3 install \
#  click \
#  gitpython \
#  python-gitlab \
#  termcolor \
#  virtualenv
