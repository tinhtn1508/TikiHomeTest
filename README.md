# Hometest

Implement a small search engine by two method (reverse index and BM25)

### Prerequisites

Linux 18.04
g++ (Ubuntu 8.3.0-6ubuntu1) 8.3.0

### Structure

Hometest
|-- build
|   |-- sources
|   |   |--subdir.mk
|   |-- makefile
|   |-- objects.mk
|   |-- sources.mk
|-- data
|   |-- 100_query.txt
|   |-- product_names.txt
|-- document
|   |-- Backend Hometest_Discovery Team.pdf
|-- result
|   |-- report.txt
|   |-- result_bm25.txt
|   |-- result_reverse_index.txt
|-- sources
|   |-- BM25.cc
|   |-- BM25.hh
|   |-- main.cc
|   |-- ParsingTextFile.cc
|   |-- ParsingTextFile.hh
|   |-- SearchHandler.cc
|   |-- SearchHandler.hh
|-- README.md

### Building

Build with "__BM25__" flag to enable BM25 algorithm
or not disable

Execute commands:

cd ./Hometest/build
make clean; make -j8

## Running

Execute binary file in "build" folder

## Versioning

v1.0.0

## Authors

* **Tinh Nguyen** - *Tiki-HomeTest* - [tinhtn96](https://github.com/tinhtn96)
