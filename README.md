# RTEMS 4.12
export RTEMS_MAKEFILE_PATH=$HOME/development/rtems/bsps/4.12/sparc-rtems4.12/sis/

export PATH=$HOME/development/rtems/4.12/bin:$PATH

To make one application start, just do:
cd [PATH OF APP]
make
sparc-rtems4.12-gdb o-optimize/posix_hello.exe make
tar sim
load
r
