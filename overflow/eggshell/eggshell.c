#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE_MAX 48 // Maximum size of item's data buffer
#define SIZE_STR 64 // Maximum size of the string buffer to be entered

struct item{
	char data[SIZE_MAX];
	struct item * next;
};

void spawn_shell(){
	execl("/bin/bash", "/bin/bash", "-p", NULL);
}

int main(){
	char data1[SIZE_STR], data2[SIZE_STR];
	FILE * file;
	
	file = fopen("data1.dat", "r");
	if(file){
		fread(data1, 1, SIZE_STR, file);
		fclose(file);
	}

	file = fopen("data2.dat", "r");
	if(file){
		fread(data2, 1, SIZE_STR, file);
		fclose(file);
	}

	struct item item1, item2;
	item1.next = &item2;
	item2.next = &item2;

	memcpy(item1.data, data1, SIZE_STR); // The size we are allowing it to copy is larger than the target's buffer; if we overflow this, we end up overwriting the next field of item1
	fprintf(stderr, "item1.data: %s\nitem2.data: %s\n", item1.data, item2.data); // This line is here so that item1 and item2 don't get optimized out
	memcpy(item1.next, data2, SIZE_MAX); // This is the correct size

	_IO_putc('*', stdout);
}


