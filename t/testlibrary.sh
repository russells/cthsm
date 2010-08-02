#!/bin/sh

# Library for testing.

TestNumber=0
RunTestNumber="$1"
RunOut=run-output
RunErr=run-error
TestOut=test-output
TestErr=test-error
TestLibraryLocal=./testlibrary-local.sh

if [ -f "$TestLibraryLocal" ] ; then
	. "$TestLibraryLocal"
fi


colour () {
	local Colour="$1"
	shift
	case "$Colour" in
		boldblue) tput bold ; tput setaf 4 ;;
		boldred)  tput bold ; tput setaf 1 ;;
		yellow)               tput setaf 3 ;;
		green)                tput setaf 2 ;;
	esac
	printf "$@"
	tput sgr0
}

test_header () {
	colour boldblue "$(basename $0): %s\n" "$1"
}

test_trailer () {
	echo
}

# Return true if the current test is to be run.

do_this_test () {
	TestNumber=$((TestNumber+1))
	if [ -n "$RunTestNumber" ] ; then
		if [ x"$RunTestNumber" = x"$TestNumber" ] ; then
			return 0
		else
			return 1
		fi
	fi
	return 0
}

run_test_usage () {
	echo "function usage: run_test \"desc\" \"command\" \"test\" [SKIP | SKIPFAIL]" 1>&2
}


# run_test takes four arguments: a description, a command to run, an expected
# exit code from that command, and a second command to run that can test the
# results of the first.  Two commands are used so that the output from the
# first is written to storage completely (or at least to fs cache, which is the
# same to us) and the second command can see all of that output.
#
# There is an optional fifth argument, which can be "SKIP" to indicate that we
# don't want this test run, or "SKIPFAIL" to indicate that this test will
# probably fail but we want to continue with other tests.
#
# If you only want to run one command, put it in the third argument (the second
# command).

run_test () {
	if [ $# -ne 4 -a $# -ne 5 ] ; then
		run_test_usage
		return 1
	fi

	local Description="$1"
	local RunCommand="$2"
	local RunExitCode=$3
	local TestCommand="$4"
	local skip=false
	local skipfail=false

	case "$5" in
		"SKIP")
			skip=true ;;
		"SKIPFAIL")
			skipfail=true ;;
		"")
			: ;;
		*)
			run_test_usage
			return 1 ;;
	esac

	printf '%2d: %-60s ... ' $TestNumber "$Description"
	rm -f "$RunOut" "$RunErr" "$TestOut" "$TestErr"
	if $skip ; then
		colour yellow 'SKIP\n'
		return 0
	fi
	#echo "RunCommand=\"$RunCommand\""
	( eval "$RunCommand" >"$RunOut" 2>"$RunErr" )
	ExitCode=$?
	echo $ExitCode > run-exitcode
	if [ $ExitCode -ne $RunExitCode ] ; then
		if $skipfail ; then
			colour yellow 'FAIL (skip)\n'
			return 0
		else
			colour boldred 'FAIL\n'
			cat "$RunErr"
			return 1
		fi
	fi
	#echo "TestCommand=\"$TestCommand\""
	( eval "$TestCommand" >"$TestOut" 2>"$TestErr" )
	Ret=$?
	if [ 0 -eq $Ret ] ; then
		colour green 'pass\n'
	else
		if $skipfail ; then
			colour yellow 'FAIL (skip)\n'
			return 0
		else
			colour boldred 'FAIL\n'
			cat "$TestErr"
			return 1
		fi
	fi
	return $Ret
}
