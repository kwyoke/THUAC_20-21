// this script contins all mongo shell commands required to populate and shard collections for this project

// enable sharding for database
use ddbs
sh.enableSharding("ddbs")

//----------------- shard user
db.user.createIndex({"region": 1, "uid": 1})
sh.shardCollection("ddbs.user", {"region": 1, "uid": 1})
sh.disableBalancing("ddbs.user")
sh.addShardTag("dbms1", "BJ")
sh.addTagRange(
    "ddbs.user",
    {"region": "Beijing", "uid": MinKey},
    {"region": "Beijing", "uid": MaxKey},
    "BJ"
)
sh.addShardTag("dbms2", "HK")
sh.addTagRange(
    "ddbs.user",
    {"region": "Hong Kong", "uid": MinKey},
    {"region": "Hong Kong", "uid": MaxKey},
    "HK"
)
sh.enableBalancing("ddbs.user")

// -----------------shard article
db.article.createIndex({"category": 1, "aid": 1})
sh.shardCollection("ddbs.article", {"category": 1, "aid": 1})
sh.disableBalancing("ddbs.article")
sh.addShardTag("dbms1", "SCI")
sh.addTagRange(
            "ddbs.article",
            {"category": "science", "aid": MinKey},
            {"category": "science", "aid": MaxKey},
            "SCI"
        )
sh.addShardTag("dbms2", "TECH")
sh.addTagRange(
            "ddbs.article",
            {"category": "technology", "aid": MinKey},
            {"category": "technology", "aid": MaxKey},
            "TECH"
        )
sh.enableBalancing("ddbs.article")

// create articlesci and assign to dbms2
db.article.aggregate([
        { $match: {category: "science"}},
        { $merge: {into: "articlesci", whenMatched: "replace"}}
    ])
db.articlesci.createIndex({"category": 1, "aid": 1})
sh.shardCollection("ddbs.articlesci", {"category": 1, "aid": 1})
sh.disableBalancing("ddbs.articlesci")
sh.addShardTag("dbms2", "SCI2")
sh.addTagRange(
            "ddbs.articlesci",
            {"category": "science", "aid": MinKey},
            {"category": "science", "aid": MaxKey},
            "SCI2"
        )
sh.enableBalancing("ddbs.articlesci")

// ------------------- shard read 
// add region and category column to read
db.user.aggregate([
            { $project: {uid:1, region: 1}},
            { $out: "uid_reg"}
        ])
db.read.aggregate([
                { $lookup: {from: "uid_reg", localField: "uid", foreignField: "uid", as: "someField"}},
                { $addFields: { region: "$someField.region"}},
                { $unwind: "$region"},
                { $project: { someField: 0}},
                { $out: "read"}
            ],
            { allowDiskUse: true }
            )
db.article.aggregate([
            { $project: {aid:1, category: 1, timestamp: 1}},
            { $out: "aid_cat_ts"}
        ])
db.read.aggregate([
                { $lookup: {from: "aid_cat_ts", localField: "aid", foreignField: "aid", as: "someField"}},
                { $addFields: { category: "$someField.category", article_ts: "$someField.timestamp"}},
                { $unwind: "$category"},
                { $unwind: "$article_ts"},
                { $project: { someField: 0}},
                { $out: "read"}
            ],
            { allowDiskUse: true }
            )
//sharding
db.read.createIndex({"region": 1, "id": 1})
sh.shardCollection("ddbs.read", {"region": 1, "id": 1})
sh.disableBalancing("ddbs.read")
sh.addShardTag("dbms1", "BJ")
sh.addTagRange(
            "ddbs.read",
            {"region": "Beijing", "id": MinKey},
            {"region": "Beijing", "id": MaxKey},
            "BJ"
        )
sh.addShardTag("dbms2", "HK")
sh.addTagRange(
            "ddbs.read",
            {"region": "Hong Kong", "id": MinKey},
            {"region": "Hong Kong", "id": MaxKey},
            "HK"
        )
sh.enableBalancing("ddbs.read")


