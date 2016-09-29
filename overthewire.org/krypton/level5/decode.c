#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define KEYLEN 9

int main(int argc, char **argv) {
	int i=0;

	if (argc < 3) {
		printf("%s <file> <key>\n", argv[0]);
		return 1;
	}

	int f = open(argv[1], O_RDONLY);
	if (f < 0)
		return 1;

	if(strlen(argv[2]) < KEYLEN)
		return 1;

	char t[5000] = {0};
	char *key = argv[2];

	printf("key: %s\n", key);
	for(i=0; i < KEYLEN; i++)
		key[i] -= 'A' + 4;
	i=0;

	while (read(f, t, 5000) > 0) {
		char *buf = t;
		for(; *buf != '\0'; i++) {
			*buf -= 'A';
			*buf += 26;
			*buf -= (key[i%KEYLEN]);
			*buf %= 26;
			*buf += 'A';
			printf("%c", *buf);
			buf++;

			if(i%(10*KEYLEN) == 10*KEYLEN-1)
				printf("\n");
			else if(i%KEYLEN == KEYLEN-1)
				printf(" ");
		}
	}
	printf("\n");

	return 0;
}
