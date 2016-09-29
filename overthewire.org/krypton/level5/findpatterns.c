#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("%s <file> <patternlength>\n", argv[0]);
		return 1;
	}


	int f = open(argv[1], O_RDONLY);
	if (f < 0)
		return 1;

	char data[4096];
	int len = atoi(argv[2]);

	int filelen = read(f, data, 4096);
	if (filelen < 0)
		return 1;

	char *search = calloc(len+1, 1);
	for (int i = 0; i < filelen-len; i++) {
		memcpy(search, data+i, len);

		for (int j = 0; j < filelen-len; j++) {
			if (i == j) {
				/* we found ourself */
				continue;
			}

			if (!memcmp(data+j, search, len)) {
				if ((j-i) < 0) {
					/* search pattern already processed previously */
					break;
				}

				printf("pattern: %s (dist = %d)\n", search, j-i);
			}
		}
	}

	return 0;
}
