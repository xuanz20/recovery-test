user=$1
password=$2 

./eval $user $password 0 0.5 throughput_5_noRDB.txt
./eval $user $password 0 0.9 throughput_9_noRDB.txt