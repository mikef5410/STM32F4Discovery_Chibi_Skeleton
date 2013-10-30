/**
 * @file    bulk_usb.c
 * @brief   Bulk bytes over USB Driver code.
 *
 * @addtogroup SERIAL_USB
 * @{
 */

#include "ch.h"
#include "hal.h"

// Our control I/O is a bulk stream on endpoint 3
#define USB_CTRL_EP 0x03
#define USB_BULK_IN_EP USB_CTRL_EP
#define USB_BULK_OUT_EP USB_CTRL_EP

#include "bulk_usb.h"
extern BulkUSBDriver BDU1;

#if HAL_USE_SERIAL_USB || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * Interface implementation.
 */

static size_t write(void *ip, const uint8_t *bp, size_t n) {

  return chOQWriteTimeout(&((BulkUSBDriver *)ip)->oqueue, bp,
                          n, TIME_INFINITE);
}

static size_t read(void *ip, uint8_t *bp, size_t n) {

  return chIQReadTimeout(&((BulkUSBDriver *)ip)->iqueue, bp,
                         n, TIME_INFINITE);
}

static msg_t put(void *ip, uint8_t b) {

  return chOQPutTimeout(&((BulkUSBDriver *)ip)->oqueue, b, TIME_INFINITE);
}

static msg_t get(void *ip) {

  return chIQGetTimeout(&((BulkUSBDriver *)ip)->iqueue, TIME_INFINITE);
}

static msg_t putt(void *ip, uint8_t b, systime_t timeout) {

  return chOQPutTimeout(&((BulkUSBDriver *)ip)->oqueue, b, timeout);
}

static msg_t gett(void *ip, systime_t timeout) {

  return chIQGetTimeout(&((BulkUSBDriver *)ip)->iqueue, timeout);
}

static size_t writet(void *ip, const uint8_t *bp, size_t n, systime_t time) {

  return chOQWriteTimeout(&((BulkUSBDriver *)ip)->oqueue, bp, n, time);
}

static size_t readt(void *ip, uint8_t *bp, size_t n, systime_t time) {

  return chIQReadTimeout(&((BulkUSBDriver *)ip)->iqueue, bp, n, time);
}

static const struct BulkUSBDriverVMT vmt = {
  write, read, put, get,
  putt, gett, writet, readt
};

/**
 * @brief   Notification of data removed from the input queue.
 */
static void inotify(GenericQueue *qp) {
  size_t n, maxsize;
  BulkUSBDriver *bdup = chQGetLink(qp);

  /* If the USB driver is not in the appropriate state then transactions
     must not be started.*/
  if (usbGetDriverStateI(bdup->config->usbp) != USB_ACTIVE)
    return;

  /* If there is in the queue enough space to hold at least one packet and
     a transaction is not yet started then a new transaction is started for
     the available space.*/
  maxsize = bdup->config->usbp->epc[USB_BULK_OUT_EP]->out_maxsize;
  if (!usbGetReceiveStatusI(bdup->config->usbp, USB_BULK_OUT_EP) &&
      ((n = chIQGetEmptyI(&bdup->iqueue)) >= maxsize)) {
    chSysUnlock();

    n = (n / maxsize) * maxsize;
    usbPrepareQueuedReceive(bdup->config->usbp,
                            USB_BULK_OUT_EP,
                            &bdup->iqueue, n);

    chSysLock();
    usbStartReceiveI(bdup->config->usbp, USB_BULK_OUT_EP);
  }
}

/**
 * @brief   Notification of data inserted into the output queue.
 */
