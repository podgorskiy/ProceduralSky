cmake_minimum_required(VERSION 2.8)

project(tinyxml)

file(GLOB SOURCES *.cpp *.h)

add_library(tinyxml ${SOURCES})
