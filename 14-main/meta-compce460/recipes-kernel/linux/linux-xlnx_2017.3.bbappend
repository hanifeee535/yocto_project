PV := "${PV}-rt"

FILESEXTRAPATHS_prepend := "${THISDIR}/linux-xlnx:"

SRC_URI_append_pynq = " \
        file://rt.cfg;sha256sum=65a5bb77ce6063c436507023b3ce0982556fae029c4edd0358e1ce421d265895 \
	https://mirrors.edge.kernel.org/pub/linux/kernel/projects/rt/4.9/older/patch-4.9-rt1.patch.xz;sha256sum=c777fec0c28c1ab27fb798990e0572af8413c7993a4f6017faca8908c1d90579 \
        "
