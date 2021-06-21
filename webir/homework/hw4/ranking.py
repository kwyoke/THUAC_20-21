import requests
from bs4 import BeautifulSoup
import random
import re
import json
import pandas as pd
import os
import numpy as np

#get doc freq info
df_dict = {}
with open("document_frequency.tsv") as f:
	next(f)
	for line in f:
		word, freq = line.split()
		df_dict[word.lower()] = int(freq.replace(',', ''))

#compute doc statistics for each query
def doc_stats_perq(query, doc_dir):
    query_list = query.lower().split()
    uniq_queryterms = list(set(query_list))
    
    stats = {}
    stats['tf'] = np.zeros((len(uniq_queryterms), 20)) #init
    stats['dl'] = np.zeros(20)
    stats['qtf'] = np.zeros(len(uniq_queryterms))
    stats['df'] = np.zeros(len(uniq_queryterms))
    stats['avgdl'] = 500
    stats['N'] = 10**11
    
    for i, term in enumerate(uniq_queryterms):
        stats['qtf'][i] = query_list.count(term)
        stats['df'][i] = df_dict[term]
        
        for docnum in range(20):
            docname = doc_dir + str(docnum+1) + '.txt'
            
            with open(docname) as f:
                lines = [line.rstrip() for line in f] # All lines including the blank ones
                lines = [line for line in lines if line] # Non-blank lines
                
                for line in lines:
                    line_list = line.lower().split()
                    stats['dl'][docnum] += len(line_list)
                    stats['tf'][i,docnum] += line_list.count(term)
    return stats

#vsm ranking model
def score_vsm(stats_dict):
    score = np.zeros((20))
    s=0.2 #empirical param
    num_terms = len(stats_dict['df'])
    
    
    for d in range(20):
        for t in range(num_terms):
            if stats_dict['tf'][t][d] != 0:
                term1 = (1 + np.log(1 +np.log(stats_dict['tf'][t][d])))/( (1-s) + s*stats_dict['dl'][d]/stats_dict['avgdl'])
                term2 = stats_dict['qtf'][t]
                term3 = np.log((stats_dict['N']+1)/stats_dict['df'][t])
                #print(term1, term2, term3)
                score[d] +=  term1 * term2 * term3 
    return score

#bm25 ranking model
def score_bm25(stats_dict):
    score = np.zeros(20)
    
    k1 = 1.2
    b = 0.75
    k3 = 1000
    
    num_terms = len(stats_dict['df'])
    for d in range(20):
        K = k1 *( (1-b) + b*stats_dict['dl'][d]/stats_dict['avgdl'])
        
        for t in range(num_terms):
            w = np.log( (stats_dict['N'] - stats_dict['df'][t] + 0.5) / (stats_dict['df'][t] + 0.5))
            term2 = ( (k1 + 1)*stats_dict['tf'][t][d]) / (K + stats_dict['tf'][t][d])
            term3 = ( (k3 + 1)*stats_dict['qtf'][t])/ (k3 + stats_dict['qtf'][t])
            score[d] += w*term2*term3
    
    return score    

# compute ranks from scores
def get_ranks(scores):
    array = np.array(scores)*-1
    temp = array.argsort()
    ranks = np.empty_like(temp)
    ranks[temp] = np.arange(len(array))
    ranks = ranks + 1
    return ranks   


if __name__ == "__main__":
	num_q = 21
	num_docs = 20

	#save html landing page content as txt
	print('saving html content as txt...')
	for q in range(1, num_q + 1):
		for d in range(1, num_docs + 1):
			html = 'ranking/Query_{}/Document/{}.html'.format(q,d)
			soup = BeautifulSoup(open(html), "html.parser")
			text = soup.getText()
			splitted = text.lower().split()
			sp_char = " .,!#$%^&*();:\n\t\\\"?!{}[]<>|~`'+=-_'"
			cleaned_text = [term.strip(sp_char) for term in splitted]
			
			text = ""
			for word in cleaned_text:
				text += word + " "
			
			outfilename = 'ranking/Query_{}/Document/{}.txt'.format(q,d)
			open(outfilename, 'w').write(text)

	
	
	#get query strings
	all_queries = {}
	for query in range(21):
		with open('ranking/Query_' + str(query + 1) + '/urls.tsv') as f:
			next(f)
			for line in f:
				q, _, _, _ = line.split('\t')
				all_queries[str(query + 1)] = q
				break

	#compute rank scores with vsm and bm25 models
	print('computing rank scores for vsm and bm25 models..')
	scores_vsm = {}
	scores_bm25 = {}
	for query in range(21):
		stats = doc_stats_perq(all_queries[str(query + 1)], 'ranking/Query_' + str(query+1) + '/Document/')
		scores_vsm[str(query+1)] = score_vsm(stats)
		scores_bm25[str(query+1)] = score_bm25(stats)

	print('scores computed. saving now...')
	#save computed scores into files
	for query in range(21):
		outfile_bm25 = 'ranking/RM_BM25_' + str(query + 1) + '_2020280598.csv'
		outfile_vsm = 'ranking/RM_VSM_' + str(query + 1) + '_2020280598.csv'
		
		urls = []
		with open('ranking/Query_' + str(query + 1) + '/urls.tsv') as f:
			next(f)
			for line in f:
				qtext,qdesc, url, docid = line.split('\t')
				urls.append(url)
			f.close()

		with open(outfile_bm25, 'a') as file:
			file.write('query text,query description,url,docID,rank,rankScore\n')
			for d in range(20):
				file.write('"{}","{}","{}","{}","{}","{:.3f}"\n'.format(qtext, qdesc, urls[d], d+1, get_ranks(scores_bm25[str(query + 1)])[d], scores_bm25[str(query + 1)][d]))
			file.close()
			
		with open(outfile_vsm, 'a') as file:
			file.write('query text,query description,url,docID,rank,rankScore\n')
			for d in range(20):
				file.write('"{}","{}","{}","{}","{}","{:.3f}"\n'.format(qtext, qdesc, urls[d], d+1, get_ranks(scores_vsm[str(query + 1)])[d], scores_vsm[str(query + 1)][d]))
			file.close()