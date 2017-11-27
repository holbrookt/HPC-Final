#!/usr/bin/env python

infile = open("YearPredictionMSD.txt","r")
outfile = open("songs_test.txt","w")
lines = 0
for line in infile:
    if lines < 50000:
        continue
    data = line.rstrip().split(",")
    if int(data[0]) >= 2000:
        outfile.write("1")
        print "{} {}".format(data[0], 1)
    else:
        outfile.write("0")
        print "{} {}".format(data[0], 0)
    for i in range(1, len(data)):
        outfile.write(" {}:{}".format(i, data[i]))
    outfile.write("\n");
    lines += 1
    if lines == 60000:
        break

infile.close()
outfile.close()
