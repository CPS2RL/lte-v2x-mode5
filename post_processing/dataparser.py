"""
Created on 6/27/2019 9:09 AM
@author: Monowar Hasan
"""
from __future__ import division
import pandas as pd
import numpy as np
import statsmodels.api as sm
from collections import defaultdict


class KPIInfoDataClass:

    #  the class contains all information about the trace file

    def __init__(self, arrival_time_ms, tx_time_ms, ch_id, tx_id, rx_id, dist, sinr):
        self.arrival_time_ms = arrival_time_ms  # packet arrival time
        self.tx_time_ms = tx_time_ms  # tx subframe
        self.ch_id = ch_id
        self.tx_id = tx_id
        self.rx_id = rx_id
        self.dist = dist
        self.sinr = sinr


class PERKPIInfoDataClass:

    #  the class contains all information about the trace file [with per]

    def __init__(self, arrival_time_ms, tx_time_ms, tx_id, rx_id, dist, per):
        self.arrival_time_ms = arrival_time_ms  # packet arrival time
        self.tx_time_ms = tx_time_ms  # tx subframe
        self.tx_id = tx_id
        self.rx_id = rx_id
        self.dist = dist
        self.per = per


# print the data given the list of all data
def print_data(all_data_list):
    for indx, adl in enumerate(all_data_list):
        print "\n==> Entry#", indx, "<=="
        print "Arrival Time:", adl.arrival_time_ms
        print "TX Subframe:", adl.tx_time_ms
        print "Subchannel ID:", adl.ch_id
        print "TX id:", adl.tx_id
        print "RX id:", adl.rx_id
        print "Dist:", adl.dist
        print "SINR:", adl.sinr


# read the trace file and return as a panda dataframe
def get_data_by_filename(filename):
    print "Reading", filename, "and return as a Panda dataframe..."
    df = pd.read_csv(filename, delimiter="\t", header=None)
    df = df.values  # change to numpy array
    return df

# parse raw data and return list of all data with individual elements
def parse_raw_data(filename):
    rawdata = get_data_by_filename(filename)

    print "Reading rawdata from file complete."

    all_data_list = []

    fn = filename.split("/")
    fn = fn[-1]  # extract filename from relative filename path

    totaldatalen = len(rawdata)

    for indx, data in enumerate(rawdata):
        print "Parsing", fn, ":", indx, "/", totaldatalen, "-->", \
            round((indx / totaldatalen) * 100), "\b% of total trace..."

        datastring = ''.join(data)
        tokenized_data = datastring.split(";")

        arrival_time_ms = int(tokenized_data[0].strip())
        tx_time_ms = int(tokenized_data[1].strip())
        ch_id = int(tokenized_data[2].strip())
        tx_id = int(tokenized_data[3].strip())
        rx_id = int(tokenized_data[4].strip())
        dist = float(tokenized_data[5].strip())
        sinr = float(tokenized_data[6].strip())

        adl = KPIInfoDataClass(arrival_time_ms, tx_time_ms, ch_id, tx_id, rx_id, dist, sinr)
        all_data_list.append(adl)

    return all_data_list


# parse raw data and return list of all data with individual elements and packet TX SFs
def parse_raw_data_with_tx_packet_sf(filename):
    rawdata = get_data_by_filename(filename)

    print "Reading rawdata from file complete."

    all_data_list = []

    fn = filename.split("/")
    fn = fn[-1]  # extract filename from relative filename path

    totaldatalen = len(rawdata)
    tx_sf_list = set()  # saves all the (unique) TX packet subframes

    for indx, data in enumerate(rawdata):
        print "Parsing", fn, ":", indx, "/", totaldatalen, "-->", \
            round((indx / totaldatalen) * 100), "\b% of total trace..."

        datastring = ''.join(data)
        tokenized_data = datastring.split(";")

        arrival_time_ms = int(tokenized_data[0].strip())
        tx_time_ms = int(tokenized_data[1].strip())
        ch_id = int(tokenized_data[2].strip())
        tx_id = int(tokenized_data[3].strip())
        rx_id = int(tokenized_data[4].strip())
        dist = float(tokenized_data[5].strip())
        sinr = float(tokenized_data[6].strip())

        adl = KPIInfoDataClass(arrival_time_ms, tx_time_ms, ch_id, tx_id, rx_id, dist, sinr)
        all_data_list.append(adl)
        tx_sf_list.add(tx_time_ms)  # save all the TX SF

    tx_sf_list = list(tx_sf_list)  # convert to list

    return all_data_list, tx_sf_list


