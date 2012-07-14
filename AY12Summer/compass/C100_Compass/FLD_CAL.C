/* $Header:   F:/c100_pc/user/vcs/fld_cal.c_v   1.4   06 May 1993 10:21:34   DavidH  $ */
/* $Revision:   1.4  $ */
/*
 * $Log:   F:/c100_pc/user/vcs/fld_cal.c_v  $
 * 
 *    Rev 1.4   06 May 1993 10:21:34   DavidH
 * Added delay between end-of-cal and reading the cal score for baud rates 1200 
 * and below.
 * 
 *    Rev 1.3   05 May 1993 15:04:28   DavidH
 * Added code to implement a 3 second delay between end of cal and reading the 
 * cal score when using 300 baud.
 * 
 *    Rev 1.2   08 Apr 1992 14:48:10   DavidH
 * Modified code in response to Laser Tech's comments.
 * 
 *    Rev 1.1   30 Mar 1992 10:51:30   DavidH
 * 
 *    Rev 1.0   12 Mar 1992 14:59:30   DavidH
 * Initial revision.
 */


#include <dos.h>
#include <vv_form.h>
#include "c100.h"
#include "c100usr.h"

#pragma warn -par

#define L_BLINK LAVR1
#define L_ERROR LAVR2



/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function performs a field calibration on the C100 board and displays  *
 * the status of the cal once it is completed.                                *
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int eight_point_cal(FORMPTR fmp)
{
WINDOWPTR   wnp;
int         status,done;
int         score,count,environment;
int         space_pressed;
double      heading;
char        cal_msg[81];
char        serial_nbr[8],unit_type[5],cal_date[9];
char        hardware_rev,software_rev;


    latt_rpl(L_BLINK,NORMAL|BLINK,BLUE|BLINK,WHITE,LATT_SYS);
    latt_rpl(L_ERROR,REVERSE,     RED|BLINK, WHITE,LATT_SYS);

    wnp = wn_def(CENTER_WN,CENTER_WN,12,60,LREVERSE,BDR_DLNP);
    sw_shad(TRANSPARENT,LSHADOW,BOTTOMRIGHT,wnp);
    sw_updnfp(wn_expset,wn_expunset,wnp);

    wn_up(wnp);

    status = c100_open_channel(dflt_port,dflt_baud);
    if (status != 0) {
        v_printf(wnp,"Error: unable to open compass channel - %s",
                     c100_error_name(status));
        goto END;
    } /* if */

    status = c100_halt_data();
    if (status != 0) {
        v_printf(wnp,"Error: unable to halt data messages - %s",
                     c100_error_name(status));
        goto END;
    } /* if */

    status = c100_get_who(serial_nbr,&software_rev,&hardware_rev,
                                      unit_type,cal_date);
    if (status != 0) {
        v_printf(wnp,"Error: unable to read software revision level.  %s",
                 c100_error_name(status));
        goto END;
    } /* if */


    /*--- Start 8-point Calibration --*/
    sw_title("[Press SPACE when done...]",L_BLINK,BOTTOMCENTER,wnp);
    v_bdr(BDR_DLNP,wnp);
    v_st("Eight-Point Calibration started!\n",wnp);

   done = FALSE;
   do {

      status = c100_cal_mode(FORCED_EIGHT_POINT_CAL,&heading,cal_msg);
      switch (status) {
         case OK:
            v_printf(wnp,"\nPosition compass to ñ%.0fø...",heading);
            break;

         case DONE:
            v_st("\n\n8-point calibration complete!",wnp);

            if (dflt_baud <= 1200)
                delay(3000);

            if (software_rev >= 'C') {
               status = c100_get_cal_score(LONG_SCORE,&score,&count,
                                           &environment);
               if (status != 0) {
                  v_printf(wnp,"\nError: unable to read cal score - %s",
                           c100_error_name(status));
               } else {
                  v_printf(wnp,"\nCal score = %d, environment = %d, "
                           "count = %d.",score,environment,count);
                  if (environment < 5)
                     v_st("\nBad magnetic environment.  Consider re-"
                          "locating and re-calibrating the compass.",wnp);
               } /* if */
            } else {
               status = c100_get_cal_score(SHORT_SCORE,&score,&count,NULL);
               if (status != 0) {
                  v_printf(wnp,"\nError: unable to read cal score - %s",
                           c100_error_name(status));
               } else {
                  v_printf(wnp,"\nCal score = %d, count = %d.",
                           score,count);
               } /* if */
            } /* if */

            if (score < 7)
               v_statt("\nThis calibration may NOT be accurate.",
                       L_ERROR,wnp);
            done = TRUE;
            break;

         case MSG_NAKKED:
            v_printf(wnp,"\n\nError: %s",cal_msg);
            if (software_rev < 'C')
               c100_zap();
            else
               c100_cal_mode(ABORT_CAL,NULL,NULL);
            done = TRUE;
            break;

         default:
            v_printf(wnp,"\n\nError: unable to complete calibration - %s",
                     c100_error_name(status));
            if (software_rev < 'C')
               c100_zap();
            else
               c100_cal_mode(ABORT_CAL,NULL,NULL);
            done = TRUE;
            break;

      } /* switch */

      if (!done) {
         space_pressed = FALSE;
         do {
            switch (ki()) {
               case KEY_ESC:
                  if (continue_abort("ESC pressed!") == ABORT) {
                     if (software_rev < 'C')
                        c100_zap();
                     else
                        c100_cal_mode(ABORT_CAL,NULL,NULL);
                     v_st("\n8-point calibration aborted by user!",wnp);
                     goto END;
                  } /* if */
                  break;

               case KEY_SPACE:
                  space_pressed = TRUE;
                  break;

               default:
                  break;

            } /* switch */
         } while (!space_pressed);
      } /* if */

    } while (!done);


END:
//  if (c100_mode == SENDING) {
//     c100_send_data();
//  } /* if */

    sw_title("[Press any key...]",L_BLINK,BOTTOMCENTER,wnp);
    v_bdr(BDR_DLNP,wnp);
    beep_vv(BPMEDIUM,BPLOW);
    beep_vv(BPMEDIUM,BPMIDDLE);
    beep_vv(BPMEDIUM,BPHIGH);
    flush_keybuf();
    ki();

    c100_close_channel();
    wn_dn(wnp);
    wn_free(wnp);

    return(SAMELEVEL);

} /* eight_point_cal() */


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
int three_point_cal(FORMPTR fmp)
{
WINDOWPTR       wnp;
DFORMPTR        dfmp;
DFIELDPTR       fldp;
int             status,done;
int             score,count;
double          heading;
char            cal_msg[81];
char            serial_nbr[8],unit_type[5],cal_date[9];
char            hardware_rev,software_rev;


   latt_rpl(L_BLINK,NORMAL|BLINK,BLUE|BLINK,WHITE,LATT_SYS);
   latt_rpl(L_ERROR,REVERSE,     RED|BLINK, WHITE,LATT_SYS);

   wnp = wn_def(CENTER_WN,CENTER_WN,12,60,LREVERSE,BDR_DLNP);
   sw_shad(TRANSPARENT,LSHADOW,BOTTOMRIGHT,wnp);
   sw_updnfp(wn_expset,wn_expunset,wnp);

   wn_up(wnp);

   dfmp = fm_def(12,CENTER_WN,3,25,LNORMAL,BDR_DLNP);
   sw_shad(TRANSPARENT,LSHADOW,BOTTOMRIGHT,dfmp->wnp);
   sw_updnfp(wn_expset,wn_expunset,dfmp->wnp);
   fldp = fld_def(0,1,"Actual Heading: ",FADJACENT,"###@#",
                  F_DOUBLE,&heading,dfmp);
   sf_msg("Enter actual compass heading and press ENTER.",fldp);

   status = c100_open_channel(dflt_port,dflt_baud);
   if (status != 0) {
      v_printf(wnp,"Error: unable to open compass channel - %s",
               c100_error_name(status));
      goto END;
   } /* if */

   status = c100_halt_data();
   if (status != 0) {
      v_printf(wnp,"Error: unable to halt data messages - %s",
               c100_error_name(status));
      goto END;
   } /* if */

   status = c100_get_who(serial_nbr,&software_rev,&hardware_rev,
                         unit_type,cal_date);
   if (status != 0) {
      v_printf(wnp,"Error: unable to read software revision level.  %s",
               c100_error_name(status));
      goto END;
   } /* if */
   if (software_rev < 'C') {
      v_st("\nThree Point Cal requires software revision C or higher.",wnp);
      v_printf(wnp,"\nThis C100 has rev %c software.",software_rev);
      v_st("\n\nCalibration aborted.",wnp);
      goto END;
   } /* if */

   v_st("Three-Point Calibration started.\n",wnp);

   done    = FALSE;
   heading = 0.0;

   while (!done) {
      status = c100_cal_mode(THREE_POINT_CAL,&heading,cal_msg);

      switch (status) {
         case OK:
            v_printf(wnp,"\nPosition compass to about %.0fø and enter actual"
                     " heading.",heading);
            if (fm_proc(0,dfmp) != AC_EXIT) {
               c100_cal_mode(ABORT_CAL,NULL,NULL);
               v_st("\n\nCalibration aborted by user.",wnp);
               done = TRUE;
            } /* if */
            break;


         case DONE:
            v_st("\n\nThree-Point calibration complete!",wnp);
            status = c100_get_cal_score(SHORT_SCORE,&score,&count,NULL);
            if (status != 0)
               v_printf(wnp,"\nError: unable to read cal score - %s",
                        c100_error_name(status));
            else
               v_printf(wnp,"\nCal score = %d, count = %d.",
                        score,count);
            done = TRUE;
            break;


         case MSG_NAKKED:
            v_printf(wnp,"\n\nError: %s",cal_msg);
            c100_cal_mode(ABORT_CAL,NULL,NULL);
            done = TRUE;
            break;


         default:
            v_printf(wnp,"\n\nError: unable to complete calibration - %s",
                     c100_error_name(status));
            c100_cal_mode(ABORT_CAL,NULL,NULL);
            done = TRUE;
            break;

      } /* switch */

   } /* while */


END:
   if (c100_mode == SENDING) {
      c100_send_data();
   } /* if */

   sw_title("[Press any key...]",L_BLINK,BOTTOMCENTER,wnp);
   v_bdr(BDR_DLNP,wnp);
   beep_vv(BPMEDIUM,BPLOW);
   beep_vv(BPMEDIUM,BPMIDDLE);
   beep_vv(BPMEDIUM,BPHIGH);
   flush_keybuf();
   ki();

   c100_close_channel();
   fm_free(dfmp);
   wn_dn(wnp);
   wn_free(wnp);

   return(SAMELEVEL);


} /* three_point_cal() */


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#define COLLECTING      0
#define CALCULATING     1
int circular_cal(FORMPTR fmp)
{
WINDOWPTR       wnp;
int             status,done;
int             score,count,environment;
int             row,col;
int             state;
char            cal_msg[81];
char            serial_nbr[8],unit_type[5],cal_date[9];
char            hardware_rev,software_rev;


   latt_rpl(L_BLINK,NORMAL|BLINK,BLUE|BLINK,WHITE,LATT_SYS);
   latt_rpl(L_ERROR,REVERSE,     RED|BLINK, WHITE,LATT_SYS);

   wnp = wn_def(CENTER_WN,CENTER_WN,12,60,LREVERSE,BDR_DLNP);
   sw_shad(TRANSPARENT,LSHADOW,BOTTOMRIGHT,wnp);
   sw_updnfp(wn_expset,wn_expunset,wnp);

   wn_up(wnp);

   status = c100_open_channel(dflt_port,dflt_baud);
   if (status != 0) {
      v_printf(wnp,"Error: unable to open compass channel - %s",
               c100_error_name(status));
      goto END;
   } /* if */

   status = c100_halt_data();
   if (status != 0) {
      v_printf(wnp,"Error: unable to halt data messages - %s",
               c100_error_name(status));
      goto END;
   } /* if */

   status = c100_get_who(serial_nbr,&software_rev,&hardware_rev,
                         unit_type,cal_date);
   if (status != 0) {
      v_printf(wnp,"Error: unable to read software revision level.  %s",
               c100_error_name(status));
      goto END;
   } /* if */
   if (software_rev < 'C') {
      v_st("\nCircular Cal requires software revision C or higher.",wnp);
      v_printf(wnp,"\nThis C100 has rev %c software.",software_rev);
      v_st("\n\nCalibration aborted.",wnp);
      goto END;
   } /* if */



//-----------
//   Start Circular Calibration
//-----------
   status = c100_cal_mode(CIRCULAR_CAL,NULL,cal_msg);
   if (status != 0) {
      v_printf(wnp,"Error: unable to start circular cal. %s.",
               c100_error_name(status));
      goto END;
   } /* if */

   v_st("Circular calibration started.\n\n",wnp);
   v_st("Turn compass SLOWLY in a circle...",wnp);
   row = wnp->r;
   col = wnp->c;
   v_statt("Collecting",L_BLINK,wnp);
   csr_mvwn(row,col,wnp);

   state = COLLECTING;
   done = FALSE;
   do {

      if (ki_chk()) {
         if (ki() == KEY_ESC) {
            if (continue_abort("ESC pressed!") == ABORT) {
               v_st("ABORTED    ",wnp);
               c100_cal_mode(ABORT_CAL,NULL,NULL);
               v_st("\nCircular calibration aborted by user!",wnp);
               goto END;
            } /* if */
         } /* if */
      } /* if */

      status = c100_check_for_ack(cal_msg);
      switch (status) {
         case ACK_RECEIVED:
            if (state == COLLECTING) {
               state = CALCULATING;
               v_statt("Calculating",L_BLINK,wnp);
               csr_mvwn(row,col,wnp);
            } else {
               v_st("Finished    ",wnp);
               v_st("\n\nCircular calibration complete!",wnp);
               if (software_rev >= 'C') {
                  if (dflt_baud <= 1200)
                     delay(3000);

                  status = c100_get_cal_score(LONG_SCORE,&score,&count,
                                              &environment);
                  if (status != 0) {
                     v_printf(wnp,"\nError: unable to read cal score - %s",
                              c100_error_name(status));
                  } else {
                     v_printf(wnp,"\nCal score = %d, environment = %d, "
                              "count = %d.",score,environment,count);
                     if (environment < 5)
                        v_st("\nBad magnetic environment.  Consider re-"
                             "locating and re-calibrating the compass.",wnp);
                  } /* if */

               } else {

                  status = c100_get_cal_score(SHORT_SCORE,&score,&count,NULL);
                  if (status != 0) {
                     v_printf(wnp,"\nError: unable to read cal score - %s",
                              c100_error_name(status));
                  } else {
                     v_printf(wnp,"\nCal score = %d, count = %d.",
                              score,count);
                  } /* if */
               } /* if */

               if (score < 7)
                  v_statt("\nThis calibration may NOT be accurate!",
                          L_ERROR,wnp);
               done = TRUE;
            } /* if */
            break;


         case NAK_RECEIVED:
            v_st("ABORTED    ",wnp);
            v_printf(wnp,"\nError: unable to complete calibration - %s",
                     cal_msg);
            c100_cal_mode(ABORT_CAL,NULL,NULL);
            done = TRUE;
            break;


         default:
            break;

      } /* switch */

   } while (!done);


END:
   if (c100_mode == SENDING) {
      c100_send_data();
   } /* if */

   sw_title("[Press any key...]",L_BLINK,BOTTOMCENTER,wnp);
   v_bdr(BDR_DLNP,wnp);
   beep_vv(BPMEDIUM,BPLOW);
   beep_vv(BPMEDIUM,BPMIDDLE);
   beep_vv(BPMEDIUM,BPHIGH);
   flush_keybuf();
   ki();

   c100_close_channel();
   wn_dn(wnp);
   wn_free(wnp);

   return(SAMELEVEL);

} /* circular_cal() */

