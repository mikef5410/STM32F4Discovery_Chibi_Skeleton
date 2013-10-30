
#ifndef _BBI2C_INCLUDED
#define _BBI2C_INCLUDED

#include <stm32f4xx.h>
#include <stm32f4xx_i2c.h>
#include <stm32f4xx_rcc.h>

#ifdef GLOBAL_BBI2C
#define BBI2CGLOBAL
#define BBI2CPRESET(A) = (A)
#else
#define BBI2CPRESET(A)
#ifdef __cplusplus
#define BBI2CGLOBAL extern "C"
#else
#define BBI2CGLOBAL extern
#endif  /*__cplusplus*/
#endif                          /*GLOBAL_BBI2C */

#ifdef __cplusplus
extern "C" {
#endif
  typedef uint8_t bool;

// ----------------------------------------------------------------
// PRIVATE API AND SUBJECT TO CHANGE!
// ----------------------------------------------------------------
  BBI2CGLOBAL void i2c_start_cond(void);
  BBI2CGLOBAL void i2c_stop_cond(void);
  BBI2CGLOBAL void i2c_write_bit(bool bit);
  BBI2CGLOBAL bool i2c_read_bit(void); 
  BBI2CGLOBAL bool i2c_write_byte(bool send_start,  bool send_stop, unsigned char byte); 
  BBI2CGLOBAL unsigned char i2c_read_byte(bool nack, bool send_stop);

// ----------------------------------------------------------------
// PUBLIC API definition
// ----------------------------------------------------------------

#define GPIOBANK GPIOB
#define SCL_PIN 8
#define SDA_PIN 7

#define BBI2C_TRANSMIT 0x0
#define BBI2C_RECEIVE 0x1
#define SEND_START 0x1
#define SEND_STOP 0x1
#define NO_START 0x0
#define NO_STOP 0x0
#define SEND_NACK 0x1
#define NO_NACK 0x0

//Error returns
#define OK 0
#define ADDRESS_NAK 1
#define REGADDR_FAIL 2
#define WRITE_FAIL 3
#define TIMEOUT 4

  typedef struct {
    int dummy;
  } I2CDEV_t;

  
BBI2CGLOBAL void init_bbI2C(void);
BBI2CGLOBAL bool bbI2C_start(uint8_t address, uint8_t direction);
BBI2CGLOBAL bool bbI2C_write(uint8_t data);
BBI2CGLOBAL uint8_t bbI2C_read_ack(void);
BBI2CGLOBAL uint8_t bbI2C_read_nack(void);
BBI2CGLOBAL void bbI2C_stop(void);
BBI2CGLOBAL uint8_t bbI2C_bufio(uint8_t devaddr, uint8_t regaddr, 
                                   uint8_t *txbuf, uint8_t ntx, 
                                   uint8_t *rxbuf, uint8_t nrx);
#ifdef __cplusplus
}
#endif


#endif                          //_BBI2C_INCLUDED
