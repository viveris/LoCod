FROM ubuntu:16.04
MAINTAINER Hugo KIEFFER <hugo.kieffer@viveris.fr>
LABEL description="Vivado docker used as a base system for locod image."

# Install Vivado dependencies
RUN apt-get update && apt-get install -y \
    wget \
    build-essential \
    libglib2.0-0 \
    libsm6 \
    libxi6 \
    libxrender1 \
    libxrandr2 \
    libfreetype6 \
    libfontconfig \
    git

# Copy and run the vivado install
ARG VIVADO_TAR_FILE=Xilinx_Unified_2020.1_0602_1208
ARG VIVADO_VERSION=2020.1
COPY install_config.txt /
COPY ${VIVADO_TAR_FILE}.tar.gz /
RUN echo "Extracting Vivado tar file" && \
    tar xzf ${VIVADO_TAR_FILE}.tar.gz && \
    rm ${VIVADO_TAR_FILE}.tar.gz && \
    echo "Installing ${VIVADO_TAR_FILE}" && \
    /${VIVADO_TAR_FILE}/xsetup --agree 3rdPartyEULA,WebTalkTerms,XilinxEULA --batch Install --config install_config.txt && \
    rm -rf ${VIVADO_TAR_FILE}*

# Add a locod user
RUN adduser --disabled-password --gecos '' locod
# Add vivado tools to path
RUN echo "source /opt/Xilinx/Vivado/${VIVADO_VERSION}/settings64.sh" >> /home/locod/.bashrc

# Copy in the license file
RUN mkdir /home/locod/.Xilinx
COPY Xilinx.lic /home/locod/.Xilinx/
RUN chown -R locod: /home/locod/.Xilinx/