static void onotify(GenericQueue *qp) {
  size_t n;
  BulkUSBDriver *bdup = chQGetLink(qp);

  /* If the USB driver is not in the appropriate state then transactions
     must not be started.*/
  if (usbGetDriverStateI(bdup->config->usbp) != USB_ACTIVE)
    return;

  /* If there is not an ongoing transaction and the output queue contains
     data then a new transaction is started.*/
  if (!usbGetTransmitStatusI(bdup->config->usbp, USB_BULK_IN_EP) &&
      ((n = chOQGetFullI(&bdup->oqueue)) > 0)) {
    chSysUnlock();

    usbPrepareQueuedTransmit(bdup->config->usbp,
                             USB_BULK_IN_EP,
                             &bdup->oqueue, n);

    chSysLock();
    usbStartTransmitI(bdup->config->usbp, USB_BULK_IN_EP);
  }
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Bulk Driver initialization.
 * @note    This function is implicitly invoked by @p halInit(), there is
 *          no need to explicitly initialize the driver.
 *
 * @init
 */
void bduInit(void) {
}

/**
 * @brief   Initializes a generic full duplex driver object.
 * @details The HW dependent part of the initialization has to be performed
 *          outside, usually in the hardware initialization code.
 *
 * @param[out] bdup     pointer to a @p BulkUSBDriver structure
 *
 * @init
 */
void bduObjectInit(BulkUSBDriver *bdup) {

  bdup->vmt = &vmt;
  chEvtInit(&bdup->event);
  bdup->state = BDU_STOP;
  chIQInit(&bdup->iqueue, bdup->ib, BULK_USB_BUFFERS_SIZE, inotify, bdup);
  chOQInit(&bdup->oqueue, bdup->ob, BULK_USB_BUFFERS_SIZE, onotify, bdup);
}

/**
 * @brief   Configures and starts the driver.
 *
 * @param[in] bdup      pointer to a @p BulkUSBDriver object
 * @param[in] config    the bulk bytes over USB driver configuration
 *
 * @api
 */
void bduStart(BulkUSBDriver *bdup, const BulkUSBConfig *config) {

  chDbgCheck(bdup != NULL, "bduStart");

  chSysLock();
  chDbgAssert((bdup->state == BDU_STOP) || (bdup->state == BDU_READY),
              "bduStart(), #1",
              "invalid state");
  bdup->config = config;
  config->usbp->param = bdup;
  bdup->state = BDU_READY;
  chSysUnlock();
}

/**
 * @brief   Stops the driver.
 * @details Any thread waiting on the driver's queues will be awakened with
 *          the message @p Q_RESET.
 *
 * @param[in] bdup      pointer to a @p BulkUSBDriver object
 *
 * @api
 */
void bduStop(BulkUSBDriver *bdup) {

  chDbgCheck(bdup != NULL, "sdStop");

  chSysLock();
  chDbgAssert((bdup->state == BDU_STOP) || (bdup->state == BDU_READY),
              "bduStop(), #1",
              "invalid state");
  bdup->state = BDU_STOP;
  chSysUnlock();
}

/**
 * @brief   USB device configured handler.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @iclass
 */

//  ******* W A R N I N G   W A R N I N G   W A R N I N G *******
// There's an architectural problem here. We have a layered stack of drivers ...
// BulkUSBDriver sits on top of USBDriver. We have the same instance of USBDriver for 
// our CDC-ACM  channel, but both upper layer drivers want to overwrite the usb driver's
// user defined pointer (param) to point up to themselves. These routines then use this
// param to get pointers to themselves. I've hacked around this with a hardcoded
// fixup of bdup.  This is bad, but it gets around  the immediate problem.

void bduConfigureHookI(USBDriver *usbp) {
//  BulkUSBDriver *bdup = usbp->param;
  BulkUSBDriver *bdup = &BDU1;

  chIQResetI(&bdup->iqueue);
  chOQResetI(&bdup->oqueue);
  chnAddFlagsI(bdup, CHN_CONNECTED);

  /* Starts the first OUT transaction immediately.*/
  usbPrepareQueuedReceive(usbp, USB_BULK_OUT_EP, &bdup->iqueue,
                          usbp->epc[USB_BULK_OUT_EP]->out_maxsize);
  usbStartReceiveI(usbp, USB_BULK_OUT_EP);
}

/**
 * @brief   Default requests hook.
 * @details Applications wanting to use the Bulk bytes over USB driver can use
 *          this function as requests hook in the USB configuration.
 *          We don't implement any requests, but this is how to do it
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @return              The hook status.
 * @retval TRUE         Message handled internally.
 * @retval FALSE        Message not handled.
 */
bool_t bduRequestsHook(USBDriver *usbp) {

  if ((usbp->setup[0] & USB_RTYPE_TYPE_MASK) == USB_RTYPE_TYPE_CLASS) {
    switch (usbp->setup[1]) {
#if 0
    case CDC_GET_LINE_CODING:
      usbSetupTransfer(usbp, (uint8_t *)&linecoding, sizeof(linecoding), NULL);
      return TRUE;
    case CDC_SET_LINE_CODING:
      usbSetupTransfer(usbp, (uint8_t *)&linecoding, sizeof(linecoding), NULL);
      return TRUE;
    case CDC_SET_CONTROL_LINE_STATE:
      /* Nothing to do, there are no control lines.*/
      usbSetupTransfer(usbp, NULL, 0, NULL);
      return TRUE;
#endif
    default:
      return FALSE;
    }
  }
  return FALSE;
}

/**
 * @brief   Default data transmitted callback.
 * @details The application must use this function as callback for the IN
 *          data endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 */
void bduDataTransmitted(USBDriver *usbp, usbep_t ep) {
  size_t n;

  BulkUSBDriver *bdup = &BDU1;

  (void)ep;

  chSysLockFromIsr();
  chnAddFlagsI(bdup, CHN_OUTPUT_EMPTY);

  if ((n = chOQGetFullI(&bdup->oqueue)) > 0) {
    /* The endpoint cannot be busy, we are in the context of the callback,
       so it is safe to transmit without a check.*/
    chSysUnlockFromIsr();

    usbPrepareQueuedTransmit(usbp, ep, &bdup->oqueue, n);

    chSysLockFromIsr();
    usbStartTransmitI(usbp, ep);
  }
  else if (!(usbp->epc[ep]->in_state->txsize &
            (usbp->epc[ep]->in_maxsize - 1))) {
    /* Transmit zero sized packet in case the last one has maximum allowed
       size. Otherwise the recipient may expect more data coming soon and
       not return buffered data to app. See section 5.8.3 Bulk Transfer
       Packet Size Constraints of the USB Specification document.*/
    chSysUnlockFromIsr();

    usbPrepareQueuedTransmit(usbp, ep, &bdup->oqueue, 0);

    chSysLockFromIsr();
    usbStartTransmitI(usbp, ep);
  }

  chSysUnlockFromIsr();
}

/**
 * @brief   Default data received callback.
 * @details The application must use this function as callback for the OUT
 *          data endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 */
void bduDataReceived(USBDriver *usbp, usbep_t ep) {
  size_t n, maxsize;

  BulkUSBDriver *bdup = &BDU1;

  
  (void)ep;
  chSysLockFromIsr();
  chnAddFlagsI(bdup, CHN_INPUT_AVAILABLE);

  /* Writes to the input queue can only happen when there is enough space
     to hold at least one packet.*/
  maxsize = usbp->epc[USB_BULK_OUT_EP]->out_maxsize;
  if ((n = chIQGetEmptyI(&bdup->iqueue)) >= maxsize) {
    /* The endpoint cannot be busy, we are in the context of the callback,
       so a packet is in the buffer for sure.*/
    chSysUnlockFromIsr();

    n = (n / maxsize) * maxsize;
    usbPrepareQueuedReceive(usbp, ep, &bdup->iqueue, n);

    chSysLockFromIsr();
    usbStartReceiveI(usbp, ep);
  }
  chSysUnlockFromIsr();

}


#endif /* HAL_USE_SERIAL */

/** @} */
