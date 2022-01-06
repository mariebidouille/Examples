/******************************************************************************
 * @file gate
 * @brief Service gate
 * @author Luos
 ******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include "inspector.h"
#include "pipe_link.h"
#include "data_manager.h"
#include "luos_list.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/
service_t *inspector;
static uint16_t pipe_id = 0;
/*******************************************************************************
 * Function
 ******************************************************************************/
/******************************************************************************
 * @brief init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void Inspector_Init(void)
{
    revision_t revision = {.major = 1, .minor = 0, .build = 0};
    // inspector service creation
    inspector = Luos_CreateService(0, INSPECTOR_TYPE, "inspector", revision);
}

/******************************************************************************
 * @brief loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void Inspector_Loop(void)
{
#ifndef NODETECTION
    static uint8_t init_flag = 1;
#endif
    // check if the network is detected
    if (Luos_IsNodeDetected())
    {
#ifndef NODETECTION
        init_flag = 0;
#endif
        // Network have been detected, We are good to go
        if (pipe_id == 0)
        {
            // We dont have spotted any pipe yet. Try to find one
            pipe_id = PipeLink_Find(inspector);
        }
        if (Luos_NbrAvailableMsg() > 0)
        {
            DataManager_GetServiceMsg(inspector);
        }
    }
    else
    {
        // if the network is not yet detected reset the pipe id
        pipe_id = 0;
        // send to Luos a flag in order to filter the messages
        Luos_SetFilterState(true, inspector);
#ifndef NODETECTION
        if (init_flag && (Luos_GetSystick() > 20))
        {
            Luos_Detect(inspector);
            init_flag = 0;
        }
#endif
    }
}
