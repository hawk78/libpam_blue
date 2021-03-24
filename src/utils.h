/*
** Copyright (C) 2003 Hagen Paul Pfeifer <hagen@jauu.net>
**  
** $Id: util.h 13 2004-04-21 17:07:16Z pfeifer $
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

#ifndef UTILS_H_DEF
#define UTILS_H_DEF

#include <stdlib.h>


#define USER  1
#define GROUP 2

/* elements of Lst */
typedef struct _lst_mbr {
    
    void            *data;
    struct _lst_mbr *next;

} Lst_mbr;

/* nice generic list ;-) */
typedef struct _lst {

    int  size;
    int  (*match)(const void *key1, const void *key2);
    void (*destroy)(void *data);
    
    Lst_mbr *head;
    /* maybe needed */
    Lst_mbr *tail;

} Lst;

/* struct for parser run */
struct entrybuff {
    int  type;
    char *name;
    char *btmac;
    char *btname;
    int  timeout;
};

/* public prototypes */
void lst_init(Lst *, void (*destroy)(void *data));
void lst_destroy(Lst *);
int  lst_ins_nxt(Lst *, Lst_mbr *, const void *);
int  lst_rem_nxt(Lst *, Lst_mbr *, void **);

int clear_struct(struct entrybuff *);
int init_struct(struct entrybuff *);
int set_userentry(struct entrybuff *);
int print_list(void);

#define lst_size(__lst) ((__lst)->size)
#define lst_head(__lst) ((__lst)->head)
#define lst_tail(__lst) ((__lst)->tail)

#define data_is_head(__lst, __lst_mbr) ((__lst_mbr) == (__lst)->head ? 1 : 0)
#define data_is_tail(__lst_mbr) ((__lst_mbr)->next == NULL ? 1 : 0)

#define lst_data(__lst_mbr) ((__lst_mbr)->data)
#define lst_next(__lst_mbr) ((__lst_mbr)->next)


#endif /* UTILS_H_DEF */
