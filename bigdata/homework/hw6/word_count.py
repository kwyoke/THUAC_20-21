import re
import sys
from pyspark import SparkConf, SparkContext
import time


if __name__ == '__main__':
    conf = SparkConf()
    sc = SparkContext(conf=conf)
    lines = sc.textFile(sys.argv[1])

    first = time.time()

    # Students: Implement Word Count!
    counts = lines.flatMap(lambda x: re.split('[^\w]+', x)).map(lambda word: (word,1)).reduceByKey(lambda a,b: a+b)
    print(counts.sortBy(lambda x: -x[1]).collect()[:10])
    
    last = time.time()

    print("Total program time: %.2f seconds" % (last - first))
    sc.stop()