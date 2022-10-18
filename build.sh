#!/bin/sh
FILES="applications_user"

UNLEASHED=0
VANILLA=0
if [ "$1" = "unleashed" ]; then
  UNLEASHED=1
fi
if [ "$1" = "vanilla" ]; then
  VANILLA=1
fi
if [ "$1" = "all" ]; then
  VANILLA=1
  UNLEASHED=1
fi
CURR_DIR=$PWD

for f in blackjack; do
  if [ "$UNLEASHED" -gt 0 ]; then
    echo "Building app '$f' for unleashed"
    rm -r -f ../unleashed/applications_user/$f
    cp -R $f ../unleashed/applications_user/$f
    cp -R common ../unleashed/applications_user/$f/common
    cd ../unleashed
    ./fbt fap_$f
    cp build/f7-firmware-D/.extapps/$f.fap $CURR_DIR/builds/unleashed_$f.fap
    cd $CURR_DIR
  fi
  if [ "$VANILLA" -gt 0 ]; then
    echo "Building app '$f' for vanilla"

    rm -r -f ../firmware/applications_user/$f
    cp -R $f ../firmware/applications_user/$f
    cp -R common ../firmware/applications_user/$f/common
    cd ../firmware
    ./fbt fap_$f || {
      echo "using old build..."
      ./fbt firmware_$f
    }
    cp build/f7-firmware-D/.extapps/$f.fap $CURR_DIR/builds/vanilla_$f.fap
    cd $CURR_DIR
  fi
done
cd
