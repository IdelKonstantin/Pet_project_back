#!/bin/bash

cmake . && make -B && ./test_lib 2>/dev/null