/*
** Copyright (C) 2003 Hagen Paul Pfeifer <hagen@jauu.net>
**  
** $Id: bluepmain.c 19 2004-04-21 18:24:46Z pfeifer $
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

#define DEFAULT_USER "nobody"

#include <stdio.h>

#define PAM_SM_AUTH
#define PAM_SM_ACCOUNT
#define PAM_SM_SESSION
#define PAM_SM_PASSWORD

#include <security/pam_modules.h>
#include <security/_pam_macros.h>

#include <sys/types.h>
#include <pwd.h>

#define _XOPEN_SOURCE
#include <unistd.h>

int debug = 0;
 
PAM_EXTERN int
pam_sm_authenticate(pam_handle_t *pamh, int flags,
                    int argc, const char **argv)
{
  int i, error = 0;
  int retval;
  char *reterror = NULL;
  struct pam_conv *conv;
  struct pam_message  *pmsg[3], msg[3];
  struct pam_response *response;   
  struct entrybuff *bptr;
  const char *user=NULL;
  Lst *lst;
  Lst_mbr *lst_mbrp;

  openlog("pam_blue", LOG_NDELAY | LOG_CONS | LOG_PID, LOG_AUTHPRIV);

  if (argc >= 1)
    if (!strncmp(argv[0], "debug", 5))
      debug = 1;
  
  /* whoWeAre.com ;-) */
  retval = pam_get_user( pamh, &user, NULL);
  if (retval != PAM_SUCCESS) {
    syslog (LOG_ERR, "Can't determine username");
    return PAM_USER_UNKNOWN;
  }

  retval = pam_get_item(pamh, PAM_CONV, (const void **) &conv);
  if (retval != PAM_SUCCESS) {
    user = NULL;
    syslog (LOG_ERR, "pam errror [%s]!", (pam_strerror (pamh,retval)));
    return PAM_SYSTEM_ERR;
  } 
  
  if (user == NULL || *user == '\0' ) {
    /* D (("pam_get_user: invalid username")); */
    user = NULL;
    syslog (LOG_ERR, "Invalid username!");
    return PAM_USER_UNKNOWN;
  }

  if ( (lst = parse_rc()) == NULL) { /* something goes wrong (no config file, ...) */
    user = NULL;
    syslog (LOG_ERR, "Can't parse configuration file [%s]!" CONFIG_FILE);
    return PAM_AUTH_ERR;
  }
    
  /* fine we had a valid username; check for user in list
   * break if we found the first valid entry
   * TODO: implement something like priorities: user are higher rated then
   * group entrys
   * 
   * Starting at this point make sure you free(3) all malloced datastructs!
   * 
   */
  
  lst_mbrp = lst->head;
  reterror = "User not found in configuration file [" CONFIG_FILE "]!";
  for(i = 0; i < lst_size(lst); i++) {
    bptr = (struct entrybuff *) lst_mbrp->data;
    if (! strcmp(bptr->name, user)) { /* found user in list */
      reterror = "User found in configuration file [" CONFIG_FILE "], but scan failed!";
      /* TODO: add some error return codes for speparate logging */
      if (bluescan(bptr) > 0) {
        if (debug)
          syslog (LOG_ERR, "scan succeed for user %s!", user);
        user = NULL;
        lst_destroy(lst);
        return PAM_SUCCESS;
      } else { /* failure */
        syslog (LOG_ERR, "Bluetooth scan failure [bluetooth device %s up?]", bptr->btmac);
      }
    }
    lst_mbrp = lst_mbrp->next;
  }

  if (reterror) { /* free struct, log and return failure */
    user = NULL;
    lst_destroy(lst);
    syslog (LOG_ERR, "%s", reterror);
    return PAM_AUTH_ERR;

  }

  /* catch all rule */
  user = NULL;
  lst_destroy(lst);
  syslog (LOG_ERR, "Programmed failure!!!");
  return PAM_AUTH_ERR;
}

PAM_EXTERN
int pam_sm_setcred(pam_handle_t *pamh,int flags,int argc
    ,const char **argv)
{
  return PAM_SUCCESS;
}

/* --- account management functions --- */

PAM_EXTERN
int pam_sm_acct_mgmt(pam_handle_t *pamh,int flags,int argc
    ,const char **argv)
{
  return PAM_SUCCESS;
}

/* --- password management --- */

PAM_EXTERN
int pam_sm_chauthtok(pam_handle_t *pamh,int flags,int argc
    ,const char **argv)
{
  return PAM_SUCCESS;
}

/* --- session management --- */

PAM_EXTERN
int pam_sm_open_session(pam_handle_t *pamh,int flags,int argc
    ,const char **argv)
{
  return PAM_SUCCESS;
}

PAM_EXTERN
int pam_sm_close_session(pam_handle_t *pamh,int flags,int argc
    ,const char **argv)
{
  return PAM_SUCCESS;
}

/* end of module definition */

#ifdef PAM_STATIC

/* static module data */

struct pam_module _pam_permit_modstruct = {
  "pam_permit",
  pam_sm_authenticate,
  pam_sm_setcred,
  pam_sm_acct_mgmt,
  pam_sm_open_session,
  pam_sm_close_session,
  pam_sm_chauthtok
};

#endif

#if 0
  /* searching prompt */
  if (1)
  {
    pmsg[0] = &msg[0];
    msg[0].msg_style = PAM_TEXT_INFO;
    msg[0].msg = (char *) malloc (sizeof (char) * (50 + strlen (user)));
    snprintf (msg[0].msg, 50 + strlen (user), "Scanning ... ");
    retval = conv->conv (1, (const struct pam_message **) pmsg, &response, conv->appdata_ptr);
  }

  
  /* ..and found prompt */
  if (1)
  {
    pmsg[0] = &msg[0];
    msg[0].msg_style = PAM_TEXT_INFO;
    msg[0].msg = (char *) malloc (sizeof (char) * (50 + strlen (user)));
    snprintf (msg[0].msg, 50 + strlen (user), "found");
    retval = conv->conv (1, (const struct pam_message **) pmsg, &response, conv->appdata_ptr);
  }

#endif 
