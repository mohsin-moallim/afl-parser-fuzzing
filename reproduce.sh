#!/bin/sh
# Run the parser on one crashing input AFL saved, to see the ASan report.
# usage: ./reproduce.sh out/default/crashes/id:000000,...
./parser "$1"
