#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv) {
	int i=0;

	if (argc < 3) {
		printf("%s <file> <key>\n", argv[0]);
		return 1;
	}

	int f = open(argv[1], O_RDONLY);
	if (f < 0)
		return 1;

	if(strlen(argv[2]) < 6)
		return 1;

	char t[4096];
	char *key = argv[2];

	printf("key: %s\n", key);
	for(i=0; i < 6; i++)
		key[i] -= 'A' + 4;
	i=0;

	while (read(f, t, 4096) > 0) {
		char *buf = t;
		for(; *buf != '\0'; i++) {
			*buf -= 'A';
			*buf += 26;
			*buf -= (key[i%6]);
			*buf %= 26;
			*buf += 'A';
			printf("%c", *buf);
			buf++;

			if(i%(18*6) == 18*6-1)
				printf("\n");
			else if(i%6 == 5)
				printf(" ");
		}
	}
	printf("\n");

	return 0;
}
