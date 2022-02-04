# -*- coding: utf-8 -*-
"""
Created on Thu Jul  5 10:20:10 2018

@author: Monowar Hasan
"""


"""
NS2 mobility file format:
The start-position and future destinations for a mobilenode
may be set by using the following APIs:

$node set X_ \<x1\>
$node set Y_ \<y1\>
$node set Z_ \<z1\>

$ns at $time $node setdest \<x2\> \<y2\> \<speed\>
At $time sec, the node would start moving from its
initial position of (x1,y1)
towards a destination (x2,y2) at the defined speed.
"""

"""
NOTE: the time in NS2 mobility format is always in secods. Our NS3 simulator
uses timescale in Milliseconds (to match with subframe length)
"""

import os
import re


def do_parsing (filename):

    if not os.path.isfile(filename):
        print("Unable to find file in the location")
        return

    nodelist = set()  # create an epty set
    pattern = re.compile(r'node_\(.*?\)')
    timelist = set()

     # read and parse file
    with open(filename) as f:
        content = f.read().splitlines()

    for line in content:

        # extract timing info
        if 'at' in line:
            tm = re.findall(r'\d+', line)[:1]  # Get the first item (the time param)
            timelist.add(int(tm[0]))  # first entry

        # extract node information
        for match in re.findall(pattern, line):
            nids = re.findall(r"\d+", match)
            for nid in nids:
                nodelist.add(int(nid))

    nodelist = list(nodelist)
    timelist = list(timelist)

    num_node = len(nodelist)
    sim_duration = max(timelist)

    return num_node, sim_duration


if __name__=='__main__':

    filename='cross1.tcl'
#    filename='box1l_ns2mobility.tcl'

    num_node, sim_duration = do_parsing(filename)

    print("Number of vehicles:", num_node)
    print("Simulation Duriation (sec.):", sim_duration)

    print("Script Finished!")
