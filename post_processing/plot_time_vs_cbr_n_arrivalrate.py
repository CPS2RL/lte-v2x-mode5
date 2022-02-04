"""
Created on 7/1/2019 10:11 AM
@author: Monowar Hasan
"""


from __future__ import division
import matplotlib.pyplot as plt

import dataparser as dp

g_time_cutoff = 1500  # we take traces after 1500 ms


# return dist and avg prr from the trace file
def get_cbr_arrival_xy(filename, target_tx):

    rawdata = dp.get_data_by_filename(filename)

    print "Extracting CBR..."
    cbr_list = [float(''.join(data).split(";")[2].strip()) for data in rawdata
                if int(''.join(data).split(";")[1].strip()) == target_tx and
                int(''.join(data).split(";")[0].strip()) >= g_time_cutoff]

    print "Extracting packet arrival rate..."
    pckt_arrival_list = [float(''.join(data).split(";")[3].strip()) for data in rawdata
                         if int(''.join(data).split(";")[1].strip()) == target_tx and
                         int(''.join(data).split(";")[0].strip()) >= g_time_cutoff]

    print "Extracting logging time..."
    log_time_list = [float(''.join(data).split(";")[0].strip()) for data in rawdata
                     if int(''.join(data).split(";")[1].strip()) == target_tx and
                     int(''.join(data).split(";")[0].strip()) >= g_time_cutoff]

    # print "Extracting TX Power..."
    # tx_power_list = [float(''.join(data).split(";")[4].strip()) for data in rawdata
    #                  if int(''.join(data).split(";")[1].strip()) == target_tx and
    #                  int(''.join(data).split(";")[0].strip()) >= g_time_cutoff]

    # return the values
    return log_time_list, cbr_list, pckt_arrival_list


if __name__ == '__main__':
    # change font to Arial
    plt.rcParams["font.family"] = "Arial"
    plt.rcParams['font.size'] = 16
    plt.rcParams['legend.fontsize'] = 15
    plt.rcParams['axes.titlesize'] = 17
    plt.rcParams['ytick.labelsize'] = 14
    plt.rcParams['xtick.labelsize'] = 14
    plt.rcParams['legend.edgecolor'] = 'k'

    root_trace_dir = "../v2x_simulator/v2x_traces/kpi_traces/time_vs_cbr_arrival_resel/"
    trace_file_10MHz = "10MHz_HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s_NTX_cbrNarrival.txt"
    trace_file_20MHz = "20MHz_HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s_NTX_cbrNarrival.txt"

    fname_10MHz = root_trace_dir + trace_file_10MHz
    fname_20MHz = root_trace_dir + trace_file_20MHz

    target_tx_id = 50

    time_10, cbr_10, arrival_10 = get_cbr_arrival_xy(filename=fname_10MHz, target_tx=target_tx_id)
    time_20, cbr_20, arrival_20 = get_cbr_arrival_xy(filename=fname_20MHz, target_tx=target_tx_id)

    # plot
    fig = plt.figure(1)

    fig.suptitle("HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s_VID:"+str(target_tx_id), fontsize=12)

    # plt.subplot(2, 1, 1)
    plt.plot(time_10, cbr_10, color='maroon', marker='o', linestyle='--', label='10 MHz Bandwidth')
    plt.plot(time_20, cbr_20, color='b', marker='x', linestyle='-', label='20 MHz Bandwidth')

    # set grid
    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=':', linewidth=0.3, which='both', alpha=0.5)

    # Set labels
    plt.xlabel('Time (MS)')
    plt.ylabel('CBR')
    plt.legend(loc='lower right', handletextpad=0.4, ncol=1)
    plt.show()

    fig = plt.figure(2)
    fig.suptitle("HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s_VID:" + str(target_tx_id), fontsize=12)

    # plt.subplot(2, 1, 2)

    plt.plot(time_10, arrival_10, color='maroon', marker='o', linestyle='--', label='10 MHz Bandwidth')
    plt.plot(time_20, arrival_20, color='b', marker='x', linestyle='-', label='20 MHz Bandwidth')

    # Set labels
    plt.xlabel('Time (MS)')
    plt.ylabel('Packet Arrival Rate (MS)')

    plt.legend(loc='lower right', handletextpad=0.4, ncol=1)

    # set grid
    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=':', linewidth=0.3, which='both', alpha=0.5)

    plt.legend(loc='lower right', handletextpad=0.4, ncol=1)

    # plt.savefig("dist_vs_avg_prr.pdf", pad_inches=0.1, bbox_inches='tight')
    plt.show()

    print "Script finished!"
