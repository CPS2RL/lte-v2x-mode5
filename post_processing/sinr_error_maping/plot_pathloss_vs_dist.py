# -*- coding: utf-8 -*-
"""
Created on Thu Jul 12 11:16:20 2018

@author: Monowar Hasan
"""

import numpy as np
from math import pi as M_PI
import matplotlib.pyplot as plt; 


def get_pathloss_by_dist(dist):
    
    m_antennaHeight_1 = 1.5
    m_antennaHeight_2 = 1.5
    m_carrierFrequency = 5900000000  # in Hz
    m_lambda = 299792458.0 / m_carrierFrequency;
    Lbp = abs (20 * np.log10 ((m_lambda * m_lambda) / (8 * M_PI * m_antennaHeight_1 * m_antennaHeight_2)));
    Rbp = (4 * m_antennaHeight_1 * m_antennaHeight_2) / m_lambda;
    
    loss = 0.0;

    if (dist <= Rbp):
        loss = Lbp + 6 + (20 * np.log10 (dist / Rbp));
    else:
        loss = Lbp + 6 + (40 * np.log10 (dist / Rbp));

    return loss;


def show_plot(x, y, outfilename):
    # change font to Arial
    plt.rcParams["font.family"] = "Arial"
    plt.rcParams['font.size'] = 15
    plt.rcParams['legend.fontsize'] = 13
    plt.rcParams['axes.titlesize'] = 15
    plt.rcParams['ytick.labelsize'] = 12
    plt.rcParams['xtick.labelsize'] = 12
    
    
    
    plt.plot(x, y, marker='None', linestyle='-', color = 'b')
    
    plt.grid(linestyle=':', linewidth=0.5, which='both')
    plt.ylim([50, 130])
    
    # Set labels
    plt.xlabel('Distance (m)')
    plt.ylabel('Pathloss (dB)')

    plt.savefig(outfilename, pad_inches=0.1, bbox_inches='tight')
    plt.show()
    
    

if __name__=='__main__':
    
    outfilename = 'dist_vs_pl.pdf'
    min_dist = 10   # meter
    max_dist = 1000  # meter
    dist_list = list(range(min_dist, max_dist))
    pl_list = []
    for dist in dist_list:
        pl = get_pathloss_by_dist(dist)
        pl_list.append(pl)
    
    show_plot(dist_list, pl_list, outfilename)
#    print("PL list", pl_list)
#    print("Script Finished!")