##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

CHIBIOS = ..
USE_FWLIB  := no
USE_VERBOSE_COMPILE := yes

# Compiler options here.
ifeq ($(USE_OPT),)
  USE_OPT = -O2 -ggdb -fomit-frame-pointer -falign-functions=16
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT = 
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = -fno-rtti
endif

# Enable this if you want the linker to remove unused code and data
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# If enabled, this option allows to compile the application in THUMB mode.
ifeq ($(USE_THUMB),)
  USE_THUMB = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

#
# Build global options
##############################################################################

##############################################################################
# Architecture or project specific options
#

# Enables the use of FPU on Cortex-M4.
# Enable this if you really want to use the STM FWLib.
ifeq ($(USE_FPU),)
  USE_FPU = no
endif

# Enable this if you really want to use the STM FWLib.
ifeq ($(USE_FWLIB),)
  USE_FWLIB = no
else
  USE_OPT += -DUSE_STDPERIPH_DRIVER
  FWLIB_DIR := $(CHIBIOS)/ext/STM32F4xx_StdPeriph_Driver
  FWLIB_INC := $(FWLIB_DIR)/inc
#  FWLIB_SRC := misc.c stm32f4xx_adc.c stm32f4xx_can.c stm32f4xx_crc.c stm32f4xx_cryp_aes.c stm32f4xx_cryp.c stm32f4xx_cryp_des.c stm32f4xx_cryp_tdes.c stm32f4xx_dac.c stm32f4xx_dbgmcu.c stm32f4xx_dcmi.c stm32f4xx_dma.c stm32f4xx_exti.c stm32f4xx_flash.c stm32f4xx_fsmc.c stm32f4xx_gpio.c stm32f4xx_hash.c stm32f4xx_hash_md5.c stm32f4xx_hash_sha1.c stm32f4xx_i2c.c stm32f4xx_iwdg.c stm32f4xx_pwr.c stm32f4xx_rcc.c stm32f4xx_rng.c stm32f4xx_rtc.c stm32f4xx_sdio.c stm32f4xx_spi.c stm32f4xx_syscfg.c stm32f4xx_tim.c stm32f4xx_usart.c stm32f4xx_wwdg.c
  FWLIB_SRC := $(FWLIB_DIR)/src/stm32f4xx_rcc.c $(FWLIB_DIR)/src/stm32f4xx_i2c.c 
endif

#
# Architecture or project specific options
##############################################################################

##############################################################################
# Project, sources and paths
#

# Define project name here
PROJECT = ch

# Imported source files and paths
include $(CHIBIOS)/boards/ST_STM32F4_DISCOVERY/board.mk
include $(CHIBIOS)/os/hal/platforms/STM32F4xx/platform.mk
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/ports/GCC/ARMCMx/STM32F4xx/port.mk
include $(CHIBIOS)/os/kernel/kernel.mk


# Define linker script file here
LDSCRIPT= $(PORTLD)/STM32F407xG.ld
#LDSCRIPT= $(PORTLD)/STM32F407xG_CCM.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC = $(PORTSRC) \
       $(KERNSRC) \
       $(TESTSRC) \
       $(HALSRC) \
       $(PLATFORMSRC) \
       $(BOARDSRC) $(FWLIB_SRC) \
       $(CHIBIOS)/os/various/devices_lib/accel/lis302dl.c \
       $(CHIBIOS)/os/various/shell.c \
       $(CHIBIOS)/os/various/chprintf.c \
       usbcfg.c bulk_usb.c bbi2c.c cmd_shell.c instr_task.c md5_tek.c instr_cmds.c main.c

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC =

# C sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACSRC =

# C++ sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACPPSRC =

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCSRC =

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCPPSRC =

# List ASM source files here
ASMSRC = $(PORTASM)

INCDIR = $(PORTINC) $(KERNINC) $(TESTINC) \
         $(HALINC) $(PLATFORMINC) $(BOARDINC) $(FWLIB_INC) \
         $(CHIBIOS)/os/various/devices_lib/accel \
         $(CHIBIOS)/os/various \
         $(CHIBIOS)/application/tek_opts

#
# Project, sources and paths
##############################################################################

##############################################################################
# Compiler settings
#

MCU  = cortex-m4

#TRGT = arm-elf-
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CPPC = $(TRGT)g++
# Enable loading with g++ only if you need C++ runtime support.
# NOTE: You can use C++ even without C++ support if you are careful. C++
#       runtime support makes code size explode.
LD   = $(TRGT)gcc
#LD   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
OD   = $(TRGT)objdump
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary

# ARM-specific options here
AOPT =

# THUMB-specific options here
TOPT = -mthumb -DTHUMB

# Define C warning options here
CWARN = -Wall -Wextra -Wstrict-prototypes

# Define C++ warning options here
CPPWARN = -Wall -Wextra

#
# Compiler settings
##############################################################################

##############################################################################
# Start of default section
#

# List all default C defines here, like -D_DEBUG=1
DDEFS =

# List all default ASM defines here, like -D_DEBUG=1
DADEFS =

# List all default directories to look for include files here
DINCDIR =

# List the default directory to look for the libraries here
DLIBDIR =

# List all default libraries here
DLIBS =

#
# End of default section
##############################################################################

##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
UDEFS =

# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR =

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

#
# End of user defines
##############################################################################

ifeq ($(USE_FPU),yes)
  USE_OPT += -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -fsingle-precision-constant
  DDEFS += -DCORTEX_USE_FPU=TRUE
else
  DDEFS += -DCORTEX_USE_FPU=FALSE
endif

#ifeq ($(USE_FWLIB),yes)
#  include $(CHIBIOS)/ext/stm32lib/stm32lib.mk
#  CSRC += $(STM32SRC)
#  INCDIR += $(STM32INC)
#  USE_OPT += -DUSE_STDPERIPH_DRIVER
#endif

include $(CHIBIOS)/os/ports/GCC/ARMCMx/rules.mk

include version.mk

download: build/ch.bin
	st-flash write  build/ch.bin 0x08000000
