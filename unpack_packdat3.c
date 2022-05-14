#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>

#include "readbytes.h"

/* PackDat3 */
/* 4-byte num of files */
/* 256 bytes filename */
/* 4-byte offset */
/* 4-byte filesize */
/* 4-byte filesize? */

int main(int argc, char *argv[])
{
	FILE *archive, *out;
	char filename[256 + 1];
	unsigned char buf[268];
	unsigned int count, file_offset, file_size, end_of_file, pos;
	unsigned char magic[] = {0x50,0x61,0x63,0x6b,0x44,0x61,0x74,0x33};

	if(argc < 2)
	{
		printf("%s: <archive.CAB>\n", basename(argv[0]));
		exit(1);
	}

	archive = fopen(argv[1], "rb");

	if(fread(buf, 1, 12, archive) != 12)
	{
		puts("not a packdat3 archive, exiting...");
		exit(1);
	}

	/* magic? */
	if(memcmp(buf, magic, 8))
	{
		puts("not a packdat3 archive, exiting...");
		exit(1);
	}

	count = read_uint32_le(&buf[8]);

	pos = ftell(archive);
	fseek(archive, 0, SEEK_END);
	end_of_file = ftell(archive);
	fseek(archive, pos, SEEK_SET);

	printf("index count: %u\n", count);

	while(count)
	{
		fread(buf, 1, 268, archive);

		memcpy(filename, buf, 256);
		filename[257] = '\0';

		file_offset = read_uint32_le(&buf[256]);
		file_size = read_uint32_le(&buf[260]);

		if(file_offset > end_of_file)
		{
			printf("reached end of archive unexpectedly");
			exit(1);
		}

		pos = ftell(archive);

#if 0
		printf("filename: %s\noffset: %u\nsize: %u\n", filename, file_offset, file_size);
#else
		printf("filename: %s\n", filename);
#endif

		fseek(archive, file_offset, SEEK_SET);

		out = fopen(filename, "wb");

		while(file_size)
		{
			putc(fgetc(archive), out);
			file_size--;
		}
		fclose(out);
		fseek(archive, pos, SEEK_SET);
		count--;
	}

	return 0;
}
