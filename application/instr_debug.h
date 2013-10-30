/**
 * @file    instr_debug.h
 * @brief   debug macros, with include-guard
 *
 * @{
 */

#ifndef _INSTR_DEBUG_H_
#define _INSTR_DEBUG_H_



// HACK ALERT:
//   If we fill up the virtual com port's output buffer with nobody connected,
//   the USB device will stall, waiting to drain off our printf's, so ...
//      only print if queue has at least 64 bytes of "headroom"
#define RELEASE
#ifndef RELEASE
#define dprintf(args...)                    \
  if (chOQGetFullI(&SDU1.oqueue)<64) {      \
     chprintf(shell_cfg1.sc_channel, args); \
     chThdSleep(MS2ST(50)); }
#else
#define dprintf(args...)
#endif

#define GREEN_ON palSetPad(GPIOD,12)
#define GREEN_OFF palClearPad(GPIOD,12)
#define ORANGE_ON palSetPad(GPIOD,13)
#define ORANGE_OFF palClearPad(GPIOD,13)
#define RED_ON palSetPad(GPIOD,14)
#define RED_OFF palClearPad(GPIOD,14)
#define BLUE_ON palSetPad(GPIOD,15)
#define BLUE_OFF palClearPad(GPIOD,15)
  
//  DEBUG 1 thru 5 are on outside consecutive pins
#define DB1_HI palSetPad  (GPIOE, 7)
#define DB1_LO palClearPad(GPIOE, 7)
         
#define DB2_HI palSetPad  (GPIOE, 9)
#define DB2_LO palClearPad(GPIOE, 9)
         
#define DB3_HI palSetPad  (GPIOE,11)
#define DB3_LO palClearPad(GPIOE,11)
         
#define DB4_HI palSetPad  (GPIOE,13)
#define DB4_LO palClearPad(GPIOE,13)
         
#define DB5_HI palSetPad  (GPIOE,15)
#define DB5_LO palClearPad(GPIOE,15)

#define DB6_HI palSetPad  (GPIOE,8)
#define DB6_LO palClearPad(GPIOE,8)

#define DB7_HI palSetPad  (GPIOE,10)
#define DB7_LO palClearPad(GPIOE,10)

#define DB8_HI palSetPad  (GPIOE,12)
#define DB8_LO palClearPad(GPIOE,12)

#define DB9_HI palSetPad  (GPIOE,14)
#define DB9_LO palClearPad(GPIOE,14)

#endif // _INSTR_DEBUG_H_
