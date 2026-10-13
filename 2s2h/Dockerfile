FROM ubuntu:latest

ARG MY_XAUTH_COOKIE

ENV LANG C.UTF-8
ARG DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture i386
RUN apt-get update

RUN apt-get install -y \
    binutils:i386 \
    gcc-12:i386 \
    g++-12:i386 && \
    ln -sf /usr/bin/python3.10 /usr/bin/python3 && \
    ln -s /usr/bin/gcc-12 /usr/bin/gcc && \
    ln -s /usr/bin/gcc-12 /usr/bin/cc && \
    ln -s /usr/bin/g++-12 /usr/bin/g++ && \
    ln -s /usr/bin/g++-12 /usr/bin/c++

RUN apt-get install -y \
    libsdl2-dev:i386 \
    zlib1g-dev:i386 \
    libbz2-dev:i386 \
    libpng-dev:i386 \
    libgles2-mesa-dev

RUN apt-get install -y \
    make \
    cmake \
    git \
    gdb \
    lld \
    python3.10 \
    ninja-build \
    lsb-release \
    clang-format

RUN git clone https://github.com/Perlmint/glew-cmake.git && \
    cmake -B glew-cmake/builddir -S glew-cmake -GNinja && \
    cmake --build glew-cmake/builddir && \
    cmake --install glew-cmake/builddir --prefix /usr && \
    mv /usr/lib/libglew-shared.so /usr/lib/libglew.so

RUN git clone https://github.com/libsdl-org/SDL_net.git -b SDL2 && \
    cmake -B SDL_net/build -S SDL_net -DCMAKE_BUILD_TYPE=Release && \
    cmake --build SDL_net/build --config Release --parallel && \
    cmake --install SDL_net/build --config Release

RUN git clone https://github.com/libsdl-org/SDL.git -b SDL2 && \
    cmake -B SDL/build -S SDL -DCMAKE_BUILD_TYPE=Release && \
    cmake --build SDL/build --config Release --parallel && \
    cmake --install SDL/build --config Release

RUN touch /root/.Xauthority && \
    xauth add $MY_XAUTH_COOKIE


RUN mkdir /2ship
WORKDIR /2ship
