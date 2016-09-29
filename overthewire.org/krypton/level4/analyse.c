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
		printf("%s <file> <pos>\n", argv[0]);
		return 1;
	}

	int pos = atoi(argv[2]);

	int f = open(argv[1], O_RDONLY);
	if (f < 0)
		return 1;

	char temp[4096] = {0};
	char *buf = temp;
	int count[26] = {0};
	while (read(f, buf, 4096) > 0) {
		for(; *buf != '\0'; i++) {
			if(i%6 == pos) {
				*buf -= 'A';
				count[*buf]++;
			}
			buf++;
		}
	}

	for(i=0; i<26; i++)
		printf("%4d %c\n", count[i], 'A'+i);

	return 0;
}
