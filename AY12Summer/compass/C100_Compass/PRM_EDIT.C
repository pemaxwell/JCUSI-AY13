/* $Header:   F:/c100_pc/user/vcs/prm_edit.c_v   1.4   10 May 1993 14:35:06   DavidH  $ */
/* $Revision:   1.4  $ */
/*
 * $Log:   F:/c100_pc/user/vcs/prm_edit.c_v  $
 * 
 *    Rev 1.4   10 May 1993 14:35:06   DavidH
 * Added code to set the parameter form's ALTERED flag to force the program to
 * write any changes to the C100 after using Ctrl-D to restore defaults.
 * 
 *    Rev 1.3   06 May 1993 10:23:26   DavidH
 * Added code to Ctrl-S event function to force underlying data structure to be
 * updated before writing the defaults to disk.
 * Changed declaration and validation of variavtion and a-offset fields to 
 * correctly handle mils/degrees.
 *
 *    Rev 1.2   05 May 1993 15:05:44   DavidH
 * Changed order in which parameters are written to the C100 to fix problem when
 * setting strobed digital output with sum-and-dump filtering.
 *
 *    Rev 1.1   08 Apr 1992 14:48:16   DavidH
 * Modified code in response to Laser Tech's comments.
 *
 *    Rev 1.0   12 Mar 1992 14:59:32   DavidH
 * Initial revision.
 */

#include <string.h>
#include <vv_form.h>
#include <vv_bool.h>
#include "c100.h"

#pragma warn -par


#define MAG_HEADING     1
#define TRUE_HEADING    0

#define LWAIT           LAVR1

typedef struct {
    char   serial_nbr[8];
    char   unit_type[5];
    char   cal_date[9];
    char   hrdwr_rev;
    char   sftwr_rev;
    char   true_mag;
    double variation;
    double a_offset;
    char   powerup_mode;
    int    baud_rate;
    int    msg_rate;
    char   msg_type;
    char   msg_units;
    char   digital_type;
    char   digital_fmt;
    char   analog_fmt;
    char   damping_type;
    char   damping_rate;
} PARAMETERS;


extern void msg_window(char *msg);



/*------ local function prototypes ------*/
static DFORMPTR create_parameters_form(void);
static int      parameters_beginfp(FORMPTR fmp);
static int      parameters_endfp(FORMPTR fmp);
static int      save_defaults(FORMPTR fmp);
static int      restore_defaults(FORMPTR fmp);
static int      check_baud_msg_rate(FORMPTR fmp);
static int      ValidateVariation(FORMPTR fmp);
static int      ValidateAOffset(FORMPTR fmp);


/*------ global variables ------*/
static PARAMETERS parameters;
static PARAMETERS dflt_prm;
static MFILEPTR   clist_mfp;
extern int        dflt_port;
extern long       dflt_baud;
extern char       *_vv_valmsg[];
static int        error_flag;



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int parameter_editor(FORMPTR fmp)
{
DFORMPTR        dfmp;
char            *save_ptr;


   /*------ temporarily modify validation message table ------*/
   save_ptr = _vv_valmsg[15];
   _vv_valmsg[15] = "Field may not be edited - Press <F2> for choice list.";

   latt_rpl(LWAIT,LREVERSE|BLINK,RED|BLINK,BLUE,LATT_SYS);

   /*------ add hot-keys to form event table ------*/
   et_rplevnt(KEY_CD,(EVENTFP)restore_defaults,FMETP);
   et_rplevnt(KEY_CS,(EVENTFP)save_defaults,   FMETP);

   /*------ define memory file and read choice list text ------*/
   clist_mfp = mf_def(60,40);
   mf_rd("CLIST.TXT",clist_mfp);

   /*------ create and process the parameters form ------*/
   dfmp = create_parameters_form();
   fm_proc(0,dfmp);
   fm_free(dfmp);

   mf_free(clist_mfp);

   /*------ restore form event table ------*/
   et_rplevnt(KEY_CD,NULLFP,FMETP);
   et_rplevnt(KEY_CS,NULLFP,FMETP);

   /*------ restore validation message table ------*/
   _vv_valmsg[15] = save_ptr;

   return(SAMELEVEL);

} /* parameter_editor() */




