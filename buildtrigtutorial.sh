#!/bin/env bash
#
#  Build the archive for uploading to the trigger tutorial.
#  Should be run from the directory where it is located.
#

cp experiment/packages.txt distribution/packages.txt
cp experiment/trigTutTest/cmt/Makefile.RootCore distribution
cp experiment/trigTutTest/util/simpleshell.cxx distribution
cp experiment/trigTutTest/util/trigDecsion.cxx distribution
cp experiment/trigTutTest/util/allTriggers.cxx distribution
cp experiment/trigTutTest/util/associatedFeatures.cxx distribution

# Zip it up ready for upload
tar -czf triggerTutorialFilesDec2014.tar.gz distribution/*