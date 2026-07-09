#                     __            ___             _ 
#                    / /    ___    / __\  ___    __| |
#                   / /    / _ \  / /    / _ \  / _` |
#                  / /___ | (_) |/ /___ | (_) || (_| |
#                  \____/  \___/ \____/  \___/  \__,_|
#
#            ***********************************************
#                             LoCod Project
#                 URL: https://github.com/viveris/LoCod
#            ***********************************************
#                 Copyright © 2024 Viveris Technologies
#
#                  Developed in partnership with CNES
#              (DTN/TVO/ET: On-Board Data Handling Office)
#
#  This file is part of the LoCod framework.
#
#  The LoCod framework is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

#!/bin/bash
set -e


# ========== Variables ==========
# Execution dir
BASE_DIR=$(pwd)
# Script dir
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

INSTALL_ULTRA96=0
INSTALL_ENCLUSTRA=0
INSTALL_NGULTRA=0


# ========== Arguments parsing ==========
function help() {
echo "Usage: install.sh
    [--enable-ultra96]      < install requierments for Xilinx Ultra96 board >
    [--enable-enclustra]    < install requierments for Xilinx Enclustra XU7 board >
    [--enable-ngultra]      < install requierments for NanoXplore NG-Ultra board >
    [--help]                < display this help >"
    exit 1
}

while [[ $# -gt 0 ]]; do
    case $1 in
        --enable-ultra96)
            INSTALL_ULTRA96=1
            shift # past argument
            ;;
        --enable-enclustra)
            INSTALL_ENCLUSTRA=1
            shift # past argument
            ;;
        --enable-ngultra)
            INSTALL_NGULTRA=1
            shift # past argument
            ;;
        --help)
            help
            shift # past argument
            ;;
        -*|--*)
            echo "Unknown option $1"
            exit 1
            ;;
    esac
done

# If no target is specified, exit
if [[ $INSTALL_ULTRA96 == 0 && $INSTALL_ENCLUSTRA == 0 && $INSTALL_NGULTRA == 0 ]]; then
    echo "You need to specify at list one target board"
    exit 1
fi


# ========== User inputs ==========
function get_user_inputs() {
    if [[ $INSTALL_ULTRA96 == 1 || $INSTALL_ENCLUSTRA == 1 ]]; then
        read -p "Enter path of the Vivado 2022.2 environment script : " -e VIVADO_ENVIRONMENT_SCRIPT
    fi
    if [[ $INSTALL_ULTRA96 == 1 ]]; then
        read -p "Enter Ultra96 SDK 'sdk-ultra96.sh' installation script : " -e SDK_ULTRA96_INSTALL_SCRIPT
    fi
    if [[ $INSTALL_ENCLUSTRA == 1 ]]; then
        read -p "Enter Enclustra XU7 SDK 'sdk-enclustra.sh' installation script : " -e SDK_ENCLUSTRA_INSTALL_SCRIPT
    fi
    if [[ $INSTALL_NGULTRA == 1 ]]; then
        read -p "Enter NanoXplore Gitlab username : " -e NX_USERNAME
        read -p "Enter NanoXplore Gitlab personal acces token : " -e NX_PERSONAL_ACCESS_TOKEN
        read -p "Enter NanoXplore 'license.lic' license file : " -e NX_LICENSE_FILE
        read -p "Enter Hostame used with the NanoXplore license : " -e NX_LICENSE_HOSTNAME
        read -p "Enter Mac Address used with the NanoXplore license : " -e NX_LICENSE_MAC_ADDR
        read -p "Enter NX Design Suite 23.5.1.2 'nxdesignsuite-23.5.1.2.tar.gz' installation archive : " -e NXDESIGNSUITE_23_5_1_2_TAR_ARCHIVE
        read -p "Enter NxBase2 2.5.3 'NxBase2-2.5.3.tar.gz' installation archive : " -e NXBASE2_2_5_3_TAR_ARCHIVE
        read -p "Enter NXLMD 2.2 'NXLMD-2.2-linux.tar.gz' installation archive : " -e NXLMD_2_2_TAR_ARCHIVE
    fi
}

