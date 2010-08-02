#!/bin/sh

. ./testlibrary.sh

test_header "Basic HSMs"

do_this_test && {
	(
	cd t01 &&
	run_test "basic HSM" ./test1.sh 0 :
	)
}

do_this_test && {
	(
	cd t01 &&
	run_test "Transition action" ./test2.sh 0 :
	)
}

test_trailer

