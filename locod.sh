#!/bin/bash
set -e

#**********************************************/
#***************** Variables ******************/
#**********************************************/
# Execution dir
BASE_DIR=$(pwd)

# Locod folders
LOCOD_CPU_DIR=locod-core
LOCOD_FPGA_DIR=submodules/locod-fpga

# Docker files
PANDA_DOCKER_IMG=panda-bambu:9.8.0
ULTRA96_SDK_DOCKER_IMG=sdk-ultra96:1.0
ENCLUSTRA_SDK_DOCKER_IMG=sdk-enclustra:1.0
NG_ULTRA_SDK_DOCKER_IMG=
NX_DOCKER_IMG=nx-tools:1.0
VIVADO_DOCKER_IMG=

#Impulse license
NX_HOSTNAME=localhost.localdomain
NX_MAC_ADDR=86:8a:dd:8d:51:a8

# Panda-Bambu compilation parameters
BAMBU_OPT="--writer=V --generate-interface=MINIMAL --memory-allocation-policy=LSS --channels-type=MEM_ACC_11 --memory-ctrl-type=D21 --data-bus-bitsize=32 --addr-bus-bitsize=32 -DLOCOD_FPGA"

# Varaibles to select wich part we want to build (exectable or bitstream)
CPU=1
FPGA=1

#**********************************************/
#***************** Functions ******************/
#**********************************************/
function get_max_acc_number()
{
	grep -oP '(?<=init_accel_system\().*?(?=\))' $1
}

function get_fpga_func()
{
	grep -oP '(?<=FPGA\().*?(?=,)' $1
}

function get_used_acc_number()
{
	grep -oP '(?<=FPGA\().*?(?=,)' $1 | wc -l
}

function get_fct_acc_number()
{
	grep -oP "(?<=FPGA\($2).*" $1 | grep -oP ',\s*[0-9]+\s*\)' | grep -oP '[0-9]+'
}


#******************************************************/
#***************** Arguments parsing ******************/
#******************************************************/
help()
{
	echo "Usage: locod	[ -t | --target ]
		[ -f | --file ]
		[ -c | --component ]
		[ -h | --help  ]"
	exit 2
}

POSITIONAL_ARGS=()

while [[ $# -gt 0 ]]; do
	case $1 in
		-t|--target)
			if [[ $2 != ultra96 && $2 != enclustra && $2 != ng-ultra ]]; then
				echo "Unknown target $2"
				exit 1
			fi
			TARGET="$2"
			shift # past argument
			shift # past value
			;;
		-f|--file)
			FILE="$2"
			shift # past argument
			shift # past value
			;;
		--no-soft)
			CPU=0
			shift # past argument
			;;
		--no-hard)
			FPGA=0
			shift # past argument
			;;
		-h|--help)
			help
			shift # past argument
			;;
		-*|--*)
			echo "Unknown option $1"
			exit 1
			;;
		*)
			POSITIONAL_ARGS+=("$1") # save positional arg
			shift # past argument
			;;
	esac
done

set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters

if [[ $TARGET == "" ]]; then
	echo "No target specified"
	help
	exit 1
fi

if [[ $FILE == "" ]]; then
	echo "No file specified"
	help
	exit 1
fi


#**********************************************************/
#***************** Checking requierments ******************/
#**********************************************************/
echo "Checking dependecies ..."
#Docker Panda Bambu
if docker run --rm -t --workdir /tmp -u $(id -u):$(id -g) ${PANDA_DOCKER_IMG} bambu --version; then
	echo "- Panda docker found"
else
	echo "- Panda docker not found"
	exit 1
fi

#Docker Petalinux SDK Ultra96
if [ $(docker run --rm -t --workdir /tmp -u $(id -u):$(id -g) ${ULTRA96_SDK_DOCKER_IMG} bash -c 'source /opt/petalinux-sdk/environment-setup-cortexa72-cortexa53-xilinx-linux;echo $CC' | tr -d '[:space:]') != "" ]; then
	echo "- Ultra96 SDK docker found"
