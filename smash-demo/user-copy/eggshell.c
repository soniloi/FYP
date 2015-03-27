#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE_MAX 48 // Maximum size of item's data buffer
#define SIZE_BUFFER 56 // Maximum size of the string buffer to be entered

struct item{
	char data[SIZE_MAX];
	struct item * next;
};

void spawn_shell(){
	execl("/bin/bash", "/bin/bash", "-p", NULL);
}

int main(int argc, char ** argv){
	char * filename1 = argv[1];
	char * filename2 = argv[2];

	FILE * file;
	char buffer1[SIZE_BUFFER], buffer2[SIZE_BUFFER];

	file = fopen(filename1, "r");
	if(file){
		fread(buffer1, 1, SIZE_BUFFER, file);
		fclose(file);
	}

	file = fopen(filename2, "r");
	if(file){
		fread(buffer2, 1, SIZE_BUFFER, file);
		fclose(file);
	}

	struct item item1, item2;
	item1.next = &item2;
	item2.next = &item1;

	memcpy(item1.data, buffer1, SIZE_BUFFER); // Potential for overflow!!
	memcpy(item1.next, buffer2, SIZE_MAX); // This is the correct size

	_IO_putc('*', stdout);
}


