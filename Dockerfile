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
		make \
		cmake \
		curl \
		git \
		lld \
		libglew-dev \
		libsdl2-dev \
		zlib1g-dev \
		libbz2-dev \
		libpng-dev \
		libgles2-mesa-dev && \    
	update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCCVER} 10 && \
	update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-${GCCVER} 10 && \
	gcc --version && \
	g++ --version
	
RUN apt-get clean autoclean && apt-get autoremove --yes && rm -rf /var/lib/apt /var/lib/cache /var/lib/log

RUN git clone https://github.com/Perlmint/glew-cmake.git && \
	cmake glew-cmake && \
	make -j$(nproc) && \
	make install

ENV SDL2VER=2.0.22
RUN curl -sLO https://libsdl.org/release/SDL2-${SDL2VER}.tar.gz && \
	tar -xzf SDL2-${SDL2VER}.tar.gz && \
	cd SDL2-${SDL2VER} && \
	./configure --prefix=/usr --libdir=/lib/x86_64-linux-gnu/ && \
	make -j$(nproc) && make install && \
	rm ../SDL2-${SDL2VER}.tar.gz

RUN mkdir /soh
WORKDIR /soh