function print_user_inputs() {
    echo "Installation parameters :"
    if [[ $INSTALL_ULTRA96 == 1 || $INSTALL_ENCLUSTRA == 1 ]]; then
        echo "VIVADO_ENVIRONMENT_SCRIPT=${VIVADO_ENVIRONMENT_SCRIPT}"
    fi
    if [[ $INSTALL_ULTRA96 == 1 ]]; then
        echo "SDK_ULTRA96_INSTALL_SCRIPT=${SDK_ULTRA96_INSTALL_SCRIPT}"
    fi
    if [[ $INSTALL_ENCLUSTRA == 1 ]]; then
        echo "SDK_ENCLUSTRA_INSTALL_SCRIPT=${SDK_ENCLUSTRA_INSTALL_SCRIPT}"
    fi
    if [[ $INSTALL_NGULTRA == 1 ]]; then
        echo "NX_USERNAME=${NX_USERNAME}"
        echo "NX_PERSONAL_ACCESS_TOKEN=${NX_PERSONAL_ACCESS_TOKEN}"
        echo "NX_LICENSE_FILE=${NX_LICENSE_FILE}"
        echo "NX_LICENSE_HOSTNAME=${NX_LICENSE_HOSTNAME}"
        echo "NX_LICENSE_MAC_ADDR=${NX_LICENSE_MAC_ADDR}"
        echo "NXDESIGNSUITE_23_5_1_2_TAR_ARCHIVE=${NXDESIGNSUITE_23_5_1_2_TAR_ARCHIVE}"
        echo "NXBASE2_2_5_3_TAR_ARCHIVE=${NXBASE2_2_5_3_TAR_ARCHIVE}"
        echo "NXLMD_2_2_TAR_ARCHIVE=${NXLMD_2_2_TAR_ARCHIVE}"
    fi
}

USER_INPUTS_OK="N"
get_user_inputs
echo ""
print_user_inputs
echo ""
echo "Is it correct? (Y/N)"
read USER_INPUTS_OK
echo ""
while [[ $USER_INPUTS_OK != [yY] && $USER_INPUTS_OK != [yY][eE][sS] ]]; do
    get_user_inputs
    echo ""
    print_user_inputs
    echo ""
    echo "Is it correct? (Y/N)"
    read USER_INPUTS_OK
    echo ""
done


# ========== Docker images names ==========
function get_docker_images_names() {
    read -p "PandA-Bambu docker image : " -i "panda-bambu:latest" -e PANDA_DOCKER_IMG
    if [[ $INSTALL_ULTRA96 == 1 ]]; then
        read -p "Ultra96 SDK docker image : " -i "sdk_ultra96:latest" -e SDK_ULTRA96_DOCKER_IMG
    fi
    if [[ $INSTALL_ENCLUSTRA == 1 ]]; then
        read -p "Enclustra XU7 SDK docker image : " -i "sdk_enclustra:latest" -e SDK_ENCLUSTRA_DOCKER_IMG
    fi
    if [[ $INSTALL_NGULTRA == 1 ]]; then
        read -p "NX Design Suite docker image : " -i "nx_design_suite:latest" -e NX_DESIGN_SUITE_DOCKER_IMG
        read -p "NG-Ultra SDK docker image : " -i "sdk_ngultra:latest" -e SDK_NGULTRA_DOCKER_IMG
    fi
}

function print_docker_images_names() {
    echo "Docker images names :"
    echo "PANDA_DOCKER_IMG=${PANDA_DOCKER_IMG}"
    if [[ $INSTALL_ULTRA96 == 1 ]]; then
        echo "SDK_ULTRA96_DOCKER_IMG=${SDK_ULTRA96_DOCKER_IMG}"
    fi
    if [[ $INSTALL_ENCLUSTRA == 1 ]]; then
        echo "SDK_ENCLUSTRA_DOCKER_IMG=${SDK_ENCLUSTRA_DOCKER_IMG}"
    fi
    if [[ $INSTALL_NGULTRA == 1 ]]; then
        echo "NX_DESIGN_SUITE_DOCKER_IMG=${NX_DESIGN_SUITE_DOCKER_IMG}"
        echo "SDK_NGULTRA_DOCKER_IMG=${SDK_NGULTRA_DOCKER_IMG}"
    fi
}

DOCKER_IMAGES_NAMES_OK="N"
get_docker_images_names
echo ""
print_docker_images_names
echo ""
echo "Is it correct? (Y/N)"
read DOCKER_IMAGES_NAMES_OK
echo ""
while [[ $DOCKER_IMAGES_NAMES_OK != [yY] && $DOCKER_IMAGES_NAMES_OK != [yY][eE][sS] ]]; do
    get_docker_images_names
    echo ""
    print_docker_images_names
    echo ""
    echo "Is it correct? (Y/N)"
    read DOCKER_IMAGES_NAMES_OK
    echo ""
done


# ========== Build Docker images ==========
# Create temporary directory to store installation files
rm -rf ${BASE_DIR}/tmp
mkdir ${BASE_DIR}/tmp

if [[ $INSTALL_ULTRA96 == 1 ]]; then
    cp ${SDK_ULTRA96_INSTALL_SCRIPT} ${BASE_DIR}/tmp
