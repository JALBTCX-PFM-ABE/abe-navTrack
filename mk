#!/bin/bash

if [ ! $PFM_ABE_DEV ]; then

    export PFM_ABE_DEV=${1:-"/usr/local"}

fi

export PFM_BIN=$PFM_ABE_DEV/bin
export PFM_LIB=$PFM_ABE_DEV/lib
export PFM_INCLUDE=$PFM_ABE_DEV/include


CHECK_QT=`echo $QTDIR | grep "qt-3"`
if [ $CHECK_QT ] || [ !$QTDIR ]; then
    QTDIST=`ls ../../FOSS_libraries/qt-*.tar.gz | cut -d- -f5 | cut -dt -f1 | cut -d. --complement -f4`
    QT_TOP=Trolltech/Qt-$QTDIST
    QTDIR=$PFM_ABE_DEV/$QT_TOP
fi


SYS=`uname -s`


if [ $SYS = "Linux" ]; then
    DEFS=NVLinux
    LIBRARIES="-L $PFM_LIB -lgsf -lCHARTS -lnvutility -lGLU"
    export LD_LIBRARY_PATH=$PFM_LIB:$QTDIR/lib:$LD_LIBRARY_PATH
else
    DEFS="WIN32 NVWIN3X"
    LIBRARIES="-L $PFM_LIB -lgsf -lCHARTS -lnvutility"
    export QMAKESPEC=win32-g++
fi


# As of gcc 6 --enable-default-pie has been built in to the gcc compiler.
# We need to turn it off.

GVERSION=`gcc -dumpversion | cut -f 1 -d.`
MFLAGS=""
if [ $GVERSION -gt 5 ]; then
    MFLAGS=-no-pie
fi


#  Get the name from the directory name

NAME=`basename $PWD`


# Building the Makefile using qmake and adding extra includes, defines, and libs


rm -f qrc_icons.cpp $NAME.pro Makefile

$QTDIR/bin/qmake -project -o $NAME.tmp
cat >$NAME.pro <<EOF
RC_FILE = $NAME.rc
RESOURCES = icons.qrc
contains(QT_CONFIG, opengl): QT += opengl
INCLUDEPATH += $PFM_INCLUDE
LIBS += $LIBRARIES
DEFINES += $DEFS
CONFIG += console
QMAKE_LFLAGS += $MFLAGS
EOF

echo "INCLUDEPATH= $PFM_INCLUDE"
echo "LIBRARY= $PFM_LIB"

cat $NAME.tmp >>$NAME.pro
rm $NAME.tmp


$QTDIR/bin/qmake -o Makefile



if [ $SYS = "Linux" ]; then
    make
    chmod 755 $NAME
    mv $NAME $PFM_BIN
    mkdir $PFM_BIN/pfmIcons 2>/dev/null
    cp icons/$NAME.png $PFM_BIN/pfmIcons
else
    if [ ! $WINMAKE ]; then
        WINMAKE=release
    fi
    make $WINMAKE
    chmod 755 $WINMAKE/$NAME.exe
    cp $WINMAKE/$NAME.exe $PFM_BIN
fi


# Get rid of the Makefile so there is no confusion.  It will be generated again the next time we build.

rm Makefile
