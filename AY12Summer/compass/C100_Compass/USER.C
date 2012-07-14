/*----------------------------------------------------------------------------*/
/* USER.C	This is the user program for the C100 compass board. It allows*/
/*		the user to view/modify various parameters on the board via an*/
/*		on-screen form. The user may also use the built-in terminal   */
/*		emulator to do anything they wish to the board. The program   */
/*		also provides a field calibration function.		      */
/*                                                                            */
/* Copyright (C) 1991, KVH Industries Inc, Middletown, RI.		      */
/* Written by David R. Hoy, July/August 1991.                                 */
/*----------------------------------------------------------------------------*/

/* $Header:   C:/c100/user/vcs/user.c_v   1.0   12 Mar 1992 14:59:40   DavidH  $ */
/* $Revision:   1.0  $ */
/*
 * $Log:   C:/c100/user/vcs/user.c_v  $
 * 
 *    Rev 1.0   12 Mar 1992 14:59:40   DavidH
 * Initial revision.
 */


#pragma warn -par			/* turn off 'parameter not used' warning */

#include <vv_str.h>
#include <vv_int.h>
#include <vv_char.h>
#include <vv_bool.h>
#include <vv_date.h>
#include <vv_doubl.h>
#include <vv_radio.h>
#include <vv_clist.h>
#include <vv_sr.h>
#include <vv_menu.h>
#include <vv_main.h>

#include <asiports.h>
#include <string.h>
#include <ctype.h>
#include "c100.h"

#define flush_keybuf()	while (ki_chk()) { ki(); }
#define CONTINUE	0
#define	ABORT		1

/*------ local function prototypes ------*/
MFORMPTR build_main_menu(void);
void 	 msg_window(char *msg);
int	 continue_abort(char *msg);

/*------ external function prototypes */
extern int terminal_mode(FORMPTR fmp);
extern int system_defaults(FORMPTR fmp);
extern int parameter_editor(FORMPTR fmp);
extern int eight_point_cal(FORMPTR fmp);
extern int three_point_cal(FORMPTR fmp);
extern int circular_cal(FORMPTR fmp);


/*------ global variables ------*/
int	dflt_port = COM_1;			/* default port number for C100 */
long	dflt_baud = 4800L;			/* default baud rate for C100 */
char	exe_path[41];				/* path to .exe file */
float	version = 1.4;


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
int main(int argc,char *argv[])
{
MFORMPTR main_menu;
char	 *ptr;


   strcpy(exe_path,argv[0]);			/* make a copy of .exe path */
   ptr = strrchr(exe_path,'\\');		/* find last backslash in string */
   *ptr = '\0';					/* replace it with a NULL */

   vv_init();					/* fire up Vermont Views */
   vs_clr();					/* clear the screen */
   se_datefmt("YY/MM/DD");			/* set system date format */

   main_menu = build_main_menu();		/* generate the main menu */
   mn_proc(0,main_menu);			/* pass control to menu system */
   mn_free(main_menu);				/* kill the main menu */

   vv_exit();					/* kill Vermont Views */
   return(0);					/* return to DOS */

} /* main() */


