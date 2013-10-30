
#define GLOBAL_BBI2C
#include "bbi2c.h"

#include <hal.h>
#include <stdint.h>

// I2CSPEED 100 = 156.25kHz or 64ns/Loop
//   100kHz = 10uS  or I2CSPEED=156
//    50kHz = 20uS  or I2CSPEED=313
#define I2CSPEED  156

#define false 0
#define true 1

#define SCL_PIN_BIT PAL_PORT_BIT(SCL_PIN)
#define SDA_PIN_BIT PAL_PORT_BIT(SDA_PIN)

static inline bool read_SCL(void);  // Set SCL as input and return current level of line, 0 or 1
static inline bool read_SDA(void);  // Set SDA as input and return current level of line, 0 or 1
static inline void clear_SCL(void); // Actively drive SCL signal low
static inline void clear_SDA(void); // Actively drive SDA signal low
void arbitration_lost(void);
 
bool started = false; // global data

static inline void I2C_delay(void) 
{ 
  volatile int v; 
  int i; 
  for (i=0; i < I2CSPEED; i++) v;
}


static inline void initGPIO(void)
{
  palSetPadMode(GPIOBANK, SDA_PIN, PAL_MODE_OUTPUT_OPENDRAIN|PAL_STM32_PUDR_PULLUP|PAL_STM32_OSPEED_HIGHEST); //SDA
  palSetPadMode(GPIOBANK, SCL_PIN, PAL_MODE_OUTPUT_OPENDRAIN|PAL_STM32_PUDR_PULLUP|PAL_STM32_OSPEED_HIGHEST); //SCL
}        

//Set pin high, then read
static inline bool read_SCL(void) {
  bool retval = 0;
  
  pal_lld_setport(GPIOBANK, SCL_PIN_BIT);
  retval = palReadPad(GPIOBANK, SCL_PIN);
  return retval;
}

static inline bool read_SDA(void) {
  bool retval = 0;
  
  pal_lld_setport(GPIOBANK, SDA_PIN_BIT);
  retval = palReadPad(GPIOBANK, SDA_PIN);
  return retval;
}

static inline void clear_SCL(void) {
  pal_lld_clearport(GPIOBANK, SCL_PIN_BIT);
  return;
}

 inline void clear_SDA(void) {
  pal_lld_clearport(GPIOBANK, SDA_PIN_BIT);
  return;
}

static void clearBus(void) {
  bool sclstate = 0;
  bool sdastate = 0;
  int k;

  sclstate = read_SCL();
  sdastate = read_SDA();

  if (!sclstate || !sdastate) {
    for (k=0; k<9; k++) {
      I2C_delay();
      clear_SCL();
      I2C_delay();
      sclstate = read_SCL();
      sdastate = read_SDA();
      if (sclstate && sdastate) break;
    }
  }
  return;
}

BBI2CGLOBAL void i2c_start_cond(void) {
  if (started) { // if started, do a restart cond
    // set SDA to 1
    read_SDA();
    I2C_delay();
    while (read_SCL() == 0) {   // Clock stretching
      // You should add timeout to this loop
    }
    // Repeated start setup time, minimum 4.7us
    I2C_delay();
  }
  if (read_SDA() == 0) {
    arbitration_lost();
  }
  // SCL is high, set SDA from 1 to 0.
  clear_SDA();
  I2C_delay();
  clear_SCL();
  started = true;
}
 
BBI2CGLOBAL void i2c_stop_cond(void){
  // set SDA to 0
  clear_SDA();
  I2C_delay();
  // Clock stretching
  while (read_SCL() == 0) {
    // You should add timeout to this loop.
  }
  // Stop bit setup time, minimum 4us
  I2C_delay();
  // SCL is high, set SDA from 0 to 1
  if (read_SDA() == 0) {
    arbitration_lost();
  }
  I2C_delay();
  started = false;
}
 
// Write a bit to I2C bus
BBI2CGLOBAL void i2c_write_bit(bool bit) {
  if (bit) {
    read_SDA();
  } else {
    clear_SDA();
  }
  I2C_delay();
  while (read_SCL() == 0) {   // Clock stretching
    // You should add timeout to this loop
  }
  // SCL is high, now data is valid
  // If SDA is high, check that nobody else is driving SDA
  if (bit && read_SDA() == 0) {
    arbitration_lost();
  }
  I2C_delay();
  clear_SCL();
}
 