/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
static int parameters_beginfp(FORMPTR fmp)
{
int         i,status,on_off;
char        variation_onoff;
char        err_msg[80];
DFIELDPTR   fldp;
WINDOWPTR   wnp;

   error_flag = TRUE;

   memset(&parameters,0,sizeof(parameters));
   for (i = 0; i <= i_maxnum(fmp); i++) {
      fldp = i_numptr(i,fmp);
      fld_blank(fldp,fmp);
   } /* for */

   wnp = wn_def(vs_rowq()-1,0,1,vs_colq(),LWAIT,BDR_NULLP);
   wn_up(wnp);
   v_st(" Reading parameters from compass, please wait...",wnp);

   status = c100_open_channel(dflt_port,dflt_baud);
   if (status != 0) {
      strcpy(err_msg,"Unable to talk to compass.");
      goto ERROR;
   } /* if */

   c100_zap();                  /* re-boot compass to force any changes
                                   made in terminal mode into effect */

   status = c100_halt_data();
   if (status != 0) {
      strcpy(err_msg,"Unable to halt data.");
      goto ERROR;
   } /* if */

   status = c100_get_who(parameters.serial_nbr,&parameters.sftwr_rev,
                         &parameters.hrdwr_rev,parameters.unit_type,
                         parameters.cal_date);
   if (status != 0) {
      strcpy(err_msg,"Unable to read serial number.");
      goto ERROR;
   } /* if */

   status = c100_get_variation_onoff(&variation_onoff);
   if (status != 0) {
      strcpy(err_msg,"Unable to read variation flag.");
      goto ERROR;
   } /* if */
   parameters.true_mag = (variation_onoff == 't') ? TRUE_HEADING : MAG_HEADING;

   status = c100_get_variation(&parameters.variation);
   if (status != 0) {
      strcpy(err_msg,"Unable to read variation.");
      goto ERROR;
   } /* if */

   status = c100_get_a_offset(&parameters.a_offset);
   if (status != 0) {
      strcpy(err_msg,"Unable to read A offset.");
      goto ERROR;
   } /* if */

   status = c100_get_powerup_mode(&parameters.powerup_mode);
   if (status != 0) {
      strcpy(err_msg,"Unable to read power-up mode.");
      goto ERROR;
   } /* if */

   status = c100_get_baud(&parameters.baud_rate);
   if (status != 0) {
      strcpy(err_msg,"Unable to read baud rate.");
      goto ERROR;
   } /* if */

   status = c100_get_msg_rate(&parameters.msg_rate);
   if (status != 0) {
      strcpy(err_msg,"Unable to read message rate.");
      goto ERROR;
   } /* if */

   status = c100_get_msg_type(&parameters.msg_type);
   if (status != 0) {
      strcpy(err_msg,"Unable to read message type.");
      goto ERROR;
   } /* if */

   status = c100_get_damp_type(&parameters.damping_type);
   if (status != 0) {
      strcpy(err_msg,"Unable to read damping type.");
      goto ERROR;
   } /* if */

   status = c100_get_damp_rate(&parameters.damping_rate);
   if (status != 0) {
      strcpy(err_msg,"Unable to read damping rate.");
      goto ERROR;
   } /* if */

   status = c100_get_digital_type(&parameters.digital_type);
   if (status != 0) {
      strcpy(err_msg,"Unable to read digital output type.");
      goto ERROR;
   } /* if */

   status = c100_get_digital_fmt(&parameters.digital_fmt);
   if (status != 0) {
      strcpy(err_msg,"Unable to read digital output format.");
      goto ERROR;
   } /* if */

   status = c100_get_analog_fmt(&parameters.analog_fmt);
   if (status != 0) {
      strcpy(err_msg,"Unable to read analog output format.");
      goto ERROR;
   } /* if */

   status = c100_get_msg_units(&parameters.msg_units);
   if (status != 0) {
      strcpy(err_msg,"Unable to read analog output format.");
      goto ERROR;
   } /* if */


   /*--- if in SUPERUSER mode, allow access to certain fields ---*/
   on_off = (c100_in_superuser_mode() ? OFF : ON);

   fldp = i_namptr("serial_nbr",fmp);
   sf_opt(SKIP,on_off,fldp);

   fldp = i_namptr("unit_type",fmp);
   sf_opt(SKIP,on_off,fldp);

   fldp = i_namptr("cal_date",fmp);
   sf_opt(SKIP,on_off,fldp);

   fldp = i_namptr("hardware_rev",fmp);
   sf_opt(SKIP,on_off,fldp);

   fldp = i_namptr("software_rev",fmp);
   sf_opt(SKIP,on_off,fldp);

   error_flag = FALSE;

ERROR:
   wn_dn(wnp);
   wn_free(wnp);

   if (status != 0) {      /* was there an error? */
      strcat(err_msg," ");
      strcat(err_msg,c100_error_name(status));
      msg_window(err_msg);
      sfm_nextitem(AC_EXIT,fmp);
   } else {
      fm_updflds(fmp);
   } /* if */

   return(TRUE);

} /* parameters_beginfp() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
static int parameters_endfp(FORMPTR fmp)
{
int         status;
char        err_msg[80];
char        variation_onoff;
WINDOWPTR   wnp;
                      

   if (error_flag)
      goto END;

   wnp = wn_def(vs_rowq()-1,0,1,vs_colq(),LWAIT,BDR_NULLP);
   wn_up(wnp);
   v_st(" Updating compass parameters, please wait...",wnp);

   status = 0;
   if ((fm_nextitem(fmp) == AC_QUIT) || !fm_isaltered(fmp))
      goto ERROR;             /* avoid writing if nothing changed */


   if (c100_in_superuser_mode()) {
      status = c100_set_who(parameters.serial_nbr,parameters.sftwr_rev,
                            parameters.hrdwr_rev,parameters.unit_type,
                            parameters.cal_date);
      if (status != 0) {
         strcpy(err_msg,"Unable to update serial number.");
         goto ERROR;
      } /* if */
   } /* if */

   variation_onoff = (parameters.true_mag == TRUE_HEADING) ? 't' : 'm';
   status = c100_set_variation_onoff(variation_onoff);
   if (status != 0) {
      strcpy(err_msg,"Unable to update variation flag.");
      goto ERROR;
   } /* if */

   status = c100_set_variation(parameters.variation);
   if (status != 0) {
      strcpy(err_msg,"Unable to update variation.");
      goto ERROR;
   } /* if */

   status = c100_set_a_offset(parameters.a_offset);
   if (status != 0) {
      strcpy(err_msg,"Unable to update A offset.");
      goto ERROR;
   } /* if */

