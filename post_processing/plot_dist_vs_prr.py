"""
Created on 6/27/2019 9:29 AM
@author: Monowar Hasan
"""

from __future__ import division
import matplotlib.pyplot as plt

import dataparser as dp


# return dist and avg prr from the trace file
def get_prr_xy(filename):
    rawdata = dp.get_data_by_filename(filename)

    dist_min_list = []
    avg_prr_list = []

    for data in rawdata:
        datastring = ''.join(data)
        tokenized_data = datastring.split(";")

        dist_min = float(tokenized_data[0].strip())
        # dist_max = float(tokenized_data[1].strip())
        prr = float(tokenized_data[2].strip())

        dist_min_list.append(dist_min)
        avg_prr_list.append(prr)

    return dist_min_list, avg_prr_list


if __name__ == '__main__':
    # change font to Arial
    plt.rcParams["font.family"] = "Arial"
    plt.rcParams['font.size'] = 16
    plt.rcParams['legend.fontsize'] = 15
    plt.rcParams['axes.titlesize'] = 17
    plt.rcParams['ytick.labelsize'] = 14
    plt.rcParams['xtick.labelsize'] = 14
    plt.rcParams['legend.edgecolor'] = 'k'

    root_trace_dir = "../v2x_simulator/v2x_traces/kpi_traces/dist_vs_prr/"
    trace_file_10MHz = "distVSprr_10MHz_HARQ_DIS_RC_ENB_PC_ENB_CBR_T2_50KMH_3s_500TX_per.txt"
    trace_file_20MHz = "distVSprr_20MHz_HARQ_DIS_RC_ENB_PC_ENB_CBR_T2_50KMH_3s_500TX_per.txt"

    title_text = "HARQ_DIS_RC_ENB_PC_ENB_CBR_T2_W_IBE_50KMH_3s_500TX_AoITrim"

    fname_10MHz = root_trace_dir + trace_file_10MHz
    fname_20MHz = root_trace_dir + trace_file_20MHz

    dist_10, avg_prr_10 = get_prr_xy(fname_10MHz)
    dist_20, avg_prr_20 = get_prr_xy(fname_20MHz)

    # plot dist vs average PRR
    fig = plt.figure(1)
    fig.suptitle(title_text, fontsize=12)

    # Set labels
    plt.xlabel('Distance (m)')
    plt.ylabel('Average Packet Reception Ratio')

    # set grid
    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=':', linewidth=0.3, which='both', alpha=0.5)

    plt.ylim([-0.05, 1.05])

    plt.plot(dist_10, avg_prr_10, color='maroon', marker='o', linestyle='--', label='10 MHz')
    plt.plot(dist_20, avg_prr_20, color='b', marker='x', label='20 MHz')

    plt.legend(loc='upper right', handletextpad=0.4, ncol=1)

    # plt.savefig("dist_vs_avg_prr.pdf", pad_inches=0.1, bbox_inches='tight')
    plt.show()

    print "Script finished!"