// Read a bit from I2C bus
BBI2CGLOBAL bool i2c_read_bit(void) {
  bool bit;
  // Let the slave drive data
  read_SDA();
  I2C_delay();
  while (read_SCL() == 0) {   // Clock stretching
    // You should add timeout to this loop
  }
  // SCL is high, now data is valid
  bit = read_SDA();
  I2C_delay();
  clear_SCL();
  return bit;
}
 
// Write a byte to I2C bus. Return 0 if ack by the slave.
BBI2CGLOBAL bool i2c_write_byte(bool send_start,
                    bool send_stop,
                    unsigned char byte) {
  unsigned bit;
  bool nack;
  if (send_start) {
    i2c_start_cond();
  }
  for (bit = 0; bit < 8; bit++) {
    i2c_write_bit((byte & 0x80) != 0);
    byte <<= 1;
  }
  nack = i2c_read_bit();
  if (send_stop) {
    i2c_stop_cond();
  }
  return nack;
}
 
// Read a byte from I2C bus
BBI2CGLOBAL unsigned char i2c_read_byte(bool nack, bool send_stop) {
  unsigned char byte = 0;
  unsigned bit;
  for (bit = 0; bit < 8; bit++) {
    byte = (byte << 1) | i2c_read_bit();
  }
  i2c_write_bit(nack);
  if (send_stop) {
    i2c_stop_cond();
  }
  return byte;
}

BBI2CGLOBAL void init_bbI2C(void) {
  volatile bool pin1;
  volatile bool pin2;

  initGPIO();
  pin1 = read_SDA();
  pin2 = read_SCL();

  if ( !pin1 || !pin2) {
    //somebody else has the bus or things are screwed up ...
    clearBus();
  }

  return;
}

BBI2CGLOBAL bool bbI2C_start(uint8_t address, uint8_t direction) {
  bool sclstate = 0;
  bool sdastate = 0;
  uint8_t addr = address | direction;

  sclstate = read_SCL();
  sdastate = read_SDA();
  if (!sclstate || !sdastate) clearBus();  

  return(i2c_write_byte(SEND_START,NO_STOP, addr));
}

BBI2CGLOBAL uint8_t bbI2C_read_ack(void) {
  return(i2c_read_byte(NO_NACK, NO_STOP));
}

BBI2CGLOBAL uint8_t bbI2C_read_nack(void) {
  return(i2c_read_byte(SEND_NACK, NO_STOP));
}

BBI2CGLOBAL bool bbI2C_write(uint8_t data) {
  return(i2c_write_byte(NO_START, NO_STOP, data));
}

BBI2CGLOBAL void bbI2C_stop(void) {
  i2c_stop_cond();
  return;
}

BBI2CGLOBAL uint8_t bbI2C_bufio(uint8_t devaddr, uint8_t regaddr, 
                                   uint8_t *txbuf, uint8_t ntx, 
                                   uint8_t *rxbuf, uint8_t nrx)
{
  int nack; int k;

  nack = bbI2C_start(devaddr, BBI2C_TRANSMIT);
  if ( nack ) return(ADDRESS_NAK);

  nack = bbI2C_write(regaddr);
  if ( nack ) return(REGADDR_FAIL);

  if (ntx) {
    for (k=0; k<ntx; k++) {
      nack=bbI2C_write(*txbuf++);
      if ( nack ) return(WRITE_FAIL);
    }
    bbI2C_stop();
  }
  if (nrx) {  
    nack = bbI2C_start(devaddr, BBI2C_RECEIVE);
    if ( nack ) return(ADDRESS_NAK);

    for (k=0; k<nrx; k++) {
      if (k == (nrx-1)) { //last byte?
        *rxbuf++ = bbI2C_read_nack();
      } else {
        *rxbuf++ = bbI2C_read_ack();
      }
    }
    bbI2C_stop();
  }

  return(OK);
}
  
  

//ignore arb lost
void arbitration_lost(void) {
  return;
}