//--------
// setting the message rate to 1 before setting the baud rate will prevent
// problems caused by trying to set conflicting values
//--------
   status = c100_set_msg_rate(1);
   if (status != 0) {
      strcpy(err_msg,"Unable to update message rate.");
      goto ERROR;
   } /* if */

   status = c100_set_baud(parameters.baud_rate);
   if (status != 0) {
      strcpy(err_msg,"Unable to update baud rate.");
      goto ERROR;
   } /* if */

   status = c100_set_msg_rate(parameters.msg_rate);
   if (status != 0) {
      strcpy(err_msg,"Unable to update message rate.");
      goto ERROR;
   } /* if */

   status = c100_set_msg_type(parameters.msg_type);
   if (status != 0) {
      strcpy(err_msg,"Unable to update message type.");
      goto ERROR;
   } /* if */

   status = c100_set_digital_type(parameters.digital_type);
   if (status != 0) {
      strcpy(err_msg,"Unable to update digital output type.");
      goto ERROR;
   } /* if */

   status = c100_set_digital_fmt(parameters.digital_fmt);
   if (status != 0) {
      strcpy(err_msg,"Unable to update digital output format.");
      goto ERROR;
   } /* if */

   status = c100_set_analog_fmt(parameters.analog_fmt);
   if (status != 0) {
      strcpy(err_msg,"Unable to update analog output format.");
      goto ERROR;
   } /* if */

   status = c100_set_damp_type(parameters.damping_type);
   if (status != 0) {
      strcpy(err_msg,"Unable to update damping type.");
      goto ERROR;
   } /* if */

   status = c100_set_damp_rate(parameters.damping_rate);
   if (status != 0) {
      strcpy(err_msg,"Unable to update damping rate.");
      goto ERROR;
   } /* if */

   status = c100_set_msg_units(parameters.msg_units);
   if (status != 0) {
      strcpy(err_msg,"Unable to update analog output format.");
      goto ERROR;
   } /* if */

   if (parameters.powerup_mode == SENDING) {
      status = c100_send_data();
      if (status != 0) {
         strcpy(err_msg,"Unable to turn data messages on.");
      } /* if */
   } /* if */

   c100_zap();                          /* reboot to effect changes */


