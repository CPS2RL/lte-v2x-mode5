"""
Created on 7/1/2019 10:57 AM
@author: Monowar Hasan
"""


from __future__ import division
import matplotlib.pyplot as plt

import dataparser as dp


# return list of reselection SF
def get_reselection_sf_list(filename):
    rawdata = dp.get_data_by_filename(filename)
    sf_list = [float(''.join(data).split(";")[1].strip()) for data in rawdata]
    tx_list = [float(''.join(data).split(";")[0].strip()) for data in rawdata]
    tx_list = list(set(tx_list))  # get the unique TX ids
    return sf_list, tx_list


# returns the number of vehicles do reselection within this window [w1, w2)
def get_resel_count_by_window(sf_list, w1, w2):
    sfl = [s for s in sf_list if w1 <= s < w2]
    return len(sfl)


# returns the count for all sliding windows
def get_all_resel_count(sf_list, window_list, win_gap):
    count_list = []
    for w in window_list:
        w1 = w
        w2 = w + win_gap
        cnt = get_resel_count_by_window(sf_list, w1, w2)
        count_list.append(cnt)

    return count_list


# plot histogram
def plot_histogram():

    root_trace_dir = "../v2x_simulator/v2x_traces/kpi_traces/time_vs_cbr_arrival_resel/"
    trace_file_10MHz = "10MHz_HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s_NTX_ressel.txt"
    trace_file_20MHz = "20MHz_HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s_NTX_ressel.txt"

    sf_cut_off = 1500
    n_bins = 20

    title_text = "HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s_NTX_NBIN_"+str(n_bins)

    fname_10MHz = root_trace_dir + trace_file_10MHz
    fname_20MHz = root_trace_dir + trace_file_20MHz
    
    sf_list_10, tx_list_10 = get_reselection_sf_list(fname_10MHz)
    sf_list_20, tx_list_10 = get_reselection_sf_list(fname_20MHz)

    # trim a bit

    sf_list_10 = [sf for sf in sf_list_10 if sf >= sf_cut_off]
    sf_list_20 = [sf for sf in sf_list_20 if sf >= sf_cut_off]

    fig = plt.figure(1)
    fig.suptitle(title_text,  fontsize=12)

    alpha = 0.6

    # plt.hist(sf_list_10, normed=False, bins=n_bins, alpha=alpha,
    #          color='maroon', edgecolor='k', label='10 MHz Bandwidth')
    # plt.hist(sf_list_20, normed=False, bins=n_bins, alpha=alpha,
    #          color='b',  edgecolor='k', label='20 MHz Bandwidth')

    plt.hist([sf_list_10, sf_list_20], bins=n_bins, color=['maroon', 'b'], edgecolor='k',
             alpha=alpha, label=['10 MHz', '20 MHz'])

    # Set labels
    plt.xlabel('Time (MS)')
    plt.ylabel('Total Number of Reselection')

    plt.legend(loc='upper left', ncol=2)

    plt.show()


# main routine
def plot_percentage():
    root_trace_dir = "../v2x_simulator/v2x_traces/kpi_traces/time_vs_cbr_arrival_resel/"
    trace_file_10MHz = "10MHz_HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s_NTX_ressel.txt"
    trace_file_20MHz = "20MHz_HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s_NTX_ressel.txt"

    title_text = "HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s_NTX_SL100"

    fname_10MHz = root_trace_dir + trace_file_10MHz
    fname_20MHz = root_trace_dir + trace_file_20MHz

    win_min = 1500
    win_max = 3000
    win_gap = 100

    window_list = list(range(win_min, win_max + win_gap, win_gap))

    sf_list_10, tx_list_10 = get_reselection_sf_list(fname_10MHz)
    sf_list_20, tx_list_20 = get_reselection_sf_list(fname_20MHz)

    clist_10 = get_all_resel_count(sf_list_10, window_list, win_gap)
    clist_10 = [(cl / len(tx_list_10)) * 100 for cl in clist_10]

    clist_20 = get_all_resel_count(sf_list_20, window_list, win_gap)
    clist_20 = [(cl / len(tx_list_20)) * 100 for cl in clist_20]

    fig = plt.figure(1)
    fig.suptitle(title_text, fontsize=12)

    # set grid
    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=':', linewidth=0.3, which='both', alpha=0.5)

    plt.plot(window_list, clist_10, color='maroon', marker='o', linestyle='--', label='10 MHz')
    plt.plot(window_list, clist_20, color='b', marker='x', linestyle='-', label='20 MHz')

    # Set labels
    plt.xlabel('Time (MS)')
    plt.ylabel('Percentage of Vehicles\nPerform Resource Reselection (%)')

    plt.legend(loc='upper left')
    plt.show()

    # sf_cut_off = 1500
    # n_bins = 20
    # alpha = 0.6
    #
    # sf_list_10, tx_list_10 = get_reselection_sf_list(fname_10MHz)
    # sf_list_20, tx_list_10 = get_reselection_sf_list(fname_20MHz)
    #
    # # trim a bit
    #
    # sf_list_10 = [sf for sf in sf_list_10 if sf >= sf_cut_off]
    # sf_list_20 = [sf for sf in sf_list_20 if sf >= sf_cut_off]
    #
    # fig = plt.figure(1)
    # fig.suptitle("HARQ_DIS_CC_ENB_CBR_T2_50KMH_3s")
    #
    # # plt.hist(sf_list_10, normed=False, bins=n_bins, alpha=alpha,
    # #          color='maroon', edgecolor='k', label='10 MHz Bandwidth')
    # # plt.hist(sf_list_20, normed=False, bins=n_bins, alpha=alpha,
    # #          color='b',  edgecolor='k', label='20 MHz Bandwidth')
    #
    # plt.hist([sf_list_10, sf_list_20], bins=n_bins, color=['maroon', 'b'], edgecolor='k',
    #          alpha=alpha, label=['10 MHz', '20 MHz'])
    #
    # # Set labels
    # plt.xlabel('Time (MS)')
    # plt.ylabel('Total Number of Reselection')
    #
    # plt.legend(loc='upper right')

    plt.show()


if __name__ == '__main__':

    # change font to Arial
    plt.rcParams["font.family"] = "Arial"
    plt.rcParams['font.size'] = 16
    plt.rcParams['legend.fontsize'] = 15
    plt.rcParams['axes.titlesize'] = 17
    plt.rcParams['ytick.labelsize'] = 14
    plt.rcParams['xtick.labelsize'] = 14
    plt.rcParams['legend.edgecolor'] = 'k'

    # call main routine
    # plot_histogram()
    plot_percentage()

    print "Script finished!"
