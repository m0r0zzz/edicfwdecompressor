
edickeygenerator e 12345678 < id.txt | edicidgenerator id.bin 1
edicplancomposer plan.txt plan.bin
lzma e edicapp-ng.bin edicapp-ng.bin.lzma -a9 -lc1 -lp1
datatoc edicapp-ng.bin.lzma code.h
call EDIC.BAT
call flash6.cmd
edictool COM6: doflash.run
pause