ERROR:
   if (parameters.powerup_mode == SENDING)
      c100_send_data();

   wn_dn(wnp);
   wn_free(wnp);

   if (status != 0) {                   /* was there an error? */
      strcat(err_msg," ");
      strcat(err_msg,c100_error_name(status));
      msg_window(err_msg);
   } /* if */

END:
   c100_close_channel();
   return(TRUE);

} /* parameters_endfp() */




/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function is called by the event handler whenever the CTRL-S key is    *
 * pressed while the user is editing parameters. It is installed into the     *
 * data form keytable (FMKTP). It allows the user to store common parameters  *
 * and then recall them later.                                                *
 *----------------------------------------------------------------------------*/
/* */
static int save_defaults(FORMPTR fmp)
{
FILE            *dflt_file;


   fm_convert(fmp);                             /* force data structure update */

   dflt_prm = parameters;                       /* copy parms to save area */

   dflt_file = fopen("C100.CFG","wb");          /* copy parms to disk */
   if (dflt_file != NULL) {
      fwrite(&parameters,sizeof(PARAMETERS),1,dflt_file);
      fclose(dflt_file);
   } /* if */

   msg_window("Default parameters saved.");     /* tell user what we did */
   return(TRUE);                                /* return to form */

} /* save_defaults() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function is called by the event handler whenever the CTRL-D key is    *
 * pressed while the user is editing parameters. It is installed into the     *
 * data form keytable (FMKTP). It allows the user to recall common parameters *
 * previously saved by 'save_defaults()'.                                     *
 *----------------------------------------------------------------------------*/
