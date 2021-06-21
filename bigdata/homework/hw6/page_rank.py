import sys
import re
from pyspark import SparkConf, SparkContext
import time

if __name__ == '__main__':

    # Create Spark context.
    conf = SparkConf()
    sc = SparkContext(conf=conf)
    lines = sc.textFile(sys.argv[1])

    first = time.time()

    # Students: Implement PageRank!
    links = lines.flatMap(lambda x: x.split('\n')) \
        .map(lambda node: (node.split('\t')[0],node.split('\t')[1])) \
        .map(lambda keyval: (keyval[0], [ keyval[1] ])) \
        .reduceByKey(lambda a,b: list(set(a+b)))

    numNodes = lines.flatMap(lambda x: re.split('[^\w]+', x)) \
        .map(lambda node: (node,1)) \
        .reduceByKey(lambda a,b: a+b) \
        .count()

    init_score = 1/numNodes
    
    ranks = lines.flatMap(lambda x: re.split('[^\w]+', x)) \
        .map(lambda node: (node, init_score)) \
        .reduceByKey(lambda a,b: a) 

    # damping factor d = 0.8
    d = 0.8
    for i in range(100):
        ranks = links.join(ranks) \
            .flatMap(lambda x: map(lambda dest: (dest, d* ((x[1][1] + (1-d)/numNodes)/ len(x[1][0]) ) ), x[1][0])) \
            .reduceByKey(lambda a,b: a+b)
    ranks = ranks.map(lambda nodescore: (nodescore[0], nodescore[1] + (1-d)/numNodes)) #add (1-d)/N to every node

    highest = ranks.sortBy(lambda x: -x[1]).collect() 
    print("5 highest:", highest[:5])

    last = time.time()

    print("Total program time: %.2f seconds" % (last - first))
    sc.stop()