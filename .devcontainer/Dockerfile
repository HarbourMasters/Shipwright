FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-22.04

RUN apt-get update && apt-get install -y libsdl2-dev libsdl2-net-dev libpng-dev libglew-dev ninja-build

# Install latest SDL2
RUN wget https://www.libsdl.org/release/SDL2-2.26.1.tar.gz && \
   tar -xzf SDL2-2.26.1.tar.gz && \
    cd SDL2-2.26.1 && \
    ./configure && \
    make && \
    make install && \
    cd .. && \
    rm -rf SDL2-2.26.1 && \
    rm SDL2-2.26.1.tar.gz && \
    cp -av /usr/local/lib/libSDL* /lib/x86_64-linux-gnu/

# Install latest SDL2_net
RUN wget https://www.libsdl.org/projects/SDL_net/release/SDL2_net-2.2.0.tar.gz && \
   tar -xzf SDL2_net-2.2.0.tar.gz && \
    cd SDL2_net-2.2.0 && \
    ./configure && \
    make && \
    make install && \
    cd .. && \
    rm -rf SDL2_net-2.2.0 && \
    rm SDL2_net-2.2.0.tar.gz && \
    cp -av /usr/local/lib/libSDL* /lib/x86_64-linux-gnu/
