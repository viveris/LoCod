FROM vivado-2020.1:0.2

LABEL maintainer="Hugo KIEFFER <hugo.kieffer@viveris.fr>"
LABEL description="PandA docker"

# Version of PandA tools to install
ARG PANDA_VERSION=v0.9.6.1

# Install PandA depenencies
RUN apt update && \
    apt install -y --no-install-recommends \
    autoconf autoconf-archive automake libtool g++ gcc-4.8 g++-4.8 gcc-4.9 g++-4.9 gcc-5 g++-5 gcc-4.8-plugin-dev gcc-4.9-plugin-dev gcc-5-plugin-dev  gcc-4.8-multilib gcc-4.9-multilib gcc-5-multilib g++-4.8-multilib g++-4.9-multilib g++-5-multilib gfortran-4.8 gfortran-4.8-multilib gfortran-4.9 gfortran-4.9-multilib gfortran-5 gfortran-5-multilib clang-4.0 libclang-4.0-dev libbdd-dev libboost-all-dev libmpc-dev libmpfr-dev libxml2-dev liblzma-dev libmpfi-dev zlib1g-dev libicu-dev bison doxygen flex graphviz iverilog verilator make libsuitesparse-dev libglpk-dev vim

# PandA build and install
RUN git clone -b ${PANDA_VERSION} https://github.com/ferrandi/PandA-bambu /home/panda/bambu
RUN cd /home/panda/bambu && \
    make -f Makefile.init && \
    mkdir obj && \
    cd obj && \
    ../configure --prefix=/opt/panda --enable-flopoco --enable-verilator --enable-opt --enable-release --enable-xilinx && \
    make && \
    make install

COPY sdk.sh /tmp/sdk.sh
RUN /tmp/sdk.sh && rm /tmp/sdk.sh

USER locod
WORKDIR /home/locod

RUN echo "export PATH=$PATH:/opt/panda/bin" >> /home/locod/.bashrc
