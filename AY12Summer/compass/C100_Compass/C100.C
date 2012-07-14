/* $Header:   F:/c100_pc/user/vcs/c100.c_v   1.2   10 May 1993 14:34:12   DavidH  $ */
/* $Revision:   1.2  $ */
/*
 * $Log:   F:/c100_pc/user/vcs/c100.c_v  $
 * 
 *    Rev 1.2   10 May 1993 14:34:12   DavidH
 * Changed debug code to append to rather than overwrite debug log file.
 * 
 *    Rev 1.1   05 May 1993 15:00:26   DavidH
 * Finally got rid of COMMSPEC.H - Hooray, hooray!
 *
 *    Rev 1.0   12 Mar 1992 15:03:28   DavidH
 * Initial revision.
 */

/*----------------------------------------------------------------------------*
 * C100.C -     This file contains all the routines that directly interface   *
 *          with the C100 compass board.                                      *
 *                                                                            *
 * Written by David R. Hoy                                                    *
 * Copyright (c) 1991,1992 - KVH Industries Inc, Middletown, RI.              *
 *                                                                            *
 * This code is the property of KVH Industries. You are hereby given perm-    *
 * ission to include this code in any of your own program, provided that you  *
 * include KVH's copyright notice somewhere in your code.                     *
 *----------------------------------------------------------------------------*/


//#define DEBUG

#include <string.h>                             /* standard 'C' header */
#include <stdio.h>                              /* standard 'C' header */
#include <stdlib.h>                             /* standard 'C' header */
#include <dos.h>                                /* standard 'C' header */
#include <asiports.h>                           /* Greenleaf Commlib header */
#include "c100.h"                               /* prototypes for functions in this file */


static char *c100_error_desc[] = {
   " Success.",
   " No ACK received.",
   " Malformed message.",
   " SUPERUSER mode required.",
   " Timeout error.",
   " No response from compass - please check connections.",
   " Parity error.",
   " Retry limit exceeded.",
   " Collision detected.",
   " NAK received.",
   " Invalid checksum.",
   " Not implemeted yet."
}; /* c100_error_desc[] */


#define HEXCHAR(x)      (((x) < 10)  ? '0'+(x) : 'A'+((x)-10))
#define HEXNUM(x)       (((x) < 'A') ? (x)-'0' : ((x)-'A')+10)

#define CARRIAGE_RETURN 0x0D
#define RESTART_LIMIT   3
#define SUPER_MSG_C_ACK '#'
#define MSG_C_ACK       '>'
#define MSG_C_NAK       '!'


/*------ global variables ------*/
int             c100_mode;                      /* sending/not sending flag */
static int      c100_superuser;                 /* superuser mode flag */
static int      ack_timeout = (TICKS_PER_SECOND / 2); /* 1/2 second wait for ack */
static int      retry_limit = 5;                /* ACK retry limit */
static int      port = -1;                      /* GreenLeaf port number */
static char     c100_nak_msg[80];
#ifdef DEBUG
    static FILE  *debug_file;
#endif


