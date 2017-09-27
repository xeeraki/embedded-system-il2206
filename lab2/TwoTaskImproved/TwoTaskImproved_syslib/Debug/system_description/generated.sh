#!/bin/sh
#
# generated.sh - shell script fragment - not very useful on its own
#
# Machine generated for a CPU named "cpu" as defined in:
# /home/il2206/nios2-ide-workspace-10.0/Nios2system20090707.ptf
#
# Generated: 2017-09-27 11:32:47.662

# DO NOT MODIFY THIS FILE
#
#   Changing this file will have subtle consequences
#   which will almost certainly lead to a nonfunctioning
#   system. If you do modify this file, be aware that your
#   changes will be overwritten and lost when this file
#   is generated again.
#
# DO NOT MODIFY THIS FILE

# This variable indicates where the PTF file for this design is located
ptf=/home/il2206/nios2-ide-workspace-10.0/Nios2system20090707.ptf

# This variable indicates whether there is a CPU debug core
nios2_debug_core=yes

# This variable indicates how to connect to the CPU debug core
nios2_instance=0

# This variable indicates the CPU module name
nios2_cpu_name=cpu

# Include operating system specific parameters, if they are supplied.

if test -f /opt/altera/10.0/nios2eds/components/micrium_uc_osii/build/os.sh ; then
   . /opt/altera/10.0/nios2eds/components/micrium_uc_osii/build/os.sh
fi
