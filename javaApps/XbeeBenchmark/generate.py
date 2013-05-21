#!/usr/bin/python
# needs Python Image Library (PIL)
import Image, ImageDraw
import math

# some color constants for PIL
white = (255, 255, 255)
black = (0, 0, 0)
blue = (0, 0, 255)
red = (255, 0, 0)
green = (0,128,0)

RANGE=7

width = 1024
height = 777
center = height//2
rrange = width/100
x_increment = 1
# width stretch
x_factor = 0.04
# height stretch
y_amplitude = 80

nodes = []

import random
for i in range(1,RANGE+1):
	#Open a file
	fo = open("home_"+str(i)+".data", "r")
	lines=fo.readlines()
	countreceived=0
	avgrssi=0
	rssis=[]
	for line in lines:
		parts=line.split("\t")
		rssis.append(int(line.split("\t")[2]))
		avgrssi=avgrssi+int(line.split("\t")[2])
		countreceived=countreceived+1
	#print avgrssi,countreceived
	import matplotlib.pyplot as plt
	plt.figure(figsize=(16,9))
	plt.plot(rssis,label="P"+str(i))
	plt.xlim([0,1000])
	plt.ylim([-255,0])
	plt.ylabel('rssi')
	plt.legend(bbox_to_anchor=(0.95, 1.15),ncol=4, fancybox=True, shadow=True)
	plt.savefig("home_"+str(i)+".png")
	avgrssi=-avgrssi/countreceived
	fo.close()
	fo = open("home.coords", "r")
	x_coord=0
	y_coord=0
	lines=fo.readlines()
	for line in lines:
		#print line
		parts=line.split("\t")
		if (parts[0]==str(i)):
			x_coord=int(parts[1])
			y_coord=int(parts[2])
	print x_coord,y_coord,avgrssi,countreceived
	nodes.append( (x_coord,y_coord,avgrssi))

img = Image.open("myHouse.png","r")

image1 = Image.new("RGB", (width, height), white)
image1.paste(img)

draw = ImageDraw.Draw(image1)

# do the PIL image/draw (in memory) drawings
for i in range(len(nodes)):
  draw.ellipse((nodes[i][0]-rrange, nodes[i][1]-rrange, nodes[i][0]+rrange, nodes[i][1]+rrange),fill=(255-nodes[i][2],0,0))

# PIL image can be saved as .png .jpg .gif or .bmp file
filename = "my_drawing.jpg"
image1.save(filename)
