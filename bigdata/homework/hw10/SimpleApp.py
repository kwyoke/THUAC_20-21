from __future__ import print_function
import sys
from pyspark import SparkContext
from pyspark.streaming import StreamingContext

sc = SparkContext(appName="Py_HDFSWordCount")
ssc = StreamingContext(sc, 60)
ssc.checkpoint("hdfs://intro00:9000/user/2020280598/hw10output/checkpoint")

def updateFunc(new_values, last_sum):
    return sum(new_values) + (last_sum or 0)

lines = ssc.textFileStream("hdfs://intro00:9000/user/2020280598/stream")  #  you should change path to your own directory on hdfs

running_counts = lines.flatMap(lambda line: line.split(" "))\
               .map(lambda x: (x, 1))\
               .updateStateByKey(updateFunc)

sorted_ = running_counts.transform(lambda rdd: rdd.sortBy(lambda x: x[1], ascending=False))
sorted_.pprint(100)

sorted_.saveAsTextFiles("hdfs://intro00:9000/user/2020280598/hw10output/")

ssc.start()
ssc.awaitTermination()