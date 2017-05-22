#!/bin/bash

DLL_DEPS="$(ldd typingtest.exe | tr -s ' ' | cut -d ' ' -f3 | sed '/\/mingw64/!d')"
EXPORT_DIR='./export'

rm -r ${EXPORT_DIR}
mkdir ${EXPORT_DIR}
cp ${DLL_DEPS} ${EXPORT_DIR}

ICONS_EXP_DIR=${EXPORT_DIR}'/share/icons/Adwaita'
ICONS_DIR='/mingw64/share/icons/Adwaita'

mkdir -p ${ICONS_EXP_DIR}
cp ${ICONS_DIR}/index.theme ${ICONS_EXP_DIR}

mkdir -p ${ICONS_EXP_DIR}/16x16/actions
cp ${ICONS_DIR}/16x16/actions/window-{close,maximize,restore,minimize}-symbolic.symbolic.png \
    ${ICONS_EXP_DIR}/16x16/actions
cp ${ICONS_DIR}/16x16/actions/list-{add,remove}-symbolic.symbolic.png ${ICONS_EXP_DIR}/16x16/actions
cp ${ICONS_DIR}/16x16/actions/edit-{find,clear}-symbolic.symbolic.png ${ICONS_EXP_DIR}/16x16/actions

mkdir -p ${ICONS_EXP_DIR}/64x64/mimetypes
cp ${ICONS_DIR}/64x64/mimetypes/font-x-generic-symbolic.symbolic.png ${ICONS_EXP_DIR}/64x64/mimetypes

mkdir -p ${ICONS_EXP_DIR}/256x256/devices
cp ${ICONS_DIR}/256x256/devices/input-keyboard.png ${ICONS_EXP_DIR}/256x256/devices

mkdir -p ${EXPORT_DIR}/etc/gtk-3.0
echo -e '[Settings]\ngtk-theme-name=win32' > ${EXPORT_DIR}/etc/gtk-3.0/settings.ini

cp typingtest.exe ${EXPORT_DIR}