/*------ function prototypes ------*/
static int  send_command(char *command,char *response,unsigned size);
static int  verify_checksum(char *msg);
static void add_checksum(char *msg);



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
char *c100_error_name(int err_code)
{
static char tmp[20];


    if (err_code <= 0)                           /* Greenleaf error code? */
        return(CommErrorName(err_code));          /* return Greenleaf error msg */

    if (err_code == MSG_NAKKED)
        return(c100_nak_msg);

    if (err_code > (int)(sizeof(c100_error_desc) / sizeof(char *))) {
        sprintf(tmp,"Undefined error #%d.",err_code);
        return(tmp);
    } /* if */

    return(c100_error_desc[err_code]);

} /* c100_error_name() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
void add_checksum(char *msg)
{
unsigned char   checksum,nibble;
unsigned char   *ptr;


    ptr = msg;
    if (*ptr == '$')
        ptr++;

    checksum = 0;
    while (*ptr)
        checksum ^= *ptr++;               /* calculate the checksum */

    *ptr++ = '*';
    nibble = checksum >> 4;
    *ptr++ = HEXCHAR(nibble);
    nibble = checksum & 0x0F;
    *ptr++ = HEXCHAR(nibble);

    *ptr   = '\0';                       /* null terminate the string */

} /* add_checksum() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int verify_checksum(char *msg)
{
unsigned char   msg_checksum,calc_checksum;
unsigned char   *ptr;


    if (strchr(msg,'*') == NULL)         /* no checksum? */
        return(TRUE);                     /* return to caller */

    ptr = msg;
    if (*ptr == '$')                     /* skip over NMEA start character */
        ptr++;

    calc_checksum = 0;
    while (*ptr != '*')
        calc_checksum ^= *ptr++;

    *ptr++ = '\0';

    msg_checksum  = HEXNUM(*ptr) << 4;
    ptr++;
    msg_checksum += HEXNUM(*ptr);

    return(calc_checksum == msg_checksum);

} /* verify_checksum() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
static int send_command(char *command,char *response,unsigned size)
{
char    output[80],input[80];
int     done,ch,tries;
int     restart_cnt = 0;


    strcpy(output,command);

RESTART:
    if (restart_cnt > 0) {                      /* did we restart? */
        asputc(port,CARRIAGE_RETURN);           /* send a CR */
        delay(500);                             /* wait 1/2 second */
        asiclear(port,ASIN);                    /* flush receive buffer */
    } /* if */


    /*------ send 'command' out serial port ------*/
    asiclear(port,ASIN);
    asputs(port,output,CARRIAGE_RETURN);
#ifdef DEBUG
    fprintf(debug_file,"%s\n",output);
#endif


    /*------ wait for ack from compass ------*/
    done  = FALSE;                               /* clear loop exit flag */
    tries = 0;
    do {                                         /* start of loop */
        asigets_timed(port,input,sizeof(input),
                      CARRIAGE_RETURN,ack_timeout);

        if (_aserror < ASSUCCESS)
            ch = _aserror;
        else if (input[0] == 0x10)                /* throw away leading <LF> */
            ch = input[1];
        else
            ch = input[0];

        switch (ch) {
            case ASBUFREMPTY:
                if (restart_cnt >= RESTART_LIMIT) {
                    return(ACK_TIMEOUT);
                } /* if */
                restart_cnt++;
                goto RESTART;

            case SUPER_MSG_C_ACK:
            case MSG_C_ACK:
                c100_superuser = (ch == SUPER_MSG_C_ACK);
                done = TRUE;
                break;

            case MSG_C_NAK:
                asigets_timed(port,c100_nak_msg,sizeof(c100_nak_msg),
                              CARRIAGE_RETURN,TICKS_PER_SECOND);
                return(MSG_NAKKED);

            default:
                if (++tries > retry_limit) {
                    return(RETRY_LIMIT_EXCEEDED);
                } /* if */
                break;

        } /* switch */

    } while (!done);                             /* loop until exit flag is set */


    /*------ read response string, if required ------*/
    if ((response != NULL) && (size > 0)) {
        asigets_timed(port,response,size,CARRIAGE_RETURN,
                      TICKS_PER_SECOND * 2);
        if (_aserror != ASSUCCESS)
            return(_aserror);

#ifdef DEBUG
        fprintf(debug_file,"%s\n",response);
#endif

        if (!verify_checksum(response))
            return(INVALID_CHECKSUM);
    } /* if */

    return(0);

} /* send_command() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_check_for_ack(char *cal_msg)
{
int     retval,ch;
char    msg[81];


   ch = asigetc(port);

   switch (ch) {
      case ASBUFREMPTY:
         retval = NOTHING_YET;
         break;

      case MSG_C_ACK:
      case SUPER_MSG_C_ACK:
         asigetc(port);                         /* get & discard <CR> */
         asigets_timed(port,msg,sizeof(msg),CARRIAGE_RETURN,
                       TICKS_PER_SECOND);
         if (_aserror == ASSUCCESS)
            strcpy(cal_msg,msg);
         retval = ACK_RECEIVED;
         break;

      case MSG_C_NAK:
         asigetc(port);                         /* get & discard <CR> */
         asigets_timed(port,msg,sizeof(msg),CARRIAGE_RETURN,
                       TICKS_PER_SECOND);
         if (_aserror == ASSUCCESS)
            strcpy(cal_msg,msg);
         retval = NAK_RECEIVED;
         break;

      default:
         retval = ch;
         break;

   } /* switch */

   return(retval);

} /* c100_check_for_ack() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_open_channel(int com_port,long baud_rate)
{
int     status;


   if (port != -1)                              /* is channel already open? */
      return(0);                                /* return to caller */

   status = asiopen(com_port,ASIN|ASCII|NORMALRX,
                    1024,MIN_BUF_SIZE,baud_rate,
                    P_NONE,1,8,OFF,OFF);
   if (status != ASSUCCESS)                     /* channel not opened? */
      return(status);                           /* return error code */
   port = com_port;

