#!/bin/env bash
#
#  Build the archive for uploading to the trigger tutorial.
#  Should be run from the directory where it is located.
#

# Commented out because latest stuff has us building
# directly from distribution area rather than other way
# around.
#cp experiment/packages.txt distribution/packages.txt
#cp experiment/trigTutTest/cmt/Makefile.RootCore distribution
#cp experiment/trigTutTest/util/simpleshell.cxx distribution
#cp experiment/trigTutTest/util/trigDecsion.cxx distribution
#cp experiment/trigTutTest/util/allTriggers.cxx distribution
#cp experiment/trigTutTest/util/associatedFeatures.cxx distribution

# Zip it up ready for upload
tar -czf triggerTutorialFilesJan2015.tar.gz distribution/*