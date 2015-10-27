
edickeygenerator e 12345678 < id.txt | edicidgenerator id.bin 1
edicplancomposer plan.txt plan.bin
lzma e edicapp-ng.bin edicapp-ng.bin.lzma -a9 -lc1 -lp1
datatoc edicapp-ng.bin.lzma code.h
call EDIC.BAT
call flash5.cmd
edictool COM5: doflash.run
pause