import pandas as pd
import os
import numpy as np

#helper function to get relevance scores from csv file
def get_annot(filename):
    list_annot = []
    with open(filename) as f:
        next(f)
        for line in f:
            list_line = line.split(',')
            list_int = list_line[-1][1:-2].split(';')
            list_int = [int(i) for i in list_int]
            list_annot.append( list_int)
    return list_annot


#calculate pairwise kappa score
def pairwise_kappa(annot1, annot2, numlev):
    both_agree = 0
    nk1 = np.zeros(numlev) #number of times annot1 rated k
    nk2 = np.zeros(numlev)
    
    # set relevant to rating 0, non-rel to rating -1
    if numlev == 2:
        annot1 = [0 if el > 0 else -1 for el in annot1]
        annot2 = [0 if el > 0 else -1 for el in annot2]
    
    N = len(annot1)
    
    for i in range(N):
        if annot1[i] == annot2[i]:
            both_agree += 1
        nk1[annot1[i] + 1] += 1
        nk2[annot2[i] + 1] += 1
    
    PA = both_agree/N
    PE = 0
    for i in range(numlev):
        PE += nk1[i]*nk2[i]
    PE /= N**2
    
    if PE==1: #set to 1 for perfect agreement
        return 1
    
    Kappa = (PA - PE)/ (1 - PE)
    
    return Kappa

#helper function to get BM25 and VSM ranks from csv file from Task 1
def get_ranks_frfile(filename):
    list_rank = []
    with open(filename) as f:
        next(f)
        for line in f:
            list_line = line.split('","')
            rank = int(list_line[-2])
            list_rank.append(rank)
    return list_rank

#calculate 11-point MAP
def MAP_11(rel_scores):
    cum_recall = []
    cum_prec = []
    
    # check total number of relevant elements
    totalrel = 0
    for score in rel_scores:
        if score >=1:
            totalrel += 1       
    if totalrel == 0:
        return 0
            
    # get cumulative recall and precision scores
    num_retrievedrel = 0
    for i in range(len(rel_scores)):
        if (rel_scores[i] >= 1):
            num_retrievedrel += 1
        cum_recall.append(num_retrievedrel/(totalrel))
        cum_prec.append(num_retrievedrel/(i+1))
    
    # only retain rec and prec scores that change
    rec_changes = []
    prec_changes = []
    rec_changes.append(cum_recall[0])
    prec_changes.append(cum_prec[0])
    for i in range(1, len(cum_recall)):
        if cum_recall[i] != cum_recall[i-1]:
            rec_changes.append(cum_recall[i])
            prec_changes.append(cum_prec[i])
    
    # assign retained rec and prec scores to the correct bin
    recall_template = np.arange(11)*0.1
    prec_template = [0]*11
    num_changes = len(rec_changes)
    for i in range(num_changes):
        for j in range(1,11):
            if rec_changes[i] < recall_template[j] and rec_changes[i] >= recall_template[j-1]:
                prec_template[j-1] = prec_changes[i]
    if rec_changes[-1] == recall_template[10]:
        prec_template[10] = prec_changes[-1]
    
    # set every prec value corresponding to each rec bin
    for i in range( len(prec_template) - 1):
        prec_template[i] = max(prec_template[i+1:])
        
    return np.mean(np.array(prec_template))


#calculate NDCG scores
def NDCG(rel_scores):
    DCG = np.zeros(len(rel_scores))
    DCG[0] = rel_scores[0]
    for i in range(1, len(rel_scores)):
        DCG[i] = DCG[i-1] + rel_scores[i]/np.log2(i+1)
    
    perfect_rank_scores = sorted(rel_scores, reverse = True)
    if perfect_rank_scores[0] == 0:
        return [0]*len(rel_scores) #prevent divide by 0

    iDCG = np.zeros(len(rel_scores))
    iDCG[0] = perfect_rank_scores[0]
    for i in range(1, len(rel_scores)):
        iDCG[i] = iDCG[i-1] + perfect_rank_scores[i]/np.log2(i+1)
        
    score_NDCG = [i / j for i, j in zip(DCG, iDCG)]
    
    return score_NDCG

# helper function to get SE involved in query
def get_SE(filename):
    with open(filename) as f:
        for line in f:
            list_line = line.split(',')
            SE1 = list_line[1].split('_')[0]
            SE2 = list_line[2].split('_')[0]
            return SE1, SE2



