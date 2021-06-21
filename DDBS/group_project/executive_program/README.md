# Instructions on running system executive program

This directory contains files required to quickly set up all the docker containers and shard clusters, and also bulkload, populating and sharding collections, and storing multimedia data in GridFS. Note that the docker-compose.yaml files here are slightly different from the ones in the mongoshell_tutorial directory (different container names, port numbers etc) for obvious reasons so don't mix up the yaml files.

## Generate toy data

Since the point of the executive program is to allow quick setup, the dataset should also be small. In proj_data/, there is a python script genTable_mongoDB_toy.py. Navigate to proj_data and run:
```
python genTable_mongoDB_toy.py
```
This generates roughly 80MB of read.dat, article.dat and read.dat data together with their multimedia data, stored in the proj_data/ directory.

## Run Makefile

The Makefile contains all the commands required for setting up docker containers, bulkloading, populating and sharding collections and storing multimedia data. Inspect the Makefile for more details. 

Also, **change the first line of the Makefile to your host IP address** where you want the MongoDB servers to connect to, you could find your local host IP address using 'hostname -I'. To run everything, navigate to directory containing Makefile (executive_program/) and run in bash:
```
make all
```
If you only want to run specific parts, e.g. just the setting up containers, then run: 
```
make setup # containers
make bulkload # copy and load user, article, tables
make popNshard # populate and shard collections (beread, popRank)
make store_multimedia # store multimedia data in GridFS servers
```
Note that there could be error messages about MongoDB failing to connect successfully. In this case, just run the same command again, usually the connection error is temporary only and the next try should be successful.

## Other information
Pretty much the Makefile sets everything up. The commands for populating and sharding the collections are in popNshard.js, and are identical to the mongo shell commands covered in the tutorial in mongoshell_tutorial/01_load_shard_populate_doc.md. 

Run auto_refresh_onarticle.py and auto_refresh_onread.py in terminals so that the collections will be updated when there are any changes.