else
	echo "- Ultra96 SDK docker not found"
	exit 1
fi

#Docker Petalinux SDK Enclustra
if [ $(docker run --rm -t --workdir /tmp -u $(id -u):$(id -g) ${ENCLUSTRA_SDK_DOCKER_IMG} bash -c 'source /opt/petalinux-sdk/environment-setup-cortexa72-cortexa53-xilinx-linux;echo $CC' | tr -d '[:space:]') != "" ]; then
	echo "- Enclustra SDK docker found"
else
	echo "- Enclustra SDK docker not found"
	exit 1
fi

#Docker NG-Ultra SDK
#TODO

#Vivado
if vivado -version &> /dev/null; then
	echo "- Vivado found"
else
	echo "- Vivado not found"
	exit 1
fi

#Docker Impulse
if docker run --rm -t --workdir /tmp -u $(id -u):$(id -g) --hostname ${NX_HOSTNAME} --mac-address ${NX_MAC_ADDR} ${NX_DOCKER_IMG} bash -c 'lmgrd;sleep 1;nxpython --version'; then
	echo "- NX docker found"
else
	echo "- NX docker not found"
	exit 1
fi


#*******************************************************************/
#***************** Initializing files and folders ******************/
#*******************************************************************/
rm -rf $LOCOD_FPGA_DIR/src/generated_files/*
mkdir -p temp
rm -rf temp/*
mkdir -p locod-output
rm -rf locod-output/*


#**************************************************************/
#***************** Step 1 : Compiling C code ******************/
#**************************************************************/
if [[ $CPU == 1 ]]; then

echo -n "Compiling C code ... "

case $TARGET in
	ultra96)
		cp $FILE $LOCOD_CPU_DIR/src/main.c
		docker run --rm -t --workdir /tmp -u $(id -u):$(id -g) -v $BASE_DIR/$LOCOD_CPU_DIR:/tmp/locod-cpu ${ULTRA96_SDK_DOCKER_IMG} bash -c \
			'source /opt/petalinux-sdk/environment-setup-cortexa72-cortexa53-xilinx-linux;\
			cd locod-cpu;\
			make re'
		cp $LOCOD_CPU_DIR/bin/locod-cpu locod-output/locod-cpu
		;;
	enclustra)
		cp $FILE $LOCOD_CPU_DIR/src/main.c
		docker run --rm -t --workdir /tmp -u $(id -u):$(id -g) -v $BASE_DIR/$LOCOD_CPU_DIR:/tmp/locod-cpu ${ENCLUSTRA_SDK_DOCKER_IMG} bash -c \
			'source /opt/petalinux-sdk/environment-setup-cortexa72-cortexa53-xilinx-linux;\
			cd locod-cpu;\
			make re'
		cp $LOCOD_CPU_DIR/bin/locod-cpu locod-output/locod-cpu
		;;
	ng-ultra)
		#TODO
		;;
esac

echo "Done !"

fi

#*************************************************************************************/
#***************** Step 2 : Extracting accel parameters from C code ******************/
#*************************************************************************************/
if [[ $FPGA == 1 ]]; then

echo -n "Extracting accel parameters from C code ... "

MAX_ACC_NB=$(get_max_acc_number $FILE)
FPGA_FUNC=$(get_fpga_func $FILE)
USED_ACC_NB=$(get_used_acc_number $FILE)


echo "Done !"

fi


#*******************************************************************************************/
#***************** Step 3 : Generating RTL code of accelerators functions ******************/
#*******************************************************************************************/
if [[ $FPGA == 1 ]]; then

echo -n "Generating RTL code of accelerators functions ... "

cp $FILE temp/main.c

for ACC in $FPGA_FUNC; do
	docker run --rm -t --workdir /tmp -u $(id -u):$(id -g) -v $PWD/temp:/tmp ${PANDA_DOCKER_IMG} bash -c \
		"mkdir bambu;\
		cd bambu;\
		bambu ${BAMBU_OPT} --top-fname=${ACC} ../main.c;\
		cp ${ACC}.v ../;\
		cd ..;\
		rm -rf bambu" &> /dev/null
	cp temp/${ACC}.v ${LOCOD_FPGA_DIR}/src/generated_files/
