#!/bin/bash

#
#  If not stated otherwise in this file or this component's Licenses.txt file
#  the following copyright and licenses apply:
#
#  Copyright 2016 RDK Management
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
# 
#  http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#  

INCS="-I./ -I../../src -I../../../../../install/usr/include/rbus -I../../../../../install/usr/include/rtmessage"
LIBS="-L../../../../../install/usr/lib"
gcc $INCS $LIBS -o Sample_app Sample_app.c Sample_rbus.c Sample_hal.c ../../src/rbus_context_helpers.c -lrbus -lrbuscore -lrtMessage -lmsgpackc -lcjson -g
