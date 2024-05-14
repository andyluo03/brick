# bash script for building the project
# Usage: ./build.sh <build_type>
# build_type: debug, release (default: debug)

# Set default build type
BUILD_TYPE="Debug"

# Check if build type is provided
if [ $# -eq 1 ]; then
    # convert to lowercase
    BUILD_TYPE=$(echo $1 | tr '[:upper:]' '[:lower:]')
    if [ $BUILD_TYPE != "debug" ] && [ $BUILD_TYPE != "release" ]; then
        echo "Invalid build type. Use 'debug' or 'release'"
        exit 1
    fi

    # set build type to "Debug" or "Release"
    if [ $BUILD_TYPE == "release" ]; then
        BUILD_TYPE="Release"
    fi
fi

echo "Build type: $BUILD_TYPE"

# Create build directory if not exists
if [ ! -d "build" ]; then
    mkdir build
fi

# execute build command
cd build && cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE .. && make

