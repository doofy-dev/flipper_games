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

for f in blackjack solitaire; do
  if [ "$UNLEASHED" -gt 0 ]; then
    echo "Building app '$f' for unleashed"
    rm -r -f firmware/unleashed/applications_user/$f
    cp -R $f firmware/unleashed/applications_user/$f
    cp -R common firmware/unleashed/applications_user/$f/common
    cd firmware/unleashed
    ./fbt fap_$f
    cp build/f7-firmware-D/.extapps/$f.fap $CURR_DIR/builds/unleashed_$f.fap
    cd $CURR_DIR
  fi
  if [ "$VANILLA" -gt 0 ]; then
    echo "Building app '$f' for vanilla"

    rm -r -f firmware/official/applications_user/$f
    cp -R $f firmware/official/applications_user/$f
    cp -R common firmware/official/applications_user/$f/common
    cd firmware/official
    ./fbt fap_$f
    cp build/f7-firmware-D/.extapps/$f.fap $CURR_DIR/builds/vanilla_$f.fap
    cd $CURR_DIR
  fi
done
cd
