user=$1
password=$2 

function shut_server() {
    kill $server_pid
    sleep 1
    exit
}

while true
do
    redis-server &
    wait $!
    echo ""
    sleep 1
done &

server_pid=$!

trap "shut_server" EXIT

./eval $user $password 0.9 throughput_0.9.txt
./eval $user $password 0.5 throughput_0.5.txt

kill $server_pid