if __name__ == "__main__":
	print('computing kappa scores...')
	#calculate kappa scores
	kappa5 = []
	kappa2 = []
		
	for query in range(21): 
		tmp5 = []
		tmp2 = []
		list_annot = get_annot('evaluation/Query{}/relevance_annotation.csv'.format(query + 1))
		
		num_annots = len(list_annot[0])
		for i in range(num_annots):
			for j in range(i+1, num_annots):
				annot1 = list(np.array(list_annot)[:, i])
				annot2 = list(np.array(list_annot)[:, j])

				#10,11, 13, 14 have only one annotator, so set kappa score as None
				tmp5.append(pairwise_kappa(annot1, annot2, 5))
				tmp2.append(pairwise_kappa(annot1, annot2, 2))
				
		kappa5.append(np.mean(np.array(tmp5)))
		kappa2.append(np.mean(np.array(tmp2))) 

	print('computing MAP scores...')
	#calculate MAP scores
	map_bm25 = []
	map_vsm = []
	map_1 = []
	map_2 = []
		
	for query in range(21): 
		list_annot = get_annot('evaluation/Query{}/relevance_annotation.csv'.format(query + 1))
		rel_score = list(np.mean(np.array(list_annot), axis = 1))

		map_1.append(MAP_11(rel_score[:10]))
		map_2.append(MAP_11(rel_score[10:]))
		
		ranks_bm25 = get_ranks_frfile('ranking/RM_BM25_{}_2020280598.csv'.format(query + 1))
		rel_orderbm25 = [0]*20
		for i in range(20):
			rank_ind = ranks_bm25[i] - 1
			rel = rel_score[i]
			rel_orderbm25[rank_ind] = rel
			
		ranks_vsm = get_ranks_frfile('ranking/RM_VSM_{}_2020280598.csv'.format(query + 1))
		rel_ordervsm = [0]*20
		for i in range(20):
			rank_ind = ranks_vsm[i] -1
			rel = rel_score[i]
			rel_ordervsm[rank_ind] = rel
			
		map_bm25.append(MAP_11(rel_orderbm25))
		map_vsm.append(MAP_11(rel_ordervsm))

	
	print('computing ndcg scores...')
	#calculate NDCG scores
	ndcg5_bm25 = []
	ndcg10_bm25 = []
	ndcg5_vsm = []
	ndcg10_vsm = []

	ndcg5_1 = []
	ndcg10_1 = []
	ndcg5_2 = []
	ndcg10_2 = []
		
	for query in range(21):
		list_annot = get_annot('evaluation/Query{}/relevance_annotation.csv'.format(query + 1))
		rel_score = list(np.mean(np.array(list_annot), axis = 1))
		
		for i,sc in enumerate(rel_score):
			if sc >= 2:
				rel_score[i] = 3
			elif sc > 0.5 and sc < 2:
				rel_score[i] = 2
			elif sc > 0 and sc <=0.5:
				rel_score[i] = 1
			else:
				rel_score[i] = 0

		ndcg = NDCG(rel_score[:10])
		ndcg5_1.append(ndcg[4])
		ndcg10_1.append(ndcg[-1])
		
		ndcg = NDCG(rel_score[10:])
		ndcg5_2.append(ndcg[4])
		ndcg10_2.append(ndcg[-1])
		
		ranks_bm25 = get_ranks_frfile('ranking/RM_BM25_{}_2020280598.csv'.format(query + 1))
		rel_orderbm25 = [0]*20
		for i in range(20):
			rank_ind = ranks_bm25[i] - 1
			rel = rel_score[i]
			rel_orderbm25[rank_ind] = rel
			
		ranks_vsm = get_ranks_frfile('ranking/RM_VSM_{}_2020280598.csv'.format(query + 1))
		rel_ordervsm = [0]*20
		for i in range(20):
			rank_ind = ranks_vsm[i] -1
			rel = rel_score[i]
			rel_ordervsm[rank_ind] = rel
		
		ndcg = NDCG(rel_orderbm25)
		ndcg5_bm25.append(ndcg[4])
		ndcg10_bm25.append(ndcg[9])
		
		ndcg = NDCG(rel_ordervsm)
		ndcg5_vsm.append(ndcg[4])
		ndcg10_vsm.append(ndcg[9])

	print('saving all eval scores...')
	#save all evaluation scores
	all_scores = {}
	for query in range(21):
		SE1, SE2 = get_SE('evaluation/Query{}/SE_ranking.csv'.format(query+1))
		
		outfile = 'evaluation/EVA_{}_2020280598.csv'.format(query+1)
		with open(outfile, 'a') as f:
			f.write('measure,score\n')
			f.write('kappa2,{}\n'.format(kappa2[query]))
			f.write('kappa5,{}\n'.format(kappa5[query]))
			f.write('MAP-{},{}\n'.format(SE1, map_1[query]))
			f.write('MAP-{},{}\n'.format(SE2, map_2[query]))
			f.write('MAP-VSM,{}\n'.format(map_vsm[query]))
			f.write('MAP-BM25,{}\n'.format(map_bm25[query]))
			f.write('NDCG@5-{},{}\n'.format(SE1, ndcg5_1[query]))
			f.write('NDCG@5-{},{}\n'.format(SE2, ndcg5_2[query]))
			f.write('NDCG@5-VSM,{}\n'.format(ndcg5_vsm[query]))
			f.write('NDCG@5-BM25,{}\n'.format(ndcg5_bm25[query]))
			f.write('NDCG@10-{},{}\n'.format(SE1, ndcg10_1[query]))
			f.write('NDCG@10-{},{}\n'.format(SE2, ndcg10_2[query]))
			f.write('NDCG@10-VSM,{}\n'.format(ndcg10_vsm[query]))
			f.write('NDCG@10-BM25,{}\n'.format(ndcg10_bm25[query]))
		f.close()