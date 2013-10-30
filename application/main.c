/*
  ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
  2011,2012 Giovanni Di Sirio.

  This file is part of ChibiOS/RT.

  ChibiOS/RT is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  ChibiOS/RT is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include "shell.h"

#include "cmd_shell.h"

#include "bulk_usb.h"
#include "usbcfg.h"
#include <strings.h>

#include "instr_task.h"
#include "instr_debug.h"       // common debug macros:  dprintf

extern SerialUSBDriver SDU1;   // virtual serial port over USB
extern BulkUSBDriver BDU1;
extern const ShellConfig shell_cfg1;

/*
 * SPI2 configuration structure.
 * Speed 21MHz, CPHA=0, CPOL=0, 8bits frames, MSb transmitted first.
 * The slave select line is the pin 12 on the port GPIOA.
 */
static const SPIConfig spi2cfg = {
  NULL,
  /* HW dependent part.*/
  GPIOB,
  12,
  0
};


/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

static WORKING_AREA(waInstrumentThread, 4096);

/*
 * Application entry point.
 */
int main(void) {
  Thread *shelltp = NULL;

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /* LED GPIO init
   * PD12 - Green, PD13 - Orange, PD14 - Red, PD15 - Blue
   */
  palSetPadMode(GPIOD, 12, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOD, 13, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOD, 14, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOD, 15, PAL_MODE_OUTPUT_PUSHPULL);

  GREEN_OFF;
  GREEN_OFF;
  RED_ON;
  GREEN_OFF;
  

  /* DEBUG pins used for timing, debug: PB4 == "debug 1", PB5 = "debug 2"
   *   These macros are defined in instr_debug.h
   *       DB1_HI, DB1_LO
   *       DB2_HI, DB2_LO
   */
  // DEBUG 1-5 are all on outside consecutive pins
  palSetPadMode(GPIOE, 7, PAL_MODE_OUTPUT_PUSHPULL);  // DEBUG1
  palSetPadMode(GPIOE, 9, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOE,11, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOE,13, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOE,15, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOE, 8, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOE,10, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOE,12, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOE,14, PAL_MODE_OUTPUT_PUSHPULL);  // DEBUG9

  DB1_LO;DB2_LO;DB3_LO;DB4_LO;DB5_LO;DB6_LO;DB7_LO;DB8_LO;DB9_LO;

  /*
   * Shell manager initialization.
   */
  shellInit();

  /*
   * Initializes a serial-over-USB CDC driver.
   */

  // Bulk usb driver setup
  bduObjectInit(&BDU1);
  bduStart(&BDU1, &blkusbcfg);


  //This one needs to be last so the usb device's upstream pointer is this driver
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);
  
  
  /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1000);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  /*
   * Activates the serial driver 2 using the driver default configuration.
   * PA2(TX) and PA3(RX) are routed to USART2.
   */
  sdStart(&SD2, NULL);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

  /*
   * Initialize I2C #1 Driver. Setup SDA=PB7, SCL=PB8
   */
#ifdef _BBI2C_INCLUDED
  init_bbI2C();

  //Clear and setup the equalizer chip
  hmc6545setup(&equalizer, NULL, 0x1c, "equalizer");
  hmc6545softRst(&equalizer);
  hmc6545clearChip(&equalizer);
  BLUE_ON;
#endif


  /*
   * Initializes the SPI driver 2. The SPI2 signals are routed as follow:
   * PB12 - NSS.
   * PB13 - SCK.
   * PB14 - MISO.
   * PB15 - MOSI.
   */
  spiStart(&SPID2, &spi2cfg);
  palSetPad(GPIOB, 12);
  palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL |
                PAL_STM32_OSPEED_HIGHEST|PAL_MODE_ALTERNATE(5) );           /* NSS.     */
  palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(5) |
                PAL_STM32_OSPEED_HIGHEST);           /* SCK.     */
  palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5));              /* MISO.    */
  palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) |
                PAL_STM32_OSPEED_HIGHEST);           /* MOSI.    */


  /*
   * Creates the Instrument thread
   */
  chThdCreateStatic(waInstrumentThread, sizeof(waInstrumentThread),
                    NORMALPRIO + 10, InstrumentThread, NULL);

  /*
   * Normal main() thread activity, in this demo it just performs
   * a shell respawn upon its termination.
   */
  while (TRUE) {
    if (!shelltp) {
      if (SDU1.config->usbp->state == USB_ACTIVE) {
        /* Spawns a new shell.*/
        shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
      }
    }
    else {
      /* If the previous shell exited.*/
      if (chThdTerminated(shelltp)) {
        /* Recovers memory of the previous shell.*/
        chThdRelease(shelltp);
        shelltp = NULL;
      }
    }
    chThdSleepMilliseconds(500);
  }
}
