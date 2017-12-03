#!/bin/sh
myFile="access.log"
if [ -f "$myFile" ]; then
 cp access.log ~/access.log_bk
 echo "update file"
fi
