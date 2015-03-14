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

