

FROM ubuntu:21.04 as build

ENV LANG C.UTF-8
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        libc6-dev \
        binutils \
        gcc-10 \
        g++-10 \
        python \
        ccache \
        cmake \
        make \
        git \
        lld \
        libsdl2-dev \
        zlib1g-dev \
        libbz2-dev \
        libpng-dev \
        libgles2-mesa-dev && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 10 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 10

RUN git clone https://github.com/Perlmint/glew-cmake.git && \
    cmake glew-cmake && \
    make -j$(nproc) && \
    make install

RUN mkdir /soh
WORKDIR /soh
