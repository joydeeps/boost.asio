for a in {1..1000}; 
do (sleep 1.$RANDOM; 
echo -e "hello world $RANDOM\\0" | nc localhost 11235)& done; 
wait
