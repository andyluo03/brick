set -e

# check if potion/dev image exists
# if not, build it
if [[ "$(docker images -q potion/dev 2> /dev/null)" == "" ]]; then
    echo "Building potion/dev image..."
    docker build -t potion/dev -f dev.Dockerfile .
fi

# permanent or temporary
read -p "Would you like your container to be permanent? [y/n] " -n 1 -r permanent
echo

# confirm volume location as current directory - display current directory
echo "Current directory: $(pwd)"
read -p "We will use this directory as a volume? Do you want to continue? [y/n] " -n 1 -r volume
echo

# abort if volumn is != y or Y
if [[ ! $volume =~ ^[Yy]$ ]]
then
    echo "Aborted"
    exit 1
fi

if [[ $permanent =~ ^[Yy]$ ]]
then
    echo "Creating permanent container..."
    destroy=""
else
    echo "Creating temporary container..."
    destroy="--rm"
fi

docker run -it \
    $destroy \
    --name=potion-dev \
    -v $(pwd):/potion \
    -w /potion \
    --user root \
    potion/dev




