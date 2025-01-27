FILE_NAME=main.cpp
BUILD_PATH=./build/main
BIN_PATH=./build/main

CXX=clang++
VERSION=-std=c++17
BUILD = ${CXX} -o ${BUILD_PATH} ${FILE_NAME} 

	
all: 
	${BUILD}
	${BIN_PATH}
	
build: 
	${BUILD}	
	
build-d:
	${BUILD} --debug

run: 
	${BIN_PATH}
	
