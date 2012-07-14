/* $Header:   F:/c100_pc/user/vcs/sys_dflt.c_v   1.2   05 May 1993 15:09:24   DavidH  $ */
/* $Revision:   1.2  $ */
/*
 * $Log:   F:/c100_pc/user/vcs/sys_dflt.c_v  $
 * 
 *    Rev 1.2   05 May 1993 15:09:24   DavidH
 * Major change to system defaults editor - most of it ported from the DDGM
 * factory cal software.  
 * Also added function to read and write a .INI file to save settings between
 * sessions.
 * 
 *    Rev 1.1   08 Apr 1992 14:48:20   DavidH
 * Modified code in response to Laser Tech's comments.
 * 
 *    Rev 1.0   12 Mar 1992 14:59:36   DavidH
 * Initial revision.
 */


/*----------------------------------------------------------------------------*
 * SYS_DLFT.C - This module is used to interactively change the communications*
 *              settings.                                                     *
 *                                                                            *
 * Written by David R. Hoy                                                    *
 * Copyright (C) 1993, KVH Industries.                                        *
 *----------------------------------------------------------------------------*/

#include <vv_form.h>
#include <vv_radio.h>
#include <vv_push.h>
#include <asiports.h>
#include <stdlib.h>
#include <string.h>

#pragma warn -par

#define RADIO_ON_CH             0x07
#define RADIO_OFF_CH            0x20

#define INI_FILE_NAME           "C100USR.INI"
#define DEFAULT_COMPASS_PORT    COM1
#define DEFAULT_COMPASS_BAUD    4800L
#define DEFAULT_ECHO            TRUE
#define DEFAULT_UNPRINTABLES    FALSE


/*------ local type/structure declarations ------*/
typedef struct {
    char dummy;
    int  compass_port;
    int  compass_baud;
    int  echo,unprintables;
} DEFAULTS;


int write_ini_file(void);
int read_ini_file(void);

/*------ global variable declarations ------*/
int  dflt_port;
long dflt_baud;
int  echo,unprintables;


/*------ local variable declarations ------*/
static DEFAULTS defaults;
static long baud_array[] = { 9600, 4800, 2400, 1200, 600, 300 };
static char *baud_rates[]= { "9600","4800","2400","1200","600","300"};
static char *com_ports[] = { "COM1","COM2","COM3","COM4" };

#define NBR_PORTS   (sizeof(com_ports)  / sizeof(char *))
#define NBR_BAUDS   (sizeof(baud_rates) / sizeof(char *))


/*------ local function prototypes ------*/
static int      defaults_beginfp(FORMPTR fmp);
static int      defaults_endfp(FORMPTR fmp);
static DFORMPTR create_defaults_form(void);
int             button_ok(DFORMPTR fmp);
int             button_cancel(DFORMPTR fmp);



/*PAGE*/
/*----------------------------------------------------------------------------*
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/* */
int system_defaults(FORMPTR fmp)
{
DFORMPTR        dfmp;

    dfmp = create_defaults_form();
    fm_proc(0,dfmp);
    fm_free(dfmp);

    return(SAMELEVEL);

} /* system_defaults() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
static int defaults_beginfp(FORMPTR fmp)
{
int   i;

    defaults.compass_port = dflt_port;

    defaults.compass_baud = 1;               /* default to 4800 baud */
    for (i = 0; i < (sizeof(baud_array) / sizeof(long)); i++) {
        if (baud_array[i] == dflt_baud)
            defaults.compass_baud = i;
    } /* for */

    defaults.echo         = echo;
    defaults.unprintables = unprintables;

    fm_updflds(fmp);

    return(TRUE);

} /* defaults_beginfp() */




