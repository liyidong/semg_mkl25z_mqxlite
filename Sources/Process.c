/* ###################################################################
 **     THIS COMPONENT MODULE IS GENERATED BY USER
 **     Filename    : Process.h
 **     Project     : semg_mkl15z128vlh4
 **     Processor   : MKL15Z128VLH4
 **     Component   :
 **     Version     :
 **     Datasheet   :
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2013-Aug 5, 2013, 8:54:36 PM, # CodeGen: 1
 **     Author      : Dong
 **     Abstract    :
 **         This file contains functions for kinds of data processing.
 **     Settings    :
 **
 **     Contents    :
 **                 MainLoop    - void MainLoop(void)
 **
 **     Mail        : pzdongdong@163.com
 **
 **     Revision    : No.  Name        Data/Time        Content
 ** ###################################################################*/
/*!
 * @file Process.h
 * @version 01.00
 * @brief
 *      This file contains functions for kinds of data processing.
 */
/*!
 *  @addtogroup Proc Process module documentation
 *      This module contains functions for kinds of data processing.
 *  @{
 */

/* Module Process */

/* {Default RTOS Adapter} No RTOS includes */
#include "PE_Error.h"
#include "PE_Types.h"

#include "MyHeaders.h"
#include "string.h"

static void Process(void);
//static void TransmitMCUData(void);
static LDD_TError SplitRawData(void);
static void CopyADCDataToMCUData(void);
static void PackData(void);
//static void SwapARMDataBuffer(void);
static void ReadADCData(void);

/*
 * ===================================================================
 * Global Variables
 * ===================================================================
 */
static int chDataCnt = 0;
static uint8 overflowCnt = 0;

/*
 * ===================================================================
 *     Method      :  MainLoop ()
 */
/*!
 *     @brief
            This methods starts main loop, ADC's conversion and so on.
 *     @param
 *          void
 *     @return
 *          void
 */
/* ===================================================================*/
void MainLoop(void)
{
    extern TMCUPtr tMCUPtr;
    extern volatile bool isStart;

    tMCUPtr->mcuStatus.isSPI0RxDMATransCompleted = TRUE;
    tMCUPtr->mcuStatus.isSPI0TxDMATransCompleted = TRUE;
    tMCUPtr->mcuStatus.isSPI1RxDMATransCompleted = TRUE;
    tMCUPtr->mcuStatus.isSPI1TxDMATransCompleted = TRUE;

//    SPI0_BR = (SPI_BR_SPPR(0x02) | SPI_BR_SPR(0x02));   /* Set baud rage register, 1M */
    SPI0_BR = (SPI_BR_SPPR(0x01) | SPI_BR_SPR(0x00)); /* Set baud rate register,6M */
//    SPI0_BR = (SPI_BR_SPPR(0x00) | SPI_BR_SPR(0x00)); /* Set baud rate register,12M */

    /* Enable PortA's interrupt. */
    EIntADNotReady0Enable(EINT_AD_NOT_DRDY0);
//    EIntADNotReady1Enable(EINT_AD_NOT_DRDY1);
    EIntSyncInterruptEnable(EINT_SYNC_INT);
    NVIC_ISER |= NVIC_ISER_SETENA(0x40000000);      /* Enable PortA's hardware interrupt */

//    StartTaskTimer();

    //ADCStartConvertByCommand(eADC0);
    //ADCStartConvertByCommand(eADC1);
    /* Start ADC's conversion at the same time by Pull the start pin high. */
//    tARMPtr->backBuffer = tARMPtr->armDataLeft.dataFrame;
//    GPIOB_PDOR |= 0x06U;    /* B1, B2 */
    while(!isStart);

    for(;;)
    {
//        SwapARMDataBuffer();    /* Swap the data buffer if needed. */

        /* If data of ADC is ready, read it. */
        ReadADCData();

        /* Processing the Data. */
        Process();
        //Process(eADC1);

        /*  If the ARM requires data, transmit. */
        //TransmitMCUData();
    }
}

/*
 * ===================================================================
 *     Method      : ReadADCData(Module Process)
 */