/* */
static int restore_defaults(FORMPTR fmp)
{
FILE            *dflt_file;


   if (strcmp(dflt_prm.unit_type,"C100") != 0) {/* is nothing in save area? */

      dflt_file = fopen("C100.CFG","rb");
      if (dflt_file == NULL) {                  /* is nothing saved on disk? */
         msg_window("No defaults saved yet!");  /* tell user so */
         return(TRUE);                          /* return to form editor */
      } /* if */

      fread(&dflt_prm,sizeof(PARAMETERS),1,
            dflt_file);                         /* load parms off disk */
      fclose(dflt_file);
   } /* if */

   parameters.true_mag     = dflt_prm.true_mag;
   parameters.variation    = dflt_prm.variation;
   parameters.a_offset     = dflt_prm.a_offset;
   parameters.powerup_mode = dflt_prm.powerup_mode;
   parameters.baud_rate    = dflt_prm.baud_rate;
   parameters.msg_rate     = dflt_prm.msg_rate;
   parameters.msg_type     = dflt_prm.msg_type;
   parameters.msg_units    = dflt_prm.msg_units;
   parameters.digital_type = dflt_prm.digital_type;
   parameters.digital_fmt  = dflt_prm.digital_fmt;
   parameters.analog_fmt   = dflt_prm.analog_fmt;
   parameters.damping_type = dflt_prm.damping_type;
   parameters.damping_rate = dflt_prm.damping_rate;

   fm_updflds(fmp);                             /* update screen display */
   fmp->flags |= FMALTERED;                     /* force ALTERED flag on */

   msg_window("Default parameters restored.");  /* tell user what we did */

   return(TRUE);                                /* return to form editor */

} /* restore_defaults() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
static DFORMPTR create_parameters_form(void)
{
DFORMPTR    fmp;
DFIELDPTR   fldp;
int         int_min,int_max;

   clist_mfp = mf_def(60,40);
   mf_rd("CLIST.TXT",clist_mfp);

   fmp = fm_def(CENTER_WN,CENTER_WN, 22, 52, LREVERSE, BDR_NULLP);
   sw_bdratt(LREVERSE, fmp->wnp);
   sw_shad(TRANSPARENT,LSHADOW,BOTTOMRIGHT,fmp->wnp);
   sw_updnfp(wn_expset,wn_expunset,fmp->wnp);
   sw_title("[C100 Parameter Editor]", LREVERSE, TOPLEFT, fmp->wnp);
   sfm_opt(CURSORCROSS|CURSORFREE|AUTOEXIT|AUTOMOVE|VERIFYEXIT,OFF,fmp);
   sfm_opt(FMWRAP|VERIFYQUIT,ON,fmp);
   sw_opt(MOU_RESIZE | MOU_DRAG | MOU_MODQUIT, OFF, fmp->wnp);
   sw_opt(POPUP | MOU_MODAL, ON, fmp->wnp);

   sfm_begfp(parameters_beginfp, fmp);
   sfm_endfp(parameters_endfp, fmp);

   bg_linedef(0 ,29,18,DOWN  | X_LINE | X_LINEBDR, LREVERSE, LINE_SLNP,fmp);
   bg_linedef(6 ,29,23,RIGHT | X_LINE | X_LINEBDR, LREVERSE, LINE_SLNP,fmp);
   bg_linedef(7 , 0,30,RIGHT | X_LINE | X_LINEBDR, LREVERSE, LINE_SLNP,fmp);
   bg_linedef(10,29,23,RIGHT | X_LINE | X_LINEBDR, LREVERSE, LINE_SLNP,fmp);
   bg_linedef(12, 0,30,RIGHT | X_LINE | X_LINEBDR, LREVERSE, LINE_SLNP,fmp);
   bg_linedef(13,29,23,RIGHT | X_LINE | X_LINEBDR, LREVERSE, LINE_SLNP,fmp);
   bg_linedef(17, 0,52,RIGHT | X_LINE | X_LINEBDR, LREVERSE, LINE_SLNP,fmp);
   bg_boxdef(0 , 0,22, 52, LREVERSE, BDR_SLNP, fmp);
   bg_txtdef(0 , 2,"[General]", LREVERSE, fmp);
   bg_txtdef(0 ,31,"[Serial]", LREVERSE, fmp);
   bg_txtdef(6 ,31,"[Digital]", LREVERSE, fmp);
   bg_txtdef(7 , 2,"[Variation/Declination]", LREVERSE, fmp);
   bg_txtdef(10,31,"[Analog]", LREVERSE, fmp);
   bg_txtdef(12, 2,"[Power-Up Mode]", LREVERSE, fmp);
   bg_txtdef(13,31,"[Damping]", LREVERSE, fmp);
   bg_txtdef(18, 2,"ESC", LNORMAL, fmp);
   bg_txtdef(18, 5,"-Quit without update.", LMNUNAVAIL, fmp);
   bg_txtdef(18,28,"F2", LNORMAL, fmp);
   bg_txtdef(18,30,"-Display choice list", LMNUNAVAIL, fmp);
   bg_txtdef(19, 2,"F10", LNORMAL, fmp);
   bg_txtdef(19, 5,"-Update and exit.", LMNUNAVAIL, fmp);
   bg_txtdef(19,31,"(if any).", LMNUNAVAIL, fmp);
   bg_txtdef(20, 3,"^S", LNORMAL, fmp);
   bg_txtdef(20, 5,"-Set Defaults.", LFLDSKIP, fmp);
   bg_txtdef(20,28,"^D", LNORMAL, fmp);
   bg_txtdef(20,30,"-Restore Defaults", LFLDSKIP, fmp);

   fldp = fld_def(1,5,"Serial Number:",20,"XXXXXXX",F_STRING,parameters.serial_nbr,fmp);
   i_namset("serial_nbr",fldp);
   sf_msg("C100 Serial number.",fldp);

   fldp = fld_def(2,9,"Unit Type:",20,"XXXX",F_STRING,parameters.unit_type,fmp);
   i_namset("unit_type",fldp);
   sf_msg("Unit type - should always be 'C100'.  Cannot be altered.", fldp);
   sf_opt(PROTECTED,ON,fldp);

   fldp = fld_def(3,2,"Calibration Date:",20,"99/99/99",F_DATE,parameters.cal_date,fmp);
   i_namset("cal_date",fldp);
   sf_msg("Date of factory calibration. Date format is YY/MM/DD.", fldp);

   fldp = fld_def(4,6,"Hardware Rev:",20,"!",F_CHAR,&parameters.hrdwr_rev,fmp);
   i_namset("hardware_rev",fldp);
   sf_msg("Hardware revision level.", fldp);

   fldp = fld_def(5,6,"Software Rev:",20,"!",F_CHAR,&parameters.sftwr_rev,fmp);
   i_namset("software_rev",fldp);
   sf_msg("Software revision level.  Cannot be altered", fldp);
   sf_opt(PROTECTED,ON,fldp);

   fldp = fld_def(8,6,"Heading Type:",20,"X",F_BOOL,&parameters.true_mag,fmp);
   i_namset("true_mag", fldp);
   sf_msg("Press <SPACE> to toggle TRUE/MAGNETIC.", fldp);
   sf_booltoggle("MAGNETIC","TRUE",LFLDINACT,LFLDINACT,fldp);

   fldp = fld_def(9,9,"Variation:",20,"#####@#",F_DOUBLE,&parameters.variation,fmp);
   i_namset("variation",fldp);
   sf_msg("Enter variation (declination) - range ñ359.9ø or ñ6399 mils.", fldp);
   sf_opt(RTADJUST | CLEARONKEY, ON, fldp);
   sf_endfp(ValidateVariation,fldp);

   fldp = fld_def(10,8,"'A' offset:",20,"#####@#",F_DOUBLE,&parameters.a_offset,fmp);
   i_namset("a_offset", fldp);
   sf_msg("Enter angle between TRUE and COMPASS forward - range ñ359.9ø or ñ6399 mils.", fldp);
   sf_opt(RTADJUST | CLEARONKEY, ON, fldp);
   sf_endfp(ValidateAOffset,fldp);

   fldp = fld_def(13,3,"Powerup Mode:",17,"X",F_BOOL,&parameters.powerup_mode,fmp);
   i_namset("powerup_mode", fldp);
   sf_msg("Press <SPACE> to toggle.", fldp);
   sf_booltoggle("SENDING","NOT SENDING",LFLDINACT, LFLDINACT, fldp);

   fldp = fld_def(1,35,"Baud Rate:",46,"9999",F_INT,&parameters.baud_rate,fmp);
   i_namset("baud_rate", fldp);
   sf_endfp(check_baud_msg_rate,fldp);
   sf_msg("Press <F2> to select serial baud rate from choice list.", fldp);
   sf_opt(RTADJUST | CLEARONKEY | PROTECTED , ON, fldp);
   sf_clist("*BAUD_RATE",0,clist_mfp,fldp);

   fldp = fld_def(2,32,"Message Rate:",46,"999",F_INT,&parameters.msg_rate,fmp);
   i_namset("msg_rate", fldp);
   sf_endfp(check_baud_msg_rate,fldp);
   sf_msg("Press <F2> to select message rate from choice list, or enter it directly.",
          fldp);
   sf_opt(RTADJUST | CLEARONKEY , ON, fldp);
   int_min = 0;
   int_max = 600;
   sf_range(&int_min,&int_max,fldp);
   sf_clist("*MESSAGE_RATE",0,clist_mfp,fldp);

   fldp = fld_def(3,32,"Message Type:",46,"9",F_CHAR,&parameters.msg_type,fmp);
   i_namset("msg_type", fldp);
   sf_msg("Press <F2> to select serial message type from choice list.", fldp);
   sf_opt(PROTECTED , ON, fldp);
   sf_clist("*MESSAGE_TYPE",0,clist_mfp,fldp);

   fldp = fld_def(4,31,"Message Units:",46,"9",F_CHAR,&parameters.msg_units,fmp);
   i_namset("msg_units", fldp);
   sf_msg("Press <F2> to select serial message units from choice list.", fldp);
   sf_opt(PROTECTED , ON, fldp);
   sf_clist("*MESSAGE_UNITS",0,clist_mfp,fldp);

   fldp = fld_def(7,33,"Output Type:",46,"9",F_CHAR,&parameters.digital_type,fmp);
   i_namset("digital_type", fldp);
   sf_msg("Press <F2> to select digital output type from choice list.", fldp);
   sf_opt(PROTECTED , ON, fldp);
   sf_clist("*DIGITAL_TYPE",0,clist_mfp,fldp);

   fldp = fld_def(8,31,"Output Format:",46,"9",F_CHAR,&parameters.digital_fmt,fmp);
   i_namset("digital_fmt", fldp);
   sf_msg("Press <F2> to select digital output format from choice list.", fldp);
   sf_opt(PROTECTED , ON, fldp);
   sf_clist("*DIGITAL_FORMAT",0,clist_mfp,fldp);

   fldp = fld_def(11,31,"Output Format:",46,"9",F_CHAR,&parameters.analog_fmt,fmp);
   i_namset("analog_fmt", fldp);
   sf_msg("Press <F2> to select analog output format from choice list.", fldp);
   sf_opt(PROTECTED , ON, fldp);
   sf_clist("*ANALOG_FORMAT",0,clist_mfp,fldp);

   fldp = fld_def(14,40,"Type:",46,"X",F_CHAR,&parameters.damping_type,fmp);
   i_namset("damping_type", fldp);
   sf_msg("Press <F2> to select damping type from choice list.", fldp);
   sf_opt(PROTECTED , ON, fldp);
   sf_clist("*DAMPING_TYPE", 0, clist_mfp, fldp);

   fldp = fld_def(15,40,"Rate:",46,"X",F_CHAR,&parameters.damping_rate,fmp);
   i_namset("damping_rate", fldp);
   sf_msg("Press <F2> to select damping rate from choice list.", fldp);
   sf_opt(PROTECTED , ON, fldp);
   sf_clist("*DAMPING_RATE", 0, clist_mfp, fldp);


   return(fmp);

} /* create_parameters_form() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function is an 'end-field' function, invoked by Vermont Views when    *
 * the user leaves the baud or message rate fields.  It checks to see if the  *
 * selected baud and message rates are compatible.                            *
 *----------------------------------------------------------------------------*/
