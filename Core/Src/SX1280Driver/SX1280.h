#pragma once

//#include "targets.h"
#include "SX1280_Regs.h"
#include "SX1280_hal.h"
#include "stm32g0xx_hal.h"

class Transmission;

class SX1280Driver
{
public:
    static SX1280Driver *instance;

    ///////Callback Function Pointers/////
    void (*RXdoneCallback)(); //function pointer for callback
    void (*TXdoneCallback)(); //function pointer for callback

    ///////////Radio Variables////////
    #define TXRXBuffSize 16
    volatile uint8_t TXdataBuffer[TXRXBuffSize];
    volatile uint8_t RXdataBuffer[TXRXBuffSize];

    uint16_t timeout = 0xFFFF;

    uint32_t currFreq;
    uint8_t PayloadLength;
    bool IQinverted;
    ///////////////////////////////////

    /////////////Packet Stats//////////
    int8_t LastPacketRSSI;
    int8_t LastPacketSNR;

    ////////////////Configuration Functions/////////////
    SX1280Driver();
    bool Begin();
    void End();
    void SetIdleMode() { SetMode(SX1280_MODE_FS); }; // set Idle mode used when switching from RX to TX
    void Config(uint8_t bw, uint8_t sf, uint8_t cr, uint32_t freq,
                uint8_t PreambleLength, bool InvertIQ, uint8_t PayloadLength, uint32_t interval,
                uint32_t flrcSyncWord=0, uint16_t flrcCrcSeed=0, uint8_t flrc=0);
    void SetFrequencyHz(uint32_t freq);
    void SetFrequencyReg(uint32_t freq);
    void SetRxTimeoutUs(uint32_t interval);
    void SetOutputPower(int8_t power);
    void SetOutputPowerMax() { SetOutputPower(13); };

    int32_t GetFrequencyError();

    void TXnb();
    void RXnb();

    uint16_t GetIrqStatus();
    void ClearIrqStatus(uint16_t irqMask);

    void GetStatus();

    bool GetFrequencyErrorbool();
    uint8_t GetRxBufferAddr();
    int8_t GetRssiInst();
    void GetLastPacketStats();

    static void IsrCallback();

private:
    SX1280_RadioOperatingModes_t currOpmode = SX1280_MODE_SLEEP;

    void SetMode(SX1280_RadioOperatingModes_t OPmode);
    void SetFIFOaddr(uint8_t txBaseAddr, uint8_t rxBaseAddr);

    // LoRa functions
    void ConfigModParamsLoRa(uint8_t bw, uint8_t sf, uint8_t cr);
    void SetPacketParamsLoRa(uint8_t PreambleLength, SX1280_RadioLoRaPacketLengthsModes_t HeaderType,
                             uint8_t PayloadLength, SX1280_RadioLoRaCrcModes_t crc,
                             uint8_t InvertIQ);
    // FLRC functions
    void ConfigModParamsFLRC(uint8_t bw, uint8_t cr, uint8_t bt=SX1280_FLRC_BT_0_5);
    void SetPacketParamsFLRC(uint8_t HeaderType,
                             uint8_t crc,
                             uint8_t PreambleLength,
                             uint8_t PayloadLength,
                             uint32_t syncWord,
                             uint16_t crcSeed);

    void SetDioIrqParams(uint16_t irqMask,
                         uint16_t dio1Mask=SX1280_IRQ_RADIO_NONE,
                         uint16_t dio2Mask=SX1280_IRQ_RADIO_NONE,
                         uint16_t dio3Mask=SX1280_IRQ_RADIO_NONE);



    void RXnbISR(); // ISR for non-blocking RX routine
    void TXnbISR(); // ISR for non-blocking TX routine
};
