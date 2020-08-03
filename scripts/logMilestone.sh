#!/bin/sh
# ============================================================================
# RDK MANAGEMENT, LLC CONFIDENTIAL AND PROPRIETARY
# ============================================================================
# This file (and its contents) are the intellectual property of RDK Management, LLC.
# It may not be used, copied, distributed or otherwise  disclosed in whole or in
# part without the express written permission of RDK Management, LLC.
# ============================================================================
# Copyright (c) 2019 RDK Management, LLC. All rights reserved.
# ============================================================================

. /etc/include.properties

export PATH=$PATH:/usr/bin:/bin:/usr/local/bin:/sbin:/usr/local/sbin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/lib/

if [ -z "$1" ];
then
    echo "Usage: `basename $0` <MILESTONE_EVENT>"
    exit 1
fi

#getClockUptime will give uptime info in milliseconds.
UPTIME_BIN="/usr/bin/getClockUptime"

MILESTONE_EVENT=$1
MILESTONE_LOG="$LOG_PATH/rdk_milestones.log"

if [ -f "$UPTIME_BIN" ]; then
    #Read uptime using getClockUptime binary
    uptime=`$UPTIME_BIN`
else
    echo "$UPTIME_BIN not found..!"
    exit -1
fi

#Log the Milestone info with uptime into rdk_milestones.log
echo "$MILESTONE_EVENT:$uptime" >> $MILESTONE_LOG
