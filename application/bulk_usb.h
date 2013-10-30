/**
 * @file    bulk_usb.h
 * @brief   Bytes over USB Driver macros and structures.
 *
 * 
 * @{
 */

#ifndef _BULK_USB_H_
#define _BULK_USB_H_

#if HAL_USE_SERIAL_USB || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    BULK_USB configuration options
 * @{
 */
/**
 * @brief   Bulk bytes over USB buffers size.
 * @details Configuration parameter, the buffer size must be a multiple of
 *          the USB data endpoint maximum packet size.
 * @note    The default is 256 bytes for both the transmission and receive
 *          buffers.
 */
#if !defined(BULK_USB_BUFFERS_SIZE) || defined(__DOXYGEN__)
#define BULK_USB_BUFFERS_SIZE     2560
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if !HAL_USE_USB || !CH_USE_QUEUES || !CH_USE_EVENTS
#error "Bulk bytes over USB Driver requires HAL_USE_USB, CH_USE_QUEUES, "
       "CH_USE_EVENTS"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief Driver state machine possible states.
 */
typedef enum {
  BDU_UNINIT = 0,                   /**< Not initialized.                   */
  BDU_STOP = 1,                     /**< Stopped.                           */
  BDU_READY = 2                     /**< Ready.                             */
} bdustate_t;

/**
 * @brief   Structure representing a bulk bytes over USB driver.
 */
typedef struct BulkUSBDriver BulkUSBDriver;

/**
 * @brief   Bulk over USB Driver configuration structure.
 * @details An instance of this structure must be passed to @p bduStart()
 *          in order to configure and start the driver operations.
 */
typedef struct {
  /**
   * @brief   USB driver to use.
   */
  USBDriver                 *usbp;
} BulkUSBConfig;

/**
 * @brief   @p BulkDriver specific data.
 */
#define _bulk_usb_driver_data                                             \
  _base_asynchronous_channel_data                                           \
  /* Driver state.*/                                                        \
  bdustate_t                state;                                          \
  /* Input queue.*/                                                         \
  InputQueue                iqueue;                                         \
  /* Output queue.*/                                                        \
  OutputQueue               oqueue;                                         \
  /* Input buffer.*/                                                        \
  uint8_t                   ib[BULK_USB_BUFFERS_SIZE];                    \
  /* Output buffer.*/                                                       \
  uint8_t                   ob[BULK_USB_BUFFERS_SIZE];                    \
  /* End of the mandatory fields.*/                                         \
  /* Current configuration data.*/                                          \
  const BulkUSBConfig     *config;

/**
 * @brief   @p BulkUSBDriver specific methods.
 */
#define _bulk_usb_driver_methods                                          \
  _base_asynchronous_channel_methods

/**
 * @extends BaseAsynchronousChannelVMT
 *
 * @brief   @p BulkDriver virtual methods table.
 */
struct BulkUSBDriverVMT {
  _bulk_usb_driver_methods
};

/**
 * @extends BaseAsynchronousChannel
 *
 * @brief   Full duplex bulk driver class.
 * @details This class extends @p BaseAsynchronousChannel by adding physical
 *          I/O queues.
 */
struct BulkUSBDriver {
  /** @brief Virtual Methods Table.*/
  const struct BulkUSBDriverVMT *vmt;
  _bulk_usb_driver_data
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void bduInit(void);
  void bduObjectInit(BulkUSBDriver *sdp);
  void bduStart(BulkUSBDriver *bdup, const BulkUSBConfig *config);
  void bduStop(BulkUSBDriver *bdup);
  void bduConfigureHookI(USBDriver *usbp);
  bool_t bduRequestsHook(USBDriver *usbp);
  void bduDataTransmitted(USBDriver *usbp, usbep_t ep);
  void bduDataReceived(USBDriver *usbp, usbep_t ep);
  void bduInterruptTransmitted(USBDriver *usbp, usbep_t ep);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_BULK_USB */

#endif /* _BULK_USB_H_ */

/** @} */
