cmd_crypto/deflate.ko := mipsel-openwrt-linux-uclibc-ld -r  -m elf32ltsmip -T /home/lin/openwrt/openwrt_system/mtk7620a/openwrt-3.10.14/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620/linux-3.10.14-p112871/scripts/module-common.lds --build-id  -o crypto/deflate.ko crypto/deflate.o crypto/deflate.mod.o
