#!/usr/bin/env/python

# Result bundle
class result:

	def __init__(self):
		self.smashed = False
		self.metrics = {}

	def to_string(self):
		st = '\tsmashed? ' + str(self.smashed)
		for metricname, metricvalue in self.metrics.iteritems():
			st += '\t' + metricname + ': ' + str(metricvalue)
		return st
		#return '\tSmashed? ' + str(self.smashed) + '\tBinary size: ' + str(self.size) + '\tInstructions retired: ' + str(self.instructions_retired) + '\tStack usage: ' + str(self.stack_use) + '\tHeap usage: ' + str(self.heap_use)

