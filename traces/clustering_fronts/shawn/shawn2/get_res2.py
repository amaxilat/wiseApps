#!/usr/bin/python
import re
w = open("newdata.dat",'w')
for k in ["1","2","4","6","8"]:	
	w.write(k)
	w.write("\t")
	for path in ["1/" , "2/" ,"3/","6/","11/"]:
		exp = open(path+"shawn_"+k+"_100000.out",'r').readlines()
		events=0
		msg43=0
		msg44=0
		msg45=0
		for line in exp:
			if line.count("Present")>0:
				events+=1
			if re.match("Send::[a-z0-9]+::43",line):
				msg43+=1
			if re.match("Send::[a-z0-9]+::44",line):
				msg44+=1
			if re.match("Send::[a-z0-9]+::45",line):
				msg45+=1
		print path+"shawn_"+k+"_100000.out","Events:"+str(events),"Msg43:"+str(msg43),"Msg44:"+str(msg44),"Msg45:"+str(msg45)
		w.write(str(events))
		w.write("\t")
		w.write(str(msg43))
		w.write("\t")
		w.write(str(msg44))
		w.write("\t")
		w.write(str(msg45))
		w.write("\t")
	w.write("\n")
w.close()

