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
 * @file    instr_task.h
 * @brief   Instrument communications task handler
 *
 * @{
 */

#ifndef _INSTR_TASK_H_
#define _INSTR_TASK_H_

#include "usbcmdio.h"

/**
 * @brief   definition of the instrument thread
 */
__attribute__((noreturn)) msg_t InstrumentThread(void *arg);

#endif /* _INSTR_TASK_H_ */

/** @} */
