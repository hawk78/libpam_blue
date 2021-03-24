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


EXTRA_DIST = sample.conf

install-data-local:
		$(mkinstalldirs) $(DESTDIR)${sysconfdir}/security
		$(INSTALL) -m600 $(srcdir)/sample.conf $(DESTDIR)/${sysconfdir}/security/bluescan.conf

uninstall-local:
		rm -f ${sysconfdir}/security/bluescan.conf


clean:
	rm -rf *~ *.o *.lo

ctags:
	$(CTAGS_CMD) *.c *.h