fi
if [[ $INSTALL_ENCLUSTRA == 1 ]]; then
    cp ${SDK_ENCLUSTRA_INSTALL_SCRIPT} ${BASE_DIR}/tmp
fi
if [[ $INSTALL_NGULTRA == 1 ]]; then
    cp ${NX_LICENSE_FILE} ${BASE_DIR}/tmp
    cp ${NXDESIGNSUITE_23_5_1_2_TAR_ARCHIVE} ${BASE_DIR}/tmp
    cp ${NXBASE2_2_5_3_TAR_ARCHIVE} ${BASE_DIR}/tmp
    cp ${NXLMD_2_2_TAR_ARCHIVE} ${BASE_DIR}/tmp
fi

# Build PandA-Bambu image
docker build -t ${PANDA_DOCKER_IMG} -f ${SCRIPT_DIR}/../submodules/docker/LoCod-docker-PandA/panda/Dockerfile ${BASE_DIR}/tmp

# Build Ultra96 SDK image
if [[ $INSTALL_ULTRA96 == 1 ]]; then
    docker build -t ${SDK_ULTRA96_DOCKER_IMG} -f ${SCRIPT_DIR}/../submodules/docker/LoCod-docker-sdk-ultra96/Dockerfile ${BASE_DIR}/tmp
fi

# Build Enclustra SDK image
if [[ $INSTALL_ENCLUSTRA == 1 ]]; then
    docker build -t ${SDK_ENCLUSTRA_DOCKER_IMG} -f ${SCRIPT_DIR}/../submodules/docker/LoCod-docker-sdk-enclustra/Dockerfile ${BASE_DIR}/tmp
fi

# Build NX Design Suite and NG-Ultra SDK image
if [[ $INSTALL_NGULTRA == 1 ]]; then
    docker build -t ${NX_DESIGN_SUITE_DOCKER_IMG} --build-arg NX_USERNAME=${NX_USERNAME} --build-arg NX_PERSONAL_ACCESS_TOKEN=${NX_PERSONAL_ACCESS_TOKEN} -f ${SCRIPT_DIR}/../submodules/docker/LoCod-docker-nanoxplore/Dockerfile ${BASE_DIR}/tmp
    docker build -t ${SDK_NGULTRA_DOCKER_IMG} --build-arg NX_USERNAME=${NX_USERNAME} --build-arg NX_PERSONAL_ACCESS_TOKEN=${NX_PERSONAL_ACCESS_TOKEN} -f ${SCRIPT_DIR}/../submodules/docker/LoCod-docker-sdk-ngultra/Dockerfile ${BASE_DIR}/tmp
fi

# Remove temporary directory
rm -rf ${BASE_DIR}/tmp


# ========== Creates LoCod environment file ==========
rm -rf ${SCRIPT_DIR}/../locod_env.sh
touch ${SCRIPT_DIR}/../locod_env.sh

echo "#!/bin/bash
export PANDA_DOCKER_IMG=${PANDA_DOCKER_IMG}" >> ${SCRIPT_DIR}/../locod_env.sh

if [[ $INSTALL_ULTRA96 == 1 || $INSTALL_ENCLUSTRA == 1 ]]; then
    echo "source ${VIVADO_ENVIRONMENT_SCRIPT}" >> ${SCRIPT_DIR}/../locod_env.sh
fi

if [[ $INSTALL_ULTRA96 == 1 ]]; then
    echo "export SDK_ULTRA96_DOCKER_IMG=${SDK_ULTRA96_DOCKER_IMG}" >> ${SCRIPT_DIR}/../locod_env.sh
fi

if [[ $INSTALL_ENCLUSTRA == 1 ]]; then
    echo "export SDK_ENCLUSTRA_DOCKER_IMG=${SDK_ENCLUSTRA_DOCKER_IMG}" >> ${SCRIPT_DIR}/../locod_env.sh
fi

if [[ $INSTALL_NGULTRA == 1 ]]; then
    echo "export SDK_NGULTRA_DOCKER_IMG=${SDK_NGULTRA_DOCKER_IMG}" >> ${SCRIPT_DIR}/../locod_env.sh
    echo "export NX_DESIGN_SUITE_DOCKER_IMG=${NX_DESIGN_SUITE_DOCKER_IMG}" >> ${SCRIPT_DIR}/../locod_env.sh
    echo "export NX_LICENSE_HOSTNAME=${NX_LICENSE_HOSTNAME}" >> ${SCRIPT_DIR}/../locod_env.sh
    echo "export NX_LICENSE_MAC_ADDR=${NX_LICENSE_MAC_ADDR}" >> ${SCRIPT_DIR}/../locod_env.sh
fi


# ========== End ==========
exit 0