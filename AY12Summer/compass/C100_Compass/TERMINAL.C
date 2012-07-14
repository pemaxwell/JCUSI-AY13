/* $Header:   F:/c100_pc/user/vcs/terminal.c_v   1.2   05 May 1993 15:11:26   DavidH  $ */
/* $Revision:   1.2  $ */
/*
 * $Log:   F:/c100_pc/user/vcs/terminal.c_v  $
 * 
 *    Rev 1.2   05 May 1993 15:11:26   DavidH
 * Major change to terminal mode - most of it ported from DDGM factory cal
 * software.  Takes advantage of new setup variables, such as Echo, etc.
 * 
 *    Rev 1.1   08 Apr 1992 14:48:24   DavidH
 * Modified code in response to Laser Tech's comments.
 * 
 *    Rev 1.0   12 Mar 1992 14:59:38   DavidH
 * Initial revision.
 */


#pragma warn -par

#include <dos.h>
#include <ctype.h>
#include <asiports.h>
#include <vv_form.h>


/*--- external function prototypes ---*/
extern void msg_window(char *msg);


/*--- externally defined variables ---*/
extern int  dflt_port;
extern long dflt_baud;
extern int  echo;
extern int  unprintables;



/*PAGE*/
/*----------------------------------------------------------------------------*
 *                                                                            *
 *      in:                                                                   *
 *     out:                                                                   *
 * returns:                                                                   *
 *----------------------------------------------------------------------------*/
/* Simple terminal emulator for direct communication. */
int terminal_mode(FORMPTR fmp)
{
int         ch,done,row,col;
WINDOWPTR   wnp,wnp2;
PORT        *port;


    wnp = wn_def(vs_rowq()-1,0,1,vs_colq(),LURGENT,BDR_NULLP);
    wn_up(wnp);

    v_st(" Establishing link with compass, please wait....",wnp);
    port = PortOpenGreenleaf(dflt_port,dflt_baud,'N',8,1);
    wn_dn(wnp);
    wn_free(wnp);
    if (port->status != ASSUCCESS) {
        msg_window("Error: unable to establish compass link.");
        return(SAMELEVEL);
    } /* if */



    /*--- display serial parameters on screen ---*/
    wnp2 = wn_def(vs_rowq()-1,0,1,vs_colq(),LNORMAL,BDR_NULLP);
    wn_up(wnp2);
    v_printf(wnp2,"\nCOM%d,%ld,NONE,8,1  ECHO=%s  UNPRINTABLES=%s",
             dflt_port+1,dflt_baud,((echo)? "ON" : "OFF"),
             ((unprintables)? "ON" : "OFF"));


    wnp = wn_def(CENTER_WN,CENTER_WN,20,70,LREVERSE,BDR_DLNP);
    sw_title("[Terminal Mode. Press ESC to Quit.]",
             LREVERSE,TOPLEFT,wnp);
    sw_updnfp(wn_expset,wn_expunset,wnp);
    sw_shad(TRANSPARENT,LSHADOW,BOTTOMRIGHT,wnp);
    sw_opt(CSRPLACE,ON,wnp);
    wn_up(wnp);

    done = FALSE;                           /* clear loop exit flag */
    do {                                    /* loop */
        if (ki_chk()) {                     /* was a key pressed? */
            switch (ch = ki()) {            /* which key was pressed? */

                case KEY_ESC:               /* was it ESC? */
                    done = TRUE;            /* set loop exit flag */
                    break;

                case KEY_ENTER:             /* was it ENTER? */
                    WriteChar(port,ch);     /* send character to compass */
                    if (echo) {
                        if (unprintables)
                            v_st("<cr>\n",wnp); /* write a CR-LF to screen */
                        else
                            v_ch('\n',wnp);
                        csr_plwn(wnp);
                    } /* if */
                    break;

                case KEY_BACK:              /* was it BACKSPACE? */
                    WriteChar(port,0x08);   /* send backspace */
                    if (echo) {
                        row = wnp->r;       /* get current row */
                        col = wnp->c-1;     /* get & decrement current column */
                        if (col >= 0) {     /* don't go past 1st column */
                            v_chattrow(row,col,' ',  /* wipe last character on screen */
                                       0,1,CH,wnp);
                            cs_mv(row,col,wnp);      /* move cursor back */
                            csr_plwn(wnp);  /* update cursor pos on screen */
                        } /* if */
                    } /* if */
                    break;

                default:                    /* any other key? */
                    if (isprint(ch)) {      /* is it a printable character? */
                        WriteChar(port,ch); /* send character to compass */
                        if (echo)
                            v_ch(ch,wnp);   /* write character to screen */
                    } /* if */
                    break;

            } /* switch */
        } /* if */

        ch = ReadChar(port);                /* try to read a char from device */
        if (ch >= ASSUCCESS) {              /* was a character read? */
            switch (ch) {

                case 0x08:                  /* backspace */
                    row = wnp->r;           /* get current row */
                    col = wnp->c-1;         /* get & decrement current column */
                    if (col >= 0) {         /* don't go past 1st column */
                        v_chattrow(row,col,' ', /* wipe last character on screen */
                                   0,1,CH,wnp);
                        cs_mv(row,col,wnp);     /* move cursor back */
                        csr_plwn(wnp);      /* update cursor pos on screen */
                    } /* if */
                    break;

                case 0x0D:                  /* carriage-return */
                    if (unprintables)
                        v_st("<cr>\n",wnp);
                    else
                        v_ch('\n',wnp);
                    csr_plwn(wnp);
                    break;

                default:                    /* everything else */
                    if (unprintables) {
                        if (isprint(ch))
                            v_ch(ch,wnp);
                        else
                            v_printf(wnp,"<%d>",ch);
                    } else {
                        if (isprint(ch))
                            v_ch(ch,wnp);
                    } /* if */
                    break;

            } /* switch */
        } /* if */

    } while (!done);                        /* loop until exit flag is set */

    wn_dn(wnp);                             /* remove window from screen */
    wn_free(wnp);                           /* release window memory */
    wn_dn(wnp2);
    wn_free(wnp2);

    PortClose(port);                        /* shut down serial channel */

    return(SAMELEVEL);                      /* return to menu system */

} /* terminal_mode() */


