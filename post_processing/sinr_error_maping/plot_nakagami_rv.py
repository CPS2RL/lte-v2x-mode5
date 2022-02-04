# -*- coding: utf-8 -*-
"""
Created on Thu Jul 26 10:19:55 2018

@author: Monowar Hasan
"""

import matplotlib.pyplot as plt
import numpy as np
import math


def get_nakagami_pdf_xy(m, gamma=1):
    
    # k is the shape and \theta the scale
    k = m;
    theta = gamma/m;
    
    shape = k
    scale = theta
    
    num_samples = 1000000
    s = np.random.gamma(shape, scale, num_samples)
    
    s = [math.sqrt(i) for i in s]
    
    # Create the histogram and normalize the counts to 1
    hist, bins = np.histogram(s, bins=70)
#    max_val = max(hist)
#    hist = [ float(n)/max_val for n in hist]
    hist = [float(h)/num_samples for h in hist]
    
    center = 0.5*(bins[1:]+bins[:-1])
    
#    plt.plot(center, hist)
#    plt.show()
    
    return center, hist
    

def get_log_from_linear(xlinear):
    
    xlog = [10 * np.log10(x) for x in xlinear]
    return xlog
    

def plot_nakagami(outputfilename=None):
    
     # change font to Arial
    plt.rcParams["font.family"] = "Arial"
    plt.rcParams['font.size'] = 15
    plt.rcParams['legend.fontsize'] = 11
    plt.rcParams['axes.titlesize'] = 13
    plt.rcParams['ytick.labelsize'] = 12
    plt.rcParams['xtick.labelsize'] = 12
    
    x3, y3 = get_nakagami_pdf_xy(m=3)
    x15, y15 = get_nakagami_pdf_xy(m=1.5)
    x1, y1 = get_nakagami_pdf_xy(m=1)
    
   
    
    plt.subplot(2,2,1)
    
    # set grid
    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=':', linewidth=0.3, which='both', alpha=0.7)
    
    plt.plot(x3, y3, label='m=3')
    plt.plot(x1, y1, label='m=1')
    plt.plot(x15, y15, label='m=1.5')
    plt.title("PDF (Linear)")
    
    plt.xticks(range(0, 5, 1))
    
    
    
    
    # log scale
    x3log = get_log_from_linear(x3)
    x1log = get_log_from_linear(x1)
    x15log = get_log_from_linear(x15)
    
    

    plt.subplot(2,2,2)
    
    # set grid
    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=':', linewidth=0.3, which='both', alpha=0.7)
    
    
    plt.plot(x3log, y3, label='m=3')
    plt.plot(x1log, y1, label='m=1')
    plt.plot(x15log, y15, label='m=1.5')

    plt.xticks(range(-20, 20, 10))
    
    plt.title("PDF (Log)")

    ax.legend(loc='upper center', 
              bbox_to_anchor=(-0.1, -0.15),  
              ncol=3, edgecolor='k')
    
    
    
    if outputfilename is not None:
        plt.savefig(outputfilename, pad_inches=0.1, bbox_inches='tight')
    
    
    plt.show()
    


if __name__=='__main__':
    
    outfilename = 'nakagami_pdf.pdf'
#    outfilename = None
    
    plot_nakagami(outfilename)
    
    
    
    print("Script Finished!")