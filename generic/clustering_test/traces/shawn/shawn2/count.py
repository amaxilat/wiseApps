#!/usr/bin/python
import sys
#from random import random
#from random import randint
#if len(sys.argv)==3:
bidis = open(sys.argv[1],'r').readlines()
tot=0
for item in bidis:
	z = item.split()
	tot+=int(z[1])
print len(bidis) , float(tot)/len(bidis)
