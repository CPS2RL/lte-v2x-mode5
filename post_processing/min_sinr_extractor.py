"""
Created on 7/1/2019 9:27 AM
@author: Monowar Hasan
"""


import dataparser as dp


if __name__ == '__main__':

    root_trace_dir = "../v2x_simulator/v2x_traces/kpi_traces/"
    in_trace_file_10Hz = "10MHz_HARQ_DIS_CC_DIS_CBR_T2_50KMH_3s_NTX_sinr.txt"
    in_trace_file_20Hz = "20MHz_HARQ_DIS_CC_DIS_CBR_T2_50KMH_3s_NTX_sinr.txt"

    out_trace_file_10Hz = "10MHz_HARQ_DIS_CC_DIS_CBR_T2_50KMH_3s_NTX_sinr_min.txt"
    out_trace_file_20Hz = "20MHz_HARQ_DIS_CC_DIS_CBR_T2_50KMH_3s_NTX_sinr_min.txt"

    in_fname_10Hz = root_trace_dir + in_trace_file_10Hz
    in_fname_20Hz = root_trace_dir + in_trace_file_20Hz

    out_fname_10Hz = root_trace_dir + out_trace_file_10Hz
    out_fname_20Hz = root_trace_dir + out_trace_file_20Hz

    # dp.extract_min_sinr(in_fname_20Hz, out_fname_20Hz)
    dp.extract_min_sinr(in_fname_10Hz, out_fname_10Hz)
