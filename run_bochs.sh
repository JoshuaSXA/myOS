sudo /sbin/losetup /dev/loop0 ./image/CocOS.img
sudo bochs -f ./configure/bochsrc
sudo /sbin/losetup -d /dev/loop0
