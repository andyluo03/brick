# check if potion-dev-test exists
if [ "$(docker ps -a | grep potion-dev)" ]; then 
    docker start potion-dev
    docker exec -it potion-dev tmux
else
    echo "potion-dev does not exist. Run docker_create.sh first"
fi

