#!/bin/sh

. ./testlibrary.sh

test_header "Basic HSMs"

do_this_test && {
	cd t01 &&
	run_test "basic HSM" ./test.sh 0 :
}

test_trailer

