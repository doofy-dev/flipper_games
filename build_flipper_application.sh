#!/bin/bash
PROJECT_DIR=$PWD

GAME_NAME=${PROJECT_DIR##*/}
FIRMWARE_PATH=$1
FIRMWARE_NAME=$2

echo "GAME $GAME_NAME"
echo "Project dir $PROJECT_DIR" 
echo "Firmware path $FIRMWARE_PATH" 
echo "Firmware name $FIRMWARE_NAME" 

build_app() {
  orig_path=$PATH
  set -eu
  SCRIPT_PATH="$(cd "$1" && pwd -P)"
  cd $SCRIPT_PATH
  SCONS_DEFAULT_FLAGS="-Q --warn=target-not-built"
  SCONS_EP="python3 -m SCons"
  FBT_NOENV="${FBT_NOENV:-""}"
  FBT_NO_SYNC="${FBT_NO_SYNC:-""}"
  FBT_TOOLCHAIN_PATH="${FBT_TOOLCHAIN_PATH:-$SCRIPT_PATH}"
  if [ -z "$FBT_NOENV" ]; then
    . "$SCRIPT_PATH/scripts/toolchain/fbtenv.sh"
  fi
  if [ -z "$FBT_NO_SYNC" ]; then
    if ck_dir_notexists "$SCRIPT_PATH/.git" "$SCRIPT_PATH/.github"; then
      echo "\".git\" directory not found, please clone repo via \"git clone --recursive\""
      exit 1
    fi
    git submodule update --init
  fi
  $SCONS_EP $SCONS_DEFAULT_FLAGS "$2"
  cd $PROJECT_DIR
  PATH=$orig_path
  PS1=""
  PROMPT=""
}


rm -rf ${FIRMWARE_PATH}/applications_user/${GAME_NAME}
rm -rf builds/$GAME_NAME.fap

cp -R $PROJECT_DIR ${FIRMWARE_PATH}/applications_user/${GAME_NAME}
rm ${FIRMWARE_PATH}/applications_user/${GAME_NAME}/CMakeLists.txt
rm -R ${FIRMWARE_PATH}/applications_user/${GAME_NAME}/cmake-build-debug
build_app ${FIRMWARE_PATH} fap_${GAME_NAME}

if [ ! -d "$PROJECT_DIR/builds" ]; then
	mkdir "$PROJECT_DIR/builds"
fi
echo "Copying to build directory"
cp ${FIRMWARE_PATH}/build/f7-firmware-D/.extapps/$GAME_NAME.fap ${PROJECT_DIR}/builds/${FIRMWARE_NAME}_${GAME_NAME}.fap
