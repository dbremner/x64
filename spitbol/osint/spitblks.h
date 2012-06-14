/*
/   File:  SPITBLKS.H		Version:  01.01
/   -------------------------------------------
/
/   This header file defines structures used by the Macro SPITBOL compiler
/   that are passed into the OS interface.
*/
/*  Copyright 1991 Robert Goldberg and Catspaw, Inc. */

/*
/   First, define the C type word to be the same size as a word used
/   by the Macro SPITBOL compiler.  The type of a word is a signed
/   integer for now.
*/

/*
 * 	BUFFER CONTROL BLOCK
 *
 *	A buffer control block (BCBLK) is created by the BUFFER
 *	function, and serves as an indirect control header for the
 *	buffer. It contains the number of characters currently
 *	stored in the buffer.
 */
struct bcblk {
	word	typ;					/* type word							*/
	word	idv;					/* identifier value						*/
	word	len;					/* number of chars in use in bfblk		*/
	struct bsblk *bcbuf;			/* pointer to bfblk						*/
	};

/*
 * 	STRING BUFFER BLOCK
 *
 *	A string buffer block (BFBLK) contains the actual buffer
 *	memory area. It specifies the largest string that can be
 *	stored in the buffer.
 */
struct bsblk {
	word	typ;					/* type word							*/
	word	bsalc;					/* allocated size of buffer				*/
	char	bschr[1];				/* characters of string					*/
	};


/*
 * CODE BLOCK
 *
 * A code block (CDBLK) is present for every source statement.
 */

struct cdblk {
	word			cdjmp;			/* ptr to routine to execute statement	*/
	word			cdstm;			/* statement number						*/
	word			cdsln;			/* source file line number				*/
	word			cdlen;			/* length of CDBLK in bytes				*/
	union {
		struct cdblk NEAR *cdnxt;	/* if failure exit is next statement	*/
		struct vrblk NEAR *cdlab;	/* if failure exit is a simple label	*/
		char 		 NEAR *cdnof;	/* no failure exit (-NOFAIL mode)		*/
		word		  cddir;		/* failure exit is complex or direct	*/
		}			cdfal;			/* Failure exit							*/
	word			cdcod[1];		/* executable pseudo-code				*/
	};


/*
/   CHFCB - chain of FCBs block
/
/   For every FCB created by OSINT, the compiler creates a CHFCB pointing
/   to the FCB and links it onto a chain of CHFCBs.  At EOJ the head of this
/   CHFCB chain is passed to the interface function SYSEJ so that all files
/   can be closed.
*/

struct	chfcb {
	word	typ;				/*  type word			*/
	word	len;				/*  block length		*/
	struct	chfcb NEAR *nxt;	/*  pointer to next chfcb	*/
	struct	fcblk NEAR *fcp;	/*  pointer to fcb		*/
	};


/*
/   EFBLK - external function block
/
*/

struct	efblk {
	word	fcode;				/*  type word			*/
	word	fargs;				/*  number of arguments	*/
	word	eflen;				/*  block length		*/
	word	efuse;				/*  usage count			*/
	void NEAR *efcod;			/*  pointer to XNBLK	*/
	struct vrblk NEAR *efvar;	/*  pointer to VRBLK	*/
	word	efrsl;				/*  result type			*/
	word	eftar[1];			/*  argument types		*/
	};

/*
/   ICBLK - integer block
/
/   Integer values are stored in ICBLKs.  Field icval should be defined
/   to be the appropriate type for the implementation.
*/

struct	icblk {
	word	typ;		/*  type word - b$icl		*/
	IATYPE	val;
	};

/*
/	RCBLK - real block
/
/	Real values are stored in RCBLKs.  Field rcval should be defined
/	to be the appropriate type for the implementation.
*/

struct	rcblk {
	word	typ;		/*	type word - b$rcl */
	double	rcval;		/*	real value */
	};

/*
/   SCBLK - string block
/
/   String values are stored in SCBLKs.  Notice that the scstr field
/   is defined as an array of characters of length 1.  This is a slight
/   kludge to side-step C's lack of support for varying length strings.
/
/   The actual length of a SCBLK is 2 words + the number of words necessary
/   to hold a string of length sclen.
*/

struct	scblk {
	word	typ;		/*  type word - b$scl		*/
	word	len;		/*  string length		*/
	char	str[1];		/*  string characters		*/
	};


/*
 * VARIABLE BLOCK
 *
 * A variable block (VRBLK) is used to hold a program variable.
 */

struct vrblk {
	word			vrget;			/* routine to load variable onto stack	*/
	word			vrsto;			/* routine to store stack top into var.	*/
	union block	 NEAR *vrval;		/* variable value						*/
	word			vrtra;			/* routine to transfer to label			*/
	union block	 NEAR *vrlbl;		/* pointer to code for label			*/
	union block	 NEAR *vrfnc;		/* function block if name is function	*/
	struct vrblk NEAR *vrnxt;		/* next vrblk on hash chain				*/
	word			vrlen;			/* length of name						*/
	char			vrchs[1];		/* characters of name					*/
	};




/*
/	BLOCK - an arbitrary block
*/

union block {
	struct bcblk	bcb;
	struct bsblk	bsb;
	struct cdblk	cdb;
	struct chfcb	fcb;
	struct efblk	efb;
	struct icblk	icb;
	struct rcblk	rcb;
	struct scblk	scb;
	struct vrblk	vrb;
	};