/*!
 *     @brief
 *          This method reads ADC's data when ADC's is ready to be read.
 *          This method is called in function
 *          EINT_AD_NOT_DRDY0_OnInterrupt or EINT_AD_NOT_DRDY0_OnInterrupt,
 *          file Events.c
 *     @param
 *          void
 *     @return
 *          void
 */
/* ===================================================================*/
static void ReadADCData(void)
{
    extern TADCPtr tADCPtr[USING_ADC_COUNT];
    //EADCFlag adcFlag;

    if(tADCPtr[eADC0]->adcStatus.isDataReady)// && tADCPtr[eADC1]->adcStatus.isDataReady)
    {
        EnableADCSPI(eADC0);
        ADCReadContinuousData(tADCPtr[eADC0]->adcData.rawData, RAW_DATA_SIZE);
        tMCUPtr->mcuStatus.isReceivingADCData = TRUE;
//        tMCUPtr->mcuStatus.isSPI0RxDMATransCompleted = FALSE;
//        tMCUPtr->mcuStatus.isSPI0TxDMATransCompleted = FALSE;
        tADCPtr[eADC0]->adcStatus.adcDataStatus = eReceiving;
        tADCPtr[eADC0]->adcStatus.transmitionContent = eData;

//        while(!tMCUPtr->mcuStatus.isSPI0RxDMATransCompleted || !tMCUPtr->mcuStatus.isSPI0TxDMATransCompleted);
        DisableADCSPI(eADC0);

        tMCUPtr->mcuStatus.isReceivingADCData = FALSE;
        tADCPtr[eADC0]->adcStatus.isDataReady = FALSE;
        tADCPtr[eADC0]->adcStatus.adcDataStatus = eReceived;
        tADCPtr[eADC0]->adcStatus.transmitionContent = eNothing;

        //Process(eADC0);

        EnableADCSPI(eADC1);
        ADCReadContinuousData(tADCPtr[eADC1]->adcData.rawData, RAW_DATA_SIZE);
        tMCUPtr->mcuStatus.isReceivingADCData = TRUE;
//        tMCUPtr->mcuStatus.isSPI0RxDMATransCompleted = FALSE;
//        tMCUPtr->mcuStatus.isSPI0TxDMATransCompleted = FALSE;
        tADCPtr[eADC1]->adcStatus.adcDataStatus = eReceiving;
        tADCPtr[eADC1]->adcStatus.transmitionContent = eData;

//        while(!tMCUPtr->mcuStatus.isSPI0RxDMATransCompleted || !tMCUPtr->mcuStatus.isSPI0TxDMATransCompleted);
        DisableADCSPI(eADC1);

        tMCUPtr->mcuStatus.isReceivingADCData = FALSE;
        tADCPtr[eADC1]->adcStatus.isDataReady = FALSE;
        tADCPtr[eADC1]->adcStatus.adcDataStatus = eReceived;
        tADCPtr[eADC1]->adcStatus.transmitionContent = eNothing;

        //Process(eADC1);
    }

//    while(tADCPtr[eADC0]->adcStatus.isDataReady || tADCPtr[eADC1]->adcStatus.isDataReady)
//    {
//        adcFlag = (TRUE == tADCPtr[eADC0]->adcStatus.isDataReady) ? eADC0 : eADC1;
//        EnableADCSPI(adcFlag);
//        ADCReadContinuousData(tADCPtr[adcFlag]->adcData.rawData, RAW_DATA_SIZE);
//        tMCUPtr->mcuStatus.isReceivingADCData = TRUE;
//        tMCUPtr->mcuStatus.isSPI0RxDMATransCompleted = FALSE;
//        tMCUPtr->mcuStatus.isSPI0TxDMATransCompleted = FALSE;
//        tADCPtr[adcFlag]->adcStatus.adcDataStatus = eReceiving;
//        tADCPtr[adcFlag]->adcStatus.transmitionContent = eData;
//
//        while(!tMCUPtr->mcuStatus.isSPI0RxDMATransCompleted);
//        DisableADCSPI(adcFlag);
//
//        tMCUPtr->mcuStatus.isReceivingADCData = FALSE;
//        tADCPtr[adcFlag]->adcStatus.isDataReady = FALSE;
//        tADCPtr[adcFlag]->adcStatus.adcDataStatus = eReceived;
//        tADCPtr[adcFlag]->adcStatus.transmitionContent = eNothing;
//
//        Process(adcFlag);
//    }
}


