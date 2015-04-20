#!/usr/bin/env python
import random

max_statements = 3 # Maximum number of statements in a basic block
new_block_chance = 1 # Starting at one in this many statements, a new basic block will be opened
elsepart_chance = 2 # One in this many if-statements will not be followed by an else-part
max_depth = 4 # Cannot nest any deeper than this

operators_arith = ['+', '-', '*']
operators_bool = ['==', '!=', '<', '>', '<=', '>=']
operands = ['z', 'y', 'x']

def get_spacing(depth):
	spacing = ''
	for i in range(0, depth):
		spacing += '\t'
	return spacing

def get_closing(depth):
	clo = get_spacing(depth)
	clo += '}'
	return clo

def get_statement(depth):
	stat = get_spacing(depth)
	stat += operands[random.randint(0, len(operands)-1)] + ' = '
	stat += operands[random.randint(0, len(operands)-1)] + ' '
	stat += operators_arith[random.randint(0, len(operators_arith)-1)] + ' '
	stat += operands[random.randint(0, len(operands)-1)] + ';'
	return stat

def get_condition(depth):
	cond = get_spacing(depth)
	cond += ('if(')
	cond += operands[random.randint(0, len(operands)-1)] + ' '
	cond += operators_bool[random.randint(0, len(operators_bool)-1)] + ' '
	cond += operands[random.randint(0, len(operands)-1)] + '){'
	return cond

def get_else(depth):
	els = get_spacing(depth)
	els += 'else{'
	return els

def get_basic_block(statements_sofar, depth):
	for i in range(0, random.randint(1, max_statements)):
		statement = get_statement(depth)
		statements_sofar.append(statement)
		if depth < max_depth and random.randint(0, depth*new_block_chance-1) == 0:
			statements_sofar.append('')
			statements_sofar.append(get_condition(depth))
			get_basic_block(statements_sofar, depth+1)
			statements_sofar.append(get_closing(depth))
			if(random.randint(0, elsepart_chance-1) != 0):
				statements_sofar.append(get_else(depth))
				get_basic_block(statements_sofar, depth+1)
				statements_sofar.append(get_closing(depth))
			statements_sofar.append('')

def generate_function(funcname, seed):
	random.seed(seed)
	lines = []
	lines.append('/* function auto-generated using seed: ' + str(seed) + ' */')
	lines.append('void ' + funcname + '(int x, int y){')
	lines.append('\tint z = 1;')

	get_basic_block(lines, 1)

	lines.append('')
	lines.append('\tprintf("%d", z);')
	lines.append(get_closing(0))

	return lines
