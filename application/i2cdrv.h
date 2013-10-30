
#ifndef _I2CDRV_INCLUDED
#define _I2CDRV_INCLUDED

//#include "OSandPlatform.h"
#include <stm32f4xx.h>
#include <stm32f4xx_i2c.h>
#include <stm32f4xx_rcc.h>

#ifdef GLOBAL_I2CDRV
#define I2CDRVGLOBAL
#define I2CDRVPRESET(A) = (A)
#else
#define I2CDRVPRESET(A)
#ifdef __cplusplus
#define I2CDRVGLOBAL extern "C"
#else
#define I2CDRVGLOBAL extern
#endif  /*__cplusplus*/
#endif                          /*GLOBAL_I2CDRV */

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------
// PRIVATE API AND SUBJECT TO CHANGE!
// ----------------------------------------------------------------


// ----------------------------------------------------------------
// PUBLIC API definition
// ----------------------------------------------------------------

I2CDRVGLOBAL void init_I2C1(void);
I2CDRVGLOBAL void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
I2CDRVGLOBAL void I2C_write(I2C_TypeDef* I2Cx, uint8_t data);
I2CDRVGLOBAL uint8_t I2C_read_ack(I2C_TypeDef* I2Cx);
I2CDRVGLOBAL uint8_t I2C_read_nack(I2C_TypeDef* I2Cx);
I2CDRVGLOBAL void I2C_stop(I2C_TypeDef* I2Cx);

#ifdef __cplusplus
}
#endif


#endif                          //_I2CDRV_INCLUDED
