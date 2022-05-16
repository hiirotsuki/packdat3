#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>

#include "writebytes.h"

#include "repack_packdat3.h"

int main(int argc, char *argv[])
{
	FILE *out, *in;
	struct stat st;
	char filename[256];
	struct dirent **entries;
	int bytes_written, current_offset, num, i;
	unsigned char magic[] = {0x50,0x61,0x63,0x6b,0x44,0x61,0x74,0x33};

	if(argc < 2)
	{
		printf("usage: %s <archive.CAB> [optional: <directory>]\n", argv[0]);
		exit(1);
	}

	if(argc >= 3)
	{
		num = scandir(argv[2], &entries, allowed_exts, alphasort);
	}
	else
	{
		num = scandir(".", &entries, allowed_exts, alphasort);
	}

	if(num <= 0)
	{
		puts("no files to write");
		exit(1);
	}

	printf("files to be written: %d\n", num);

	out = fopen(argv[1], "wb");

	fwrite(magic, 1, 8, out);
	write_uint32_le(num, out);

	fseek(out, num * 268 + 11, SEEK_SET);
	fputc(0, out);
	fseek(out, 0, SEEK_SET);

	fflush(out);

	i = 0;
	while(i < num)
	{
		/*printf("%s %d\n", entries[files_written]->d_name, num);*/
		if(strlen((entries[i])->d_name) > 256)
		{
			puts("filename is longer than max filename length");
			exit(1);
		}

		bytes_written = 0;
		stat(entries[i]->d_name, &st);
		printf("filename: %s size: %d\n", entries[i]->d_name, (int)st.st_size);

		in = fopen(entries[i]->d_name, "rb");

		if(!in)
		{
			printf("could not open file: %s\n", entries[i]->d_name);
			exit(1);
		}

		fseek(out, 0, SEEK_END);
		current_offset = ftell(out);
		fseek(out, current_offset, SEEK_SET);
		/*printf("end of file %d\n", current_offset);*/
		while(bytes_written < st.st_size)
		{
			putc(fgetc(in), out);
			bytes_written++;
		}

		if(!i)
		{
			fseek(out, 12, SEEK_SET);
		}
		else
		{
			fseek(out, (i * 268) + 12, SEEK_SET);
		}

		memset(filename, '\0', sizeof(filename));
		memcpy(filename, entries[i]->d_name, strlen(entries[i]->d_name));
		fwrite(filename, sizeof(filename), 1, out);

		fseek(out, 256 - sizeof(filename), SEEK_CUR);

		write_uint32_le(current_offset, out);
		write_uint32_le(st.st_size, out);
		write_uint32_le(st.st_size, out);
		i++;
	}

	fclose(out);
	return 0;
}

int allowed_exts(const struct dirent *ent)
{
	char *p;
	char *exts[] = {"png", "ogg", "txt"};

	if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
		return 0;

	if((p = strrchr(ent->d_name, '.')) != 0)
	{
		if(!strcmp(p + 1, exts[0]) || !strcmp(p + 1, exts[1]) || !strcmp(p + 1, exts[2]))
		{
			return 1;
		}
	}
	return 0;
}
