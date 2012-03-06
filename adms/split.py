#! /usr/bin/env python

fin = open("gnucap.xml")

fcnt = 0
fopened = 0
fout = 0

for line in fin:
	if( fopened == 0 ):
		fopened = 1
		fout = open("gnucap_"+str(fcnt)+".xml", "w")
		fcnt = fcnt + 1
	
	fout.write(line)
	if( line == "</admst>\n" ):
		fout.close()
		fopened = 0