#ifdef DEBUG
   debug_file = fopen("DEBUG.TXT","at");
#endif

   /*------ determine current compass mode ------*/
   delay(1000);                                 /* wait 1 second */
   c100_mode = (isrxempty(port) == TRUE)? NOT_SENDING : SENDING;

   return(0);                                   /* return to caller */

} /* c100_open_channel() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_close_channel(void)
{
int     status;


   status = asiquit(port);                      /* shut down RS-232 channel */
   if (status != ASSUCCESS)                     /* channel not closed? */
      return(status);                           /* return error code */

#ifdef DEBUG
   fclose(debug_file);
#endif

   port = -1;                                   /* set an invalid port number */
   return(0);                                   /* return to caller */

} /* c100_close_channel() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function tells the compass to stop sending continuous data messages.  *
 * It accomplishes the opposite of 'send_data()'.                             *
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_halt_data(void)
{
int     status;

   status = send_command("h",NULL,0);
   return(status);                              /* return to caller */

} /* c100_halt_data() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function tells the compass to start sending continuous data messages  *
 * using the default message type. It accomplishes the opposite of a call to  *
 * 'halt_data()'.                                                             *
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/* */
int c100_send_data(void)
{
int     status;

   status = send_command("s",NULL,0);
   return(status);                              /* return to caller */

} /* c100_send_data() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function retrieves the serial number, hardware version number, soft-  *
 * ware version number, unit type and date of factory calibration from the    *
 * compass board. The unit_type should ALWAYS be 'C100'. If not, something is *
 * wrong!                                                                     *
 *                                                                            *
 *       In: serial_nbr - pointer to location to store serial number          *
 *           sft_nbr    - pointer to location to store software version nbr   *
 *           hrd_nbr    - pointer to location to store hardware version nbr   *
 *           unit_type  - pointer to location to store unit type              *
 *           cal_date   - pointer to location to store factory cal date       *
 *                                                                            *
 *      Out: 0        - Success, all parameters updated.                      *
 *           BAD_MSG  - Received message is invalid                           *
 *           Other    - Error code returned by Greenleaf functions            *
 *                                                                            *
 * Cautions: Pointer arguments must point to valid locations of correct size, *
 *           or memory overwrites may occur.                                  *
 *----------------------------------------------------------------------------*/
/* */
int c100_get_who(char *serial_nbr,char *software_rev,char *hardware_rev,
                 char *unit_type, char *cal_date)
{
char    msg[80],*ptr;
char    *token;
int     status;

   status = send_command("?w",msg,sizeof(msg));
   if (status != 0)                             /* no ACK received? */
      return(status);                           /* return error code */

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?w") != 0))
      return(BAD_MSG);

   token = strtok(NULL,", ");
   strncpy(serial_nbr,token,7);
   serial_nbr[7] = '\0';

   token = strtok(NULL,", ");
   *software_rev = *token;

   token = strtok(NULL,", ");
   *hardware_rev = *token;

   token = strtok(NULL,", ");
   strncpy(unit_type,token,4);
   unit_type[4] = '\0';

   token = strtok(NULL,", ");
   strncpy(cal_date,token,8);
   cal_date[8] = '\0';

   return(0);                                   /* return success code */

} /* c100_get_who() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_who(char *serial_nbr,char software_rev,char hardware_rev,
                 char *unit_type,char *cal_date)
{
char    msg[80];
int     status;

   sprintf(msg,"=w %s,%c,%c,%s,%s",serial_nbr,software_rev,
           hardware_rev,unit_type,cal_date);
   if (software_rev >= 'C')
      add_checksum(msg);

   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_who() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function is used to put the compass into superuser mode. The password *
 * is the serial number of the compass board. If 'password' is a NULL pointer *
 * the compass is taken out of superuser mode. Also, if a superuser ACK is    *
 * not received, the compass will not be in superuser mode. The global flag   *
 * 'compass_mode' is updated to reflect the board status.                     *
 *                                                                            *
 *       In: character pointer to password, or NULL                           *
 *                                                                            *
 *      Out: 0 - Success                                                      *
 *           Non-Zero -                                                       *
 *                                                                            *
 * Cautions: None                                                             *
 *----------------------------------------------------------------------------*/