// --------------- populate beread
db.read.aggregate(
            [
                // group by aid and create new fields with aggregated counts and arrays
                {
                    $group: {
                        _id: "$aid",
                        category: { $first: "$category" },
                        timestamp: { $first: "$article_ts" },
                        readNum: { $sum: {$toInt: "$readOrNot" } },
                        readUidList: { $addToSet: { $cond: { if: { $eq: ["$readOrNot","1"] }, then: "$uid", else: "$$REMOVE"} } },
                        commentNum: { $sum: {$toInt: "$commentOrNot" } },
                        commentUidList: { $addToSet: { $cond: { if: { $eq: ["$commentOrNot","1"] }, then: "$uid", else: "$$REMOVE"} } },
                        agreeNum: { $sum: {$toInt: "$agreeOrNot" } },
                        agreeUidList: { $addToSet: { $cond: { if: { $eq: ["$agreeOrNot","1"] }, then: "$uid", else: "$$REMOVE"} } },
                        shareNum: { $sum: {$toInt: "$shareOrNot" } },
                        shareUidList: { $addToSet: { $cond: { if: { $eq: ["$shareOrNot","1"] }, then: "$uid", else: "$$REMOVE"} } },
                    }
                },

                // Modify aid from integer to string
                { $addFields: { "aid": {$concat: [ "a", "$_id" ]}}},

                { $out: "beread"}
            ],
            { allowDiskUse: true }
        )
//sharding
db.beread.createIndex({"category": 1, "aid": 1})
sh.shardCollection("ddbs.beread", {"category": 1, "aid": 1})
sh.disableBalancing("ddbs.beread")
sh.addShardTag("dbms1", "SCI")
sh.addTagRange(
            "ddbs.beread",
            {"category": "science", "aid": MinKey},
            {"category": "science", "aid": MaxKey},
            "SCI"
        )
sh.addShardTag("dbms2", "TECH")
sh.addTagRange(
            "ddbs.beread",
            {"category": "technology", "aid": MinKey},
            {"category": "technology", "aid": MaxKey},
            "TECH"
        )
sh.enableBalancing("ddbs.beread")
// bereadsci
db.beread.aggregate([
            { $match: {category: "science"}},
            { $merge: {into: "bereadsci", whenMatched: "replace"}}
        ])
db.bereadsci.createIndex({"category": 1, "aid": 1})
sh.shardCollection("ddbs.bereadsci", {"category": 1, "aid": 1})
sh.disableBalancing("ddbs.bereadsci")
sh.addShardTag("dbms2", "SCI2")
sh.addTagRange(
            "ddbs.bereadsci",
            {"category": "science", "aid": MinKey},
            {"category": "science", "aid": MaxKey},
            "SCI2"
        )
sh.enableBalancing("ddbs.bereadsci")

// -------------------popRank
//popRankMth
db.read.aggregate([
            // project relevant fields from db.read
            { $project: { date: {"$toDate": {"$toLong": "$timestamp"}}, aid: 1, readOrNot: 1, agreeOrNot: 1, commentOrNot: 1, shareOrNot: 1} },

            // add year and month fields
            { $addFields: {
                year: { $year: "$date" },
                month: { $month: "$date" },
                popScore: {$sum: [{$toInt: "$readOrNot"}, {$toInt: "$agreeOrNot"}, {$toInt: "$commentOrNot"}, {$toInt: "$shareOrNot"}]}}
            },

            // add unix timestamp defined only by yr and mth
            { $addFields: { timestamp: { $subtract: [ { $dateFromParts: { 'year' : "$year", 'month' : "$month"} }, new Date("1970-01-01") ] }}},

            // Group by year, month, aid and compute popularity score
            {
                $group: {
                    _id: { "timestamp": "$timestamp", "aid": "$aid"},
                    popScoreAgg: { $sum: "$popScore" }
                }
            },

            // sort by popScore each month
            { $sort: {"_id.timestamp": 1, "popScoreAgg": -1} },

            // store all articles in sorted order in array for each month
            {
                $group: {
                    _id: "$_id.timestamp",
                    articleAidList: {$push: "$_id.aid"}
                }
            },

            // keep only top five articles in array
            { 
                $project: { 
                    _id: {$concat: ["m", { $toString: "$_id" }]}, 
                    timestamp: "$_id", 
                    articleAidList: { $slice: ["$articleAidList", 5]},
                    temporalGranularity: "monthly"
                    }
            },

            // output
            {"$out": "popRankMth"}
        ],
        { allowDiskUse: true })
