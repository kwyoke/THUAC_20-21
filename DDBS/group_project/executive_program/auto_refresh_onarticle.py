import pymongo
from pymongo import MongoClient

client = MongoClient('mongodb://192.168.1.152:60000/')
db = client.ddbs

with db.article.watch(
        [{'$match': {'fullDocument.category': 'science'}}]) as stream:
    for change in stream:
        print("db.articlesci updated", change['fullDocument'])

        # aggregate instead of just insert to account for all types of updates (insertion, removal, changed fields)
        db.article.aggregate([
            {"$match": {"category": "science"}},
            {"$merge": {"into": "articlesci", "whenMatched":"replace"}}
        ])