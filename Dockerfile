FROM ubuntu:22.10 as build

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
	ln -s /usr/bin/g++-10 /usr/bin/g++ && \
	update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 10 && \
	gcc --version && \
	g++ --version
	
RUN apt-get clean autoclean && apt-get autoremove --yes && rm -rf /var/lib/apt /var/lib/cache /var/lib/log

RUN mkdir /soh
WORKDIR /soh
