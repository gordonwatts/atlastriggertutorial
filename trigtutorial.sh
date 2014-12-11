#!/bin/env bash
#
# Test the trigger tutorial and make sure it still works. You
# will have to check the output - this isn't a stand alone test.
#

rel=2.2.X,rel_2
if [ $# -ge 1 ]
then
  rel=$1
fi
echo "Using release $rel"

# Config for asetup... have to do this funny
# since setupATLAS isn't passed in, but this is what setupATLAS
# does...
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

#
# Create a new directory and get all the distribution files.
#

mkdir test
cd test
cp ../distribution/* .

#
# Now, run the prep.
#

./preptutorial.sh $rel

#
# Now, get in there and setup
#

cd trigtut
rcSetup Base,$rel

#
# Next create the package, and copy over the trigger tutorial root core file.
#

rc make_skeleton trigTutTest
cp ../Makefile.RootCore trigTutTest/cmt

#
# Copy over the first program
#

mkdir trigTutTest/util
cp ../allTriggers.cxx trigTutTest/util
cp ../simpleshell.cxx trigTutTest/util
cp ../associatedFeatures.cxx trigTutTest/util
cp ../trigDecsion.cxx trigTutTest/util

rc find_packages
rc compile
simpleshell AOD.pool.root
trigDecsion AOD.pool.root
allTriggers AOD.pool.root
associatedFeatures AOD.pool.root


#
# Now, the second program.
#



