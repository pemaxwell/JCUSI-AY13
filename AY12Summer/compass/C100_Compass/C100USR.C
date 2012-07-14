/* $Header:   F:/c100_pc/user/vcs/c100usr.c_v   1.4   06 May 1993 10:20:58   DavidH  $ */
/*
 * $Log:   F:/c100_pc/user/vcs/c100usr.c_v  $
 * 
 *    Rev 1.4   06 May 1993 10:20:58   DavidH
 * Changed version number.
 *
 *    Rev 1.3   05 May 1993 15:02:54   DavidH
 * Added code to read the .INI file at startup.
 * Added #includes for Vermont Views Pushbutton and Checkbox field types.
 *
 *    Rev 1.2   08 Apr 1992 14:47:46   DavidH
 * Modified code in response to Laser Tech's comments.
 *
 *    Rev 1.1   30 Mar 1992 10:51:26   DavidH
 *
 *    Rev 1.0   12 Mar 1992 14:59:26   DavidH
 * Initial revision.
 */


/*----------------------------------------------------------------------------* 
 * C100USR.C    This is the user program for the C100 compass board. It allows* 
 *              the user to view/modify various parameters on the board via an* 
 *              on-screen form. The user may also use the built-in terminal   *
 *              emulator to do anything they wish to the board. The program   * 
 *              also provides a field calibration function.                   * 
 *                                                                            * 
 * Copyright (C) 1991, KVH Industries Inc, Middletown, RI.                    * 
 * Written by David R. Hoy, July/August 1991.                                 * 
 *----------------------------------------------------------------------------*/

 
#pragma warn -par                       /* turn off 'parameter not used' warning */

#include <vv_str.h>
#include <vv_int.h>
#include <vv_char.h>
#include <vv_bool.h>
#include <vv_date.h>
#include <vv_doubl.h>
#include <vv_radio.h>
#include <vv_push.h>
#include <vv_check.h>
#include <vv_clist.h>
#include <vv_mouse.h>
#include <vv_menu.h>
#include <vv_main.h>

#include <asiports.h>
#include <string.h>
#include "c100.h"
#include "c100usr.h"


/*------ local function prototypes ------*/
static MFORMPTR build_main_menu(void);
static MFORMPTR build_cal_menu(void);


/*------ external function prototypes */
extern int terminal_mode(FORMPTR fmp);
extern int system_defaults(FORMPTR fmp);
extern int parameter_editor(FORMPTR fmp);
extern int eight_point_cal(FORMPTR fmp);
extern int three_point_cal(FORMPTR fmp);
extern int circular_cal(FORMPTR fmp);
extern int read_ini_file(void);


/*------ global variables ------*/
static char     exe_path[41];                   /* path to .exe file */
static float    version = 2.11;



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int main(int argc,char *argv[])
{
MFORMPTR main_menu;
char     *ptr;


    strcpy(exe_path,argv[0]);                   /* make a copy of .exe path */
    ptr = strrchr(exe_path,'\\');               /* find last backslash in string */
    *ptr = '\0';                                /* replace it with a NULL */

    read_ini_file();                            /* read config file */

    vv_init();                                  /* fire up Vermont Views */
    vs_clr();                                   /* clear the screen */
    se_datefmt("YY/MM/DD");                     /* set system date format */

    main_menu = build_main_menu();              /* generate the main menu */
    mn_proc(0,main_menu);                       /* pass control to menu system */
    mn_free(main_menu);                         /* kill the main menu */

    vv_exit();                                  /* kill Vermont Views */
    return(0);                                  /* return to DOS */

} /* main() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function, when called from the menu system, causes it to return       *
 * control to the function that invoked the menu system with a call to        *
 * 'mn_rd()' or 'mn_proc()'.                                                  *
 *----------------------------------------------------------------------------*/