/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
static int defaults_endfp(FORMPTR fmp)
{
    if (fm_nextitem(fmp) != AC_EXIT)
        return(TRUE);

    dflt_port    = defaults.compass_port;
    dflt_baud    = baud_array[defaults.compass_baud];
    echo         = defaults.echo;
    unprintables = defaults.unprintables;

    write_ini_file();

    return(TRUE);

} /* defaults_endfp() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
static DFORMPTR create_defaults_form(void)
{
DFORMPTR  fmp = (DFORMPTR) NULLP;
DFIELDPTR fldp;
DFIELDPTR rad_first;
int       i;


    /*--- create VV data form ---*/
    fmp = fm_def(CENTER_WN,CENTER_WN,18,33,LREVERSE,BDR_DLNP);
    sfm_name("default_form", fmp);
    sw_shad(TRANSPARENT,LSHADOW,BOTTOMRIGHT,fmp->wnp);
    sw_updnfp(wn_expset,wn_expunset,fmp->wnp);
    sw_bdratt(LREVERSE, fmp->wnp);
    sw_title("[System Default Setup]",LREVERSE,TOPCENTER,fmp->wnp);
    sfm_opt(CURSORCROSS | CURSORFREE | FMLINKSHOW | VERIFYQUIT,
            OFF,fmp);
    sfm_opt(AUTOMOVE | FMWRAP,ON,fmp);
    sw_opt(MOU_RESIZE,OFF,fmp->wnp);
    sw_opt(POPUP | MOU_DRAG | MOU_MODAL,ON,fmp->wnp);
    sfm_begfp(defaults_beginfp,fmp);
    sfm_endfp(defaults_endfp,fmp);


    /*--- create background objects on form ---*/
    bg_boxdef(0,2,NBR_PORTS+2,10,LREVERSE,BDR_SLNP,fmp);
    bg_txtdef(0,3,"[Port]",LREVERSE,fmp);

    bg_boxdef(0,16,NBR_BAUDS+2,11,LREVERSE,BDR_SLNP,fmp);
    bg_txtdef(0,17,"[Baud]",LREVERSE,fmp);


    /*--- create dummy field on form ---*/
    fldp = fld_def(0,0,NULLP,0,"X",F_CHAR,&defaults.dummy,fmp);
    sf_opt(SKIP,ON,fldp);
    sf_att(LREVERSE,LREVERSE,LREVERSE,fldp);


    /*--- create COM port radio-button group ---*/
    for (i = 0; i < NBR_PORTS; i++) {
        fldp = fld_def(1+i,7,com_ports[i],3,"(X)",F_RADIO,
                       &defaults.compass_port,fmp);
        if (i == 0)
            rad_first = fldp;
        sf_msg("Select COM port to which C100 is attatched.", fldp);
        sf_att(LMNSELCH, LNORMAL, LFLDSKIP, fldp);
        sf_opt(INITIALBLANKS | LFTADJUST | NOECHO | RTADJUST | THSEPARATOR |
               CLEARONKEY | FILL | INSERTINIT | PROTECTED | REQUIRED | REQ_AT_EXIT |
               RTENTRY | SKIP | CHKTRUNCATE | FORCECONVERT | PICSKIP | TRAILBLANKS |
               KEEPNEWLINES, OFF, fldp);
    } /* for */
    sf_radgroup(rad_first,fldp,fmp);
    sf_radch(RADIO_ON_CH,RADIO_OFF_CH,rad_first, fmp);


    /*--- create baud rate radio-button group ---*/
    for (i = 0; i < NBR_BAUDS; i++) {
        fldp = fld_def(1+i,21,baud_rates[i],17,"(X)",F_RADIO,
                       &defaults.compass_baud,fmp);
        if (i == 0)
            rad_first = fldp;
        sf_msg("Select baud rate at which to communicate.", fldp);
        sf_att(LMNSELCH, LNORMAL, LFLDSKIP, fldp);
        sf_opt(INITIALBLANKS | LFTADJUST | NOECHO | RTADJUST | THSEPARATOR |
               CLEARONKEY | FILL | INSERTINIT | PROTECTED | REQUIRED | REQ_AT_EXIT |
               RTENTRY | SKIP | CHKTRUNCATE | FORCECONVERT | PICSKIP | TRAILBLANKS |
               KEEPNEWLINES, OFF, fldp);
    } /* for */
    sf_radgroup(rad_first,fldp,fmp);
    sf_radch(RADIO_ON_CH,RADIO_OFF_CH,rad_first,fmp);


    /*--- create checkbox fields ---*/
    fldp = fld_def(10,6,"Echo characters?",2,"[A]",F_CHECK,
                   &defaults.echo,fmp);
    fldp = fld_def(12,6,"Display unprintables?",2,"[A]",F_CHECK,
                   &defaults.unprintables,fmp);


    /*--- create OK and CANCEL pushbuttons ---*/
    pb_def(14, 3,NULLP,"   OK   ",button_ok,fmp);
    pb_def(14,20,NULLP," CANCEL ",button_cancel,fmp);

    return(fmp);

} /* create_defaults_form() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int button_ok(DFORMPTR fmp)
{
    sfm_nextitem(AC_EXIT,fmp);
    return(TRUE);

} /* button_ok() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int button_cancel(DFORMPTR fmp)
{
    sfm_nextitem(AC_QUIT,fmp);
    return(TRUE);

} /* button_cancel() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int read_ini_file(void)
{
FILE    *ini_file;
char    str[80];
char    *token;


    ini_file = fopen(INI_FILE_NAME,"rt");
    if (ini_file == NULL) {
        dflt_port    = DEFAULT_COMPASS_PORT;
        dflt_baud    = DEFAULT_COMPASS_BAUD;
        echo         = DEFAULT_ECHO;
        unprintables = DEFAULT_UNPRINTABLES;
        return(-1);
    } /* if */

    while (!feof(ini_file)) {
        if (!fgets(str,sizeof(str),ini_file))
            continue;

        strlwr(str);
        token = strtok(str,"=");
        if (token == NULL)
            continue;

        if (strcmp(token,"port") == 0) {
            token = strtok(NULL,"");
            if (token == NULL)
                continue;
            dflt_port = atoi(token) - 1;
            continue;
        } /* if */

        if (strcmp(token,"baud") == 0) {
            token = strtok(NULL,"");
            if (token == NULL)
                continue;
            dflt_baud = atol(token);
            continue;
        } /* if */

        if (strcmp(token,"echo") == 0) {
            token = strtok(NULL,"");
            echo = atoi(token);
        } /* if */


        if (strcmp(token,"unprintables") == 0) {
            token = strtok(NULL,"");
            unprintables = atoi(token);
        } /* if */
    } /* while */

    fclose(ini_file);
    return(0);

} /* read_ini_file() */


/*FF*/
/*----------------------------------------------------------------------------*
 *----------------------------------------------------------------------------*/
/* */
int write_ini_file(void)
{
FILE    *ini_file;


    ini_file = fopen(INI_FILE_NAME,"wt");

    fputs("[c100]\n",ini_file);
    fprintf(ini_file,"port=%d\n",dflt_port+1);
    fprintf(ini_file,"baud=%ld\n",dflt_baud);
    fprintf(ini_file,"echo=%d\n",echo);
    fprintf(ini_file,"unprintables=%d\n\n",unprintables);

    fclose(ini_file);
    return(0);

} /* write_ini_file() */







