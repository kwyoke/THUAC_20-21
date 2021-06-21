from whoosh.qparser import QueryParser
from whoosh import scoring
from whoosh.index import open_dir
import sys
 
def search(indexdir, query_str, topN):
    ix = open_dir(indexdir)
    
    #query_str = "stream processing"
    # Top 'n' documents as result
    #topN = 5
    top_outputs = {}
    top_outputs['imgpath'] = []
    top_outputs['framenum'] = []
    top_outputs['slidetxt'] = []
    top_outputs['score'] = []
    
    with ix.searcher(weighting=scoring.BM25F) as searcher:
        query = QueryParser("content", ix.schema).parse(query_str)
        results = searcher.search(query,limit=topN)
        for i in range(topN):
            top_outputs['slidetxt'].append(results[i]['textdata'])
            top_outputs['score'].append(results[i].score)
            top_outputs['framenum'].append(int(results[i]['title'][:-4]))
            top_outputs['imgpath'].append(results[i]['title'][:-4] + '.jpg')
            #print(results[i]['title'], str(results[i].score), results[i]['textdata'])
    
    return top_outputs