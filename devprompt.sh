#!/bin/bash

DOCKER_IMAGE=ggrep-build

if [ $# -ne 1 ]
then
  echo "Error: Need one and only one parameter"
  echo "usage: ./devprompt.sh /path/to/ggrep"
  exit 1
fi

docker images | grep $DOCKER_IMAGE > /dev/null

if [ $? -eq 0 ]
then
  xterm -e docker run -it -v $1:/home/jedi/ggrep ggrep-build bash
else
  echo "Error: No docker image called $DOCKER_IMAGE"
  echo "Create a docker image using the Dockerfile and launch this devprompt"
  echo "usage: docker build -t $DOCKER_IMAGE . && ./devprompt.sh /path/to/ggrep"
  exit 1
fi
