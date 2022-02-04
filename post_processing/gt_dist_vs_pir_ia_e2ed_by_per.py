"""
Created on 7/24/2019 10:17 AM
@author: Monowar Hasan
"""
from __future__ import division
import dataparser as dp
import csv
import numpy as np
import os
from joblib import Parallel, delayed
# import multiprocessing
import copy

# some global variables
g_extension = '.csv'
g_Rx_App_Tperiod = 100  # 100 ms
# g_sim_duration = 5000  # ms


class PIRIAE2EDClass:

    #  the class contains list to return from multi-core version

    def __init__(self, all_vehicle_pir_list, all_vehicle_ia_list, all_vehicle_e2ed_list):
        self.all_vehicle_pir_list = all_vehicle_pir_list  # PIR
        self.all_vehicle_ia_list = all_vehicle_ia_list  # IA
        self.all_vehicle_e2ed_list = all_vehicle_e2ed_list  # E2E Delay


def get_ia_from_arrival_time(sim_duration, arrival_time_list, start_time=0):
    rx_firing_time_list = list(range(start_time,
                                     int(round(start_time + sim_duration + g_Rx_App_Tperiod)),
                                     g_Rx_App_Tperiod))

    ia_list = []
    for idx in range(len(arrival_time_list) - 1):
        rxt_list = []
        for rxt in rx_firing_time_list:
            if arrival_time_list[idx] <= rxt < arrival_time_list[idx + 1]:

                # print "arrival time: ", arrival_time_list[idx], "rxt: ", rxt

                # last entry
                if idx == len(arrival_time_list) - 2:
                    next_firing_time = rxt + g_Rx_App_Tperiod
                    rxt_list.append(next_firing_time)
                else:  # otherwise
                    rxt_list.append(rxt)

        if rxt_list:
            ia = max(rxt_list) - arrival_time_list[idx]
            # print "arrival time: ", arrival_time_list[idx], "ia: ", ia
            if ia >= 0:  # just a sanity check
                ia_list.append(ia)

    return ia_list


def get_pir_ia_e2ed_by_vehicle_pair_and_dist(sim_duration, all_data_list, tx_id, rx_id, dist_min, dist_max):

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
    pckt_arrival_time_list = []
    e2ed_list = []

    for indx in indx_list:
        adl = all_data_list[indx]

        if adl.per == 0:
            pckt_tx_time_list.append(adl.tx_time_ms)
            pckt_arrival_time_list.append(adl.arrival_time_ms)
            e2ed_list.append(adl.tx_time_ms - adl.arrival_time_ms)

    pir_list = np.diff(pckt_tx_time_list)
    pir_list = pir_list.tolist()  # change to list

    ia_list = get_ia_from_arrival_time(sim_duration=sim_duration, arrival_time_list=pckt_arrival_time_list,
                                       start_time=0)

    return pir_list, ia_list, e2ed_list


def get_pir_ia_e2ed_by_dist(sim_duration, all_data_list, dist_min, dist_max, tx_rx_dist_dict, optional_txt=''):

    all_vehicle_pir_list = []
    all_vehicle_ia_list = []
    all_vehicle_e2ed_list = []

    tx_rx_list = list(tx_rx_dist_dict[dist_min])  # extract and convert to list from the dict

    print "PIR-IA-E2ED:", optional_txt, "-> Reading distance range: [", dist_min, ",", dist_max, ")"
    print "\t |===> Number of TX-RX pairs:", len(tx_rx_list)

    for txrxid in tx_rx_list:
        tx_id = txrxid[0]
        rx_id = txrxid[1]

        # print "PIR:", optional_txt, "-> Reading distance range: [",
        # dist_min, ",", dist_max, "] ::", "Tx id:", tx_id, "Rx id:", rx_id

        pir_list, ia_list, e2ed_list = get_pir_ia_e2ed_by_vehicle_pair_and_dist(sim_duration, all_data_list,
                                                                                tx_id, rx_id, dist_min, dist_max)

        # list not empty
        if len(pir_list) > 0:
            all_vehicle_pir_list.append(pir_list)
        if len(ia_list) > 0:
            all_vehicle_ia_list.append(ia_list)
        if len(e2ed_list) > 0:
            all_vehicle_e2ed_list.append(e2ed_list)

    # flatten list
    all_vehicle_pir_list = [item for sublist in all_vehicle_pir_list for item in sublist]
    all_vehicle_ia_list = [item for sublist in all_vehicle_ia_list for item in sublist]
    all_vehicle_e2ed_list = [item for sublist in all_vehicle_e2ed_list for item in sublist]

    # some logging info
    if not all_vehicle_pir_list:
        print "PIR list is empty for the distance distance range: [", dist_min, ",", dist_max, ")"

    if not all_vehicle_ia_list:
        print "IA list is empty for the distance distance range: [", dist_min, ",", dist_max, ")"

    if not all_vehicle_e2ed_list:
        print "E2E Delay list is empty for the distance distance range: [", dist_min, ",", dist_max, ")"

    print "<=== PIR-IA-E2ED:", optional_txt, "-> Finished distance range: [", dist_min, ",", dist_max, ") ===>"

    retcls = PIRIAE2EDClass(all_vehicle_pir_list, all_vehicle_ia_list, all_vehicle_e2ed_list)

    # return all_vehicle_pir_list, all_vehicle_ia_list
    return retcls


