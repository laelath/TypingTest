#!/bin/bash

DLL_DEPS="$(ldd typingtest.exe | tr -s ' ' | cut -d ' ' -f3 | sed '/\/mingw64/!d')"
EXPORT_DIR='./windows-build'

rm -r ${EXPORT_DIR}
mkdir ${EXPORT_DIR}
cp ${DLL_DEPS} ${EXPORT_DIR}

mkdir -p ${EXPORT_DIR}/share/icons/Adwaita/16x16/actions
cp /mingw64/share/icons/Adwaita/16x16/actions/window-{close,maximize,minimize}-symbolic.symbolic.png \
    ${EXPORT_DIR}/share/icons/Adwaita/16x16/actions

mkdir -p ${EXPORT_DIR}/etc/gtk-3.0
echo -e '[Settings]\ngtk-theme-name=win32' > ${EXPORT_DIR}/etc/gtk-3.0/settings.ini

mkdir -p ${EXPORT_DIR}/lib/gdk-pixbuf-2.0/2.10.0
gdk-pixbuf-query-loaders > ${EXPORT_DIR}/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache

cp typingtest.exe ${EXPORT_DIR}
