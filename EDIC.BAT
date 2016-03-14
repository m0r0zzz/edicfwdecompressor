SET PATH=C:\Dev\Keil_v5\ARM\ARMCC\Bin;C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v7.5\bin;C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v7.5\libnvvp;C:\ProgramData\Oracle\Java\javapath;C:\Python34(x86)\;C:\Python34(x86)\Scripts;C:\Dev\WinAVR\bin;C:\Dev\WinAVR\utils\bin;C:\Program Files\Common Files\Microsoft Shared\Windows Live;C:\Program Files (x86)\Common Files\Microsoft Shared\Windows Live;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files (x86)\Windows Live\Shared; c:\Xilinx\14.4\ISE_DS\ISE\bin\nt;C:\Program Files (x86)\Git\cmd;C:\Program Files\TortoiseGit\bin;C:\Program Files\doxygen\bin;C:\Aldec\Active-HDL Student Edition\bin;c:\Program Files (x86)\Microsoft SQL Server\110\Tools\Binn\;c:\Program Files\Microsoft SQL Server\110\Tools\Binn\;c:\Program Files\Microsoft SQL Server\110\DTS\Binn\;C:\Program Files\MATLAB\MATLAB Production Server\R2015a\runtime\win64;C:\Program Files\MATLAB\MATLAB Production Server\R2015a\bin;C:\Program Files\MATLAB\R2014b\runtime\win64;C:\Program Files\MATLAB\R2014b\bin;C:\Program Files\MATLAB\R2014b\polyspace\bin;C:\Program Files\MATLAB\MATLAB Compiler Runtime\v84\runtime\win64;C:\Program Files (x86)\Skype\Phone\;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;c:\Dev\Mingw-w64\bin\;C:\Dev\CMake\bin;C:\Dev\Aptina\;C:\Program Files\Microsoft SQL Server\120\Tools\Binn\;C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\;C:\Program Files\MATLAB\MATLAB Production Server\R2015a\polyspace\bin;C:\Users\MorozZz\.dnx\bin;C:\Program Files\Microsoft DNX\Dnvm\;C:\Program Files (x86)\Nmap
SET CPU_TYPE=ARMCM0
SET CPU_VENDOR=ARM
SET UV2_TARGET=EDIC
SET CPU_CLOCK=0x00B71B00
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\startup.__i"
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\main.__i"
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\sys.__i"
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\lzma.__i"
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Objects\edicfwdecompressor.lnp"
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\sys._2i"
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\lzma._2i"
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Objects\edicfwdecompressor.l2p"
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\sys._2i"
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\lzma._2i"
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Objects\edicfwdecompressor.l2p"
"C:\Dev\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\Objects\edicfwdecompressor.axf" --i32combined --output ".\Objects\edicfwdecompressor.hex"
fromelf --bin Objects/edicfwdecompressor.axf -o edicfwdecompressor.bin