def write_pir_cdf_by_dist(pir_data, dist_min, dist_max, output_path):

    # if output_path not exists, create it
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    ecdf_x, ecdf_y, ecdf_yinv, cdf_x, cdf_y, cdf_yinv = dp.get_cdf_from_trace(pir_data)

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


def write_ia_cdf_by_dist(ia_data, dist_min, dist_max, output_path):

    # if output_path not exists, create it
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    ecdf_x, ecdf_y, ecdf_yinv, cdf_x, cdf_y, cdf_yinv = dp.get_cdf_from_trace(ia_data)

    filename = output_path + 'ia_' + str(dist_min) + '_' + str(dist_max) + g_extension

    if len(ecdf_x) != len(ecdf_y):
        print "CDF data size mismatch! Aborting..."
        return

    rows = zip(ecdf_x, ecdf_y)
    with open(filename, "w") as f:
        writer = csv.writer(f, delimiter=',', lineterminator='\n')
        f.write('IA (ms),CDF\n')
        for row in rows:
            writer.writerow(row)


def write_e2ed_cdf_by_dist(ia_data, dist_min, dist_max, output_path):

    # if output_path not exists, create it
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    ecdf_x, ecdf_y, ecdf_yinv, cdf_x, cdf_y, cdf_yinv = dp.get_cdf_from_trace(ia_data)

    filename = output_path + 'e2elat_' + str(dist_min) + '_' + str(dist_max) + g_extension

    if len(ecdf_x) != len(ecdf_y):
        print "CDF data size mismatch! Aborting..."
        return

    rows = zip(ecdf_x, ecdf_y)
    with open(filename, "w") as f:
        writer = csv.writer(f, delimiter=',', lineterminator='\n')
        f.write('E2E Latency (ms),CDF\n')
        for row in rows:
            writer.writerow(row)


