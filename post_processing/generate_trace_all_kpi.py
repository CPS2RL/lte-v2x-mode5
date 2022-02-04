"""
Created on 7/29/2019 10:50 AM
@author: Monowar Hasan
"""

import gt_dist_vs_prr_by_per as kpi_prr
import gt_dist_vs_pir_ia_e2ed_by_per as kpi_pir_ia_e2ed


def generate_all_kpi_trace(sim_duration, input_trace_filename, output_filename_prr,
                           output_path_pir, output_percentile_filename_pir,
                           output_path_ia, output_percentile_filename_ia,
                           output_cdf_path_e2ed, output_percentile_filename_e2ed,
                           optional_txt):

    # calculate dist-vs-avg-prr
    kpi_prr.calculate_and_write_prr(input_trace_filename=input_trace_filename, output_filename=output_filename_prr,
                                    optional_txt=optional_txt)

    # calculate rest of the KPIs
    kpi_pir_ia_e2ed.extract_pir_ia_e2ed_data(sim_duration=sim_duration, trace_filename=input_trace_filename,
                                             output_path_pir= output_path_pir,
                                             output_percentile_filename_pir=output_percentile_filename_pir,
                                             output_path_ia=output_path_ia,
                                             output_percentile_filename_ia=output_percentile_filename_ia,
                                             output_cdf_path_e2ed=output_cdf_path_e2ed,
                                             output_percentile_filename_e2ed=output_percentile_filename_e2ed,
                                             optional_txt=optional_txt)


if __name__ == '__main__':

    g_sim_duration = 5000  # [Simulation Duration -> ms]

    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="../v2x_simulator/v2x_traces/kpi_traces/v2x_default_log_per.txt",
                           output_filename_prr="../v2x_simulator/v2x_traces/kpi_traces/distVSprr_v2x_default_log_per.txt",
                           output_path_pir="../v2x_simulator/v2x_traces/kpi_traces/dist_vs_pir_cdf/v2x_default_log_per/",
                           output_percentile_filename_pir="../v2x_simulator/v2x_traces/kpi_traces/distVS95pir_v2x_default_log_per.txt",
                           output_path_ia="../v2x_simulator/v2x_traces/kpi_traces/dist_vs_ia_cdf/v2x_default_log_per/",
                           output_percentile_filename_ia="../v2x_simulator/v2x_traces/kpi_traces/distVS95ia_v2x_default_log_per.txt",
                           output_cdf_path_e2ed="../v2x_simulator/v2x_traces/kpi_traces/dist_vs_e2ed_cdf/v2x_default_log_per/",
                           output_percentile_filename_e2ed= "../v2x_simulator/v2x_traces/kpi_traces/distVS95e2ed_v2x_default_log_per.txt",
                           optional_txt="20MHz")

    print "All KPI Generation Script Finished!!"