/* */
static int check_baud_msg_rate(FORMPTR fmp)
{
DFIELDPTR       baud_fldp,msg_fldp;
int             baud_rate,msg_rate;


   baud_fldp = i_namptr("baud_rate",fmp);
   fld_get(&baud_rate,baud_fldp,fmp);

   msg_fldp = i_namptr("msg_rate",fmp);
   fld_get(&msg_rate,msg_fldp,fmp);

   if (curdfldp(fmp) == baud_fldp) {
      if (((baud_rate <= 600) && ( msg_rate > 120)) ||
          ((baud_rate <= 1200) && (msg_rate > 300)))
      {
         msg_window("This baud rate too slow for current message rate.");
         sfm_nextitem(AC_SAMEITEM,fmp);
      } /* if */
   } else {
      if (((baud_rate <= 600) && ( msg_rate > 120)) ||
          ((baud_rate <= 1200) && (msg_rate > 300)))
      {
         msg_window("This message rate too high for current baud rate.");
         sfm_nextitem(AC_SAMEITEM,fmp);
      } /* if */
   } /* if */

   return(TRUE);

} /* check_baud_msg_rate() */




/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function is an 'end-field' function, invoked by Vermont Views when    *
 * the user leaves the variation field.  It checks to see if the entered      *
 * variation is compatible with the current message units.                    *
 *----------------------------------------------------------------------------*/
