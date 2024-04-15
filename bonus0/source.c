#include <stdlib.h>
#include <string.h>

char	*p(char *s, char *str) {
	char	buffer[4096];

	puts(str);
	read(0, buffer, 4096);
	strchr(buffer, '\n') = 0;
	return (strncpy(s, buffer, 20));
}

char	*pp(char *buffer) {
	char			buffer1[20];
	char			buffer2[20];
	unsigned int	len;

	p(buffer1, " - ");
	p(buffer2, " - ");
	strcpy(buffer, buffer1);
	len = strlen(buffer);
 
	buffer[len] = ' ';
	buffer[len + 1] = 0;
	return (strcat(buffer, buffer2));
}

int		main(void) {
	char	buffer[42];

	pp(buffer);
	puts(buffer);
	return (0);
}