def write_cdf_n_95p_all_dist(sim_duration, all_data_list, tx_rx_dist_dict, distminlist, dist_gap, cdf_dist_min_cap,
                             output_cdf_path_pir,
                             output_percentile_filename_pir,
                             output_cdf_path_ia,
                             output_percentile_filename_ia,
                             output_cdf_path_e2ed, output_percentile_filename_e2ed,
                             optional_txt):

    # multi-core
    # retcls returns for all distance bins
    retcls = Parallel(n_jobs=-1, backend="loky")(delayed(get_pir_ia_e2ed_by_dist)(sim_duration, all_data_list, dist_min,
                                                                                  dist_min + dist_gap, tx_rx_dist_dict,
                                                                                  optional_txt)
                                                 for dist_min in distminlist)

    pir_data_list = []
    ia_data_list = []
    e2ed_data_list = []

    for rc in retcls:
        pir_data_list.append(copy.deepcopy(rc.all_vehicle_pir_list))
        ia_data_list.append(copy.deepcopy(rc.all_vehicle_ia_list))
        e2ed_data_list.append(copy.deepcopy(rc.all_vehicle_e2ed_list))

    if len(pir_data_list) != len(distminlist):
        raise Exception("PRR Data Size Mismatch.. Aborting.")

    if len(ia_data_list) != len(distminlist):
        raise Exception("IA Data Size Mismatch.. Aborting.")

    if len(e2ed_data_list) != len(distminlist):
        raise Exception("E2E Delay Data Size Mismatch.. Aborting.")

    print optional_txt, "-> PRR Extraction complete. Calculating CDFs and 95th Percentile..."
    with open(output_percentile_filename_pir, "w") as f:
        for indx, pir_data in enumerate(pir_data_list):
            if pir_data:
                dist_min = distminlist[indx]
                dist_max = dist_min + dist_gap
                if dist_min <= cdf_dist_min_cap:
                    write_pir_cdf_by_dist(pir_data, dist_min, dist_max, output_cdf_path_pir)

                # write 95th percentile
                pir_95p = np.percentile(pir_data, 95)  # return 95th percentile
                datastring = str(dist_min) + "; " + str(dist_max) + "; " + str(pir_95p) + "\n"
                f.write(datastring)

    print optional_txt, "-> IA Extraction complete. Calculating CDFs and 95th Percentile..."
    with open(output_percentile_filename_ia, "w") as f:
        for indx, ia_data in enumerate(ia_data_list):
            if ia_data:
                dist_min = distminlist[indx]
                dist_max = dist_min + dist_gap
                if dist_min <= cdf_dist_min_cap:
                    write_ia_cdf_by_dist(ia_data, dist_min, dist_max, output_cdf_path_ia)

                # write 95th percentile
                ia_95p = np.percentile(ia_data, 95)  # return 95th percentile
                datastring = str(dist_min) + "; " + str(dist_max) + "; " + str(ia_95p) + "\n"
                f.write(datastring)

    print optional_txt, "-> E2E Delay Extraction complete. Calculating CDFs..."

    with open(output_percentile_filename_e2ed, "w") as f:
        for indx, e2ed_data in enumerate(e2ed_data_list):
            if e2ed_data:
                dist_min = distminlist[indx]
                dist_max = dist_min + dist_gap
                if dist_min <= cdf_dist_min_cap:
                    write_e2ed_cdf_by_dist(e2ed_data, dist_min, dist_max, output_cdf_path_e2ed)

                # write 95th percentile
                e2ed_95p = np.percentile(e2ed_data, 95)  # return 95th percentile
                datastring = str(dist_min) + "; " + str(dist_max) + "; " + str(e2ed_95p) + "\n"
                f.write(datastring)

    print "Saving PIR CDFs (with distance) in the path:", output_cdf_path_pir
    print "Saving 95th percentile PIR trace to:", output_percentile_filename_pir
    print "Saving IA CDFs (with distance) in the path:", output_cdf_path_ia
    print "Saving 95th percentile IA trace to:", output_percentile_filename_ia
    print "Saving E2E Delay CDFs (with distance) in the path:", output_cdf_path_e2ed
    print "Saving 95th percentile E2E Delay trace to:", output_percentile_filename_e2ed


def extract_pir_ia_e2ed_data(sim_duration, trace_filename, output_path_pir, output_percentile_filename_pir,
                             output_path_ia, output_percentile_filename_ia,
                             output_cdf_path_e2ed, output_percentile_filename_e2ed,
                             optional_txt):
    amin = 0
    amax = 500
    cdf_dist_min_cap = 300   # cap for CDF distance
    dist_gap = 20

    pir_distminlist = list(range(amin, amax + dist_gap, dist_gap))

    all_data_list, tx_rx_dist_dict = dp.parse_per_raw_data_with_tx_rx_pair_dist_dict(trace_filename, pir_distminlist, dist_gap)

    write_cdf_n_95p_all_dist(sim_duration, all_data_list, tx_rx_dist_dict, pir_distminlist, dist_gap, cdf_dist_min_cap,
                             output_path_pir,
                             output_percentile_filename_pir,
                             output_path_ia,
                             output_percentile_filename_ia,
                             output_cdf_path_e2ed, output_percentile_filename_e2ed,
                             optional_txt)


if __name__ == '__main__':

    sim_dur = 5000  # ms [Simulation Duration]

    trace_fname = "../v2x_simulator/v2x_traces/kpi_traces/v2x_default_log_per.txt"
    out_path_pir = "../v2x_simulator/v2x_traces/kpi_traces/dist_vs_pir_cdf/v2x_default_log_per/"
    out_95p_fname_pir = "../v2x_simulator/v2x_traces/kpi_traces/distVS95pir_v2x_default_log_per.txt"
    out_path_ia = "../v2x_simulator/v2x_traces/kpi_traces/dist_vs_ia_cdf/v2x_default_log_per/"
    out_95p_fname_ia = "../v2x_simulator/v2x_traces/kpi_traces/distVS95ia_v2x_default_log_per.txt"
    out_path_e2ed = "../v2x_simulator/v2x_traces/kpi_traces/dist_vs_e2ed_cdf/v2x_default_log_per/"
    out_95p_fname_e2ed = "../v2x_simulator/v2x_traces/kpi_traces/distVS95e2ed_v2x_default_log_per.txt"

    extract_pir_ia_e2ed_data(sim_dur, trace_fname, out_path_pir, out_95p_fname_pir,
                             out_path_ia, out_95p_fname_ia,
                             out_path_e2ed, out_95p_fname_e2ed,
                             '20MHz')

    print "Saving PIR-IA-E2ED Script finished!"