/* VV end-field function to validate variation. */
static int ValidateVariation(FORMPTR fmp)
{

    if (fm_nextitem(fmp) == AC_QUIT)
        return(TRUE);

    fm_convert(fmp);

    switch (parameters.msg_units) {

        case DEGREES:
            if ((parameters.variation < -359.9) ||
                (parameters.variation > +359.9))
            {
                sfm_nextitem(AC_SAMEITEM,fmp);
                bell_vv();
            } /* if */
            break;


        case MILS:
            if ((parameters.variation < -6399.0) ||
                (parameters.variation > +6399.0))
            {
                sfm_nextitem(AC_SAMEITEM,fmp);
                bell_vv();
            } /* if */
            break;


        default:
            break;

    } /* switch */

    return(TRUE);

} /* ValidateVariation() */




/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function is an 'end-field' function, invoked by Vermont Views when    *
 * the user leaves the a-offset field.  It checks to see if the entered       *
 * a-offset is compatible with the current message units.                     *
 *----------------------------------------------------------------------------*/
/* VV end-field function to validate A-offset. */
static int ValidateAOffset(FORMPTR fmp)
{

    if (fm_nextitem(fmp) == AC_QUIT)
        return(TRUE);

    fm_convert(fmp);

    switch (parameters.msg_units) {

        case DEGREES:
            if ((parameters.a_offset < -359.9) ||
                (parameters.a_offset > +359.9))
            {
                sfm_nextitem(AC_SAMEITEM,fmp);
                bell_vv();
            } /* if */
            break;


        case MILS:
            if ((parameters.a_offset < -6399.0) ||
                (parameters.a_offset > +6399.0))
            {
                sfm_nextitem(AC_SAMEITEM,fmp);
                bell_vv();
            } /* if */
            break;


        default:
            break;

    } /* switch */

    return(TRUE);

} /* ValidateAOffset() */
