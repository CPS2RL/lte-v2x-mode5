"""
Created on 7/9/2019 9:35 AM
@author: Monowar Hasan
"""

from joblib import Parallel, delayed
import multiprocessing
import dataparser as dp
import numpy as np
from collections import defaultdict
import matplotlib.pyplot as plt

g_time_cutoff = 1500  # we take traces after 1500 ms


# return all the TX ids
def get_tx_ids_from_trace(rawdata):

    print "Extracting TX IDs..."
    tx_list = [int(''.join(data).split(";")[1].strip()) for data in rawdata]
    tx_list = dp.get_unique_values(tx_list)
    return tx_list


# return dist and avg prr from the trace file
def get_cbr_arrival_pow_by_tx_id(rawdata, target_tx, optional_text):

    print optional_text, ":: Extracting CBR..."
    cbr_list = [float(''.join(data).split(";")[2].strip()) for data in rawdata
                if int(''.join(data).split(";")[1].strip()) == target_tx and
                int(''.join(data).split(";")[0].strip()) >= g_time_cutoff]

    print optional_text, ":: Extracting packet arrival rate..."
    pckt_arrival_list = [float(''.join(data).split(";")[3].strip()) for data in rawdata
                         if int(''.join(data).split(";")[1].strip()) == target_tx and
                         int(''.join(data).split(";")[0].strip()) >= g_time_cutoff]

    # print optional_text, ":: Extracting logging time..."
    # log_time_list = [float(''.join(data).split(";")[0].strip()) for data in rawdata
    #                  if int(''.join(data).split(";")[1].strip()) == target_tx]

    # print optional_text, ":: Extracting TX Power..."
    # tx_power_list = [float(''.join(data).split(";")[4].strip()) for data in rawdata
    #                  if int(''.join(data).split(";")[1].strip()) == target_tx and
    #                  int(''.join(data).split(";")[0].strip()) >= g_time_cutoff]

    # return the values (average over simulation duration)

    # contains mean arrival rate, cbr etc of all vehicles
    outdict = defaultdict(list)
    outdict['cbr_list'] = np.mean(cbr_list)
    outdict['arrival_rate_list'] = np.mean(pckt_arrival_list)

    # return np.mean(cbr_list), np.mean(pckt_arrival_list)
    # return np.mean(pckt_arrival_list)
    return outdict


# returns for all vehicle
def get_all_cbr_arrival_pow(filename, optional_text=''):
    rawdata = dp.get_data_by_filename(filename=filename)
    tx_id_list = get_tx_ids_from_trace(rawdata=rawdata)

    num_cores = multiprocessing.cpu_count()
    outdict = Parallel(n_jobs=num_cores)(delayed(get_cbr_arrival_pow_by_tx_id)(rawdata, target_tx_id, optional_text)
                                        for target_tx_id in tx_id_list)

    cbr_list = []
    arrival_list = []
    for od in outdict:
        cbr_list.append(od['cbr_list'])
        arrival_list.append(od['arrival_rate_list'])

    return tx_id_list, cbr_list, arrival_list


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

    title_text = 'HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s_NTX'

    fname_10MHz = root_trace_dir + trace_file_10MHz
    fname_20MHz = root_trace_dir + trace_file_20MHz

    tx_list_10, cbr_list_10, arrival_list_10 = get_all_cbr_arrival_pow(filename=fname_10MHz, optional_text='10MHz')
    tx_list_20, cbr_list_20, arrival_list_20 = get_all_cbr_arrival_pow(filename=fname_20MHz, optional_text='20MHz')

    bar_width = 0.1
    opacity = 0.8

    # index = np.arange(len(tx_list_10))

    # plot dist vs average PRR
    fig = plt.figure(1)
    fig.suptitle(title_text, fontsize=12)

    plt.subplot(2, 1, 1)
    b1 = plt.bar(tx_list_10, arrival_list_10, bar_width,
                     alpha=opacity,
                     color='maroon',
                     label='10 MHz')
    # set grid
    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=':', linewidth=0.3, which='both', alpha=0.5)

    plt.legend(loc='upper right', handletextpad=0.4, ncol=1)

    plt.xlabel("Vehicle ID")
    plt.ylabel("Packet Arrival Rate")

    plt.subplot(2, 1, 2)
    b2 = plt.bar(tx_list_20, arrival_list_20, bar_width,
                 alpha=opacity,
                 color='b',
                 label='20 MHz')
    # set grid
    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=':', linewidth=0.3, which='both', alpha=0.5)

    plt.legend(loc='upper right', handletextpad=0.4, ncol=1)

    plt.xlabel("Vehicle ID")
    plt.ylabel("Packet Arrival Rate")

    plt.show()

