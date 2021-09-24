/******************************************************************************
 * @file sniffer
 * @brief monitoring tool that receives and transmits serially all msgs
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sniffer.h"
#include "sniffer_com.h"
#include "timestamp.h"

// avoid using LuosHAL in sniffer app ?
#include "luos_hal.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef REV
#define REV     \
    {           \
        1, 0, 0 \
    }
#endif
/*******************************************************************************
 * Variables
 ******************************************************************************/

streaming_channel_t Sniffer_StreamChannel;
msg_t msg;
uint64_t init_time = 0;
uint8_t prev_state = STOPPED;
/*******************************************************************************
 * Function
 ******************************************************************************/
static void Sniffer_MsgHandler(service_t *service, msg_t *msg);
/******************************************************************************
 * @brief init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void Sniffer_Init(void)
{
    revision_t revision = {.unmap = REV};

    Luos_CreateService(Sniffer_MsgHandler, SNIFFER_MOD, "sniffer", revision);
    //initialization of driver
    SnifferCom_Init();
    //create streaming channel
    Sniffer_StreamChannel = *create_Sniffer_StreamChannel();
}

/******************************************************************************
 * @brief loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void Sniffer_Loop(void)
{
    uint8_t state = Get_Sniffer_State();
    //check if we received the command to set up the sniffer
    if (state == INIT)
    {
        //respond that sniffer was succesfully connected
        SnifferData_SendInit();
        //reinitialize sniffer state to stopped
        Set_Default_Sniffer_State();
        // set timestamp to stop
        LuosHAL_StopTimestamp();
        return;
    }
    //Start/Stop/Pause process
    else if (state == STARTED)
    {
        //if previously the sniffer was stopped restart the timer
        if (prev_state == STOPPED)
        {
            LuosHAL_StopTimestamp();
            LuosHAL_StartTimestamp();
        }
    }
    else
    {
        //check if sniffer is stopped and send statistics
        if (prev_state == STARTED)
        {
            while (SnifferCom_Pending() == 1)
                ;
            SnifferData_SendStat();
        }
    }
    //keep the previous state
    prev_state = state;
}

/******************************************************************************
 * @brief Msg Handler call back when a msg receive for this service
 * @param Service destination
 * @param Msg receive
 * @return None
 ******************************************************************************/
static void Sniffer_MsgHandler(service_t *service, msg_t *msg)
{
    //if the sniffer is not started by the user we drop the message
    if (prev_state != STARTED)
    {
        return;
    }

    // decode timestamped message if needed
    uint64_t timestamp = 0;
    if (Timestamp_IsTimestampMsg(msg))
    {
        Timestamp_DecodeMsg(msg, &timestamp);
    }

    //copy message in the streaming channel
    SnifferData_PutMsg(timestamp, msg);

    //check if the transmition has already started
    uint16_t size;
    if (SnifferCom_Pending() == 0)
    {
        //send available streaming data
        size = Stream_GetAvailableSampleNBUntilEndBuffer(&Sniffer_StreamChannel);
        if (size > 0)
        {
            SnifferCom_Send(Sniffer_StreamChannel.sample_ptr, size);
        }
    }
}