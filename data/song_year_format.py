#!/usr/bin/env python

infile = open("YearPredictionMSD.txt","r")
outfile = open("songs_test.txt","w")
lines = 0
for line in infile:
    if lines < 30000:
        lines += 1
        continue
    data = line.rstrip().split(",")
    if int(data[0]) >= 2000:
        outfile.write("1")
    else:
        outfile.write("0")
    for i in range(1, len(data)):
        outfile.write(" {}:{}".format(i, data[i]))
    outfile.write("\n");
    lines += 1
    if lines == 40000:
        break

infile.close()
outfile.close()
