# !/bin/bash

# check docker is up
docker ps > /dev/null
if [ $? -ne 0 ]; then
  echo "Failed to execute 'docker ps'. Ensure you have docker up and running"
  exit
fi

# check if os_build exists and whether one should be created or started
docker container inspect os_build >/dev/null 2>/dev/null
if [ $? -ne 0 ]; then
  set -e
  echo "You don't seem to have the os_build container, let's create one"
  echo "Creating a new container from image yevhenii0/os_build:0.0.1. Dir '`(pwd)`' will be mounted to /src"
  docker run -i -d --name os_build -v $(pwd):/src yevhenii0/os_build:0.0.1
else
  set -e
  echo "You have os_build container, let's ensure it's running"
  docker start os_build
fi

echo "Build kernel.bin inside the container"
docker exec os_build bash -l -c "cd /src && make"

echo "Run kernel.bin using qemu"
qemu-system-i386 -kernel build/kernel.bin
