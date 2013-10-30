/*******************************************************************************
*           Copyright (C) 2013 Tektronix Inc., All rights reserved.
*
*                       3841 Brickway Blvd. Suite 210
*                       Santa Rosa, CA 95403
*                       Tel:(707) 595-4770
*
* Filename:     instr_cmds.h
*
* Description:  LE320 commands, typically called from the dispatcher, instr_task
*
* $Author$
* $DateTime$
* $Id$
*******************************************************************************/
//#define TRACE_PRINT 1

#ifndef _INSTR_CMDS_INCLUDED
#define _INSTR_CMDS_INCLUDED

#include "OSandPlatform.h"

#ifdef GLOBAL_INSTR_CMDS
#define INSTR_CMDSGLOBAL
#define INSTR_CMDSPRESET(A) = (A)
#else
#define INSTR_CMDSPRESET(A)
#ifdef __cplusplus
#define INSTR_CMDSGLOBAL extern "C"
#else
#define INSTR_CMDSGLOBAL extern
#endif	/*__cplusplus*/
#endif				/*GLOBAL_INSTR_CMDS */

// ----------------------------------------------------------------
// PRIVATE API AND SUBJECT TO CHANGE!
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// PUBLIC API definition
// ----------------------------------------------------------------
#include "usbcmdio.h"
#include "md5_tek.h"

//  fill out the firmware version ID response payload
void get_instrument_ID(usb_packet_t *buffer);
void get_instrument_SSN(usb_packet_t *buffer);
void get_instrument_UID(usb_packet_t *buffer);
MD5_TEK ssn_to_MD5(void);  // this is a 96-bit SMT32 version
void print_UID48(BaseSequentialStream *chp, MD5_TEK *pCTXT);
void print_ID   (BaseSequentialStream *chp, usb_packet_t *pPkt);
void print_SSN  (BaseSequentialStream *chp, usb_packet_t *pPkt);




#endif				//_INSTR_CMDS_INCLUDED