/* Put C100 into superuser mode. */
int c100_superuser_mode(char *password)
{
char    msg[10];
int     status;


   sprintf(msg,"u %s",password);        /* build the compass message */
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_superuser_mode() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *       In: pointer to a double precision number                             *
 *                                                                            *
 *      Out: 0 - Success, 'heading' contains next desired angle               *
 *           DONE - Success, cal is complete.                                 *
 *           Non-Zero -                                                       *
 *                                                                            *
 * Cautions: None                                                             *
 *----------------------------------------------------------------------------*/
/* */
int c100_cal_mode(char cal_type,double *heading,char *cal_msg)
{
char    msg[81],*ptr;
int     status,old_timeout;
int     retval = 0;


    if (cal_type == THREE_POINT_CAL) {
        sprintf(msg,"=ce%c,%.1f",cal_type,*heading);
    } else {
        sprintf(msg,"=ce%c",cal_type);
    } /* if */

    old_timeout = ack_timeout;
    ack_timeout = 3 * TICKS_PER_SECOND;          /* set 3 second timeout */

    status = send_command(msg,msg,sizeof(msg));
    ack_timeout = old_timeout;
    if (status != 0)                             /* no ACK received? */
        return(status);                          /* return error code */

    ptr = msg;
    switch (cal_type) {
        case FACTORY_CAL:
        case FORCED_EIGHT_POINT_CAL:
        case EIGHT_POINT_CAL:
        case THREE_POINT_CAL:
            switch (*ptr++) {
                case MSG_C_ACK:
                case SUPER_MSG_C_ACK:
                    retval = DONE;
                    break;

                case '$':
                    *heading = atof(ptr);
                    break;

                case MSG_C_NAK:
                    asigets_timed(port,msg,sizeof(msg),CARRIAGE_RETURN,
                                  TICKS_PER_SECOND);
                    if (_aserror == ASSUCCESS)
                        strcpy(cal_msg,msg);
                    retval = MSG_NAKKED;
                    break;

                case 'T':
                    ptr += 21;
                    *heading = atof(ptr);
                    break;

                default:
                    retval = BAD_MSG;
                    break;

            } /* switch */
            break;


        case CIRCULAR_CAL:
        case ABORT_CAL:
        default:
            /* do nothing */
            break;

    } /* switch */

    return(retval);                              /* return success/failure code */

} /* c100_cal_mode() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_cal_score(char score_type,int *score,int *count,
                       int *environment)
{
char    msg[20],*ptr;
int     status;


   sprintf(msg,"?cs%c",score_type);
   status = send_command(msg,msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strncmp(ptr,"?cs",3) != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,"");
   if (score_type == SHORT_SCORE)
      sscanf(ptr,"%d,%d",score,count);
   else
      sscanf(ptr,"%d,%d,%d",score,environment,count);

   return(0);

} /* c100_get_cal_score() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_cal_coefs(char cal_type,double *x0,double *y0,
                       double *a,double *b,double *c,double *d)
{
char    msg[80],*ptr;
int     status;


   sprintf(msg,"?ce%c",cal_type);
   status = send_command(msg,msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strncmp(ptr,"?ce",3) != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,"");
   status = sscanf(ptr,"%lf,%lf,%lf,%lf,%lf,%lf",
                   x0,y0,a,b,c,d);
   if (status != 6)
      return(BAD_MSG);

   return(0);

} /* c100_get_cal_coefs() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_cal_coefs(char cal_type,double x0,double y0,
                       double a,double b,double c,double d)
{
char    msg[80];
int     status;


   sprintf(msg,"=ce%c,%f,%f,%f,%f,%f,%f",cal_type,x0,y0,a,b,c,d);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_cal_coefs() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_baud(int *baud_rate)
{
char    msg[20],*ptr;
int     status;


   status = send_command("?b",msg,sizeof(msg));
   if (status != 0)
      return(status);


   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?b") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *baud_rate = (atoi(ptr) * 100);

   return(0);

} /* c100_get_baud() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_baud(int baud_rate)
{
char    msg[20];
int     status;


   sprintf(msg,"=b %02d",(baud_rate / 100));
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_baud() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_msg_type(char *msg_type)
{
char    msg[20],*ptr;
int     status;


   status = send_command("?t",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?t") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *msg_type = *ptr;

   return(0);

} /* c100_get_msg_type() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_msg_type(char msg_type)
{
char    msg[20];
int     status;


   sprintf(msg,"=t %c",msg_type);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_msg_type() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_msg_rate(int *msg_rate)
{
char    msg[20],*ptr;
int     status;


   status = send_command("?r",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?r") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *msg_rate = atoi(ptr);

   return(0);

} /* c100_get_msg_rate() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_msg_rate(int msg_rate)
{
char    msg[20];
int     status;


   sprintf(msg,"=r %03d",msg_rate);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_msg_rate() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_msg_units(char *msg_units)
{
char    msg[20],*ptr;
int     status;

   status = send_command("?i",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?i") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *msg_units = *ptr;

   return(0);

} /* c100_get_msg_units() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_msg_units(char msg_units)
{
char    msg[20];
int     status;


   sprintf(msg,"=i %c",msg_units);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_msg_units() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_damp_type(char *damp_type)
{
char    msg[20],*ptr;
int     status;


   status = send_command("?dt0",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?dt0") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *damp_type = *ptr;

   return(0);

} /* c100_get_damp_type() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_damp_type(char damp_type)
{
char    msg[20];
int     status;


   sprintf(msg,"=dt0,%c",damp_type);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_damp_type() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_damp_rate(char *damp_rate)
{
char    msg[20],*ptr;
int     status;

   status = send_command("?d0",msg,sizeof(msg));
   if (status != 0)
      return(status);


   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?d0") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *damp_rate = *ptr;

   return(0);

} /* c100_get_damp_rate() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_damp_rate(char damp_rate)
{
char    msg[20];
int     status;


   sprintf(msg,"=d0,%c",damp_rate);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_damp_rate() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_heading(double *heading)
{
char    msg[80],*ptr;
int     status;


   status = send_command("d1",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = msg;
   if (*ptr++ != '$')
      return(BAD_MSG);

   *heading = atof(ptr);                        /* convert ASCII to x */
   return(0);

} /* c100_get_heading() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_compass_xy(double *x,double *y)
{
char    msg[80],*ptr;
int     status;

   status = send_command("d2",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = msg;
   if (*ptr++ != '$')
      return(BAD_MSG);

   sscanf(ptr,"%lf,%lf",x,y);                   /* convert ASCII to x & y */
   return(0);

} /* c100_get_compass_xy() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_variation_onoff(char *variation_onoff)
{
char    msg[22],*ptr;
int     status;


   status = send_command("?v",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?v") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *variation_onoff = *ptr;

   return(0);

} /* c100_get_variation_onoff() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_variation_onoff(char variation_onoff)
{
char    msg[20];
int     status;


   sprintf(msg,"=v %c",variation_onoff);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_variation_onoff() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_variation(double *variation)
{
char    msg[20],*ptr;
int     status;


   status = send_command("?vd",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?vd") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *variation = atof(ptr);

   return(0);

} /* c100_get_variation() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_variation(double variation)
{
char    msg[20];
int     status;

   sprintf(msg,"=vd,%05.1f",variation);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_variation() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_a_offset(double *a_offset)
{
char    msg[20],*ptr;
int     status;

   status = send_command("?vi",msg,sizeof(msg));
   if (status != 0)
      return(status);


   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?vi") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *a_offset = atof(ptr);

   return(0);

} /* c100_get_a_offset() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_a_offset(double a_offset)
{
char    msg[20];
int     status;


   sprintf(msg,"=vi,%05.1f",a_offset);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_a_offset() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_in_superuser_mode(void)
{

   send_command("",NULL,0);                   /* send dummy command */
   return(c100_superuser);

} /* c100_in_superuser_mode() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 * This function sends a 'zap' command to the compass board, telling it to    *
 * reboot itself. After the reboot, the compass will NOT be in SUPERUSER mode *
 * even if it was before.                                                     *
 *                                                                            *
 *       In: None                                                             *
 *                                                                            *
 *      Out: 0 - Success                                                      *
 *           Non-zero - Error code returned by Greenleaf function.            *
 *                                                                            *
 * Cautions: If you have changed the compass baud rate prior to executing     *
 *           this function, the new baud rate will go into effect now. This   *
 *           may result in a loss of communications unless you re-open the    *
 *           RS-232 channel with the correct baud rate.                       *
 *----------------------------------------------------------------------------*/
