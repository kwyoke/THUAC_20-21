HOSTIP := 192.168.1.152 # set to local ip address (hostname -I)

runall: setup bulkload popNshard storemultimedia

####################### setting up docker containers and shards #############################
setup: config dbms1 dbms2 grid1 grid2 mongos1 # might have connection error sporadically, just run `make setup` again

config:
	docker-compose -f config-server/docker-compose.yaml up -d
	docker exec -it cfg1 bash -c "mongo --eval 'rs.initiate({_id: \"cfg\", configsvr: true, members: [{ _id : 0, host : \"${HOSTIP}:40004\"}, { _id : 1, host : \"${HOSTIP}:40005\"}, { _id : 2, host : \"${HOSTIP}:40006\"}]})'"

dbms1:
	docker-compose -f dbms1shard/docker-compose.yaml up -d
	docker exec -it dbms1_svr bash -c "mongo --eval 'rs.initiate({_id: \"dbms1\", members: [{ _id : 0, host : \"${HOSTIP}:50005\" }]})'"

dbms2:
	docker-compose -f dbms2shard/docker-compose.yaml up -d
	docker exec -it dbms2_svr bash -c "mongo --eval 'rs.initiate({_id: \"dbms2\", members: [{ _id : 0, host : \"${HOSTIP}:50006\" }]})'"
	
grid1:
	docker-compose -f grid1shard/docker-compose.yaml up -d
	docker exec -it grid1_svr bash -c "mongo --eval 'rs.initiate({_id: \"grid1\", members: [{ _id : 0, host : \"${HOSTIP}:50007\" }]})'"
	
grid2:
	docker-compose -f grid2shard/docker-compose.yaml up -d
	docker exec -it grid2_svr bash -c "mongo --eval 'rs.initiate({_id: \"grid2\", members: [{ _id : 0, host : \"${HOSTIP}:50008\" }]})'"
	
mongos1:
	env HOSTIP=${HOSTIP} docker-compose -f mongos/docker-compose.yaml up -d
	docker exec -it mongos1 bash -c "mongo --eval 'sh.addShard(\"dbms1/${HOSTIP}:50005\")'"
	docker exec -it mongos1 bash -c "mongo --eval 'sh.addShard(\"dbms2/${HOSTIP}:50006\")'"
	docker exec -it mongos1 bash -c "mongo --eval 'sh.addShard(\"grid1/${HOSTIP}:50007\")'"
	docker exec -it mongos1 bash -c "mongo --eval 'sh.addShard(\"grid2/${HOSTIP}:50008\")'"
	
############################ Bulkload user, article, read collections ############################
bulkload: copyfiles import2mongo

copyfiles:
	docker cp proj_data/user.dat mongos1:/user.dat
	docker cp proj_data/article.dat mongos1:/article.dat
	docker cp proj_data/read.dat mongos1:/read.dat

import2mongo:
	docker exec -it mongos1 bash -c "mongoimport --db ddbs --collection user --file user.dat"
	docker exec -it mongos1 bash -c "mongoimport --db ddbs --collection article --file article.dat"
	docker exec -it mongos1 bash -c "mongoimport --db ddbs --collection read --file read.dat"

########################## Populate and shard collections ###################################
popNshard:
	docker cp popNshard.js mongos1:/popNshard.js	
	docker exec -it mongos1 bash -c "mongo < popNshard.js"

########################## Store multimedia data in gridfs ################################
storemultimedia:
	env HOSTIP=${HOSTIP} ./bulk_store_multimedia.sh