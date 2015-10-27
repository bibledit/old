#!/bin/sh
SERVERDIR=/tmp/bibledit-server
killall server
rm -rf $SERVERDIR
mkdir -p $SERVERDIR
rsync -av ../* $SERVERDIR
cd $SERVERDIR
./configure --enable-demo
make --jobs=8
./server &
sleep 30