#!/usr/bin/env python

infile = open("cuse.raw","r")
outfile = open("cuse","w")
for line in infile:
    data = line.rstrip().split(" ")
    for i in range(0,int(data[4])):
        outfile.write("{} 1:{} 2:{} 3:{}\n".format(data[3], float(data[0])/float(4), data[1], data[2]))
infile.close()
outfile.close()