//popRankWk
db.read.aggregate([
            // project relevant fields from db.read
            { $project: { date: {"$toDate": {"$toLong": "$timestamp"}}, aid: 1, readOrNot: 1, agreeOrNot: 1, commentOrNot: 1, shareOrNot: 1} },

            // add year and month fields
            { $addFields: {
                year: { $year: "$date" },
                month: { $month: "$date" },
                week: {$week: "$date"},
                popScore: {$sum: [{$toInt: "$readOrNot"}, {$toInt: "$agreeOrNot"}, {$toInt: "$commentOrNot"}, {$toInt: "$shareOrNot"}]}}
            },

            // add unix timestamp defined only by yr and mth
            { $addFields: { timestamp: { $subtract: [ { $dateFromParts: { 'isoWeekYear' : "$year", 'isoWeek' : "$week"} }, new Date("1970-01-01") ] }}},

            // Group by year, month, aid and compute popularity score
            {
                $group: {
                    _id: { "timestamp": "$timestamp", "aid": "$aid"},
                    popScoreAgg: { $sum: "$popScore" }
                }
            },

            // sort by popScore each month
            { $sort: {"_id.timestamp": 1, "popScoreAgg": -1} },

            // store all articles in sorted order in array for each month
            {
                $group: {
                    _id: "$_id.timestamp",
                    articleAidList: {$push: "$_id.aid"}
                }
            },

            // keep only top five articles in array
            { 
                $project: { 
                    _id: {$concat: ["w", { $toString: "$_id" }]}, 
                    timestamp: "$_id", 
                    articleAidList: { $slice: ["$articleAidList", 5]},
                    temporalGranularity: "weekly"
                    }
            },

            // output
            {"$out": "popRankWk"}
        ],
        { allowDiskUse: true })
//popRankDay
db.read.aggregate([
            // project relevant fields from db.read
            { $project: { date: {"$toDate": {"$toLong": "$timestamp"}}, aid: 1, readOrNot: 1, agreeOrNot: 1, commentOrNot: 1, shareOrNot: 1} },

            // add year and month fields
            { $addFields: {
                year: { $year: "$date" },
                month: { $month: "$date" },
                day: {$dayOfYear: "$date" },
                popScore: {$sum: [{$toInt: "$readOrNot"}, {$toInt: "$agreeOrNot"}, {$toInt: "$commentOrNot"}, {$toInt: "$shareOrNot"}]}}
            },

            // add unix timestamp defined only by yr and mth
            { $addFields: { timestamp: { $subtract: [ { $dateFromParts: { 'year' : "$year", 'month' : "$month", 'day': "$day"} }, new Date("1970-01-01") ] }}},

            // Group by year, month, aid and compute popularity score
            {
                $group: {
                    _id: { "timestamp": "$timestamp", "aid": "$aid"},
                    popScoreAgg: { $sum: "$popScore" }
                }
            },

            // sort by popScore each month
            { $sort: {"_id.timestamp": 1, "popScoreAgg": -1} },

            // store all articles in sorted order in array for each month
            {
                $group: {
                    _id: "$_id.timestamp",
                    articleAidList: {$push: "$_id.aid"}
                }
            },

            // keep only top five articles in array
            { 
                $project: { 
                    _id: {$concat: ["d", { $toString: "$_id" }]}, 
                    timestamp: "$_id", 
                    articleAidList: { $slice: ["$articleAidList", 5]},
                    temporalGranularity: "daily"
                    }
            },

            // output
            {"$out": "popRankDay"}
        ],
        { allowDiskUse: true })
//combine into popRank
db.popRankMth.find().forEach( function(doc) { db.popRank.insert(doc) })
db.popRankWk.find().forEach( function(doc) { db.popRank.insert(doc) })
db.popRankDay.find().forEach( function(doc) { db.popRank.insert(doc) })
db.popRank.aggregate([ {$sort: {timestamp:1}}, {$out: "popRank"} ])

// popRankSci
db.read.aggregate([
            // retain only science articles
            { $match: {category: "science"}},

            // project relevant fields from db.read
            { $project: { date: {"$toDate": {"$toLong": "$timestamp"}}, aid: 1, readOrNot: 1, agreeOrNot: 1, commentOrNot: 1, shareOrNot: 1} },

            // add year and month fields
            { $addFields: {
                year: { $year: "$date" },
                month: { $month: "$date" },
                popScore: {$sum: [{$toInt: "$readOrNot"}, {$toInt: "$agreeOrNot"}, {$toInt: "$commentOrNot"}, {$toInt: "$shareOrNot"}]}}
            },

            // add unix timestamp defined only by yr and mth
            { $addFields: { timestamp: { $subtract: [ { $dateFromParts: { 'year' : "$year", 'month' : "$month"} }, new Date("1970-01-01") ] }}},

            // Group by year, month, aid and compute popularity score
            {
                $group: {
                    _id: { "timestamp": "$timestamp", "aid": "$aid"},
                    popScoreAgg: { $sum: "$popScore" }
                }
            },

            // sort by popScore each month
            { $sort: {"_id.timestamp": 1, "popScoreAgg": -1} },

            // store all articles in sorted order in array for each month
            {
                $group: {
                    _id: "$_id.timestamp",
                    articleAidList: {$push: "$_id.aid"}
                }
            },

            // keep only top five articles in array
            { 
                $project: { 
                    _id: {$concat: ["m", { $toString: "$_id" }]}, 
                    timestamp: "$_id", 
                    articleAidList: { $slice: ["$articleAidList", 5]},
                    temporalGranularity: "monthly"
                    }
            },

            // output
            {"$out": "popRankSciMth"}
        ],
        { allowDiskUse: true })
