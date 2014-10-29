# Zynq uflow configuration Driver

This Linux driver has been developed to run on the Xilinx Zynq FPGA.
It is a userspace input/output driver (UIO) that enables the passing
of register values to and from the Zynq FPGA. Userspace applications
use this UIO driver to configure and control the accelerator
operations.


## Compile

Kernel modules need to be built against the version of the kernel it will be
inserted in. It is recommended to uses the Linux kernel maintained by Xilinx.

``` bash
git clone https://github.com/Xilinx/linux-xlnx.git
```

The driver module can be compiling outside of the Linux kernel source tree. A
variable 'KDIR' in the Makefile is used to point to the kernel source
directory. The default value has it pointing to the default Linux install
location for kernel sources. However, if cross compiling or if the sources are
in a non-default location the value can be overridden using an exported
environmental variable or as an argument passes into the make command.

```bash
cd zynq-uflow/dev/
export KDIR=../../linux-xlnx
make
```

or

```bash
cd zynq-uflow/dev/
make KDIR=../../linux-xlnx
```


## Inserting Module

Use of the driver module requires it to be inserted into the running Linux
kernel. Once inserted it will automatically create a character device file in
'/dev' called '/dev/uflow-cfg'. However, the default permissions will not allow
non-root users to read/write to the file. These permissions can be overridden
by installing the udev rule file found in this projects 'util' directory into
the systems '/etc/udev/rules.d/' directory. Alternatively, once the modules
inserted the permissions can be changed manually using 'chmod'.

```bash
sudo cp util/80-uflow.rules /etc/udev/rules.d/
sudo insmod dev/uflow.ko
```

or

```bash
sudo insmod uflow.ko && sudo chmod 666 /dev/uflow-cfg
```

To remove the module.

```bash
sudo rmmod uflow
```

To install the module and have it loaded at boot, first install the udev rule
as shown above and then follow the below instructions.

```bash
sudo mkdir -p /lib/modules/$(uname -r)/extra/
sudo cp uflow.ko /lib/modules/$(uname -r)/extra/
sudo depmod -a
sudo modprobe uflow
sudo sh -c 'echo "uflow" >> /etc/modules'
```


## Build and install uflow library

The uflow library has to be built and installed before the demo
applications can be compiled. The library offers a number of generic
functions for use in configuring the accelerator. For example, it can
write a single values or pass in an entire array of values.

```bash
cd lib/
sudo make install
```


## Compiling and Running Demo

The demo application runs a simple configuration test.

```bash
cd demo
make
```
