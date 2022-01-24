/******************************************************************************
 * @file biometric_security
<<<<<<< Updated upstream
 * @brief app example 
=======
 * @brief This is an app exemple for a Biometric Security System. It won't work 
 * as is so if you want to see it inside of a project, I made multiple version
 * that you can go check on my github :
 * https://github.com/mariebidouille
>>>>>>> Stashed changes
 * @author MarieBidouille
 * @version 0.0.0
 ******************************************************************************/
#ifndef BIOMETRIC_SECURITY_H
#define BIOMETRIC_SECURITY_H

#include "luos.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
<<<<<<< Updated upstream
#define UPDATE_PERIOD_MS 200

typedef enum 
{
    BIOMETRIC_SECURITY_TYPE = LUOS_LAST_TYPE
}security_system_type_t;
=======
>>>>>>> Stashed changes

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Functions
 ******************************************************************************/
void BiometricSecurity_Init(void);
void BiometricSecurity_Loop(void);

#endif /* BIOMETRIC_SECURITY_H */