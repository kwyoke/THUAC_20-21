import numpy as np
import pandas as pd
import scipy.sparse as ss
import time

def read_data_file_as_coo_matrix(filename):
    "Read data file and return sparse matrix in coordinate format."
    data = pd.read_csv(filename, sep='\t', header=None, dtype=np.uint32)
    rows = data[0]  # Not a copy, just a reference.
    cols = data[1]
    ones = np.ones(len(rows), np.uint32)
    matrix = ss.coo_matrix((ones, (rows, cols)))
    return matrix


def pagerank(adj, maxiter=100, convergence=0.000001, damping=0.85):
    N = adj.shape[0]

    ### get inv diagonal matrix of outdegrees
    # first get the outdegrees
    diag_deg, _ = np.histogram(adj.nonzero()[0], np.arange(adj.shape[0]+1))
    nowherenodes = np.where(diag_deg == 0) #dangling nodes without outlinks
    diag_deg[nowherenodes] = N #assume that dangling nodes point to all the nodes
    # inverse outdeg
    diag_inv = 1/diag_deg
    # generating an inv diagonal matrix with diag_deg
    diaginv_mat = ss.coo_matrix((N, N))
    diaginv_mat.setdiag(diag_inv)
    diaginv_mat = diaginv_mat.tocsr()
    
    M = (diaginv_mat.dot(adj)).transpose() #prepare for iterative computation later
    
    ones = np.ones(N)
    v_old = ones/N #set initial scores as uniform
    i = 0
    diff = 1

    while (diff > convergence and i<maxiter):
        i+=1
        t0 = time.time()
        deadend_contrib = np.sum(v_old[nowherenodes])/N #contribution from nodes with no outlinks
        v_new = damping*M.dot(v_old) + (1-damping)/N * ones
        v_new += deadend_contrib
        #print(i, np.linalg.norm(v_new - v_old), sum(v_new))
        diff = np.linalg.norm(v_new - v_old)
        v_old = v_new.copy()
        t1 = time.time()
        duration = t1 - t0
        print(i, duration)
        
    return v_new

t0 = time.time()
A = read_data_file_as_coo_matrix('linkgraph.txt') #file has added edge 499999 to 499999 to make node 499999 appear in last row od A
A = A.tocsr()
A[499999, 499999] = 0
t1 = time.time()
preproc_time = t1-t0
print('preproc_time: ', preproc_time)

t0 = time.time()
pr = pagerank(A)
t1 = time.time()
print('PR convergence time: ', t1-t0)