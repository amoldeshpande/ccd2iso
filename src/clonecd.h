/***************************************************************************
 *   Copyright (C) 2003 by Danny Kurniawan                                 *
 *   danny@it.petra.ac.id                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>

#define SECT_SIZE 2336

typedef unsigned char ccd_sectheader_syn[12];

#pragma pack(push)
#pragma pack(1)
typedef struct {
	unsigned char sectaddr_min, sectaddr_sec, sectaddr_frac;
	unsigned char mode;
} ccd_sectheader_header /* __attribute__((packed))*/;

typedef struct {
	ccd_sectheader_syn syn;
	ccd_sectheader_header header;
} ccd_sectheader /*__attribute__((packed))*/;

typedef struct {
	ccd_sectheader sectheader;
	unsigned char data[SECT_SIZE];
} ccd_sector /*__attribute__((packed))*/;

const ccd_sectheader_syn CCD_SYN_DATA = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 };
#pragma pack(pop)
