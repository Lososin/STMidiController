
#include "midi_router.h"
#include "usb_device.h"
#include "usbd_midi.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

static uint8_t buffUsbReport[MIDI_EPIN_SIZE] = {0};
static uint8_t buffUsbReportNextIndex = 0;

static uint8_t buffUsb[MIDI_BUFFER_LENGTH] = {0};
volatile static uint8_t buffUsbNextIndex = 0;
static uint8_t buffUsbCurrIndex = 0;


void USBD_MIDI_DataInHandler(uint8_t *usb_rx_buffer, uint8_t usb_rx_buffer_length)
{
  while (usb_rx_buffer_length && *usb_rx_buffer != 0x00)
  {
    buffUsb[buffUsbNextIndex++] = *usb_rx_buffer++;
    buffUsb[buffUsbNextIndex++] = *usb_rx_buffer++;
    buffUsb[buffUsbNextIndex++] = *usb_rx_buffer++;
    buffUsb[buffUsbNextIndex++] = *usb_rx_buffer++;

    usb_rx_buffer_length -= 4;
  }
}

bool MIDI_HasUSBData(void)
{
  return buffUsbCurrIndex != buffUsbNextIndex;
}

void MIDI_ProcessUSBData(void)
{

}

void MIDI_addToUSBReport(uint8_t cable, uint8_t message, uint8_t param1, uint8_t param2)
{
  buffUsbReport[buffUsbReportNextIndex++] = (cable << 4) | (message >> 4);
  buffUsbReport[buffUsbReportNextIndex++] = (message);
  buffUsbReport[buffUsbReportNextIndex++] = (param1);
  buffUsbReport[buffUsbReportNextIndex++] = (param2);

  if (buffUsbReportNextIndex == MIDI_EPIN_SIZE)
  {
    while (USBD_MIDI_GetState(&hUsbDeviceFS) != MIDI_IDLE) {};
    USBD_MIDI_SendReport(&hUsbDeviceFS, buffUsbReport, MIDI_EPIN_SIZE);
    buffUsbReportNextIndex = 0;
  }
}

void MIDI_addToUSBReportMess(uint32_t Message1)
{
  buffUsbReport[buffUsbReportNextIndex++] = (Message1 >> 24) & 0xFF;
  buffUsbReport[buffUsbReportNextIndex++] = (Message1 >> 16) & 0xFF;
  buffUsbReport[buffUsbReportNextIndex++] = (Message1 >> 8) & 0xFF;
  buffUsbReport[buffUsbReportNextIndex++] = (Message1 >> 0) & 0xFF;

  //if (buffUsbReportNextIndex == MIDI_EPIN_SIZE)
  {
    while (USBD_MIDI_GetState(&hUsbDeviceFS) != MIDI_IDLE) {};
    USBD_MIDI_SendReport(&hUsbDeviceFS, buffUsbReport, MIDI_EPIN_SIZE);
    buffUsbReportNextIndex = 0;
  }
}

void MIDI_ProcessUARTData(void)
{
}