/*----------------------------------------------------------------------------*/
/* This function, when called from the menu system, causes it to return       */
/* control to the function that invoked the menu system with a call to        */
/* 'mn_rd()' or 'mn_proc()'.                                                  */
/*----------------------------------------------------------------------------*/
int exit_menu(MFORMPTR mfmp)
{
   return(EXITMENU);				/* return code to force menu exit */
} /* exit_menu() */


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void msg_window(char *msg)
{
WINDOWPTR msg_wndw;                             /* window handle */

   csr_hide();                                  /* hide the cursor */
   msg_wndw = wn_def(CENTER_WN,CENTER_WN,5,44,  /* define window */
		     LNORMAL,BDR_DLNP);
   sw_title("[Press Any Key]",LNORMAL,
	    BOTTOMCENTER,msg_wndw);             /* give it a title */
   sw_shad(TRANSPARENT,LSHADOW,BOTTOMRIGHT,
	   msg_wndw);                           /* give it a shadow */
   sw_mgcol(1,1,msg_wndw);                      /* give it left & right margins */

   wn_up(msg_wndw);                             /* pop it on screen */
   v_st(msg,msg_wndw);                          /* write msg in window */

   flush_keybuf();
   ki();                                        /* wait for key press */
   wn_dn(msg_wndw);                             /* pop window off screen */
   wn_free(msg_wndw);                           /* release window memory */
   csr_show();                                  /* bring back the cursor */

} /* message_window() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
int continue_abort(char *msg)
{
WINDOWPTR msg_wndw;
MFORMPTR  mfmp;
int	  retval;

   csr_hide();					/* hide the cursor */
						/* define a window */
   msg_wndw = wn_def(7,18,6,44,LNORMAL,BDR_DLNP);
   sw_shad(TRANSPARENT,LSHADOW,BOTTOMRIGHT,msg_wndw);
   sw_mgcol(1,1,msg_wndw);

   wn_up(msg_wndw);                             /* pop it on screen */
   v_st(msg,msg_wndw);                          /* write msg in window */

   mfmp = mn_def(MNSTANDARD,11,28,AUTOSIZE,AUTOSIZE,
		 LNORMAL,BDR_NULLP);
   smn_opt(MNTOPESCAPE,OFF,mfmp);
   mnf_def(0,0 ,"Continue",NULLP,NULLP,exit_menu,mfmp);
   mnf_def(0,14,"Abort"   ,NULLP,NULLP,exit_menu,mfmp);


   mn_proc(0,mfmp);
   if (curitemnum(mfmp) == 0)			/* was CONTINUE selected? */
      retval = CONTINUE;
   else						/* or ABORT? */
      retval = ABORT;

   wn_dn(msg_wndw);                             /* pop window off screen */
   wn_free(msg_wndw);                           /* release window memory */
   mn_free(mfmp);
   csr_show();                                  /* bring back the cursor */

   return(retval);

} /* continue_abort() */



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
MFORMPTR build_main_menu()
{
MFORMPTR        main_menu;
MFIELDPTR       mfldp;
int             i;
static char	title[60];


   main_menu = mn_def(MNSTANDARD,0,0,vs_rowq(),vs_colq(),LNORMAL,BDR_NULLP);
   smn_opt(MNCONFIRM|MNMSGAUTOPLACE|MNREDISPLAY|MNTOPESCAPE,OFF,main_menu);

   bg_boxdef(0,0,3,vs_colq(),LHIGHLITE,BDR_DLNP,main_menu);
   sprintf(title,"[KVH Industries Inc - C100 Setup Program rev%3.1f]",
	   version);
   bg_txtdef(0,2,title,LHIGHLITE,main_menu);
   for (i = 3; i < vs_rowq() - 1; i++) {
      bg_txtdef(i,0,pic_def(' ',vs_colq()),LFLDACT,main_menu);
   } /* for */

   mfldp = mnf_def(1,2,"Setup",NULLP,NULLP,system_defaults,main_menu);
   smnf_selectch(0,LHIGHLITE,mfldp);
   sf_msg("Set defaults for program operation.",mfldp);

   mfldp = mnf_def(1,9,"Parameters",NULLP,NULLP,parameter_editor,main_menu);
   smnf_selectch(0,LHIGHLITE,mfldp);
   sf_msg("Set C100 compass board parameters.",mfldp);

   mfldp = mnf_def(1,21,"Terminal Mode",NULLP,NULLP,terminal_mode,main_menu);
   smnf_selectch(0,LHIGHLITE,mfldp);
   sf_msg("Terminal emulator for direct connection to C100.",mfldp);

   mfldp = mnf_def(1,36,"Calibrate",NULLP,NULLP,eight_point_cal,main_menu);
   smnf_selectch(0,LHIGHLITE,mfldp);
   sf_msg("Perform Field Calibration on C100 Board.",mfldp);

   mfldp = mnf_def(1,47,"Quit",NULLP,NULLP,exit_menu,main_menu);
   smnf_selectch(0,LHIGHLITE,mfldp);
   sf_msg("Return to MS-DOS.",mfldp);

   return(main_menu);

} /* build_main_menu() */

