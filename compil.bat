
@echo off
echo -------- Compilation of : 
echo %1
echo .


SET ASM=sdasz80 
SET CC=sdcc 
SET DEST=dsk\


SET INCLUDEDIR=fusion-c\include\
SET LIBDIR=fusion-c\lib\

SET program=main
SET TARGET_DSK=disco.dsk

rem sjasm (http://www.xl2s.tk/) es un compilador de ensamblador z80 que puedo convertir tu código ensamblador en los archivos binarios.rom y .bin
rem necesitamos el .bin de la pantalla de carga y del reproductor de música
rem start /wait sjasm world0.asm
rem move /y world0.bin .\dsk



SET INC1=%INCLUDEDIR%crt0_msxdos.rel
REM SET INC2=%INCLUDEDIR
REM SET INC3=%INCLUDEDIR
REM SET INC4=%INCLUDEDIR%
REM SET INC5=%INCLUDEDIR%
REM SET INC6=%INCLUDEDIR%
REM SET INC7=%INCLUDEDIR%
REM SET INC8=%INCLUDEDIR%
REM SET INC9=%INCLUDEDIR%
REM SET INCA=%INCLUDEDIR%
REM SET INCB=%INCLUDEDIR%
REM SET INCC=%INCLUDEDIR%
REM SET INCD=%INCLUDEDIR%
REM SET INCE=%INCLUDEDIR%
REM SET INCF=%INCLUDEDIR%

SET ADDR_CODE=0x106
SET ADDR_DATA=0x0



SDCC --code-loc %ADDR_CODE% --data-loc %ADDR_DATA% --disable-warning 196 -mz80 --no-std-crt0 --opt-code-size fusion.lib -L %LIBDIR% %INC1% %INC2% %INC3% %INC4% %INC5% %INC6% %INC7% %INC8% %INC9% %INCA% %INCB% %INCC% %INCD% %INCE% %INCF% %program%.c



SET cpath=%~dp0


IF NOT EXIST %program%.ihx GOTO _end_
echo ... Compilation OK
@echo on

tools\Hex2bin\hex2bin -e com %program%.ihx

@echo off

copy %program%.com DSK\%program%.com /y

del %program%.com
del %program%.asm
del %program%.ihx
del %program%.lk
del %program%.lst
del %program%.map
del %program%.noi
del %program%.sym
del %program%.rel

echo Done.

:Emulator
rem Set MyProcess=openmsx.exe
rem tasklist | find /i "%MyProcess%">nul  && (echo %MyProcess% Already running) || start tools\emulators\openMSX-16.0\openmsx.exe -script tools\emulators\openMSX-16.0\emul_start_config.txt



if exist %program% del /f /Q %program%
copy tools\Disk-Manager\disco.dsk .\disco.dsk

for /R dsk/ %%a in (*.*) do (
    start /wait tools\Disk-Manager\DISKMGR.exe -A -F -C disco.dsk "%%a")   


:create_disk
start /wait tools/Disk-Manager/DISKMGR.exe -A -F -C %TARGET_DSK% dsk/AUTOEXEC.BAT  
start /wait tools/Disk-Manager/DISKMGR.exe -A -F -C %TARGET_DSK% dsk/loader.s08
start /wait tools/Disk-Manager/DISKMGR.exe -A -F -C %TARGET_DSK% dsk/Music1.pt3
start /wait tools/Disk-Manager/DISKMGR.exe -A -F -C %TARGET_DSK% dsk/Music2.pt3
start /wait tools/Disk-Manager/DISKMGR.exe -A -F -C %TARGET_DSK% dsk/Music3.pt3
start /wait tools/Disk-Manager/DISKMGR.exe -A -F -C %TARGET_DSK% dsk/%program%.com
start /wait tools/Disk-Manager/DISKMGR.exe -A -F -C %TARGET_DSK% dsk/COMMAND.COM
start /wait tools/Disk-Manager/DISKMGR.exe -A -F -C %TARGET_DSK% dsk/MSXDOS.SYS

copy /y %TARGET_DSK% .\docs


:Emulator
rem copy disco.dsk tools\BlueMSX
rem start /wait tools\emulators\BlueMSX\blueMSX.exe -diska %program%.dsk
rem Set MyProcess=openmsx.exe
rem tasklist | find /i "%MyProcess%">nul  && (echo %MyProcess% Already running) || start tools\openMSX-16.0\openmsx.exe -script tools\openMSX-16.0\emul_start_config.txt
start tools\emulators\openMSX-16.0\openmsx.exe -script tools\emulators\openMSX-16.0\emul_start_config.txt
:_end_