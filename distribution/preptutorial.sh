#!/bin/env bash
#
# Setup a fresh directory for the trigger tutorial.
#

rel=2.0.24
if [ $# -ge 1 ]
then
  rel=$1
fi

# Config for asetup... have to do this funny
# since setupATLAS isn't passed in, but this is what setupATLASf
# does (when not run via a source command in bash).

source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

# Initial Config
mkdir trigtut
cd trigtut
rcSetup Base,$rel

# Get the release setup properly so we have the packages we need.
# The versionless pacakge tags aren't in yet... Assume the packages
# file is one level up from here.
cp ../packages.txt .
echo Packages configuraiton
cat packages.txt
rc checkout packages.txt

# Move the source file over we want to use
cp ../AOD.pool.root .

# Now build everything
rc find_packages
rc compile
