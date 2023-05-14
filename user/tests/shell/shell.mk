$(eval $(call clear-module-vars))
LOCAL_MODULE_PATH := $(call my-dir)

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := shell
LOCAL_PROCESS_SRC := shell.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := help
LOCAL_PROCESS_SRC := help.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := ps
LOCAL_PROCESS_SRC := ps.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := ipcs
LOCAL_PROCESS_SRC := ipcs.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := echo
LOCAL_PROCESS_SRC := echo.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := clear
LOCAL_PROCESS_SRC := clear.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := kill
LOCAL_PROCESS_SRC := kill.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := sleep
LOCAL_PROCESS_SRC := sleep.c
$(eval $(call build-test-process))

$(eval $(call build-test-module))
