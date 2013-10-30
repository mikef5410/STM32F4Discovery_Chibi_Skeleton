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

/**
 * @file    cmd_shell.h
 * @brief   Refactor main.c to remove cmd shell
 *
 * @addtogroup CMD_SHELL
 * @{
 */

#ifndef _CMD_SHELL_H_
#define _CMD_SHELL_H_

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)

/**
 * @brief   cmd-shell cmd: report memory use
 */
void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]);

/**
 * @brief   cmd-shell cmd: report thread use
 */
void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]);

// added by jimj for USB CMD test/verification
void cmd_shadow (BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_id     (BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_read   (BaseSequentialStream *chp, int argc, char *argv[]);

#endif /* _CMD_SHELL_H_ */

/** @} */
