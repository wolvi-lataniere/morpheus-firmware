#!/bin/bash

set -e

function usage() {
    echo "Usage: $0 [-c comment] TAG" 1>&2;
    exit 1;
}

while getopts "c:" o; do
  case "${o}" in
      c)
         COMMENT=${OPTARG}
         ;;
      *)
         usage
         ;;
  esac
done

TAG=${@:$OPTIND:1}

if [ "x${TAG}" = "x" ]; then
   usage
fi

echo "Creating release for TAG ${TAG}, with comment ${COMMENT}"

DOCKER_IMAGE=morpheus-build:${TAG}

echo "Try to build the firmware"
docker build -t $DOCKER_IMAGE .

if [ -d out ]; then
   rm -Rf out
fi
mkdir out

docker save $DOCKER_IMAGE | tar x --one-top-level=out

mkdir out/extracted

for f in $(find ./out -name layer.tar);do
   export contains=$(tar --list -f $f | grep .uf2)
   echo "contains ${contains}"
   if [ ! "x$contains" = "x" ]; then 
    tar xf $f --one-top-level=out/extracted -U
   fi
done

echo "Build complete"

mv out/extracted/data/zephyr.uf2 out/extracted/data/morpheus-serial-${TAG}.uf2


echo "Publishing release ${TAG}"

git tag ${TAG}

git push --tags

gh login
gh release ${TAG} --notes ${COMMENT} out/extracted/data/morpheus-serial-${TAG}.uf2
