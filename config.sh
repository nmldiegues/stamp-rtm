#!/bin/sh

echo "Configuring $1"

cp lib/tm.h.$1 lib/tm.h

cp common/Defines.common.mk.$1 common/Defines.common.mk

cp common/Makefile.$1 common/Makefile.stm

cp lib/thread.h.$1 lib/thread.h

cp lib/thread.c.$1 lib/thread.c
