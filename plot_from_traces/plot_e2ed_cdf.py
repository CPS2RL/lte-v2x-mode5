"""
Created on 8/2/2019 10:10 AM
@author: Monowar Hasan
"""

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np

import expclasses as ec


def plot_e2ed_cdf_rc_cc(data_class, output_pdf_name):
    # create plot

    # change font to Arial
    plt.rcParams["font.family"] = "Arial"
    plt.rcParams['font.size'] = 25
    plt.rcParams['legend.fontsize'] = 18
    plt.rcParams['axes.titlesize'] = 25
    plt.rcParams['ytick.labelsize'] = 18
    plt.rcParams['xtick.labelsize'] = 18
    plt.rcParams['figure.figsize'] = 10, 6


    base_lw = 3
    common_lw = 1.5

    base_mw = 1.8
    common_mw = 1.2

    base_ms = 8
    common_ms = 6

    base_opacity = 0.7
    common_opacity = 0.7

    base_color = 'xkcd:dark purple'
    common_color_20 = 'xkcd:pine'
    common_color_10 = 'xkcd:brick orange'

    base_ls = '-'
    common_ls_20 = '-'
    common_ls_10 = '--'

    base_mfill = 'none'
    common_mfill_20 = 'none'
    common_mfill_10 = 'full'

    plt.plot(data_class.val_j3161[:, 0], data_class.val_j3161[:, 1],
             linewidth=base_lw,
             marker='o',
             linestyle=base_ls,
             color=base_color,
             alpha=base_opacity,
             fillstyle=base_mfill,
             markeredgewidth=base_mw,
             markersize=base_ms,
             label='J3161')

    plt.plot(data_class.val_j3161_pc[:, 0], data_class.val_j3161_pc[:, 1],
             linewidth=common_lw,
             marker='s',
             linestyle=common_ls_20,
             color=common_color_20,
             alpha=common_opacity,
             fillstyle=common_mfill_20,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_PC')

    # plt.plot(distminlist, data_class.val_j3161_nrc_pc,
    #          linewidth=common_lw,
    #          marker='d',
    #          linestyle=common_ls_20,
    #          color=common_color_20,
    #          alpha=common_opacity,
    #          fillstyle=common_mfill_20,
    #          markeredgewidth=common_mw,
    #          markersize=common_ms,
    #          label='J3161_NRC_PC')

    # plt.plot(distminlist, data_class.val_j3161_nrc_npc,
    #          linewidth=common_lw,
    #          marker='v',
    #          linestyle=common_ls_20,
    #          color=common_color_20,
    #          alpha=common_opacity,
    #          fillstyle=common_mfill_20,
    #          markeredgewidth=common_mw,
    #          markersize=common_ms,
    #          label='J3161_NRC_NPC')

    plt.plot(data_class.val_j3161_10mhz[:, 0], data_class.val_j3161_10mhz[:, 1],
             linewidth=common_lw,
             marker='o',
             linestyle=common_ls_10,
             color=common_color_10,
             alpha=common_opacity,
             fillstyle=common_mfill_10,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_10MHz')

    plt.plot(data_class.val_j3161_pc_10mhz[:, 0], data_class.val_j3161_pc_10mhz[:, 1],
             linewidth=common_lw,
             marker='s',
             linestyle=common_ls_10,
             color=common_color_10,
             alpha=common_opacity,
             fillstyle=common_mfill_10,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_PC_10MHz')

    # plt.plot(distminlist, data_class.val_j3161_nrc_pc_10mhz,
    #          linewidth=common_lw,
    #          marker='d',
    #          linestyle=common_ls_10,
    #          color=common_color_10,
    #          alpha=common_opacity,
    #          fillstyle=common_mfill_10,
    #          markeredgewidth=common_mw,
    #          markersize=common_ms,
    #          label='J3161_NRC_PC_10MHz')

    # plt.plot(distminlist, data_class.val_j3161_nrc_npc_10mhz,
    #          linewidth=common_lw,
    #          marker='v',
    #          linestyle=common_ls_10,
    #          color=common_color_10,
    #          alpha=common_opacity,
    #          fillstyle=common_mfill_10,
    #          markeredgewidth=common_mw,
    #          markersize=common_ms,
    #          label='J3161_NRC_NPC_10MHz')

    plt.ylim([-0.05, 1.05])

    plt.xlabel("End-to-End Latency (ms)")
    plt.ylabel("CDF")

    plt.legend(loc='lower left', ncol=2,
               bbox_to_anchor=(0.02, 0.97), frameon=False)

    plt.grid(linestyle=":", color='black', alpha=0.4)

    plt.savefig(output_pdf_name, pad_inches=0.1, bbox_inches='tight')
    plt.show()


# harq vs no harq
def plot_e2ed_cdf_harq_no_harq(data_class, output_pdf_name):
    # create plot

    # change font to Arial
    plt.rcParams["font.family"] = "Arial"
    plt.rcParams['font.size'] = 25
    plt.rcParams['legend.fontsize'] = 18
    plt.rcParams['axes.titlesize'] = 25
    plt.rcParams['ytick.labelsize'] = 18
    plt.rcParams['xtick.labelsize'] = 18
    plt.rcParams['figure.figsize'] = 10, 6

    base_lw = 3
    common_lw = 1.5

    base_mw = 1.8
    common_mw = 1.2

    base_ms = 8
    common_ms = 6

    base_opacity = 0.7
    common_opacity = 0.7

    base_color = 'xkcd:dark purple'
    common_color_20 = 'xkcd:pine'
    common_color_10 = 'xkcd:brick orange'

    base_ls = '-'
    common_ls_20 = '-'
    common_ls_10 = '--'

    base_mfill = 'none'
    common_mfill_20 = 'none'
    common_mfill_10 = 'full'

    plt.plot(data_class.val_j3161[:, 0], data_class.val_j3161[:, 1],
             linewidth=base_lw,
             marker='o',
             linestyle=base_ls,
             color=base_color,
             alpha=base_opacity,
             fillstyle=base_mfill,
             markeredgewidth=base_mw,
             markersize=base_ms,
             label='J3161')

    plt.plot(data_class.val_j3161_10mhz[:, 0], data_class.val_j3161_10mhz[:, 1],
             linewidth=common_lw,
             marker='o',
             linestyle=common_ls_10,
             color=common_color_10,
             alpha=common_opacity,
             fillstyle=common_mfill_10,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_10MHz')

    plt.plot(data_class.val_j3161_no_harq[:, 0], data_class.val_j3161_no_harq[:, 1],
             linewidth=common_lw,
             marker='x',
             linestyle=common_ls_20,
             color='xkcd:very dark green',
             alpha=common_opacity,
             fillstyle=common_mfill_20,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_NO_HARQ')

    plt.plot(data_class.val_j3161_10mhz_no_harq[:, 0], data_class.val_j3161_10mhz_no_harq[:, 1],
             linewidth=common_lw,
             marker='d',
             linestyle=common_ls_10,
             color='xkcd:marine blue',
             alpha=common_opacity,
             fillstyle=common_mfill_10,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_10MHz_NO_HARQ')

    plt.ylim([-0.05, 1.05])

    plt.xlabel("End-to-End Latency (ms)")
    plt.ylabel("CDF")

    plt.legend(loc='lower left', ncol=2,
               bbox_to_anchor=(0.02, 0.97), frameon=False)

    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=":", color='black', alpha=0.4)

    plt.savefig(output_pdf_name, pad_inches=0.1, bbox_inches='tight')
    plt.show()


def get_congestion_control_data_0_20():

    val_j3161 = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10KMH/e2elat_0_20.csv")
    val_j3161_10mhz = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10MHz_10KMH/e2elat_0_20.csv")
    val_j3161_pc = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_PC_10KMH/e2elat_0_20.csv")
    val_j3161_pc_10mhz = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_PC_10MHz_10KMH/e2elat_0_20.csv")
    val_j3161_nrc_pc = None
    val_j3161_nrc_pc_10mhz = None
    val_j3161_nrc_npc = None
    val_j3161_nrc_npc_10mhz = None

    cc_class = ec.CongestionControlExpclass(val_j3161, val_j3161_10mhz, val_j3161_pc,
                                            val_j3161_pc_10mhz, val_j3161_nrc_pc, val_j3161_nrc_pc_10mhz,
                                            val_j3161_nrc_npc, val_j3161_nrc_npc_10mhz)

    return cc_class


def get_congestion_control_data_80_100():

    val_j3161 = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10KMH/e2elat_80_100.csv")
    val_j3161_10mhz = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10MHz_10KMH/e2elat_80_100.csv")
    val_j3161_pc = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_PC_10KMH/e2elat_80_100.csv")
    val_j3161_pc_10mhz = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_PC_10MHz_10KMH/e2elat_80_100.csv")
    val_j3161_nrc_pc = None
    val_j3161_nrc_pc_10mhz = None
    val_j3161_nrc_npc = None
    val_j3161_nrc_npc_10mhz = None

    cc_class = ec.CongestionControlExpclass(val_j3161, val_j3161_10mhz, val_j3161_pc,
                                            val_j3161_pc_10mhz, val_j3161_nrc_pc, val_j3161_nrc_pc_10mhz,
                                            val_j3161_nrc_npc, val_j3161_nrc_npc_10mhz)

    return cc_class


def get_congestion_control_data_280_300():

    val_j3161 = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10KMH/e2elat_280_300.csv")
    val_j3161_10mhz = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10MHz_10KMH/e2elat_280_300.csv")
    val_j3161_pc = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_PC_10KMH/e2elat_280_300.csv")
    val_j3161_pc_10mhz = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_PC_10MHz_10KMH/e2elat_280_300.csv")
    val_j3161_nrc_pc = None
    val_j3161_nrc_pc_10mhz = None
    val_j3161_nrc_npc = None
    val_j3161_nrc_npc_10mhz = None

    cc_class = ec.CongestionControlExpclass(val_j3161, val_j3161_10mhz, val_j3161_pc,
                                            val_j3161_pc_10mhz, val_j3161_nrc_pc, val_j3161_nrc_pc_10mhz,
                                            val_j3161_nrc_npc, val_j3161_nrc_npc_10mhz)

    return cc_class


def get_harq_no_harq_data_0_20():
    val_j3161 = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10KMH/e2elat_0_20.csv")
    val_j3161_10mhz = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10MHz_10KMH/e2elat_0_20.csv")

    val_j3161_no_harq = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_NO_HARQ_10KMH/e2elat_0_20.csv")
    val_j3161_10mhz_no_harq = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10MHz_NO_HARQ_10KMH/e2elat_0_20.csv")

    h_class = ec.HARQExpclass(val_j3161=val_j3161, val_j3161_10mhz=val_j3161_10mhz,
                                 val_j3161_no_harq=val_j3161_no_harq, val_j3161_10mhz_no_harq=val_j3161_10mhz_no_harq)

    return h_class


def get_harq_no_harq_data_80_100():
    val_j3161 = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10KMH/e2elat_80_100.csv")
    val_j3161_10mhz = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10MHz_10KMH/e2elat_80_100.csv")

    val_j3161_no_harq = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_NO_HARQ_10KMH/e2elat_80_100.csv")
    val_j3161_10mhz_no_harq = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10MHz_NO_HARQ_10KMH/e2elat_80_100.csv")

    h_class = ec.HARQExpclass(val_j3161=val_j3161, val_j3161_10mhz=val_j3161_10mhz,
                                 val_j3161_no_harq=val_j3161_no_harq, val_j3161_10mhz_no_harq=val_j3161_10mhz_no_harq)

    return h_class

def get_harq_no_harq_data_280_300():
    val_j3161 = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10KMH/e2elat_280_300.csv")
    val_j3161_10mhz = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10MHz_10KMH/e2elat_280_300.csv")

    val_j3161_no_harq = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_NO_HARQ_10KMH/e2elat_280_300.csv")
    val_j3161_10mhz_no_harq = ec.get_e2ed_from_file("../v2x_trace_for_plot/dist_vs_e2ed_cdf/J3161_10MHz_NO_HARQ_10KMH/e2elat_280_300.csv")

    h_class = ec.HARQExpclass(val_j3161=val_j3161, val_j3161_10mhz=val_j3161_10mhz,
                                 val_j3161_no_harq=val_j3161_no_harq, val_j3161_10mhz_no_harq=val_j3161_10mhz_no_harq)

    return h_class


if __name__ == '__main__':

    plot_e2ed_cdf_rc_cc(data_class=get_congestion_control_data_0_20(), output_pdf_name='e2ed_cdf_0_20_rc_pc_10kmh.pdf')
    plot_e2ed_cdf_rc_cc(data_class=get_congestion_control_data_80_100(), output_pdf_name='e2ed_cdf_80_100_rc_pc_10kmh.pdf')
    plot_e2ed_cdf_rc_cc(data_class=get_congestion_control_data_280_300(), output_pdf_name='e2ed_cdf_280_300_rc_pc_10kmh.pdf')

    plot_e2ed_cdf_harq_no_harq(data_class=get_harq_no_harq_data_0_20(), output_pdf_name='e2ed_cdf_0_20_harq_no_harq_10kmh.pdf')
    plot_e2ed_cdf_harq_no_harq(data_class=get_harq_no_harq_data_80_100(), output_pdf_name='e2ed_cdf_80_100_harq_no_harq_10kmh.pdf')
    plot_e2ed_cdf_harq_no_harq(data_class=get_harq_no_harq_data_280_300(), output_pdf_name='e2ed_cdf_280_300_harq_no_harq_10kmh.pdf')

    print "Plotting E2E CDF Script finished"

