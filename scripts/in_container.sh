DIR="$( cd "$(dirname "$0")" && pwd )"

if test -z "$IN_CONTAINER"; then
  UID=$(id -u)
  SCRIPT=$(basename "$0")
  exec docker run --rm --interactive --tty --user $(id -u) \
    --volume "$DIR:/mnt" build_c-to-json "/mnt/$SCRIPT"
fi

BUILD_DIR="/mnt/build"