db.read.aggregate([
            // only look for science articles
            { $match: { category: "science"}},

            // project relevant fields from db.read
            { $project: { date: {"$toDate": {"$toLong": "$timestamp"}}, aid: 1, readOrNot: 1, agreeOrNot: 1, commentOrNot: 1, shareOrNot: 1} },

            // add year and month fields
            { $addFields: {
                year: { $year: "$date" },
                month: { $month: "$date" },
                week: {$week: "$date"},
                popScore: {$sum: [{$toInt: "$readOrNot"}, {$toInt: "$agreeOrNot"}, {$toInt: "$commentOrNot"}, {$toInt: "$shareOrNot"}]}}
            },

            // add unix timestamp defined only by yr and mth
            { $addFields: { timestamp: { $subtract: [ { $dateFromParts: { 'isoWeekYear' : "$year", 'isoWeek' : "$week"} }, new Date("1970-01-01") ] }}},

            // Group by year, month, aid and compute popularity score
            {
                $group: {
                    _id: { "timestamp": "$timestamp", "aid": "$aid"},
                    popScoreAgg: { $sum: "$popScore" }
                }
            },

            // sort by popScore each month
            { $sort: {"_id.timestamp": 1, "popScoreAgg": -1} },

            // store all articles in sorted order in array for each month
            {
                $group: {
                    _id: "$_id.timestamp",
                    articleAidList: {$push: "$_id.aid"}
                }
            },

            // keep only top five articles in array
            { 
                $project: { 
                    _id: {$concat: ["w", { $toString: "$_id" }]}, 
                    timestamp: "$_id", 
                    articleAidList: { $slice: ["$articleAidList", 5]},
                    temporalGranularity: "weekly"
                    }
            },

            // output
            {"$out": "popRankSciWk"}
        ],
        { allowDiskUse: true })
db.read.aggregate([
            // only look for science articles
            { $match: { category: "science"}},

            // project relevant fields from db.read
            { $project: { date: {"$toDate": {"$toLong": "$timestamp"}}, aid: 1, readOrNot: 1, agreeOrNot: 1, commentOrNot: 1, shareOrNot: 1} },

            // add year and month fields
            { $addFields: {
                year: { $year: "$date" },
                month: { $month: "$date" },
                day: {$dayOfYear: "$date" },
                popScore: {$sum: [{$toInt: "$readOrNot"}, {$toInt: "$agreeOrNot"}, {$toInt: "$commentOrNot"}, {$toInt: "$shareOrNot"}]}}
            },

            // add unix timestamp defined only by yr and mth
            { $addFields: { timestamp: { $subtract: [ { $dateFromParts: { 'year' : "$year", 'month' : "$month", 'day': "$day"} }, new Date("1970-01-01") ] }}},

            // Group by year, month, aid and compute popularity score
            {
                $group: {
                    _id: { "timestamp": "$timestamp", "aid": "$aid"},
                    popScoreAgg: { $sum: "$popScore" }
                }
            },

            // sort by popScore each month
            { $sort: {"_id.timestamp": 1, "popScoreAgg": -1} },

            // store all articles in sorted order in array for each month
            {
                $group: {
                    _id: "$_id.timestamp",
                    articleAidList: {$push: "$_id.aid"}
                }
            },

            // keep only top five articles in array
            { 
                $project: { 
                    _id: {$concat: ["d", { $toString: "$_id" }]}, 
                    timestamp: "$_id", 
                    articleAidList: { $slice: ["$articleAidList", 5]},
                    temporalGranularity: "daily"
                    }
            },

            // output
            {"$out": "popRankSciDay"}
        ],
        { allowDiskUse: true })
