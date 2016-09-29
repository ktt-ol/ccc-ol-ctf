#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define KEYLEN 9

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("%s <file> <pos>\n", argv[0]);
		return 1;
	}

	int i=0;
	int f = open(argv[1], O_RDONLY);
	if (f < 0)
		return 1;
	
	int pos = atoi(argv[2]);

	char t[5000];
	int count[26] = {0};
	while (read(f, t, 5000) > 0) {
		char *buf = t;
		for(; *buf != '\0'; i++) {
			if(i%KEYLEN == pos) {
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
