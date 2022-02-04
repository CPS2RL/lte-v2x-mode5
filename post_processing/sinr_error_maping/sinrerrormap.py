# -*- coding: utf-8 -*-
"""
Created on Wed Jul 11 10:25:14 2018

@author: Monowar Hasan
"""
from __future__ import division
from __future__ import print_function  # to support print function in python 2.7
import numpy as np
import h5py 
import scipy.interpolate
import random 
import matplotlib.pyplot as plt


class DataClass:
    """The class contain X and Y data for a given TX-RX speed"""
    
    def __init__(self, tx_rx_speed, esno, per):
        self.tx_rx_speed = tx_rx_speed  # speed
        self.esno = esno  # ES/N0
        self.per = per  # packet error rate
    

def get_xy_value(filename, varname, index):

    with h5py.File(filename, 'r') as f:
        data = f[varname][()]
        
        data = np.array(data)
        data = np.transpose(data)
        return list(data[index])
        
    
def get_tx_rx_speed(filename):
    
    data = []
    with h5py.File(filename, 'r') as f:
        for column in f['ltext']:
            row_data = []
            for row_number in range(len(column)):            
                row_data.append(''.join(map(chr, f[column[row_number]][:])))   
            data.append(row_data)
    
    data = [item for sublist in data for item in sublist]  # flatten the list
    return data


def print_all_data(dataclasslist):
    
    for index, dc in enumerate(dataclasslist):
        print("index:", index, "speed:", dc.tx_rx_speed)
        print("ES/N0:", dc.esno)
        print("ErrorRate:", dc.per)
        print("\n")
        

def get_interporate_per(esno_list, per_list, sinr):
    np.warnings.filterwarnings('ignore')  # turn off warnings for nan
    per_interp = scipy.interpolate.interp1d(esno_list, per_list)
    per = per_interp(sinr)
    
    return per


def get_error_rate(speed_string, filename, sinr):
    
    VERYLOWSINRDB = -1000
    
    all_data_list = get_all_data_list(filename)
    for data in all_data_list:
        trx = data.tx_rx_speed
        
        if trx == speed_string:
            if len(data.esno) != len(data.per):
                print("Data Size Mismatch. Aborting...")
                return None
    
            # no error
            if sinr > max(data.esno):
                return 0.0
            
            elif VERYLOWSINRDB < sinr <= min(data.esno):
                # error_rate = min(data.esno)
                # rv = random.uniform(0, 1)
                #
                # if rv > error_rate:
                #     return 0.0
                # else:
                #     return 1.0
                return 1.0
            
            # sinr is very low as marked by NS3 simulation, failed TX anyway    
            elif sinr <= VERYLOWSINRDB:
                return 1.0
                
            else:
                error_rate = get_interporate_per(data.esno, data.per, sinr)
                rv = random.uniform(0, 1)
                
                if rv > error_rate:
                    return 0.0
                else:
                    return 1.0

    print("Invalid speed_string! Returning None")
    return None        


def get_all_data_list(filename):
    xvarname = 'EsN0'
    yvarname = 'per'

    tx_rx_speeds = get_tx_rx_speed(filename)
    
    all_data_list = []
    
    for index, trx in enumerate(tx_rx_speeds):
        xval = get_xy_value(filename, xvarname, index)
        yval = get_xy_value(filename, yvarname, index)
        dc = DataClass(tx_rx_speed=trx, esno=xval, per=yval)
        all_data_list.append(dc)
    
    return all_data_list


def plot_data_by_indx(all_data_list, indx):
    
    # change font to Arial
    plt.rcParams["font.family"] = "Arial"
    plt.rcParams['font.size'] = 12
    plt.rcParams['legend.fontsize'] = 10
    plt.rcParams['axes.titlesize'] = 12
    plt.rcParams['ytick.labelsize'] = 11
    plt.rcParams['xtick.labelsize'] = 11
    plt.rcParams['legend.edgecolor'] = 'k'
    
    dc = all_data_list[indx]
    
    plt.plot(dc.esno, dc.per, label=dc.tx_rx_speed, 
             color='b')

    # Set labels
    plt.xlabel('SINR (dB)')
    plt.ylabel('Packet Error Rate')
    plt.legend(loc='upper right', handletextpad=0.4, ncol=2)

    # set grid
    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=':', linewidth=0.3, which='both', alpha=0.6)

    plt.tight_layout()
    
    plt.savefig("link_level_plot.pdf", pad_inches=0.1, bbox_inches='tight')
        
    plt.show()


if __name__ == '__main__':
    
    snr_filename = 'snr_error_mapping_variables.mat'
        
    snr_all_data_list = get_all_data_list(snr_filename)
    print_all_data(snr_all_data_list)
    
    snr_error_rate = get_error_rate(speed_string="v_{Rx}=70km/h,v_{Tx}=-70km/h", filename=snr_filename, sinr=4.0)
    
    print("Error Rate:", snr_error_rate)
    
    print(snr_error_rate)
    # #    TX=70, RX=70
    # plot_data_by_indx(snr_all_data_list, indx=6)

    #    TX=60, RX=60
    plot_data_by_indx(snr_all_data_list, indx=5)
    
    print("Script Finished!")
