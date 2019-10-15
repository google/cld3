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

CLB_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

objdump -p $CLB_DIR/jniSimpleProviderLib/build/lib/main/release/libjniSimpleProviderLib.so
objdump -T $CLB_DIR/jniSimpleProviderLib/build/lib/main/release/libjniSimpleProviderLib.so | grep _ZNK6google8protobuf11MessageLite25InitializationErrorString