/* Reboot C100. */
int c100_zap(void)
{
int     old_timeout;


   old_timeout = ack_timeout;
   ack_timeout = 0;

   send_command("zap",NULL,0);
   delay(500);                                  /* wait 1/2 second */

   ack_timeout = old_timeout;
   return(0);                                   /* return success code */

} /* c100_zap() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_powerup_mode(unsigned char *powerup_mode)
{
char    msg[20],*ptr;
int     status;


   status = send_command("?x",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?x") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   if (*ptr == 'o')
      *powerup_mode = SENDING;
   else
      *powerup_mode = NOT_SENDING;

   return(0);

} /* c100_get_powerup_mode() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_powerup_mode(unsigned char powerup_mode)
{
int     retval = 0;


   switch (powerup_mode) {
      case SENDING:
         retval = c100_send_data();
         break;

      case NOT_SENDING:
         retval = c100_halt_data();
         break;

      default:
         retval = 1;
   } /* switch */

   return(retval);

} /* c100_set_powerup_mode() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_digital_type(char *digital_type)
{
char    msg[20],*ptr;
int     status;


   status = send_command("?z",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?z") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *digital_type = *ptr;

   return(0);

} /* c100_get_digital_type() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_digital_type(char digital_type)
{
char    msg[20];
int     status;


   sprintf(msg,"=z %c",digital_type);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_digital_type() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_digital_fmt(char *digital_fmt)
{
char    msg[20],*ptr;
int     status;


   status = send_command("?zt",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?zt") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *digital_fmt = *ptr;
   return(0);

} /* c100_get_digital_fmt() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_digital_fmt(char digital_fmt)
{
char    msg[20];
int     status;


   sprintf(msg,"=zt %c",digital_fmt);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_digital_fmt() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_get_analog_fmt(char *analog_fmt)
{
char    msg[20],*ptr;
int     status;


   status = send_command("?at",msg,sizeof(msg));
   if (status != 0)
      return(status);

   ptr = strtok(msg,", ");
   if ((ptr == NULL) || (strcmp(ptr,"?at") != 0))
      return(BAD_MSG);

   ptr = strtok(NULL,", ");
   *analog_fmt = *ptr;
   return(0);

} /* c100_get_analog_fmt() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_analog_fmt(char analog_fmt)
{
char    msg[20];
int     status;


   sprintf(msg,"=at %c",analog_fmt);
   status = send_command(msg,NULL,0);
   return(status);

} /* c100_set_analog_fmt() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_all_voltage(char on_off,unsigned int counts)
{
char    msg[40];
int     status;


   sprintf(msg,"=a0 %c,%u",on_off,counts);
   status = send_command(msg,NULL,0);
   return(status);                              /* return error/success code */

} /* c100_set_all_voltage() */



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/*  */
int c100_set_voltage_constants(double m,double b,unsigned int counts)
{
char    msg[40];
int     status;


   sprintf(msg,"=ac %f,%f,%u",m,b,counts);
   status = send_command(msg,NULL,0);
   return(status);                              /* return status code */

} /* c100_set_voltage_constants() */



/******************************************************************************
 **** Give us an 'E', Give us an 'N', Give us a 'D', What does that spell? ****
 ******************************************************************************
 **************************** -----The END----- *******************************
 ******************************************************************************/
