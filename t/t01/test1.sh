#!/bin/bash

set -e
make t1
diff -q saved-output <(./t1)
