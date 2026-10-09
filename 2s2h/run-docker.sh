cookie=$(xauth list | head -1 | sed "s/unix: /unix:0 /")
sudo docker build --build-arg MY_XAUTH_COOKIE="$cookie" . -t soh
sudo docker run --rm -it -e DISPLAY --net=host --device=/dev/dri --device /dev/snd -v $(pwd):/2ship soh /bin/bash
