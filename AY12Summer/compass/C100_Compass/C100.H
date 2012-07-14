/*----------------------------------------------------------------------------*/
/* C100.H -     Header file for the C100 board interface module C100.C        */
/*                                                                            */
/* Written by David R. Hoy, Oct/Nov 1991.                                     */
/* Copyright (c) 1991, KVH Industries Inc, Middletown, RI.                    */
/*----------------------------------------------------------------------------*/

/* $Header:   F:/c100_pc/user/vcs/c100.h_v   1.1   06 May 1993 10:26:10   DavidH  $ */
/* $Revision:   1.1  $ */
/*
 * $Log:   F:/c100_pc/user/vcs/c100.h_v  $
 * 
 *    Rev 1.1   06 May 1993 10:26:10   DavidH
 *
 *    Rev 1.0   12 Mar 1992 15:03:34   DavidH
 * Initial revision.
 */


/*------ #defines used by 'c100_open_channel()' ------*/
#define COM_1                   0
#define COM_2                   1
#define COM_3                   2
#define COM_4                   3

/*------ #defines for function return values ------*/
#define OK                      0
#define NO_ACK                  1
#define BAD_MSG                 2
#define NO_SUPERUSER            3
#define TIMEOUT                 4
#define ACK_TIMEOUT             5
#define PARITY_ER               6
#define RETRY_LIMIT_EXCEEDED    7
#define COLLISION               8
#define MSG_NAKKED              9
#define INVALID_CHECKSUM        10
#define NOT_IMPLEMENTED         11
#define DONE                    999

/*------ #defines for 'c100_check_for_ack()' ------*/
#define NOTHING_YET             0
#define ACK_RECEIVED            1
#define NAK_RECEIVED            2

/*------ #defines for power-up mode functions ------*/
#define NOT_SENDING             0
#define SENDING                 1

/*------ #defines for factory/field cal functions ------*/
#define FACTORY_CAL             '0'             /* all cal functions */
#define FORCED_EIGHT_POINT_CAL  '1'             /* all cal functions */
#define EIGHT_POINT_CAL         '2'             /* all cal functions */
#define CIRCULAR_CAL            '3'             /* all cal functions */
#define THREE_POINT_CAL         '4'             /* all cal functions */
#define ABORT_CAL               'z'

/*------ #defines for get/set cal coefficients ------*/
#define FACTORY_CAL             '0'
#define FIELD_CAL               '1'
#define COALESCED_CAL           '2'

/*------ #defines for 'c100_get_cal_score()' ------*/
#define SHORT_SCORE             '1'
#define LONG_SCORE              '2'

/*------ #defines for get/set message rate/type/units ------*/
#define NMEA                    '0'
#define KVH_HEADING             '1'
#define CORR_XY                 '2'
#define UNCORR_XY               '3'
#define DEGREES                 'd'
#define MILS                    'm'

/*------ #defines for digital output type ------*/
#define CONTINUOUS              '0'
#define STROBED                 '1'

/*------ #defines for digital output format ------*/
#define DIG_OFF                 '0'
#define BCD                     '1'
#define GRAY_CODE               '2'
#define DIG_BINARY              '3'
#define LRX_DISPLAY             '4'
#define NCSC                    '5'

/*------ #defines for analog output format ------*/
#define LINEAR                  '0'
#define HYSTERESIS              '1'
#define SIN_COS_REF             '2'

/*------ #defines for variation on/off -----*/
#define VAR_ON                  't'
#define VAR_OFF                 'm'

/*------ #defines for get/set damping type ------*/
#define UNDAMPED                '0'
#define SINGLE_IIR              '1'
#define SUM_AND_DUMP            '2'
#define DOUBLE_IIR              '3'
#define WINDOW_AVG              '4'
#define _3_SEC                  '0'
#define _4_5_SEC                '1'
#define _6_SEC                  '2'
#define _7_5_SEC                '3'
#define _9_SEC                  '4'
#define _12_SEC                 '5'
#define _14_SEC                 '6'
#define _16_SEC                 '7'
#define _20_SEC                 '8'
#define _24_SEC                 '9'


/*------ Function Prototypes ------*/
extern int c100_mode;
char *c100_error_name(int err_code);
int c100_open_channel(int com_port,long baud_rate);
int c100_close_channel(void);
int c100_halt_data(void);
int c100_send_data(void);
int c100_get_who(char *serial_nbr,char *sftwr_rev,char *hrdwr_rev,
		 char *unit_type, char *cal_date);
int c100_set_who(char *serial_nbr,char sftwr_rev,char hrdwr_rev,
		 char *unit_type,char *cal_date);
int c100_superuser_mode(char *password);
int c100_cal_mode(char cal_type,double *heading,char *cal_msg);
int c100_get_cal_score(char score_type,int *score,int *count,
		       int *environment);
int c100_get_cal_coefs(char cal_type,double *x0,double *y0,
		       double *a,double *b,double *c,double *d);
int c100_set_cal_coefs(char cal_type,double x0,double y0,
		       double a,double b,double c,double d);
int c100_get_baud(int *baud_rate);
int c100_set_baud(int baud_rate);
int c100_get_msg_type(char *msg_type);
int c100_set_msg_type(char msg_type);
int c100_get_msg_rate(int *msg_rate);
int c100_set_msg_rate(int msg_rate);
int c100_get_msg_units(char *msg_units);
int c100_set_msg_units(char msg_units);
int c100_get_damp_type(char *damp_type);
int c100_set_damp_type(char damp_type);
int c100_get_damp_rate(char *damp_rate);
int c100_set_damp_rate(char damp_rate);
int c100_get_heading(double *heading);
int c100_get_compass_xy(double *x,double *y);
int c100_get_variation_onoff(char *variation_onoff);
int c100_set_variation_onoff(char variation_onoff);
int c100_get_variation(double *variation);
int c100_set_variation(double variation);
int c100_get_a_offset(double *a_offset);
int c100_set_a_offset(double a_offset);
int c100_in_superuser_mode(void);
int c100_zap(void);
int c100_get_powerup_mode(unsigned char *powerup_mode);
int c100_set_powerup_mode(unsigned char powerup_mode);
int c100_get_digital_type(char *digital_type);
int c100_set_digital_type(char digital_type);
int c100_get_digital_fmt(char *digital_fmt);
int c100_set_digital_fmt(char digital_fmt);
int c100_get_analog_fmt(char *analog_fmt);
int c100_set_analog_fmt(char analog_fmt);
int c100_set_all_voltage(char on_off,unsigned int counts);
int c100_set_voltage_constants(double m,double b,unsigned int counts);
int c100_check_for_ack(char *cal_msg);

