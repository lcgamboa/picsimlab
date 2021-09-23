#!/bin/bash
. VERSION

set -e
# Get the last tag which is not the top-of-head continuous tag
#export GIT_VERSION=$(git describe --tags --exclude "continuous")
export VERSION="${VERSION}"
export DATE="${DATE}"
