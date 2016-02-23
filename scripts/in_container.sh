SCRIPT=$(basename "$0")
DIR="$( cd "$(dirname "$0")" && pwd )"
test "$IN_CONTAINER" || exec docker run --rm --interactive --tty \
  --volume "$DIR:/mnt" build_c-to-json "/mnt/$SCRIPT"

BUILD_DIR="/mnt/build"
