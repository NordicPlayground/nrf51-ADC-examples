TARGET_CHIP := NRF51822_QFAA_CA
BOARD := BOARD_NRF6310
CFLAGS += -DNRF51822_QFAA_CA -DBLE_STACK_SUPPORT_REQD -DBOARD_NRF6310

C_SOURCE_FILES += main.c
C_SOURCE_FILES += ../../../../../Source/ble/ble_services/ble_bas.c
C_SOURCE_FILES += ../../../../../Source/ble/ble_services/ble_dis.c
C_SOURCE_FILES += ../../../../../Source/ble/ble_services/ble_hrs.c
C_SOURCE_FILES += ../../../../../Source/ble/ble_services/ble_srv_common.c
C_SOURCE_FILES += ../../../../../Source/ble/ble_error_log.c
C_SOURCE_FILES += ../../../../../Source/ble/ble_advdata.c
C_SOURCE_FILES += ../../../../../Source/ble/ble_sensorsim.c
C_SOURCE_FILES += ../../../../../Source/sd_common/softdevice_handler.c
C_SOURCE_FILES += ../../../../../Source/ble/ble_conn_params.c
C_SOURCE_FILES += ../../../../../Source/ble/ble_bondmngr.c
C_SOURCE_FILES += ../../../../../Source/app_common/pstorage.c
C_SOURCE_FILES += ../../../../../Source/app_common/crc16.c
C_SOURCE_FILES += ../../../../../Source/ble/ble_radio_notification.c
C_SOURCE_FILES += ../../../../../Source/ble/ble_debug_assert_handler.c
C_SOURCE_FILES += ../../../../../Source/app_common/app_timer.c

INCLUDEPATHS += -I"../../../../../Include/ble"
INCLUDEPATHS += -I"../../../../../Include/s110"
INCLUDEPATHS += -I"../../../../../Include/app_common"
INCLUDEPATHS += -I"../../../../../Include/ble/ble_services"
INCLUDEPATHS += -I"../../../../../Include/sd_common"
SDK_PATH = ../../../../../
OUTPUT_FILENAME := ble_hrs_gcc

DEVICE_VARIANT := xxaa
#DEVICE_VARIANT := xxab

USE_SOFTDEVICE := S110
#USE_SOFTDEVICE := s210

include $(SDK_PATH)Source/templates/gcc/Makefile.common
