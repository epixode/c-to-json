DIR="$( cd "$(dirname "$0")" && pwd )"

if test -z "$IN_CONTAINER"; then
  SCRIPT=$(basename "$0")
  exec docker run --rm --interactive --tty --env RUN_UID=$(id -u) \
    --volume "$DIR:/mnt" build_c-to-json "/mnt/$SCRIPT"
elif test 0 -eq $(id -u); then
  echo id $RUN_UID script $0
  echo "user::$RUN_UID:0:,,,:/:/bin/bash" >>/etc/passwd
  exec su -c "$0" user
fi

BUILD_DIR="/mnt/build"