db.popRankSciMth.find().forEach( function(doc) { db.popRankSci.insert(doc) })
db.popRankSciWk.find().forEach( function(doc) { db.popRankSci.insert(doc) })
db.popRankSciDay.find().forEach( function(doc) { db.popRankSci.insert(doc) })
db.popRankSci.aggregate([ {$sort: {timestamp:1}}, {$out: "popRankSci"} ])

//popRankTech
db.read.aggregate([
            // retain only technology articles
            { $match: {category: "technology"}},

            // project relevant fields from db.read
            { $project: { date: {"$toDate": {"$toLong": "$timestamp"}}, aid: 1, readOrNot: 1, agreeOrNot: 1, commentOrNot: 1, shareOrNot: 1} },

            // add year and month fields
            { $addFields: {
                year: { $year: "$date" },
                month: { $month: "$date" },
                popScore: {$sum: [{$toInt: "$readOrNot"}, {$toInt: "$agreeOrNot"}, {$toInt: "$commentOrNot"}, {$toInt: "$shareOrNot"}]}}
            },

            // add unix timestamp defined only by yr and mth
            { $addFields: { timestamp: { $subtract: [ { $dateFromParts: { 'year' : "$year", 'month' : "$month"} }, new Date("1970-01-01") ] }}},

            // Group by year, month, aid and compute popularity score
            {
                $group: {
                    _id: { "timestamp": "$timestamp", "aid": "$aid"},
                    popScoreAgg: { $sum: "$popScore" }
                }
            },

            // sort by popScore each month
            { $sort: {"_id.timestamp": 1, "popScoreAgg": -1} },

            // store all articles in sorted order in array for each month
            {
                $group: {
                    _id: "$_id.timestamp",
                    articleAidList: {$push: "$_id.aid"}
                }
            },

            // keep only top five articles in array
            { 
                $project: { 
                    _id: {$concat: ["m", { $toString: "$_id" }]}, 
                    timestamp: "$_id", 
                    articleAidList: { $slice: ["$articleAidList", 5]},
                    temporalGranularity: "monthly"
                    }
            },

            // output
            {"$out": "popRankTechMth"}
        ],
        { allowDiskUse: true })
db.read.aggregate([
            // only look for technology articles
            { $match: { category: "technology"}},

            // project relevant fields from db.read
            { $project: { date: {"$toDate": {"$toLong": "$timestamp"}}, aid: 1, readOrNot: 1, agreeOrNot: 1, commentOrNot: 1, shareOrNot: 1} },

            // add year and month fields
            { $addFields: {
                year: { $year: "$date" },
                month: { $month: "$date" },
                week: {$week: "$date"},
                popScore: {$sum: [{$toInt: "$readOrNot"}, {$toInt: "$agreeOrNot"}, {$toInt: "$commentOrNot"}, {$toInt: "$shareOrNot"}]}}
            },

            // add unix timestamp defined only by yr and mth
            { $addFields: { timestamp: { $subtract: [ { $dateFromParts: { 'isoWeekYear' : "$year", 'isoWeek' : "$week"} }, new Date("1970-01-01") ] }}},

            // Group by year, month, aid and compute popularity score
            {
                $group: {
                    _id: { "timestamp": "$timestamp", "aid": "$aid"},
                    popScoreAgg: { $sum: "$popScore" }
                }
            },

            // sort by popScore each month
            { $sort: {"_id.timestamp": 1, "popScoreAgg": -1} },

            // store all articles in sorted order in array for each month
            {
                $group: {
                    _id: "$_id.timestamp",
                    articleAidList: {$push: "$_id.aid"}
                }
            },

            // keep only top five articles in array
            { 
                $project: { 
                    _id: {$concat: ["w", { $toString: "$_id" }]}, 
                    timestamp: "$_id", 
                    articleAidList: { $slice: ["$articleAidList", 5]},
                    temporalGranularity: "weekly"
                    }
            },

            // output
            {"$out": "popRankTechWk"}
        ],
        { allowDiskUse: true })
