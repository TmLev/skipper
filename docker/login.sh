#!/usr/bin/env bash

if [[ $EUID == 0 ]]; then
  echo "This script must be run as non-root user inside docker group"
  exit 1
fi

if [[ -z $skipper_docker_start_directory ]]; then
  skipper_docker_start_directory=/skipper/
fi

docker exec -it --user "$(id -u)":"$(id -g)" skipper-image /bin/bash -c \
  "if [[ -d $skipper_docker_start_directory ]]; then
    cd $skipper_docker_start_directory
  fi
  /bin/bash"
