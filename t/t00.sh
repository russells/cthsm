#!/bin/sh

. ./testlibrary.sh

test_header "Test the test suite"

do_this_test && {
run_test \
	'Test a successful test' \
	'true' \
	0 \
	'true' \
|| exit 1
}

do_this_test && {
run_test \
	'Test skipping a test' \
	'true' \
	0 \
	'true' \
	SKIP \
|| exit 1
}

do_this_test && {
run_test \
	'Test skipping a failed test' \
	'false' \
	0 \
	'false' \
	SKIPFAIL \
|| exit 1
}

do_this_test && {
run_test \
	'Test not skipping a successful test' \
	'true' \
	0 \
	'true' \
	SKIPFAIL \
|| exit 1
}

test_trailer