/*
 * ===================================================================
 *     Method      : Process(Module Process)
 */
/*!
 *     @brief
 *         	The primary routine of processing the data.
 *     @param
 *          void
 *     @return
 *          void
 */
/* ===================================================================*/
static void Process(void)
{
    extern TADCPtr tADCPtr[USING_ADC_COUNT];

    if(eReceived == tADCPtr[eADC0]->adcStatus.adcDataStatus
            && eReceived == tADCPtr[eADC0]->adcStatus.adcDataStatus)
    {
        SplitRawData();
        tADCPtr[eADC0]->adcStatus.adcDataStatus = eIdle;
        tADCPtr[eADC1]->adcStatus.adcDataStatus = eIdle;

        CopyADCDataToMCUData();
        PackData();
    }
}

/*
 * ===================================================================
 *     Method      : TransmitADCData(Module Process)
 */
/*!
 *     @brief
 *          This method transmit MCU's data to ARM when is required.
 *     @param
 *          void
 *     @return
 *          void
 */
/* ===================================================================*/
void TransmitMCUData(void)
{
    extern TMCUPtr tMCUPtr;
    extern TARMPtr tARMPtr;

    //if(//tARMPtr->armStatus.isRequiringData &&
            //tARMPtr->armStatus.isUploadReady &&
                //tMCUPtr->mcuStatus.isSPI1TxDMATransCompleted)
    //{
        tARMPtr->armStatus.isRequiringData = FALSE;
        tMCUPtr->mcuStatus.isSPI1TxDMATransCompleted = FALSE;
        IOUploadReadySetVal();
        SPI1SendData((LDD_DMA_TAddress)tARMPtr->foreBuffer, DATA_FRAME_LENGTH + 1);
        IOUploadReadyClrVal();
        //tARMPtr->armStatus.isTransmittingData = TRUE;
        tARMPtr->armStatus.transmitionContent = eData;
//        tARMPtr->armStatus.isForeBufferEmpty = FALSE;
//        tARMPtr->armStatus.isForeBufferFull = FALSE;
        tARMPtr->armStatus.foreBufferStatus = eRead;
        tARMPtr->armStatus.isUploadReady = FALSE;
    //}
}

/*
 * ===================================================================
 *     Method      : SplitRawData(Module Process)
 */
/*!
 *     @brief
 *         	This method splits the ADC raw data to each part of the ADC data
 *         	structure.
 *     @param
 *         	void
 *     @return
 *         	                - ERR_PARAM_DATA: The head byte of the raw is not valid,
 *         	                                  so the data is invalid.
 *         	                - ERR_OK: Succeeded to split.
 */
/* ===================================================================*/
static LDD_TError SplitRawData(void)
{
    extern TADCPtr tADCPtr[USING_ADC_COUNT];

    LDD_TError err;
    byte head;
    byte loffStatP;
    byte loffStatN;
    byte regGPIOData;
    int16 channelData;

    for(int adcNum = 0; adcNum < USING_ADC_COUNT; adcNum++)
    {
        head = ((tADCPtr[adcNum]->adcData.rawData[0] & 0xF0) >> 4) & 0x0F;       /* Split head byte from raw data. */
        if(head != RAW_DATA_HEAD)                                   /* If the head byte is not right(0x0C), return with error. */
        {
            err = ERR_PARAM_DATA;
    #if DEBUG
            PrintErrorMessage(err);
            printf("%#x\n", head);
    #endif
            return err;
        }

        loffStatP  = ((byte)(tADCPtr[adcNum]->adcData.rawData[0] & 0x0F) << 4) & 0xF0;
        loffStatP |= ((byte)(tADCPtr[adcNum]->adcData.rawData[1] & 0xF0) >> 4) & 0x0F;
        loffStatN  = ((byte)(tADCPtr[adcNum]->adcData.rawData[1] & 0x0F) << 4) & 0xF0;
        loffStatN |= ((byte)(tADCPtr[adcNum]->adcData.rawData[2] & 0xF0) >> 4) & 0x0F;

        regGPIOData =  (byte)tADCPtr[adcNum]->adcData.rawData[2] & 0x0F;

        for(int i = 0; i < USING_CHANNEL_COUNT * BYTE_COUNT_PER_CHANNEL; i += 2)    /* Every channel's data is 2 Bytes. */
        {
            channelData = ((int16)tADCPtr[adcNum]->adcData.rawData[RAW_DATA_HEAD_SIZE + i] << 8) & 0xFF00;
            channelData |= (int16)tADCPtr[adcNum]->adcData.rawData[RAW_DATA_HEAD_SIZE + i + 1] & 0x00FF;
            if(eADC0 == adcNum)
            {
                //According to the hardware, the data from the 1st ADC should be reversed.
                tADCPtr[adcNum]->adcData.channelData[USING_CHANNEL_COUNT - (i / 2) - 1] = (channelData == 0x8000) ? 0x7FFF : (int16)(-channelData);
            }
            else if(eADC1 == adcNum)
            {
                tADCPtr[adcNum]->adcData.channelData[i / 2] = (int16)channelData;
            }
        }

        tADCPtr[adcNum]->adcData.head = (byte)head;
        tADCPtr[adcNum]->adcData.loffStatP = (byte)loffStatP;
        tADCPtr[adcNum]->adcData.loffStatN = (byte)loffStatN;
        tADCPtr[adcNum]->adcData.regGPIOData = (byte)regGPIOData;
    }

    return ERR_OK;
}

