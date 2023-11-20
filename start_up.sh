#!/bin/bash

gcc ./client.c -o 0clientrun
gcc ./storage_server.c -o ssrun
gcc ./naming_server.c -o nsrun

declare -i numclient=1
declare -i numss=1

# Function to open a new terminal window and execute a command
run_in_new_terminal() {
    osascript -e "tell application \"Terminal\" to do script \"$1\""
}

# Start nameserver 
run_in_new_terminal "./nsrun"

sleep 1

declare -i clientBasePort=45672
declare -i clientportstride=2

declare -i StorageServerNsBaseport=51867
declare -i StorageServerClientBaseport=51867

declare -i ssportstride=2

# Starting clients
for ((i=0; i<numclient; i++)); do
    tempcport=$((clientBasePort + i * clientportstride))
    run_in_new_terminal "./clientrun $tempcport"
done

# Starting storageserver
for ((i=0; i<numss; i++)); do
    tempnsport=$((StorageServerNsBaseport + i * ssportstride))
    tempsscport=$((StorageServerClientBaseport + i * StorageServerClientBaseport))
    run_in_new_terminal "./ssrun $tempnsport $tempsscport"
done


# #!bin/bash

# gcc ./client.c -o clientrun
# gcc ./storage_server.c -o ssrun
# gcc ./naming_server.c -o nsrun

# declare -i numclient 1
# declare -i numss 1

# # start nameserver 
# bash ./nsrun

# sleep(1)

# declare -i clientBasePort 45672
# declare -i clientportstride 2

# declare -i StorageServerNsBaseport 51867
# declare -i StorageServerClientBaseport 51867

# declare -i ssportstride 2

# # starting clients
# for ((i=0; i<$numclient;i++)) do
# tempcport = $clientBasePort + $i * $clientportstride

# bash ./clientrun $tempcport
# done

# # starting storageserver
# for ((i=0; i<$numclient;i++)) do
# tempnsport = $StorageServerNSBaseport + $i * $ssportstride
# tempsscport =  $StorageServerClientBaseport + $i * $StorageServerClientBaseport

# bash ./ssrun $tempnsport $tempsscport
# done