/* */
static int exit_menu(MFORMPTR mfmp)
{
   return(EXITMENU);                            /* return code to force menu exit */

} /* exit_menu() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
void msg_window(char *msg)
{
WINDOWPTR msg_wndw;                             /* window handle */

   csr_hide();                                  /* hide the cursor */
   msg_wndw = wn_def(CENTER_WN,CENTER_WN,5,44,LNORMAL,BDR_DLNP);
   sw_title("[Press Any Key]",LNORMAL,BOTTOMCENTER,msg_wndw);
   sw_shad(TRANSPARENT,LSHADOW,BOTTOMRIGHT,msg_wndw);
   sw_mgcol(1,1,msg_wndw);

   wn_up(msg_wndw);                             /* pop it on screen */
   v_st(msg,msg_wndw);                          /* write msg in window */

   flush_keybuf();
   ki();                                        /* wait for key press */
   wn_dn(msg_wndw);                             /* pop window off screen */
   wn_free(msg_wndw);                           /* release window memory */
   csr_show();                                  /* bring back the cursor */

} /* message_window() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int continue_abort(char *msg)
{
WINDOWPTR msg_wndw;
MFORMPTR  mfmp;
int       retval;

   csr_hide();                                  /* hide the cursor */
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
   if (curitemnum(mfmp) == 0)                   /* was CONTINUE selected? */
      retval = CONTINUE;
   else                                         /* or ABORT? */
      retval = ABORT;

   wn_dn(msg_wndw);                             /* pop window off screen */
   wn_free(msg_wndw);                           /* release window memory */
   mn_free(mfmp);
   csr_show();                                  /* bring back the cursor */

   return(retval);

} /* continue_abort() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
static MFORMPTR build_main_menu()
{
MFORMPTR        main_menu,sub_fmp;
MFIELDPTR       mfldp;
int             i;
static char     title[60];


   main_menu = mn_def(MNAUTOSELECT,0,0,vs_rowq()-1,vs_colq(),
                      LNORMAL,BDR_NULLP);
   smn_opt(MNTOPESCAPE,OFF,main_menu);

   bg_boxdef(0,0,3,vs_colq(),LHIGHLITE,BDR_DLNP,main_menu);
   sprintf(title,"[KVH Industries Inc - C100 Setup Program ver %.2f]",
           version);
   bg_txtdef(0,2,title,LHIGHLITE,main_menu);
   for (i = 3; i < vs_rowq() - 1; i++)
      bg_txtdef(i,0,pic_def(' ',vs_colq()),LFLDACT,main_menu);

   mfldp = mnf_def(1,2,"Setup","Set system defaults.",
                   NULLP,system_defaults,main_menu);
   smnf_selectch(0,LHIGHLITE,mfldp);

   mfldp = mnf_def(1,9,"Parameters","C100 Parameter Editor.",
                   NULLP,parameter_editor,main_menu);
   smnf_selectch(0,LHIGHLITE,mfldp);

   mfldp = mnf_def(1,21,"Terminal Mode",
                   "Terminal emulator for direct communiction with C100.",
                   NULLP,terminal_mode,main_menu);
   smnf_selectch(0,LHIGHLITE,mfldp);

   sub_fmp = build_cal_menu();
   mfldp = mnf_def(1,36,"Calibrate","Display Calibration Menu.",
                   sub_fmp,MENUCALL,main_menu);
   smnf_selectch(0,LHIGHLITE,mfldp);

   mfldp = mnf_def(1,47,"Quit","Return to MS-DOS.",
                   NULLP,exit_menu,main_menu);
   smnf_selectch(0,LHIGHLITE,mfldp);

   return(main_menu);

} /* build_main_menu() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
static MFORMPTR build_cal_menu(void)
{
MFORMPTR        fmp;
MFIELDPTR       fldp;


   fmp = mn_defauto(MNAUTOSELECT,LHIGHLITE,BDR_DLNP);
   sw_shad(TRANSPARENT,LSHADOW,BOTTOMRIGHT,fmp->wnp);
   smn_opt(MNMSGAUTOPLACE | MNAUTOSELECT | CURSORFREE,OFF,fmp);
   smn_opt(MNCHSELECT,ON,fmp);

   fldp = mnf_def(0,0," Eight Point ","Perform 8-point calibration on C100.",
                  NULLP,eight_point_cal,fmp);
   smnf_selectch(1,LHIGHLITE,fldp);

   fldp = mnf_def(1,0," Three Point ","Perform 3-point calibration on C100.",
                  NULLP,three_point_cal,fmp);
   smnf_selectch(1,LHIGHLITE,fldp);

   fldp = mnf_def(2,0," Circular    ","Perform circular calibration on C100.",
                  NULLP,circular_cal,fmp);
   smnf_selectch(1,LHIGHLITE,fldp);


   return(fmp);

} /* build_cal_menu() */

