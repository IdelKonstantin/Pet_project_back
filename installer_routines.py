import subprocess
import os
import shutil

#################################################################
# 
#	Это как бы функционал как бы инсталлятора как бы сервиса	
#
#################################################################

def installBinsAndLibs():

	print("===== [Установка библиотек и gcc/make/cmake...] =====\n")

	LIBS_AND_BINS_REQUIRED = [
		"build-essential",
		"cmake",
		"libboost-dev",
		"libpthread-stubs0-dev",
		"libpq-dev",
		"libc6-dev",
		"libssl-dev",
		"libuuid1"
	]

	print(">>> Установка библиотек")

	libsList = ""

	for lib in LIBS_AND_BINS_REQUIRED:
		libsList += (lib + " ")

	subprocess.Popen("sudo apt install " + libsList, shell=True).wait()

def cloneBuildAndInstallCatch2():

	print("\n===== [Сборка и установка Catch 2] =====\n")

	catchCloneCommand = "git clone https://github.com/catchorg/Catch2.git" 
	subprocess.Popen(catchCloneCommand, shell=True).wait()

	catchCatalogue = "./Catch2"

	os.chdir(catchCatalogue)
	subprocess.Popen("cmake -Bbuild -H. -DBUILD_TESTING=OFF", shell=True).wait()
	subprocess.Popen("sudo cmake --build build/ --target install", shell=True).wait()
	os.chdir("../")

	print(">>> Удаляю каталог [" + catchCatalogue + "]")
	shutil.rmtree(catchCatalogue)

def installDButils():

	print("\n===== [Установка postgre-sql & SQLite] =====\n")

	BDS_TO_INSTALL = [
		"postgresql",
		"sqlite3"
	]

	for db in BDS_TO_INSTALL:
		print(">>> Установка СУБД: " + db)
		subprocess.Popen("sudo apt install " + db, shell=True).wait()

def buildTDBsByScripts():

	print("\n===== [Создание баз данных и таблиц в postgre-sql & SQLite] =====\n")

	os.chdir("./DBs_scripts")
	subprocess.Popen("chmod +x ./create_db_POSTGRE.sh && ./create_db_POSTGRE.sh", shell=True).wait()
	subprocess.Popen("sqlite3 petproject_db.sqlite < sqlite.sql", shell=True).wait()
	os.chdir("../")

def buildBinariesAndLibs():

	print("\n===== [Сборка бинарников и статических библиотек pet-microservice'а] =====\n")

	DIRS_TO_EXECUTE = [
		"ini_file_parser",
		"pet_logger",
		"TDB_workers",
		"file_watcher",
		"base_daemon",
		"filewatch_daemon",
		"http_daemon"
	]

	for dir in DIRS_TO_EXECUTE:
		print(">>> Сборка в папке: " + dir)
		os.chdir("./" + dir + "/build")
		subprocess.Popen("cmake .. && make -B", shell=True).wait()
		os.chdir("../..")

def buildTests():

	print("\n===== [Сборка тестов компонент pet-microservice'а] =====\n")

	DIRS_TO_EXECUTE_TESTS = [
		"ini_file_parser/test_dir",
		"pet_logger/test_lib",
		"TDB_workers/test_libs",
		"file_watcher/test_dir",
		"base_daemon/test_daemon",
		"filewatch_daemon/complete_daemon",
		"http_daemon/complete_daemon"
	]

	for dir in DIRS_TO_EXECUTE_TESTS:
		print(">>> Сборка тестов в папке: " + dir)
		os.chdir("./" + dir)
		subprocess.Popen("cmake . && make -B", shell=True).wait()
		os.chdir("../..")

def installBinariesAndSharedObj():

	print("\n===== [Установка разделяемых библиотек и бинарников pet-microservice'а] =====\n")

	os.chdir("./TDB_workers/build")
	print(">>> Установка json1.so в [/usr/lib]")
	subprocess.Popen("sudo make install", shell=True).wait()
	os.chdir("../..")

	################# /opt/pet/sbin  #################

	INSTALL_DIR = "/opt/pet/sbin"

	subprocess.Popen("sudo mkdir -p " + INSTALL_DIR, shell=True).wait()

	print(">>> Установка watch_daemon в [" + INSTALL_DIR + "]")
	subprocess.Popen("sudo cp ./filewatch_daemon/complete_daemon/watch_daemon " + INSTALL_DIR, shell=True).wait()

	print(">>> Установка http_daemon в [" + INSTALL_DIR + "]")
	subprocess.Popen("sudo cp ./http_daemon/complete_daemon/http_daemon " + INSTALL_DIR, shell=True).wait()
	
	print(">>> Копирование *.ini в [" + INSTALL_DIR + "]")
	subprocess.Popen("sudo cp ./Service_files/ini_example.ini.* " + INSTALL_DIR, shell=True).wait()

	print(">>> Копирование ТБД SQLite в [" + INSTALL_DIR + "]")
	subprocess.Popen("sudo cp ./DBs_scripts/petproject_db.sqlite " + INSTALL_DIR, shell=True).wait()

	################# ./_pet_/sbin  #################

	# INSTALL_DIR = "./_pet_/sbin"

	# try:
	# 	os.makedirs(INSTALL_DIR)
	# except:
	# 	pass

	# print(">>> Установка watch_daemon в [" + INSTALL_DIR + "]")
	# subprocess.Popen("cp ./filewatch_daemon/complete_daemon/watch_daemon " + INSTALL_DIR, shell=True).wait()

	# print(">>> Установка http_daemon в [" + INSTALL_DIR + "]")
	# subprocess.Popen("cp ./http_daemon/complete_daemon/http_daemon " + INSTALL_DIR, shell=True).wait()
	
	# print(">>> Копирование *.ini в [" + INSTALL_DIR + "]")
	# subprocess.Popen("cp ./Service_files/ini_example.ini.* " + INSTALL_DIR, shell=True).wait()

	# print(">>> Копирование ТБД SQLite в [" + INSTALL_DIR + "]")
	# subprocess.Popen("cp ./DBs_scripts/petproject_db.sqlite " + INSTALL_DIR, shell=True).wait()

def startSystemdServices():
	
	SYSTEMD_DIR = "/etc/systemd/system"

	print(">>> Установка systemd-файлов в [" + SYSTEMD_DIR + "]")

	subprocess.Popen("sudo cp ./Service_files/watch_daemon.service " + SYSTEMD_DIR, shell=True).wait()
	subprocess.Popen("sudo cp ./Service_files/http_daemon.service " + SYSTEMD_DIR, shell=True).wait()

	print(">>> Запуск сервиса [watch_daemon.service]")
	subprocess.Popen("sudo systemctl enable watch_daemon.service", shell=True).wait()
	subprocess.Popen("sudo systemctl start watch_daemon.service", shell=True).wait()
	
	print(">>> Запуск сервиса [http_daemon.service]")
	subprocess.Popen("sudo systemctl enable http_daemon.service", shell=True).wait()
	subprocess.Popen("sudo systemctl start http_daemon.service", shell=True).wait()

def checkDaemonStatuses():
	subprocess.Popen("sudo systemctl status watch_daemon.service", shell=True).wait()
	subprocess.Popen("sudo systemctl status http_daemon.service", shell=True).wait()