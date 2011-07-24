#!/usr/bin/python
import re
for path in ["1/","2/","3/","6/","11/"]:
	w = open(path+"per_k2.dat",'w')
	for size in ["1000" , "10000" , "100000"]:
		w.write(size)
		w.write("\t")
		for k in ["1" , "2" ,"4","6","8"]:
			exp = open(path+"shawn_"+k+"_"+size+".out",'r').readlines()	
			clusters=0
			simple=0
			msgs=0
			for line in exp:
				if line.count("Send")>0:
					msgs+=1
				if re.match("Clusters::[a-z0-9]+::2",line):
					clusters+=1
				if re.match("Clusters::[a-z0-9]+::1",line):
					simple+=1
			avgsize = float(simple)/clusters
			clusters = int(float(size)/float(avgsize))
			print path+"shawn_"+k+"_"+size+".out","Msgs:"+str(msgs),"Clusters:"+str(clusters),"size:"+str(avgsize)
			w.write(str(msgs))
			w.write("\t")
			w.write(str(clusters))
			w.write("\t")
			w.write(str(avgsize))
			w.write("\t")
		w.write("\n")
	w.close()