db.read.aggregate([
            // only look for technology articles
            { $match: { category: "technology"}},

            // project relevant fields from db.read
            { $project: { date: {"$toDate": {"$toLong": "$timestamp"}}, aid: 1, readOrNot: 1, agreeOrNot: 1, commentOrNot: 1, shareOrNot: 1} },

            // add year and month fields
            { $addFields: {
                year: { $year: "$date" },
                month: { $month: "$date" },
                day: {$dayOfYear: "$date" },
                popScore: {$sum: [{$toInt: "$readOrNot"}, {$toInt: "$agreeOrNot"}, {$toInt: "$commentOrNot"}, {$toInt: "$shareOrNot"}]}}
            },

            // add unix timestamp defined only by yr and mth
            { $addFields: { timestamp: { $subtract: [ { $dateFromParts: { 'year' : "$year", 'month' : "$month", 'day': "$day"} }, new Date("1970-01-01") ] }}},

            // Group by year, month, aid and compute popularity score
            {
                $group: {
                    _id: { "timestamp": "$timestamp", "aid": "$aid"},
                    popScoreAgg: { $sum: "$popScore" }
                }
            },

            // sort by popScore each month
            { $sort: {"_id.timestamp": 1, "popScoreAgg": -1} },

            // store all articles in sorted order in array for each month
            {
                $group: {
                    _id: "$_id.timestamp",
                    articleAidList: {$push: "$_id.aid"}
                }
            },

            // keep only top five articles in array
            { 
                $project: { 
                    _id: {$concat: ["d", { $toString: "$_id" }]}, 
                    timestamp: "$_id", 
                    articleAidList: { $slice: ["$articleAidList", 5]},
                    temporalGranularity: "daily"
                    }
            },

            // output
            {"$out": "popRankTechDay"}
        ],
        { allowDiskUse: true })
db.popRankTechMth.find().forEach( function(doc) { db.popRankTech.insert(doc) })
db.popRankTechWk.find().forEach( function(doc) { db.popRankTech.insert(doc) })
db.popRankTechDay.find().forEach( function(doc) { db.popRankTech.insert(doc) })
db.popRankTech.aggregate([ {$sort: {timestamp:1}}, {$out: "popRankTech"} ])

//sharding popRank
db.popRank.createIndex({"_id": 1})
sh.shardCollection("ddbs.popRank", {"_id": 1})
sh.disableBalancing("ddbs.popRank")
sh.addShardTag("dbms2", "POPALL")
sh.addTagRange(
            "ddbs.popRank",
            {"_id": MinKey},
            {"_id": MaxKey},
            "POPALL"
        )
sh.enableBalancing("ddbs.popRank")

//sharding popRankTech
db.popRankTech.createIndex({"_id": 1})
sh.shardCollection("ddbs.popRankTech", {"_id": 1})
sh.disableBalancing("ddbs.popRankTech")
sh.addShardTag("dbms2", "POPTECH")
sh.addTagRange(
            "ddbs.popRankTech",
            {"_id": MinKey},
            {"_id": MaxKey},
            "POPTECH"
        )
sh.enableBalancing("ddbs.popRankTech")

//sharding popRankSci and popRankSci2
db.popRankSci.find().forEach( function(doc) { db.popRankSci2.insert(doc) })
db.popRankSci.createIndex({"_id": 1})
sh.shardCollection("ddbs.popRankSci", {"_id": 1})
sh.disableBalancing("ddbs.popRankSci")
sh.addShardTag("dbms1", "POPSCI")
sh.addTagRange(
            "ddbs.popRankSci",
            {"_id": MinKey},
            {"_id": MaxKey},
            "POPSCI"
        )
sh.enableBalancing("ddbs.popRankSci")
db.popRankSci2.createIndex({"_id": 1})
sh.shardCollection("ddbs.popRankSci2", {"_id": 1})
sh.disableBalancing("ddbs.popRankSci2")
sh.addShardTag("dbms2", "POPSCI2")
sh.addTagRange(
            "ddbs.popRankSci2",
            {"_id": MinKey},
            {"_id": MaxKey},
            "POPSCI2"
        )
sh.enableBalancing("ddbs.popRankSci2")

// ---------------------gridFS
db.fs.chunks.createIndex({"files_id": 1})
sh.shardCollection("ddbs.fs.chunks", {"files_id": "hashed"})
sh.addShardTag("grid1", "MEDIA")
sh.addTagRange(
            "ddbs.fs.chunks",
            {"files_id": MinKey},
            {"files_id": MaxKey},
            "MEDIA"
        )
sh.addShardTag("grid2", "MEDIA")
sh.addTagRange(
            "ddbs.fs.chunks",
            {"files_id": MinKey},
            {"files_id": MaxKey},
            "MEDIA"
        )
sh.enableBalancing("ddbs.fs.chunks")