# parse raw data (per) and return list of all data with individual elements and packet TX SFs
def parse_per_raw_data_with_tx_packet_sf(filename):
    rawdata = get_data_by_filename(filename)

    print "Reading rawdata from file complete."

    all_data_list = []

    fn = filename.split("/")
    fn = fn[-1]  # extract filename from relative filename path

    totaldatalen = len(rawdata)
    tx_sf_list = set()  # saves all the (unique) TX packet subframes

    for indx, data in enumerate(rawdata):
        print "Parsing", fn, ":", indx, "/", totaldatalen, "-->", \
            round((indx / totaldatalen) * 100), "\b% of total trace..."

        datastring = ''.join(data)
        tokenized_data = datastring.split(";")

        arrival_time_ms = int(tokenized_data[0].strip())
        tx_time_ms = int(tokenized_data[1].strip())
        tx_id = int(tokenized_data[2].strip())
        rx_id = int(tokenized_data[3].strip())
        dist = float(tokenized_data[4].strip())
        per = int(tokenized_data[5].strip())

        adl = PERKPIInfoDataClass(arrival_time_ms, tx_time_ms, tx_id, rx_id, dist, per)
        all_data_list.append(adl)
        tx_sf_list.add(tx_time_ms)  # save all the TX SF

    tx_sf_list = list(tx_sf_list)  # convert to list

    return all_data_list, tx_sf_list


# parse raw data (per) and return list of all data and unique TX-RX pair
def parse_per_raw_data_with_tx_rx_pair_list(filename):
    rawdata = get_data_by_filename(filename)

    print "Reading rawdata from file complete."

    all_data_list = []

    fn = filename.split("/")
    fn = fn[-1]  # extract filename from relative filename path

    totaldatalen = len(rawdata)
    tx_rx_pair = set()  # save all TX-RX pair

    for indx, data in enumerate(rawdata):
        print "Parsing", fn, ":", indx, "/", totaldatalen, "-->", \
            round((indx / totaldatalen) * 100), "\b% of total trace..."

        datastring = ''.join(data)
        tokenized_data = datastring.split(";")

        arrival_time_ms = int(tokenized_data[0].strip())
        tx_time_ms = int(tokenized_data[1].strip())
        tx_id = int(tokenized_data[2].strip())
        rx_id = int(tokenized_data[3].strip())
        dist = float(tokenized_data[4].strip())
        per = int(tokenized_data[5].strip())

        adl = PERKPIInfoDataClass(arrival_time_ms, tx_time_ms, tx_id, rx_id, dist, per)
        all_data_list.append(adl)
        tx_rx_pair.add((tx_id, rx_id))  # save all TX-RX id

    tx_rx_pair = list(tx_rx_pair)  # convert to list

    return all_data_list, tx_rx_pair


def get_dist_key_by_distance(distminlist, dist_gap, dist):

    for dist_min in distminlist:
        dist_max = dist_min + dist_gap
        if dist_min <= dist < dist_max:
            return dist_min

    return None  # return invalid


