#!/usr/bin/env/python

lines_main_begin = [
	'int main(int argc, char ** argv)',
	'{',
	'',
	'\tif(argc == 3)',
	'\t{',
	'\t\tint x = atoi(argv[1]);',
	'\t\tint y = atoi(argv[2]);',
	''
]

lines_main_end = [
	'\t}',
	'}'
]

def write_main(fileout, funcnames):
	for line in lines_main_begin:
		fileout.write(line + '\n')

	for funcname in funcnames:
		fileout.write('\t\t' + funcname + '(x, y);\n')

	for line in lines_main_end:
		fileout.write(line + '\n')
