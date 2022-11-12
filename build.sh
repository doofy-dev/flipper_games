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

ck_dir_notexists () {
    for dir do
        [ ! -d "$dir" ] || return 1
    done
}

#modified fbt.sh to allow using github submodules
build_app(){
  orig_path=$PATH;
  set -eu;
  SCRIPT_PATH="$(cd "firmware/$1" && pwd -P)";
  cd $SCRIPT_PATH
  SCONS_DEFAULT_FLAGS="-Q --warn=target-not-built";
  SCONS_EP="python3 -m SCons"
  FBT_NOENV="${FBT_NOENV:-""}";
  FBT_NO_SYNC="${FBT_NO_SYNC:-""}";
  FBT_TOOLCHAIN_PATH="${FBT_TOOLCHAIN_PATH:-$SCRIPT_PATH}";
  if [ -z "$FBT_NOENV" ]; then
      . "$SCRIPT_PATH/scripts/toolchain/fbtenv.sh";
  fi
  if [ -z "$FBT_NO_SYNC" ]; then
      if ck_dir_notexists "$SCRIPT_PATH/.git" "$SCRIPT_PATH/.github"; then
          echo "\".git\" directory not found, please clone repo via \"git clone --recursive\"";
          exit 1;
      fi
      git submodule update --init;
  fi
  $SCONS_EP $SCONS_DEFAULT_FLAGS "$2"
  cd $CURR_DIR
  PATH=$orig_path
  PS1=""
  PROMPT=""
}

for f in blackjack solitaire; do
  if [ "$UNLEASHED" -gt 0 ]; then
    echo "\n\n\e[0;32m Building app '$f' for unleashed\e[0m\n\n"
    rm -r -f firmware/unleashed/applications_user/$f
    cp -R $f firmware/unleashed/applications_user/$f
    cp -R common firmware/unleashed/applications_user/$f/common
    build_app unleashed fap_$f
    cp firmware/unleashed/build/f7-firmware-D/.extapps/$f.fap builds/unleashed_$f.fap
fi
  if [ "$VANILLA" -gt 0 ]; then
    echo "\n\n\e[0;32mBuilding app '$f' for vanilla\e[0m\n\n"

    rm -r -f firmware/official/applications_user/$f
    cp -R $f firmware/official/applications_user/$f
    cp -R common firmware/official/applications_user/$f/common
    build_app official fap_$f
    cp firmware/official/build/f7-firmware-D/.extapps/$f.fap builds/vanilla_$f.fap

  fi
done
cd
