/*
/	File:  SYSEJ.C		Version:  01.04
/	---------------------------------------
/
/	Contents:	Function zysej
*/
/*  Copyright 1991 Robert Goldberg and Catspaw, Inc. */

/*
/	zysej - end job
/
/	zysej is called to terminate spitbol's execution.  Any open files
/	will be closed before calling __exit.
/
/	Parameters:
/	    WA - value of &ABEND keyword (always 0)
/	    WB - value of &CODE keyword
/	    XL - pointer to FCBLK chain
/	Returns:
/	    NO RETURN
/
/	HISTORY
/
/  V1.04 MBE 07-Aug-90  Avoid using heap data structures if aborted in
/				the middle of a garbage collection.
/
/  V1.03 MBE 27-Nov-89	Call termhost() if 80386 version to clear any
/		        funny modes from screen host functions.
/
/  V1.02 MBE 14-Dec-87	Make file closing loop into function for access from
/			sysxi.c.
*/

#include "port.h"

#if EXTFUN
	unsigned char FAR *bufp;
#endif					/* EXTFUN */


/*
/  close_all - Close all files.
/
/  Parameters:
/	chfcb	pointer to FCBLK chain or 0
/  Returns:
/	Nothing
/  Side Effects:
/	All files on the chain are closed and buffers flushed.
*/

void close_all(chb)

register struct chfcb *chb;

{
	while( chb != 0 )
	{
		osclose( MK_MP(MK_MP(chb->fcp, struct fcblk *)->iob, struct ioblk *) );
		chb = MK_MP(chb->nxt, struct chfcb *);
	}
}



void zysej()
{
#if HOST386
	termhost();
#endif					/* HOST386 */

	if (!in_gbcol) {		/* Only if not mid-garbage collection */
		close_all( XL( struct chfcb * ) );

#if EXTFUN
		scanef();					/* prepare to scan for external functions */
		while (nextef(&bufp, 1))	/* perform closing callback to some		  */
			;
#endif					/* EXTFUN */

	}
/*
/	Pass &CODE to function __exit.  Don't call standard exit function,
/	because of its association with the stdio package.
*/
  __exit( WB(int) );

}
