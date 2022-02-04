"""
Created on 7/23/2019 9:51 AM
@author: Monowar Hasan
"""
from __future__ import division
import dataparser as dp
import csv
import numpy as np
import os
from joblib import Parallel, delayed
import multiprocessing

g_extension = '.csv'


def get_pir_by_vehicle_pair_and_dist(all_data_list, tx_id, rx_id, dist_min, dist_max):

    if tx_id == rx_id:
        print "TX and RX Id can not be same. Aborting.."
        return None

    indx_list = [indx for indx, adl in enumerate(all_data_list)
                 if (adl.tx_id == tx_id) and (adl.rx_id == rx_id) and (dist_min <= adl.dist < dist_max)]

    if not indx_list:
        return []

    # print optional_txt, ":: Processing Range: [", \
    #     dist_min, ",", dist_max, "]" , \
    #     "\t |==> Entry:", entry_indx , "/",  \
    #     total_entry, "[", round(((entry_indx/total_entry)*100), 2), "%]"

    pckt_tx_time_list = []
    for indx in indx_list:
        adl = all_data_list[indx]

        if adl.per == 0:
            pckt_tx_time_list.append(adl.tx_time_ms)

    pir_list = np.diff(pckt_tx_time_list)
    pir_list = pir_list.tolist()  # change to list

    return pir_list


def get_pir_by_dist(all_data_list, dist_min, dist_max, tx_rx_dist_dict, optional_txt=''):

    all_vehicle_pir_list = []

    tx_rx_list = list(tx_rx_dist_dict[dist_min])  # extract and convert to list from the dict

    print "PIR:", optional_txt, "-> Reading distance range: [", dist_min, ",", dist_max, ")"
    print "\t |===> Number of TX-RX pairs:", len(tx_rx_list)

    for txrxid in tx_rx_list:
        tx_id = txrxid[0]
        rx_id = txrxid[1]

        # print "PIR:", optional_txt, "-> Reading distance range: [",
        # dist_min, ",", dist_max, "] ::", "Tx id:", tx_id, "Rx id:", rx_id

        pir_list = get_pir_by_vehicle_pair_and_dist(all_data_list,
                                                    tx_id, rx_id,
                                                    dist_min, dist_max)
        # list not empty
        if len(pir_list) > 0:
            all_vehicle_pir_list.append(pir_list)

    # flatten list
    all_vehicle_pir_list = [item for sublist in all_vehicle_pir_list for item in sublist]

    if not all_vehicle_pir_list:
        print "PIR list is empty for the distance distance range: [", dist_min, ",", dist_max, ")"

    print "===> PIR:", optional_txt, "-> Finished distance range: [", dist_min, ",", dist_max, ") ===>"

    return all_vehicle_pir_list


def write_cdf_by_dist(pir_data, dist_min, dist_max, output_path):

    # if output_path not exists, create it
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    ecdf_x, ecdf_y, ecdf_yinv, cdf_x, cdf_y, cdf_yinv = dp.get_cdf_from_trace(pir_data)

    # print "distmin", dist_min
    # print "pir_data", pir_data
    # print "ecdf_x:", ecdf_x
    # print "ecdf_y", ecdf_y
    # print "\n"

    filename = output_path + 'pir_' + str(dist_min) + '_' + str(dist_max) + g_extension

    if len(ecdf_x) != len(ecdf_y):
        print "CDF data size mismatch! Aborting..."
        return

    rows = zip(ecdf_x, ecdf_y)
    with open(filename, "w") as f:
        writer = csv.writer(f, delimiter=',', lineterminator='\n')
        f.write('PIR (ms),CDF\n')
        for row in rows:
            writer.writerow(row)


def write_cdf_n_95p_all_dist(all_data_list, tx_rx_dist_dict, distminlist, dist_gap,  cdf_dist_min_cap,
                             output_cdf_path,
                             output_percentile_filename,
                             optional_txt):

    # pir_99p_data = []

    # for indx, dist_min in enumerate(distminlist):
    #     dist_max = dist_min + dist_gap
    #     pir_data = get_pir_by_dist(all_data_list, dist_min, dist_max,
    #                                tx_rx_list)
    #
    #     if pir_data:
    #         write_cdf_by_dist(pir_data, dist_min, dist_max, output_cdf_path)
    #
    #         # save 95th percentile
    #         pir_95p = np.percentile(pir_data, 95)  # return 95th percentile
    #         pir_99p_data.append((dist_min, dist_max, pir_95p))

    # multi-core
    pir_data_list = Parallel(n_jobs=-1, backend="loky")(delayed(get_pir_by_dist)(all_data_list, dist_min,
                                                                                dist_min + dist_gap, tx_rx_dist_dict,
                                                                                optional_txt)
                                                       for dist_min in distminlist)
    if len(pir_data_list) != len(distminlist):
        raise Exception("PRR Data Size Mismatch.. Aborting")

    print optional_txt, "-> PRR Extraction complete. Calculating CDFs and 95th Percentile..."
    with open(output_percentile_filename, "w") as f:
        for indx, pir_data in enumerate(pir_data_list):
            if pir_data:
                dist_min = distminlist[indx]
                dist_max = dist_min + dist_gap
                if dist_min <= cdf_dist_min_cap:
                    write_cdf_by_dist(pir_data, dist_min, dist_max, output_cdf_path)

                # write 95th percentile
                pir_95p = np.percentile(pir_data, 95)  # return 95th percentile
                datastring = str(dist_min) + "; " + str(dist_max) + "; " + str(pir_95p) + "\n"
                f.write(datastring)

    print "Saving PIR CDFs (with distance) in the path:", output_cdf_path
    print "Saving 95th percentile PIR trace to:", output_percentile_filename


def extract_interreception_data(trace_filename, output_path, output_percentile_filename, optional_txt):
    amin = 0
    amax = 500
    cdf_dist_min_cap = 300   # cap for CDF distance
    dist_gap = 20

    pir_distminlist = list(range(amin, amax + dist_gap, dist_gap))

    all_data_list, tx_rx_dist_dict = dp.parse_per_raw_data_with_tx_rx_pair_dist_dict(trace_filename, pir_distminlist, dist_gap)

    write_cdf_n_95p_all_dist(all_data_list, tx_rx_dist_dict, pir_distminlist, dist_gap,  cdf_dist_min_cap,
                             output_path,
                             output_percentile_filename, optional_txt)


if __name__ == '__main__':

    trace_fname = "../v2x_simulator/v2x_traces/kpi_traces/v2x_default_log_per.txt"
    out_path = "../v2x_simulator/v2x_traces/kpi_traces/dist_vs_pir_cdf/v2x_default_log_per/"
    out_95p_fname = "../v2x_simulator/v2x_traces/kpi_traces/distVS95pir_v2x_default_log_per.txt"

    extract_interreception_data(trace_fname, out_path, out_95p_fname, '20MHz')

    print "Saving PIR Script finished!"
