import sys
#from random import random
#from random import randint
#if len(sys.argv)==3:
topology = open('topology.xml', 'r').readlines()
to_remove = open('tmp','r').readlines()
ids_to_remove = []
for item in to_remove:
	z = item.split()
	ids_to_remove.append(int(z[1])+3)
	print int(z[1])+3
print len(ids_to_remove)
i=0;
newtopology=[]
for item in topology:
	if i in ids_to_remove:
		print "drop"
	else :
		newtopology.append(item)
	i+=1
f = open('topology2.xml', 'w')
for item in newtopology:
	f.write(item)
#	f.write("\n")
f.close()


