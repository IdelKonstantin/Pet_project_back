#!/usr/bin/env python3

import installer_routines as lr

print("#################################################################")
print("#")
print("#	Это сборщик-инсталлятор (запускать под sudo)")	
print("#")
print("#################################################################")

lr.installBinsAndLibs()
lr.cloneBuildAndInstallCatch2()
lr.installDButils()
lr.buildTDBsByScripts()
lr.buildBinariesAndLibs()
lr.buildTests()
lr.installBinariesAndSharedObj()
lr.startSystemdServices()

answer = ""
answer = input("Показать статусы демонов?(Y/N) ")

if(answer == "Y" or answer == "y"):
	lr.checkDaemonStatuses()

print("Установка завершена")

