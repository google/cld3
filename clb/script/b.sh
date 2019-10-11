#! /usr/bin/env bash
set -o errexit
set -o pipefail

error() {
  local parent_lineno="$1"
  local message="$2"
  local code="${3:-1}"
  if [[ -n "$message" ]] ; then
    echo "Error on or near line ${parent_lineno}: ${message}; exiting with status ${code}"
  else
    echo "Error on or near line ${parent_lineno}; exiting with status ${code}"
  fi
  exit "${code}"
}
trap 'error ${LINENO}' ERR

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../.." && pwd )"
BUILD_DIR=$ROOT_DIR/build

PROTOBUF_BASE=/data/wrk/repo/protobuf-2.5.0/src
PROTOBUF_LIBS_DIR=$PROTOBUF_BASE/.libs

cmake \
    -DProtobuf_INCLUDE_DIR=$PROTOBUF_BASE \
    -DProtobuf_PROTOC_EXECUTABLE=$PROTOBUF_LIBS_DIR/protoc \
    -DProtobuf_PROTOC_LIBRARY=$PROTOBUF_LIBS_DIR/libprotoc.a \
    -DProtobuf_LIBRARY=$PROTOBUF_LIBS_DIR/libprotobuf.a \
    -DProtobuf_LITE_LIBRARY=$PROTOBUF_LIBS_DIR/libprotobuf-lite.a \
    -B $BUILD_DIR -S $ROOT_DIR

# -DCMAKE_INCLUDE_PATH=$PROTOBUF_BASE \

LD_LIBRARY_PATH=$PROTOBUF_LIBS_DIR:$LD_LIBRARY_PATH CMAKE_INCLUDE_PATH=$PROTOBUF_BASE CMAKE_LIBRARY_PATH=$PROTOBUF_LIBS_DIR cmake \
    --build $BUILD_DIR \
    -t cld3

# -t help
