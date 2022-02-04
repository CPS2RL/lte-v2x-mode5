"""
Created on 6/27/2019 9:29 AM
@author: Monowar Hasan
"""

from __future__ import division

from joblib import Parallel, delayed
import multiprocessing

import dataparser as dp

# import error map module
import sys
sys.path.insert(0, 'sinr_error_maping')
import sinrerrormap as sem  # noqa: E402

# some global variables
g_mapping_string = "v_{Rx}=60km/h,v_{Tx}=60km/h"
g_map_filename = 'sinr_error_maping/snr_error_mapping_variables.mat'
# mpl.use('Agg')  # to plot script run without display


def get_total_num_vehicle_with_succ_rec(mapping_string, map_filename, all_data_list, tx_sf, tx_id, dist_min,
                                        dist_max):
    num_vehicle_succ = 0
    total_num_vehicle = 0

    indx_list = [indx for indx, adl in enumerate(all_data_list)
                 if (adl.tx_time_ms == tx_sf) and (adl.tx_id == tx_id) and (dist_min <= adl.dist < dist_max)]

    # print "indx_list size:", len(indx_list)

    for indx in indx_list:
        adl = all_data_list[indx]

        total_num_vehicle += 1

        per = sem.get_error_rate(mapping_string, map_filename, adl.sinr)
        # print "TX SF:", tx_sf, "Dist:", adl.dist, "Sinr: ", adl.sinr, "Per:", per
        if per == 0.0:
            num_vehicle_succ += 1

    #    print "TX SF:", tx_sf, "Y:", total_num_vehicle, "X:", num_vehicle_succ
    return total_num_vehicle, num_vehicle_succ


# return average PRR for a given distance bound
def calculate_avg_prr_by_dist(all_data_list, dist_min, dist_max, optional_txt=''):

    if dist_max < dist_min:
        print "dist_max is less than dist_min. Aborting..."
        return None

    prr_x = 0
    prr_y = 0

    tx_sf_list_cond = [(adl.tx_time_ms, adl.tx_id) for adl in all_data_list if dist_min <= adl.dist < dist_max]
    tx_sf_list_cond = list(set(tx_sf_list_cond))  # get unique values and change to list

    print "PRR:", optional_txt, "-> Reading distance range: [", dist_min, ",", dist_max, ")"
    print "\t |===> Number of TX packets:", len(tx_sf_list_cond)

    for txsfl in tx_sf_list_cond:
        tx_sf = txsfl[0]
        tx_id = txsfl[1]
        total_num_vehicle, num_vehicle_succ = get_total_num_vehicle_with_succ_rec(
                                        g_mapping_string, g_map_filename,
                                        all_data_list, tx_sf, tx_id, dist_min,
                                        dist_max)

        prr_x += num_vehicle_succ
        prr_y += total_num_vehicle

    if prr_y == 0:
        print "No vehicle in the distance range: [", dist_min, ",", dist_max, "]"
        return None

    average_prr = prr_x/prr_y

    # print "Distance range: [", dist_min, ",", dist_max, "]"
    # print "Tot:", prr_y
    # print "Succ:", prr_x
    # print "Average PRR:", average_prr

    return average_prr

# # single core version
# # calculate AVG PRR for all distance bins
# def calculate_avg_prr_for_all_dist(all_data_list, distminlist, distgap, optional_txt=''):
#     prr_list = []
#     for dist_min in distminlist:
#         dist_max = dist_min + distgap
#         avg_prr = calculate_avg_prr_by_dist(all_data_list, dist_min, dist_max, optional_txt)
#         prr_list.append(avg_prr)
#
#     return prr_list


# multi-core version
# calculate AVG PRR for all distance bins
def calculate_avg_prr_for_all_dist(all_data_list, distminlist, dist_gap, optional_txt=''):
    # num_cores = multiprocessing.cpu_count()

    # backend: "loky"  or "multiprocessing" 
    # n_jobs = -1 means take all cpu

    prr_list = Parallel(n_jobs=-1, backend="loky")(delayed(calculate_avg_prr_by_dist)
                                          (all_data_list, dist_min, dist_min + dist_gap, optional_txt)
                                          for dist_min in distminlist)

    return prr_list


# write to file
def write_prr_trace_to_file(output_filename, dist_list, dist_gap, avg_prr):

    # some error checking
    if len(dist_list) != len(avg_prr):
        print "Data Size mismatch while saving PRR! Aborting..."
        return

    print "Saving PRR trace to:", output_filename

    data = zip(dist_list, avg_prr)

    with open(output_filename, "w") as f:

        for d in data:
            dist_min = d[0]
            dist_max = dist_min + dist_gap
            prr = d[1]

            datastring = str(dist_min) + "; " + str(dist_max) + "; " + str(prr) + "\n"
            f.write(datastring)


if __name__ == '__main__':

    root_trace_dir = "../v2x_simulator/v2x_traces/kpi_traces/"
    in_trace_file_10MHz = "10MHz_HARQ_DIS_CC_DIS_CBR_T2_50KMH_3s_NTX_sinr_min.txt"
    in_trace_file_20MHz = "20MHz_HARQ_DIS_CC_DIS_CBR_T2_50KMH_3s_NTX_sinr_min.txt"

    out_trace_file_10MHz = "distVSprr_10MHz_HARQ_DIS_CC_DIS_CBR_T2_50KMH_3s_NTX_sinr.txt"
    out_trace_file_20MHz = "distVSprr_20MHz_HARQ_DIS_CC_DIS_CBR_T2_50KMH_3s_NTX_sinr.txt"

    amin = 0
    avgprr_amax = 500
    distgap = 20

    in_fname_10MHz = root_trace_dir + in_trace_file_10MHz
    in_fname_20MHz = root_trace_dir + in_trace_file_20MHz

    out_fname_10MHz = root_trace_dir + out_trace_file_10MHz
    out_fname_20MHz = root_trace_dir + out_trace_file_20MHz

    avgprr_distminlist = list(range(amin, avgprr_amax + distgap, distgap))
    
    # do for 10 MHz
    data_10MHz, _ = dp.parse_raw_data_with_tx_packet_sf(in_fname_10MHz)
    avg_prr_10MHz = calculate_avg_prr_for_all_dist(data_10MHz, avgprr_distminlist, distgap, '10MHz')
    write_prr_trace_to_file(output_filename=out_fname_10MHz, dist_list=avgprr_distminlist,
                        dist_gap=distgap, avg_prr=avg_prr_10MHz)

    # # do for 20 MHz
    # data_20MHz, _ = dp.parse_raw_data_with_tx_packet_sf(in_fname_20MHz)
    # avg_prr_20MHz = calculate_avg_prr_for_all_dist(data_20MHz, avgprr_distminlist, distgap, '20MHz')
    # write_prr_trace_to_file(output_filename=out_fname_20MHz, dist_list=avgprr_distminlist,
    #                         dist_gap=distgap, avg_prr=avg_prr_20MHz)



    print "Saving PRR Script finished!"
