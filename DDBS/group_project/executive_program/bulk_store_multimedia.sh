#！/bin/bash

for DIR in proj_data/articles/*/
do
    for FILE in "${DIR}"*
        do

        BASEFILENAME=$(basename $FILE)
        mongofiles --host=${HOSTIP}:60001 --local=$FILE -d=ddbs put $BASEFILENAME 
    done      
done