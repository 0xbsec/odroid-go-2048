cp ~/Arduino/odroid-go-2048/odroid-go-2048.ino ~/dev/odroid-go-2048/2048.ino
./mkfw 2048 tile.raw 0 16 1048576 app odroid-go-2048.ino.bin 
mv firmware.fw 2048.fw
cp 2048.fw ~/dev/odroid-go-2048/2048.fw
esptool.py write_flash 0 odroid-go-firmware-20181001.img 
