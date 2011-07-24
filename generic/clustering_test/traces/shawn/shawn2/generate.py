#!/usr/bin/python
import sys
from random import random
from random import randint
if len(sys.argv)==3:
	dimension = int(sys.argv[1])
	nodes_per_tile=	int(sys.argv[2])
	total=0
	f = open('topology.xml', 'w')
	nodes =[]

	f.write("<?xml version=\"1.0\" ?>\n\t<scenario>\n\t\t<snapshot id=\"0\">\n")
	print dimension,nodes_per_tile	

	for y in range(0,dimension,1):
		for x in range(0,dimension,1):
			t_x = float(x)*1
			t_y = float(y)*1
			for t_node in range(nodes_per_tile):
				c_x =t_x+random()
				c_y =t_y+random()
#				print t_x,t_y,c_x,c_y
				nodes.append(("<node id=\""+str(randint(1,1000000))+"-QOP-C\">","<location x=\""+str(c_x)+"\" y=\""+str(c_y)+"\" z=\"0\" />"))
				total+=1
	nodes.sort()
	for (id,coord) in nodes:
		f.write("\t\t\t")
		f.write(id)
		f.write(coord)
		f.write("</node>\n")
	f.write("\n\t\t</snapshot>\n\t</scenario>\n")
	f.close()
	
	print total