/*
 * ===================================================================
 *     Method      : CopyADCDataToMCUData(Module Process)
 */
/*!
 *     @brief
 *          This method copies ADC data from ADC structure to MCU structure.
 *          The format of data in MCU structure
       @verbatim
       --------------------------------------
       | ADC | (Chn) | 1ms 2ms 3ms ... 99ms |
       --------------------------------------
       |  0  | (Ch1) | [0] [1] [2] ... [99] |
       |  0  | (Ch2) | [0] [1] [2] ... [99] |
       |  0  | (Ch3) | [0] [1] [2] ... [99] |
       |  0  | (Ch4) | [0] [1] [2] ... [99] |
       |  0  | (Ch5) | [0] [1] [2] ... [99] |
       |  0  | (Ch6) | [0] [1] [2] ... [99] |
       |  0  | (Ch7) | [0] [1] [2] ... [99] |
       |  0  | (Ch8) | [0] [1] [2] ... [99] |
       ======================================
       |  1  | (Ch1) | [0] [1] [2] ... [99] |
       |  1  | (Ch2) | [0] [1] [2] ... [99] |
       |  1  | (Ch3) | [0] [1] [2] ... [99] |
       |  1  | (Ch4) | [0] [1] [2] ... [99] |
       |  1  | (Ch5) | [0] [1] [2] ... [99] |
       |  1  | (Ch6) | [0] [1] [2] ... [99] |
       |  1  | (Ch7) | [0] [1] [2] ... [99] |
       |  1  | (Ch8) | [0] [1] [2] ... [99] |
       --------------------------------------
       @endverbatim
 *     @param
 *          void
 *     @return
 *          void
 */
/* ===================================================================*/
static void CopyADCDataToMCUData(void)
{
    extern TMCUPtr tMCUPtr;
//    static uint8 chDataCnt[USING_ADC_COUNT] = {0};

    tMCUPtr->mcuData.channelData[0][0][chDataCnt] = tADCPtr[0]->adcData.channelData[0];
    tMCUPtr->mcuData.channelData[0][1][chDataCnt] = tADCPtr[0]->adcData.channelData[1];
    tMCUPtr->mcuData.channelData[0][2][chDataCnt] = tADCPtr[0]->adcData.channelData[2];
    tMCUPtr->mcuData.channelData[0][3][chDataCnt] = tADCPtr[0]->adcData.channelData[3];
    tMCUPtr->mcuData.channelData[0][4][chDataCnt] = tADCPtr[0]->adcData.channelData[4];
    tMCUPtr->mcuData.channelData[0][5][chDataCnt] = tADCPtr[0]->adcData.channelData[5];
    tMCUPtr->mcuData.channelData[0][6][chDataCnt] = tADCPtr[0]->adcData.channelData[6];
    tMCUPtr->mcuData.channelData[0][7][chDataCnt] = tADCPtr[0]->adcData.channelData[7];

    tMCUPtr->mcuData.channelData[1][0][chDataCnt] = tADCPtr[1]->adcData.channelData[0];
    tMCUPtr->mcuData.channelData[1][1][chDataCnt] = tADCPtr[1]->adcData.channelData[1];
    tMCUPtr->mcuData.channelData[1][2][chDataCnt] = tADCPtr[1]->adcData.channelData[2];
    tMCUPtr->mcuData.channelData[1][3][chDataCnt] = tADCPtr[1]->adcData.channelData[3];
    tMCUPtr->mcuData.channelData[1][4][chDataCnt] = tADCPtr[1]->adcData.channelData[4];
    tMCUPtr->mcuData.channelData[1][5][chDataCnt] = tADCPtr[1]->adcData.channelData[5];
    tMCUPtr->mcuData.channelData[1][6][chDataCnt] = tADCPtr[1]->adcData.channelData[6];
    tMCUPtr->mcuData.channelData[1][7][chDataCnt] = tADCPtr[1]->adcData.channelData[7];


//    chDataCnt[adcFlag]++;
//    chDataCnt[adcFlag] %= CHANNEL_DATA_COUNT;
}

