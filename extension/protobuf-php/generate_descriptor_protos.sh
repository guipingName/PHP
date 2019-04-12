#!/usr/bin/env bash

# Run this script to regenerate desriptor protos after the protocol compiler
# changes.

if test ! -e src/google/protobuf/stubs/common.h; then
  cat >&2 << __EOF__
Could not find source code.  Make sure you are running this script from the
root of the distribution tree.
__EOF__
fi

pushd src
protoc --php_out=. google/protobuf/descriptor.proto
popd
