#!/bin/bash

set -e
make
diff -q saved-output <(./testhsm)