/*
 * ===================================================================
 *     Method      : PackData (Module Process)
 */
/*!
 *     @brief
 *         	This method is called to pack filtered data adapting to the
 *         	format according to the protocol between MCU and ARM.
 *     @param
 *         	void
 *     @return
 *         	void
 */
/* ===================================================================*/
static void PackData(void)
{
    extern TMCUPtr tMCUPtr;
    extern TARMPtr tARMPtr;
//    static int chDataCnt[USING_ADC_COUNT] = {0};
    int off;

    if(eFull == tARMPtr->armStatus.backBufferStatus || eOverflow == tARMPtr->armStatus.backBufferStatus)
    {
        tARMPtr->armStatus.backBufferStatus = eOverflow;
        overflowCnt++;
        return;
    }

    for(int adcNum = 0; adcNum < USING_ADC_COUNT; adcNum++)
    {
        for(int channelNum = 0;  channelNum < USING_CHANNEL_COUNT; channelNum++)
        {
            off = DATA_FRAME_HEAD_SIZE /* Head Bits in a pack, eg. 0xB7, MCU No., etc. */
                    + USING_CHANNEL_COUNT * CHANNEL_PACKAGE_LENGTH * adcNum  /* The ADC0's data is in the front of ADC1's data. */
                        + 3 * (channelNum + 1)  /* Every channel's data pack has a head of 3 bits. */
                            + CHANNEL_DATA_COUNT * 2 * channelNum   /* every channel's data is 200 bits apart of the 3-bit head. */
                                + chDataCnt * 2;   /* every channel's data at one time is 2 bits. */
            tARMPtr->backBuffer[off] = tMCUPtr->mcuData.channelData[adcNum][channelNum][chDataCnt] >> 8 & 0xFFU;
            tARMPtr->backBuffer[off + 1] = tMCUPtr->mcuData.channelData[adcNum][channelNum][chDataCnt] & 0xFFU;
        }
    }

    chDataCnt++;
    chDataCnt %= CHANNEL_DATA_COUNT;
    if(!chDataCnt)
    {
//        tARMPtr->armStatus.isBackBufferEmpty = FALSE;
//        tARMPtr->armStatus.isBackBufferFull = TRUE;
        tARMPtr->armStatus.backBufferStatus = eFull;
    }
    else
    {
//        tARMPtr->armStatus.isBackBufferEmpty = FALSE;
//        tARMPtr->armStatus.isBackBufferFull = FALSE;

        tARMPtr->armStatus.backBufferStatus = eWrite;
    }
}

/*
 * ===================================================================
 *     Method      : SwapARMDataBuffer (Module Process)
 */
/*!
 *     @brief
 *          This method is called to swap the arm's data. Decide either the
 *          armLeftData or armRightData is the foreground buffer or background
 *          buffer.
 *     @param
 *          void
 *     @return
 *          void
 */
