#!/usr/bin/env/python
import random

max_statements = 20 # Maximum number of statements in a basic block
operators = ['+', '-', '*']
operands = ['z', 'y', 'x']

def get_spacing(depth):
	spacing = ''
	for i in range(0, depth):
		spacing += '\t'
	return spacing

def get_random_statement():
	stat = ''
	stat += operands[random.randint(0, len(operands)-1)] + ' = '
	stat += operands[random.randint(0, len(operands)-1)] + ' '
	stat += operators[random.randint(0, len(operators)-1)] + ' '
	stat += operands[random.randint(0, len(operands)-1)] + ';'
	return stat

def get_basic_block(statements_sofar, depth):
	for i in range(0, random.randint(0, max_statements)):
		statement = get_spacing(depth) + get_random_statement()
		statements_sofar.append(statement)

def generate_function(funcname, seed):
	random.seed(seed)
	lines = []
	lines.append('void ' + funcname + '(int x, int y){')
	lines.append('\tint z = 0;')

	get_basic_block(lines, 1)

	lines.append('}')

	return lines
