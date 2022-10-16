#!/bin/bash

function join_by { local IFS="$1"; shift; echo "$*"; }

var=$(join_by , $@)
$SNAP/CQtDeployer.sh $var -bin $var