/* ===================================================================*/
void SwapARMDataBuffer(void)
{
    if(eNull == tARMPtr->armStatus.foreBufferStatus && eNull == tARMPtr->armStatus.backBufferStatus)
    {
        tARMPtr->foreBuffer = tARMPtr->armDataRight.dataFrame;
        tARMPtr->backBuffer = tARMPtr->armDataLeft.dataFrame;
//        tARMPtr->armStatus.isForeBufferEmpty = FALSE;
//        tARMPtr->armStatus.isForeBufferFull = FALSE;
//        tARMPtr->armStatus.isBackBufferEmpty = TRUE;
//        tARMPtr->armStatus.isForeBufferFull = FALSE;
        tARMPtr->armStatus.foreBufferStatus = eEmpty;
        tARMPtr->armStatus.backBufferStatus = eEmpty;
        tARMPtr->armStatus.isUploadReady = FALSE;

        tARMPtr->foreBuffer[6] = (tARMPtr->foreBuffer[6] & 0x01U)
                               | ((tARMPtr->armStatus.foreBufferStatus << 1) & 0x0EU);

        tARMPtr->foreBuffer[7] = 0x0U;
    }
    else
    {
        //tARMPtr->backBuffer[7] = tARMPtr->armStatus.backBufferStatus;

        byte* temp;
        temp = tARMPtr->foreBuffer;
        tARMPtr->foreBuffer = tARMPtr->backBuffer;
        tARMPtr->backBuffer = temp;
        tARMPtr->armStatus.foreBufferStatus = tARMPtr->armStatus.backBufferStatus;
        tARMPtr->armStatus.backBufferStatus = eEmpty;
        tARMPtr->armStatus.isUploadReady = FALSE;

        tARMPtr->foreBuffer[6] = (tARMPtr->foreBuffer[6] & 0x01U)
                               | ((tARMPtr->armStatus.foreBufferStatus << 1) & 0x0EU);
        switch(tARMPtr->armStatus.foreBufferStatus)
        {
        case eOverflow:
            tARMPtr->foreBuffer[7] = overflowCnt;
            break;
        case eFull:
            tARMPtr->foreBuffer[7] = 0x00U;
            break;
        case eWrite:
        {
            int remainNum;
            remainNum = CHANNEL_DATA_COUNT - chDataCnt;
            tARMPtr->foreBuffer[7] = remainNum;
            //Fill the remaining position as the last value in the array.
            int off;
            for(int i = 0; i < USING_ADC_COUNT * USING_CHANNEL_COUNT; i++)
            {
                for(int j = 0; j < remainNum; j++)
                {
                    off = 210 + 203 * i - (remainNum - j) * 2;
                    tARMPtr->backBuffer[off] = tARMPtr->backBuffer[off - 2];
                    tARMPtr->backBuffer[off + 1] = tARMPtr->backBuffer[off - 1];
                }
            }
//            for(int i = 0; i < CHANNEL_DATA_COUNT; i++)
//            {
//                for(int adcNum = 0; adcNum < USING_ADC_COUNT; adcNum++)
//                {
//                    for(int channelNum = 0;  channelNum < USING_CHANNEL_COUNT; channelNum++)
//                    {
//                        for(int j = 0; j < remainNum; j++)
//                        {
//                            off = DATA_FRAME_HEAD_SIZE /* Head Bits in a pack, eg. 0xB7, MCU No., etc. */
//                                    + USING_CHANNEL_COUNT * CHANNEL_PACKAGE_LENGTH * adcNum  /* The ADC0's data is in the front of ADC1's data. */
//                                        + 3 * (channelNum + 1)  /* Every channel's data pack has a head of 3 bits. */
//                                            + CHANNEL_DATA_COUNT * 2 * channelNum   /* every channel's data is 200 bits apart of the 3-bit head. */
//                                                + (chDataCnt + j) * 2;   /* every channel's data at one time is 2 bits. */
//                            tARMPtr->backBuffer[off] = tARMPtr->backBuffer[off - 2];
//                            tARMPtr->backBuffer[off + 1] = tARMPtr->backBuffer[off - 1];
//                        }
//                    }
//                }
//            }
            break;
        }
        case eRead:
            break;
        case eEmpty:
        case eIdle:
            tARMPtr->foreBuffer[7] = CHANNEL_DATA_COUNT;
            break;
        }
    }

    chDataCnt = 0;
    overflowCnt = 0;
}

    /* End Process */

/*!
 * @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Dong
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */


