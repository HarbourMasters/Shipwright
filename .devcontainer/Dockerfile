FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-22.04

RUN apt-get update && apt-get upgrade -y \
    && rm -rf /var/lib/apt/lists/*

# download and install cmake from source
# this ensures we use the same version as in the github runner image
RUN apt-get purge -y cmake || true
ARG CMAKE_VERSION=3.31.11
# download
WORKDIR /tmp
RUN curl -fsSL \
    https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}.tar.gz \
    -o cmake.tar.gz \
 && tar -xzf cmake.tar.gz \
 && rm cmake.tar.gz
 # build and install
WORKDIR /tmp/cmake-${CMAKE_VERSION}
RUN ./bootstrap \
      --prefix=/usr/local \
      --parallel=$(nproc) \
 && make -j$(nproc) \
 && make install
# clean up
WORKDIR /
RUN rm -rf /tmp/cmake-${CMAKE_VERSION}

# download and install tinyxml2 from source
# this ensures we have the cmake files needed for find_package
RUN apt-get purge -y libtinyxml2-dev || true
ARG TINYXML2_VERSION=11.0.0
# download
WORKDIR /tmp
RUN curl -fsSL \
    https://github.com/leethomason/tinyxml2/archive/refs/tags/${TINYXML2_VERSION}.tar.gz \
    -o tinyxml2.tar.gz \
 && tar -xzf tinyxml2.tar.gz \
 && rm tinyxml2.tar.gz
 # build and install
WORKDIR /tmp/tinyxml2-${TINYXML2_VERSION}
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local \
 && cmake --build build --parallel $(nproc) \
 && cmake --install build
# clean up
WORKDIR /
RUN rm -rf /tmp/tinyxml2-${TINYXML2_VERSION}

# install apt dependencies
RUN apt-get update && apt-get install -y \
      libusb-dev libusb-1.0-0-dev libsdl2-dev libsdl2-net-dev libpng-dev \
      libglew-dev nlohmann-json3-dev libspdlog-dev ninja-build libogg-dev \
      libopus-dev opus-tools libopusfile-dev libvorbis-dev libespeak-ng-dev \
      lsb-release git clang clang-format-14 zipcmp zipmerge ziptool \
      libopengl-dev libbz2-dev libzip-dev \
 && rm -rf /var/lib/apt/lists/*

# Install latest SDL2
RUN wget https://www.libsdl.org/release/SDL2-2.26.1.tar.gz && \
   tar -xzf SDL2-2.26.1.tar.gz && \
    cd SDL2-2.26.1 && \
    ./configure && \
    make -j$(nproc) && \
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
    make -j$(nproc) && \
    make install && \
    cd .. && \
    rm -rf SDL2_net-2.2.0 && \
    rm SDL2_net-2.2.0.tar.gz && \
    cp -av /usr/local/lib/libSDL* /lib/x86_64-linux-gnu/