done

echo "Done !"

fi


#*******************************************************************************************/
#***************** Step 4 : Generating VHDL components from generated IPs ******************/
#*******************************************************************************************/
if [[ $FPGA == 1 ]]; then

echo -n "Generating VHDL components from generated IPs ... "

# cp $LOCOD_FPGA_DIR/src/top_design/rtl/top_ip_template.vhd $LOCOD_FPGA_DIR/src/generated_files/top_ip.vhd
# sed -i "s/NB_ACCELERATORS/${MAX_ACC_NB}/g" $LOCOD_FPGA_DIR/src/generated_files/top_ip.vhd

cp $LOCOD_FPGA_DIR/src/top_design/rtl/top_template.vhd $LOCOD_FPGA_DIR/src/generated_files/top.vhd
sed -i "s/NB_ACCELERATORS/${MAX_ACC_NB}/g" $LOCOD_FPGA_DIR/src/generated_files/top.vhd

for ACC in $FPGA_FUNC; do
	ACC_NUM=$(get_fct_acc_number $FILE $ACC)
	cp $LOCOD_FPGA_DIR/src/accelerator/rtl/accelerator_template.vhd $LOCOD_FPGA_DIR/src/generated_files/accelerator_${ACC_NUM}.vhd
	sed -i "s/accelerator_X/accelerator_${ACC_NUM}/g" $LOCOD_FPGA_DIR/src/generated_files/accelerator_${ACC_NUM}.vhd
	sed -i "s/generated_ip_inst : entity work.XXXX/generated_ip_inst : entity work.${ACC}/g" $LOCOD_FPGA_DIR/src/generated_files/accelerator_${ACC_NUM}.vhd
	echo 	"accelerator_${ACC_NUM}_inst : entity work.accelerator_${ACC_NUM}
			port map(
				clk					    => clk,
				rst 				    => rst,
				start 				    => registers(0)(2*${ACC_NUM}),
				reset 				    => registers(0)(2*${ACC_NUM}+1),
				param 				    => registers(3*${ACC_NUM}+2),
				result  			    => registers(3*${ACC_NUM}+2+1),
				status_end_process 	    => registers(1)(${ACC_NUM}),
				duration_count_latched  => registers(3*${ACC_NUM}+2+2),
				M_AXI_out               => M_AXI_out_array(${ACC_NUM}),
				M_AXI_in                => M_AXI_in_array(${ACC_NUM})
			);
			" >> $LOCOD_FPGA_DIR/src/generated_files/top.vhd
done

echo "end Behavioral;" >> $LOCOD_FPGA_DIR/src/generated_files/top.vhd

echo "Done !"

fi


#*********************************************************************************/
#***************** Step 5 : Synthesis of the FPGA design *************************/
#*********************************************************************************/
if [[ $FPGA == 1 ]]; then

echo -n "Synthesis of the FPGA design ... "

case $TARGET in
	ultra96 | enclustra)
		cd $LOCOD_FPGA_DIR/vivado
		vivado -mode tcl -nojournal -nolog -source generate_vivado.tcl -tclargs ${TARGET}
		cp fpga.bit $BASE_DIR/locod-output/
		rm -rf fpga.bit
		rm -rf locod-vivado_*
		cd $BASE_DIR
		;;
	ng-ultra)
		docker run --rm -t --workdir /tmp -u $(id -u):$(id -g) --hostname ${NX_HOSTNAME} --mac-address ${NX_MAC_ADDR} -v $BASE_DIR/$LOCOD_FPGA_DIR:/tmp ${NX_DOCKER_IMG} bash -c \
			"lmgrd;\
			sleep 1;\
			cd nanoxplore;\
			nxpython create_proj.py"
		;;
esac

echo "Done !"

fi


#*************************************************************************/
#***************** Removing temporary files and folders ******************/
#*************************************************************************/
rm -rf temp
