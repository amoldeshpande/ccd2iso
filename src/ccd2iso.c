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

#include <Windows.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "clonecd.h"

int read_bytes(FILE* fp, char* buffer, int bytes)
{
  int bread = 0;
  int readed = 0;
  do {
	readed = fread(buffer+ bread,1,bytes - bread,fp);
	if(readed <= 0)
	{
	  fprintf(stderr,"EOF %d bytes %d, bread %d feof %d\n ",readed,bytes, bread, feof(fp));
	  break;
	}
	bread += readed;

  } while(bread < bytes);

	return bread;
}
int read_bytes_win(HANDLE fp, char* buffer, int bytes)
{
	DWORD bread = 0;
	DWORD readed = 0;
	do {
		if (ReadFile(fp, buffer + bread,  bytes - bread, &readed,NULL) == FALSE)
		{
			fprintf(stderr, "EOF %d bytes %d, bread %d GetLastError() %d\n ", 
				readed, bytes, bread, GetLastError());
		}
		if (readed == 0)
		{
			fprintf(stderr, "EOF %d bytes %d, bread %d feof\n ", readed, bytes, bread);
			break;
		}
		bread += readed;

	} while (bread < bytes);

	return (int)bread;
}

int main(int argc, char *argv[])
{
	ccd_sector src_sect;
	int bytes_read, bytes_written, sect_num = 0;
	int total_bytes, n_bytes = 0;
	struct stat stat_file;

	FILE *src_file;
	FILE *dst_file;
	HANDLE src_file_handle;

	if ((!argv[1]) || (!argv[2]))
	{
		printf("(C) Copyright 2003. Danny Kurniawan.\n");
		printf("This software is released under GNU Public License\n\n");
		printf("Usage: ccd2iso <.img filename> <.iso filename>\n");
		return 1;
	}

	src_file = fopen(argv[1], "r");
	dst_file = fopen(argv[2], "w");
	src_file_handle = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (src_file_handle == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Could not open %s, %d\n", argv[1], GetLastError());
		return 1;
	}

	stat(argv[1], &stat_file);
	total_bytes = stat_file.st_size;

	while(!feof(src_file))
	{
		//bytes_read = fread(&src_sect, 1, sizeof(ccd_sector), src_file);
		//bytes_read = read_bytes(src_file,(char*)&src_sect,  sizeof(ccd_sector));
		bytes_read = read_bytes_win(src_file_handle,(char*)&src_sect,  sizeof(ccd_sector));
		n_bytes += bytes_read;

		if (bytes_read != 0)
		{
			if (bytes_read < sizeof(ccd_sector))
			{
				printf("Error at sector %d.\n", sect_num);
				printf("The sector does not contain complete data. Sector size must be %d, while actual data read is %d\n",
					sizeof(ccd_sector), bytes_read);
				printf("This probably means that you don't have a valid .img file.\n");
				if (bytes_read == 0)
				{
					break;
				}
				return 1;
			}

			if (sect_num == 0)
			{
				bytes_written = fwrite(&(src_sect.data[8]), 1, 2040, dst_file);
				if (bytes_written < 2040)
				{
					printf("Error writing to file.\n");
					return -1;
				}
			}
			else
			{
				bytes_written = fwrite(&(src_sect.data), 1, 2048, dst_file);
				if (bytes_written < 2048)
				{
					printf("Error writing to file.\n");
					return -1;
				}
			}

			sect_num++;
			printf("Processing sector: %d - %d of %d bytes processed\r", sect_num, n_bytes, total_bytes);
		}
	}
	fclose(src_file);
	fclose(dst_file);

	printf("\nDone.\n");

	return 0;
}
