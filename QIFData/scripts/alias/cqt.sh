#!/bin/bash

function join_by { local IFS="$1"; shift; echo "$*"; }

var=$(join_by , $@)
cqtdeployer $var -bin $var

