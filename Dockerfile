FROM ubuntu:20.04 as build

ENV LANG C.UTF-8
ARG DEBIAN_FRONTEND=noninteractive

ENV GCCVER=10
RUN apt-get update && \
	apt-get upgrade -y && \
	apt-get install -y \
		binutils \
		gcc-${GCCVER} \
		g++-${GCCVER} \
		patchelf \
		p7zip-full \
		python3 \
		cmake \
		make \
		curl \
		git \
		lld \
		libsdl2-dev \
		zlib1g-dev \
		libbz2-dev \
		libpng-dev \
		libgles2-mesa-dev \
		wget \
		gpg \
		imagemagick \
		ninja-build && \
	apt-get install -y software-properties-common && \ 
	wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null && \
	apt-add-repository "deb https://apt.kitware.com/ubuntu/ focal main" && \
	apt-get update && \
	apt-get upgrade -y && \
	update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCCVER} 10 && \
	update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-${GCCVER} 10 
	
RUN git clone https://github.com/Perlmint/glew-cmake.git && \
	cmake glew-cmake && \
	make -j$(nproc) && \
	make install

ENV SDL2VER=2.0.22
RUN curl -sLO https://libsdl.org/release/SDL2-${SDL2VER}.tar.gz && \
	tar -xzf SDL2-${SDL2VER}.tar.gz && \
	cd SDL2-${SDL2VER} && \
	./configure --build=x86_64-linux-gnu && \
	make -j$(nproc) && make install && \
	rm ../SDL2-${SDL2VER}.tar.gz && \
	cp -av /usr/local/lib/libSDL* /lib/x86_64-linux-gnu/

ENV SDL2NETVER=2.2.0
RUN curl -sLO https://www.libsdl.org/projects/SDL_net/release/SDL2_net-${SDL2NETVER}.tar.gz && \
	tar -xzf SDL2_net-${SDL2NETVER}.tar.gz && \
	cd SDL2_net-${SDL2NETVER} && \
	./configure --build=x86_64-linux-gnu && \
	make -j$(nproc) && make install && \
	rm ../SDL2_net-${SDL2NETVER}.tar.gz && \
	cp -av /usr/local/lib/libSDL* /lib/x86_64-linux-gnu/

RUN \
	ln -sf /proc/self/mounts /etc/mtab && \
	mkdir -p /usr/local/share/keyring/ && \
	wget -O /usr/local/share/keyring/devkitpro-pub.gpg https://apt.devkitpro.org/devkitpro-pub.gpg && \
	echo "deb [signed-by=/usr/local/share/keyring/devkitpro-pub.gpg] https://apt.devkitpro.org stable main" > /etc/apt/sources.list.d/devkitpro.list && \
	apt-get update -y && \
	apt-get install -y devkitpro-pacman && \
	yes | dkp-pacman -Syu switch-dev switch-portlibs wiiu-dev wiiu-portlibs --noconfirm

ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITARM=/opt/devkitpro/devkitARM
ENV DEVKITPPC=/opt/devkitpro/devkitPPC
ENV PATH=$PATH:/opt/devkitpro/portlibs/switch/bin/:$DEVKITPPC/bin
ENV WUT_ROOT=$DEVKITPRO/wut

RUN mkdir /soh
WORKDIR /soh
