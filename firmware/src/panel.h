/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

/*
 * This module implements the front panel interaction logic, i.e.
 * it reacts to key presses and updates settings accordingly, as well
 * as controls the display contents.
 */

#ifndef __PANEL_H__
#define __PANEL_H__

void Panel_Init();

void Panel_TimerTick();

#endif // __PANEL_H__
