# -*- coding: utf-8 -*-

# Created on Tue Aug  2 13:7:59 2018
# @author: Monowar Hasan

# NS2 mobility file format:
# The start-position and future destinations for a mobilenode
# may be set by using the following APIs:

# $node set X_ \<x1\>
# $node set Y_ \<y1\>
# $node set Z_ \<z1\>

# $ns at $time $node setdest \<x2\> \<y2\> \<speed\>
# At $time sec, the node would start moving from its
# initial position of (x1,y1)
# towards a destination (x2,y2) at the defined speed.



# NOTE: the time in NS2 mobility format is always in secods. Our NS3 simulator
# uses timescale in Milliseconds (to match with subframe length)


from __future__ import print_function  # to support print
import os
import re
import sys


# add the text _shifted with the input filename
def get_output_filename(input_filename):
    filename, file_extension = os.path.splitext(input_filename)
    output_filename = filename + '_shifted' + file_extension
    return output_filename


def get_first_offset(filename):
     # read and parse file
    with open(filename) as f:
        content = f.read().splitlines()

    for line in content:

        # extract timing info
        if 'at' in line:
            tm = re.findall(r'\d+', line)[:1]  # Get the first item (the time param)
            timeval = int(tm[0])  # first entry
            return timeval


def update_line_by_ofset(inputstring, offset):

    digits = re.findall("\d+\.\d+", inputstring)
    oldtime = 'at ' + digits[0]  # this is the first entry
    newtime = 'at ' + str(float(digits[0]) - offset)  # this is the first entry

    outputstring = inputstring.replace(oldtime, newtime)
    return outputstring



def update_by_shifting(inputfilename, outputfilename, offset):

    with open(inputfilename, 'r') as f:
        lines = f.readlines()
        newlines = []
        for line in lines:
            if 'at' in line:
                l2 = update_line_by_ofset(line, offset)
                newlines.append(l2)
                # print(l2)
            else:
                newlines.append(line)


    with open(outputfilename, 'w') as f:
        f.writelines(newlines)


def do_shifting(input_filename):

    output_filename= get_output_filename(input_filename)
    offset = get_first_offset(input_filename)
    ofsetstr = "Ofset is: " + str(offset)
    print(ofsetstr)

    update_by_shifting(input_filename, output_filename, offset)

if __name__=='__main__':

    # input_filename='sumo_sim/v2xsim_ns3.tcl'
    if  len(sys.argv)!=2 :
        print("Invalid arguments! Run as python ns3_trace_shifter.py input_filename")
        exit(1)

    input_filename = sys.argv[1]

    do_shifting(input_filename)
    print("Shifting complete. The ouput file is saved in:")
    print(get_output_filename(input_filename))

    # print("Script Finished!")
