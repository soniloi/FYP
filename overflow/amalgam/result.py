#!/usr/bin/env/python

# Result bundle
class result:

	def __init__(self):
		self.smashed = False
		self.size = 0
		self.instructions_retired = 0
		self.stack_use = 0
		self.heap_use = 0

	def to_string(self):
		return '\tSmashed? ' + str(self.smashed) + '\tBinary size: ' + str(self.size) + '\tInstructions retired: ' + str(self.instructions_retired) + '\tStack usage: ' + str(self.stack_use) + '\tHeap usage: ' + str(self.heap_use)

