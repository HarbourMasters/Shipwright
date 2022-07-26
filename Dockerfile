FROM ubuntu:20.04 as build

ENV LANG C.UTF-8
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
	apt-get upgrade -y && \
	apt-get install -y \
		binutils \
		gcc-10 \
		g++-10 \
		patchelf \
		p7zip-full \
		python3.9 \
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
	ln -s /usr/bin/g++-10 /usr/bin/g++ && \
	update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 10 && \
	gcc --version && \
	g++ --version
	
RUN apt-get clean autoclean && apt-get autoremove --yes && rm -rf /var/lib/apt /var/lib/cache /var/lib/log

#RUN curl https://github.com/Kitware/CMake/releases/download/v3.23.2/cmake-3.23.2-Linux-x86_64.sh \
#	-s -o /tmp/cmake-install.sh \
#	&& chmod u+x /tmp/cmake-install.sh \
#	&& mkdir /usr/bin/cmake \
#	&& /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
#	&& rm /tmp/cmake-install.sh
#ENV PATH="/usr/bin/cmake/bin:${PATH}"
	
RUN git clone https://github.com/Perlmint/glew-cmake.git && \
	cmake glew-cmake && \
	make -j$(nproc) && \
	make install
    
ENV SDL2VER=2.0.22
RUN curl -sLO https://libsdl.org/release/SDL2-${SDL2VER}.tar.gz && \
	tar -xzf SDL2-${SDL2VER}.tar.gz && \
	cd SDL2-${SDL2VER} && \
	./configure --prefix=/usr --libdir=/lib/x86_64-linux-gnu/ && \
	make && make install && \
	rm ../SDL2-${SDL2VER}.tar.gz

RUN mkdir /soh
WORKDIR /soh
