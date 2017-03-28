# FreeRDP: A Remote Desktop Protocol Client
# FreeRDP cmake build script for USB Redirector Virtual Channel Support
#
# Copyright 2012 SimplyCore Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(USBRDR_SRCS
	usbrdr_main.c
	usbrdr_main.h
)

add_library(usbrdr ${USBRDR_SRCS})
set_target_properties(usbrdr PROPERTIES PREFIX "")

target_link_libraries(usbrdr freerdp-utils)

install(TARGETS usbrdr DESTINATION ${FREERDP_PLUGIN_PATH})
