gcc -g3 -O3 -Wall -Werror -Wextra -Wshadow -Wcast-align -std=c11 -o rt\
    rt.c \
    -I. \
    -lm

./rt

nomacs rt.bmp
