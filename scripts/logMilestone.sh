#!/bin/sh

##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################

. /etc/include.properties

export PATH=$PATH:/usr/bin:/bin:/usr/local/bin:/sbin:/usr/local/sbin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/lib/

if [ -z "$1" ];
then
    echo "Usage: `basename $0` <MILESTONE_EVENT>"
    exit 1
fi

#rdkLogMileStone will give uptime info in milliseconds.
UPTIME_BIN="/usr/bin/rdkLogMileStone"

MILESTONE_EVENT=$1

if [ -f "$UPTIME_BIN" ]; then
    #write uptime using rdkLogMileStone binary
    uptime=`$UPTIME_BIN $1`
else
    echo "$UPTIME_BIN not found..!"
    exit -1
fi
