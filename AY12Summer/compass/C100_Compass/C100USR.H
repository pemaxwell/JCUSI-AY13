/* $Header:   C:/c100/user/vcs/c100usr.h_v   1.0   12 Mar 1992 15:00:26   DavidH  $ */
/* $Revision:   1.0  $ */
/*
 * $Log:   C:/c100/user/vcs/c100usr.h_v  $
 * 
 *    Rev 1.0   12 Mar 1992 15:00:26   DavidH
 * Initial revision.
 */

#define flush_keybuf()	while (ki_chk()) { ki(); }
#define CONTINUE	0
#define	ABORT		1


void 	 msg_window(char *msg);
int	 continue_abort(char *msg);


extern int	dflt_port;
extern long	dflt_baud;

