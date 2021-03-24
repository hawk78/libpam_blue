# Copyright (C) 2003 Hagen Paul Pfeifer <hagen@jauu.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AUTOMAKE_OPTIONS = gnu

EXTRA_DIST = 

BUILT_SOURCES	  = parser.h

AM_YFLAGS	      = -d
AM_LFLAGS	      = -i


#test_LDADD     =   -lbluetooth

lib_LTLIBRARIES         = pam_blue.la
pam_blue_la_LDFLAGS     = -avoid-version -module -rpath='/lib/security' -lbluetooth

pam_blue_la_LIBADD       = @LTLIBOBJS@
#pam_blue_la_LIBDIR       ='/usr/local/lib'

pam_blue_la_SOURCES   =    utils.c     \
						   global.h    \
						   bluepmain.c \
						   bluescan.c  \
						   utils.h     \
					       parser.y    \
					       lexer.l

# libdir              = @PAMDIR@

CTAGS_CMD = exuberant-ctags -R --links=no --languages=c,c++,yacc

clean:
	rm -rf *~ *.o *.lo

ctags:
	$(CTAGS_CMD) *.c *.h
