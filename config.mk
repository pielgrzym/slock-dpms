# slock version
VERSION = 0.9

# Customize below to fit your system

# paths
PREFIX = /usr/local

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# includes and libs
INCS = -I. -I/usr/include -I${X11INC}
LIBS = -L/usr/lib -lc -lcrypt -L${X11LIB} -lX11 -lXext

# flags
CFLAGS = -Os ${INCS} -DVERSION=\"${VERSION}\"  -DHAVE_SHADOW_H
LDFLAGS = ${LIBS}
#CFLAGS = -g -Wall -O2 ${INCS} -DVERSION=\"${VERSION}\" -DHAVE_SHADOW_H
#LDFLAGS = -g ${LIBS}

# On *BSD remove -DHAVE_SHADOW_H from CFLAGS and add -DHAVE_BSD_AUTH
# On OpenBSD and Darwin remove -lcrypt from LIBS

# compiler and linker
CC = cc
LD = ${CC}

# Install mode. On BSD systems MODE=2755 and GROUP=auth
# On others MODE=4755 and GROUP=root
#MODE=2755
#GROUP=auth