# parse raw data (per) and return list of all data and unique TX-RX pair (associated with each distance bin)
def parse_per_raw_data_with_tx_rx_pair_dist_dict(filename, distminlist, dist_gap):
    rawdata = get_data_by_filename(filename)

    print "Reading rawdata from file complete."

    all_data_list = []

    fn = filename.split("/")
    fn = fn[-1]  # extract filename from relative filename path

    totaldatalen = len(rawdata)

    tx_rx_pair_dist_dict = defaultdict(set)

    for indx, data in enumerate(rawdata):
        print "Parsing", fn, ":", indx, "/", totaldatalen, "-->", \
            round((indx / totaldatalen) * 100), "\b% of total trace..."

        datastring = ''.join(data)
        tokenized_data = datastring.split(";")

        arrival_time_ms = int(tokenized_data[0].strip())
        tx_time_ms = int(tokenized_data[1].strip())
        tx_id = int(tokenized_data[2].strip())
        rx_id = int(tokenized_data[3].strip())
        dist = float(tokenized_data[4].strip())
        per = int(tokenized_data[5].strip())

        adl = PERKPIInfoDataClass(arrival_time_ms, tx_time_ms, tx_id, rx_id, dist, per)
        all_data_list.append(adl)

        dist_key = get_dist_key_by_distance(distminlist, dist_gap, dist)

        if dist_key is not None:
            tx_rx_pair_dist_dict[dist_key].add((tx_id, rx_id))  # save all TX-RX id with corresponding distance


    return all_data_list, tx_rx_pair_dist_dict




# returns cdf and ecdf
def get_cdf_from_trace(data_trace):
    # empirical CDF
    sample = np.array(data_trace)
    ecdf = sm.distributions.ECDF(sample)

    ecdf_x = np.linspace(min(sample), max(sample))
    ecdf_y = ecdf(ecdf_x)
    ecdf_yinv = 1 - ecdf(ecdf_x)

    # CDF by sorting
    cdf_x = np.sort(np.array(data_trace))
    cdf_y = 1. * np.arange(len(cdf_x)) / (len(cdf_x) - 1)
    cdf_yinv = 1 - (1. * np.arange(len(cdf_x)) / (len(cdf_x) - 1))

    return ecdf_x, ecdf_y, ecdf_yinv, cdf_x, cdf_y, cdf_yinv


# trims data file and save to output filename
def data_trimmer(input_filename, output_filename, arrival_time_th_1, arrival_time_th_2):

    if arrival_time_th_1 > arrival_time_th_2:
        print ("First threshold must be less than second one! Abroting...")
        return

    rawdata = get_data_by_filename(input_filename)

    print "Reading rawdata from file complete."

    all_data_list = []

    fn = input_filename.split("/")
    fn = fn[-1]  # extract filename from relative filename path

    totaldatalen = len(rawdata)

    with open(output_filename, "w") as f:
        for indx, data in enumerate(rawdata):
            print "Parsing", fn, ":", indx, "/", totaldatalen, "-->", \
                round((indx / totaldatalen) * 100), "\b% of total trace..."

            datastring = ''.join(data)
            tokenized_data = datastring.split(";")

            arrival_time_ms = int(tokenized_data[0].strip())

            if arrival_time_th_1 <= arrival_time_ms < arrival_time_th_2:
                f.write(datastring + '\n')


# extract minimum SINR from two subchannels
# this may not required finally --> but for now we'll use this
# since we do not have any PHY layer abstraction
def extract_min_sinr(input_filename, output_filename):

    print "Parsing", input_filename, "..."

    with open(input_filename) as infile, open(output_filename, "w") as outfile:
        while True:
            try:
                ds1 = next(infile)
                ds2 = next(infile)
                tokenized_data = ds1.split(";")
                sinr_1 = float(tokenized_data[6].strip())

                tokenized_data = ds2.split(";")
                sinr_2 = float(tokenized_data[6].strip())

                outstr = ''

                if sinr_1 <= sinr_2:
                    outstr = ds1
                else:
                    outstr = ds2

                outfile.write(outstr)  # write minimum SINR
            except StopIteration:
                break

    print "Minimum SINR parsing complete. File saved to:\n", output_filename


# returns unique values from the list
def get_unique_values(input_list):
    return list(set(input_list))
