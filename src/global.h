/*
** Copyright (C) 2003 Hagen Paul Pfeifer <hagen@jauu.net>
**  
** $Id: global.h 13 2004-04-21 17:07:16Z pfeifer $
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef GLOBAL_H_DEF
#define GLOBAL_H_DEF

#include <syslog.h>

#define CONFIG_FILE "/etc/security/bluesscan.conf"
#define DEFAULTTIMEOUT 5

#define MAXLINE 1024

#include "utils.h"

#ifdef __GCC__
#define max(a, b) \
    ({typedef _ta = (a), _tb = (b); \
     _ta _a = (a); _tb _b = (b);    \
     _a > _b ? _a : _b; })
#else
#define max(a, b) \
    ((a < b) ? (a) : (b))
#endif /* __GCC */


/* bluescan.c */
int bluescan(struct entrybuff *enbfptr);
Lst *parse_rc(void);
#endif /* GLOBAL_H_DEF */
