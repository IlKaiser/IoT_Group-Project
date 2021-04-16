# name of your application
APPLICATION = I2c-riot

# utilities for MPU 
ifneq (,$(wildcard $(CURDIR)../../RIOT/tests*/.))
  DEFAULT_MODULE += test_utils_interactive_sync
  # add interactive test configuration when testing Kconfig
  ifeq (1,$(TEST_KCONFIG))
    KCONFIG_ADD_CONFIG += $(RIOTBASE)/tests/test_interactive.config
  endif
endif

ifneq (,$(filter tests_driver_%,$(APPLICATION)))
  BOARD ?= nucleo-f401re
endif

# MPU-9250 driver
DRIVER ?= mpu9250
USEMODULE += $(DRIVER)

# If no BOARD is found in the environment, use this default:
BOARD ?= nucleo-f401re

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += xtimer
USEMODULE += analog_util
USEMODULE += dht
USEMODULE += fmt
USEMODULE += servo
FEATURES_REQUIRED += periph_i2c


include $(RIOTBASE)/Makefile.include

FEATURES_OPTIONAL += periph_rtc


#to compile use -Wno-unused-variable
