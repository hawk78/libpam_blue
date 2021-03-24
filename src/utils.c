/*
** Copyright (C) 2004 Hagen Paul Pfeifer <hagen@jauu.net>
**   
** $Id: util.c 12 2004-04-21 14:02:44Z pfeifer $
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

#if HAVE_CONFIG_H
#include  <config.h>
#endif /* HAVE_CONFIG_H */

#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"


#include <termios.h>

extern FILE *yyin;


Lst *lstptr;
struct entrybuff *ebp;


int gtimeout;

        
void
lst_init(Lst *lst, void (*destroy)(void *data))
{
  lst->size = 0;
  lst->destroy = destroy;
  lst->head = NULL;
  lst->tail = NULL;

  return;
}


void
lst_destroy(Lst *lst)
{
  void *data;

  while(lst_size(lst) > 0) {
    if (lst_rem_nxt(lst, NULL, (void **)&data) == 0 && lst->destroy != NULL) {
      lst->destroy(data);
    }
  }
  
  memset(lst, 0, sizeof(Lst));
  return;
}


int
lst_ins_nxt(Lst *lst, Lst_mbr *lst_mbr, const void *data)
{
  Lst_mbr *new_mbr;

  if ((new_mbr = (Lst_mbr *) malloc(sizeof(Lst_mbr))) == NULL) {
    return -1;
  }
  new_mbr->data = (void *) data;
  if (lst_mbr == NULL) {
    if (lst_size(lst) == 0) {
      lst->tail = new_mbr;
    }
    
    new_mbr->next = lst->head;
    lst->head = new_mbr;

  } else {
    
    if (lst_mbr->next == NULL) {
      lst->tail = new_mbr;
    }

    new_mbr->next = lst_mbr->next;
    lst_mbr->next = new_mbr;
  }

  ++lst->size;
  return 0;
}



int
lst_rem_nxt(Lst *lst, Lst_mbr *lst_mbr, void **data)
{

  Lst_mbr *old_mbr;

  if (lst_size(lst) == 0) {
    return -1;
  }

  if (lst_mbr == NULL) {
    *data   = lst->head->data;
    old_mbr = lst->head;
    lst->head = lst->head->next;
    if (lst_size(lst) == 1)

      lst->tail = NULL;



  } else {

    if (lst_mbr->next == NULL) {
      return -1;
    }

    *data = lst_mbr->next->data;
    old_mbr = lst_mbr->next;
    lst_mbr->next = lst_mbr->next->next;

    if (lst_mbr->next == NULL) {
      lst->tail = lst_mbr;
    }

  }

  free(old_mbr);
  --lst->size;

  return 0;
  
}


/* Open configuration file and iterate over all entrys in it
 * malloc for every entry a entrybuff struct and add to list
 * 
 */
Lst *
parse_rc()
{
  
  /* FIXME: hard coded path */
  if ( (yyin = fopen(CONFIG_FILE, "r")) == NULL) {
      syslog (LOG_ERR, "Can't find configuration file (" CONFIG_FILE ")!");
      return NULL;
  }

  if ((lstptr = (Lst *) malloc(sizeof(Lst))) == NULL) {
    syslog(LOG_ERR, "Module Error! (Can't malloc(2) memory)");
    return NULL;
  }
  
  if ((ebp = (struct entrybuff *) malloc(sizeof(struct entrybuff))) == NULL) {
    syslog(LOG_ERR, "Module Error! (Can't malloc(2) memory)");
    return NULL;
  }

  /* bzero(3) struct for parser */
  init_struct(ebp);

  /* create list */
  lst_init(lstptr, (void *) clear_struct);
 
 if (yyparse())
   return NULL;

 /* print_list(); */


 return lstptr;

}

int
clear_struct(struct entrybuff *eb)
{

  if (eb->name != NULL) {
    free(eb->name);
  }
  
  if (eb->btmac != NULL) {
    free(eb->btmac);
  }

  if (eb->btname != NULL) {
    free(eb->btname);
  }

  if (eb) {
    free(eb);
    eb = NULL;
  }
  
  return 0;
}

int
init_struct(struct entrybuff *eb)
{
  eb->name    = NULL;
  eb->btmac   = NULL;
  eb->btname  = NULL;
  eb->timeout = 0;
  return 0;
}

int
set_gtimeout(int timeout)
{
  gtimeout = timeout;
}

int
set_userentry(struct entrybuff *eb)
{

  lst_ins_nxt(lstptr, lst_tail(lstptr), eb);

  if (eb->timeout == 0) { /* take default one */
    eb->timeout = gtimeout;
  }
  
  if ((ebp = (struct entrybuff *) malloc(sizeof(struct entrybuff))) == NULL) {
    syslog(LOG_ERR, "Module Error! (Can't malloc(2) memory)");
    exit(1);
  }

  /* bzero(3) struct for parser */
  init_struct(ebp);
  
  return 0;

}

#if 0  /* module debug entry */
int
print_list(void)
{
  int i;

  Lst_mbr *lst_mbrp = lstptr->head;
  for(i = 0; i < lst_size(lstptr); i++ ) {

    struct entrybuff *bptr = (struct entrybuff *) lst_mbrp->data;
    printf("\nUSERNAME := %s\n", bptr->name);
    printf("btmac  : %s ::\n", bptr->btmac);
    printf("btname : %s ::\n", bptr->btname);
    printf("timeout: %d ::\n", bptr->timeout);

    lst_mbrp = lst_mbrp->next;

  }

  return 0;

}

#endif  /* module debug entry */

