/*******************************************************************************
  PMSM_FOC App interface file

  Company:
    Microchip Technology Inc.

  File Name:
    mc_pmsm_foc.c

  Summary:
    This file contains functions to initialize the motor control
    peripherals and interface functions to control the motor.

  Description:
  This file contains functions to initialize the motor control
  peripherals and interface functions to control the motor.

 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "definitions.h"                // SYS function prototypes
#include "device.h"
#include "mc_derivedparams.h"
#include "mc_control_loop.h"
#include "mc_rotorposition.h"
#include "mc_errorhandler.h"
#include "mc_pmsm_foc.h"
#include "mc_currmeasurement.h"
#include "mc_voltagemeasurement.h"
#include "mc_speed.h"
#include "mc_pwm.h"
#include "mc_lib.h"
#include "mc_picontrol.h"
#include "mc_hal.h"


/******************************************************************************/
/* Local Function Prototype                                                   */
/******************************************************************************/

static void PMSM_FOC_ButtonPolling( void );

static tMCCTRL_TASK_STATE_E PMSM_FOC_IsSpeedLoopActive( void );
static tMCCTRL_TASK_STATE_E PMSM_FOC_IsPositionLoopActive( void );
static void PMSM_FOC_StartAdcInterrupt( void );

/******************************************************************************/
/*                   Global Variables                                         */
/******************************************************************************/

tPMSM_FOC_BUTTON_STATE_S                  gMCLIB_StartStopState;
tPMSM_FOC_BUTTON_STATE_S                  gMCLIB_DirectionToggleState;


/******************************************************************************/
/*                   Local Functions                                         */
/******************************************************************************/
static void PMSM_FOC_StartAdcInterrupt( void )
{
    /* ADC end of conversion interrupt generation for FOC control */
    MCHAL_IntDisable(MCHAL_CTRL_IRQ);
    MCHAL_IntClear(MCHAL_CTRL_IRQ);

    /* Enable ADC interrupt for field oriented control */
    MCHAL_ADCCallbackRegister( MCHAL_ADC_PH_U, MCCTRL_CurrentLoopTasks, (uintptr_t)NULL );
    MCHAL_IntEnable(MCHAL_CTRL_IRQ);

    /* Enable interrupt for fault detection */
    MCHAL_PWMCallbackRegister(MCHAL_PWM_PH_U, MCERR_FaultControlISR, (uintptr_t)NULL);
    MCHAL_IntEnable(MCHAL_FAULT_IRQ);

    /* Enables PWM channels. */
    MCHAL_PWMStart();

    /* Disable PWM output when the interface is available */
    MCPWM_PWMOutputDisable();
}


/******************************************************************************/
/* Function name: MCINF_IsSpeedLoopActive                                     */
/* Function parameters: None                                                  */
/* Function return: tMCINF_LOOP_STATE_E                                       */
/* Description: To be used in a state machine to decide whether               */
/* to execute slow control loop                                               */
/******************************************************************************/
static tMCCTRL_TASK_STATE_E  PMSM_FOC_IsSpeedLoopActive(void)
{
      return gMCCTRL_TaskStateSignals.speedLoopActive;
}

/*******************************************************************************/
/* Function name: MCINF_IsPositionLoopActive                                   */
/* Function parameters: None                                                   */
/* Function return: tMCINF_LOOP_STATE_E                                        */
/* Description: To be used in a state machine to decide whether                */
/* to execute slow control loop                                                */
/*******************************************************************************/
static tMCCTRL_TASK_STATE_E  PMSM_FOC_IsPositionLoopActive(void)
{
    return gMCCTRL_TaskStateSignals.positionLoopActive;
}

/******************************************************************************/
/*                   Implementation                                        */
/******************************************************************************/
/******************************************************************************/
/* Function name: MCINF_Initialize                                     */
/* Function parameters: None                                                  */
/* Function return: None                                                      */
/* Description:                                                               */
/* initializes parameters and state variables for motor  control function     */
/******************************************************************************/
void PMSM_FOC_Initialize( void )
{
    gMCCTRL_CtrlParam.rotationSign = 1U;

    /* Disable PWM output */
    MCHAL_PWMStop();

    /* Disable interrupt, and clear pending interrupts */
    MCHAL_IntDisable( MCHAL_CTRL_IRQ);
    MCHAL_IntClear( MCHAL_CTRL_IRQ);

    /* Current sense amplifiers offset calculation */
    if(gMCCUR_OutputSignals.calibDone == 0U)
    {
        MCCUR_OffsetCalibration();
    }
    else
    {
        asm("NOP");
    }

    /* Initialize speed command function */
    MCSPE_InitializeSpeedControl();

    /* Initialize current measurement module */
    MCCUR_InitializeCurrentMeasurement();

    /* Motor Controller parameter initialization */
    MCCTRL_InitializeMotorControl();

    /* Rotor position algorithm state initialization */
    MCRPOS_InitializeRotorPositionSensing();

    /* Start ADC Interrupt for current control */
    PMSM_FOC_StartAdcInterrupt();
}

/*****************************************************************************/
/* Function name: MotorStart                                                 */
/* Function parameters: None                                                 */
/* Function return: None                                                     */
/* Description: Enables fast control loop and starts the PWMs.               */
/*****************************************************************************/
void PMSM_FOC_MotorStart(void)
{
    MCERR_ErrorClear();
    /* Change motor status to RUNNING */
    PMSM_FOC_ResetParameters();

    MCCTRL_ResetMotorControl();

    MCRPOS_ResetPositionSensing(MCRPOS_FORCE_ALIGN);

    /* Switch the motor control state to MCAPP_FIELD_ALIGNMENT */
    gMCCTRL_CtrlParam.mcState = MCAPP_FIELD_ALIGNMENT;

    /* Enable / Re-enable PWM output */
    MCPWM_PWMDutyUpdate(gMCPWM_SVPWM.neutralPWM, gMCPWM_SVPWM.neutralPWM, gMCPWM_SVPWM.neutralPWM );
    MCPWM_PWMOutputEnable();

}

/******************************************************************************/
/* Function name: MotorStop                                                   */
/* Function parameters: None                                                  */
/* Function return: None                                                      */
/* Description: Stops PWM and disables fast control loop.                     */
/******************************************************************************/
void PMSM_FOC_MotorStop(void)
{
    /* Disable PWM output when the interface is available */
    MCPWM_PWMOutputDisable();

    /* Switch the motor control state to MCAPP_FIELD_ALIGNMENT */
    gMCCTRL_CtrlParam.mcState = MCAPP_IDLE;

    /* Reset global variables for next run */
    MCSPE_ResetSpeedControl();

    /* Motor Controller parameter initialization */
    MCCTRL_ResetMotorControl();
}


#ifndef MCHV3
/******************************************************************************/
/* Function name: MCAPP_DirectionToggle                                       */
/* Function parameters: None                                                  */
/* Function return: None                                                      */
/* Description: Updates global variable for motor direction change            */
/******************************************************************************/
void PMSM_FOC_DirectionToggle(void)
{
    /* Change rotation sign */
    gMCCTRL_CtrlParam.rotationSign = -gMCCTRL_CtrlParam.rotationSign;

    /* Toggle direction indicator LED */
    MCHAL_DIR_LED_TOGGLE();

}
#endif

/******************************************************************************/
/* Function name: MCINF_Tasks                                                 */
/* Function parameters: None                                                  */
/* Function return: None                                                      */
/* Description: Motor start stop and direction switch polling                 */
/******************************************************************************/
void PMSM_FOC_Tasks()
{
    /* Position Loop control tasks */
    if( MCCTRL_LOOP_ACTIVE == PMSM_FOC_IsSpeedLoopActive())
    {
        PMSM_FOC_ButtonPolling();
    }
    /* Speed Loop Control tasks  */
    PMSM_FOC_SpeedLoopTasks();

    /* Motor start stop task */


    /* Current control Loop Tasks */

 }




/******************************************************************************/
/* Function name: MCAPP_SpeedLoopTasks                                        */
/* Function parameters: None                                                  */
/* Function return: None                                                      */
/* Description: Motor start stop and direction switch polling                 */
/******************************************************************************/
void PMSM_FOC_SpeedLoopTasks()
{
    if( MCCTRL_LOOP_ACTIVE == PMSM_FOC_IsSpeedLoopActive())
    {
        if( MCAPP_CLOSED_LOOP  == gMCCTRL_CtrlParam.mcState )
        {
            /* Reference speed calculation */
            MCSPE_SpeedCommand();
        }
        else
        {
            gMCSPE_OutputSignals.commandSpeed = gMCCTRL_CtrlParam.velRef;
        }

        /* Reset Speed Loop counter */
        gMCCTRL_TaskStateSignals.speedLoopActive = MCCTRL_LOOP_INACTIVE;
    }
 }




/*****************************************************************************/
/* Function name: MCINF_PositionLoopTasks                                       */
/* Function parameters: None                                                 */
/* Function return: None                                                     */
/* Description: Motor start stop and direction switch polling                */
/*****************************************************************************/
void MCINF_PositionLoopTasks()
{
    if( MCCTRL_LOOP_ACTIVE == PMSM_FOC_IsPositionLoopActive())
    {
        /* Performs tasks for position loop  */

        /* Reset Speed Loop counter */
        gMCCTRL_TaskStateSignals.positionLoopActive = MCCTRL_LOOP_INACTIVE;

    }
 }

/*******************************************************************************/
/* Function name: MCINF_ButtonPolling                                          */
/* Function parameters: None                                                   */
/* Function return: None                                                       */
/* Description: Button Polling                                                 */
/*******************************************************************************/
void PMSM_FOC_ButtonPolling()
{
    /* Check whether S2 push button is pressed */
    if( MCAPP_IDLE == gMCCTRL_CtrlParam.mcState )
    {
        PMSM_FOC_ButtonResponse((tPMSM_FOC_SWITCH_STATE_E)(!MCHAL_START_STOP_SWITCH_GET()), &PMSM_FOC_MotorStart);

      #ifndef MCHV3
        PMSM_FOC_ButtonResponse((tPMSM_FOC_SWITCH_STATE_E)(!MCHAL_DIR_SWITCH_GET()), &PMSM_FOC_DirectionToggle);
      #endif

    }
    else
    {
        PMSM_FOC_ButtonResponse((tPMSM_FOC_SWITCH_STATE_E)(!MCHAL_START_STOP_SWITCH_GET()), &PMSM_FOC_MotorStop);
    }
 }

/******************************************************************************/
/* Function name: MCINF_ResetInfrastructure                                   */
/* Function parameters: None                                                  */
/* Function return: None                                                      */
/* Description: Reset infrastructure                                          */
/******************************************************************************/
void PMSM_FOC_ResetParameters( void )
{
    /* Reset infrastructure state variables */
    gMCCTRL_TaskStateSignals.positionLoopActive = MCCTRL_LOOP_INACTIVE;
    gMCCTRL_TaskStateSignals.speedLoopActive = MCCTRL_LOOP_INACTIVE;
}

/******************************************************************************/
/* Function name: MCLIB_ButtonResponse                                        */
/* Function parameters: None                                                  */
/* Function return: None                                                      */
/* Description: Push button debounce function                                 */
/******************************************************************************/
void PMSM_FOC_ButtonResponse( const tPMSM_FOC_SWITCH_STATE_E  buttonState,  void (*buttonFunction)(void) )
{
    switch(gMCLIB_StartStopState.state)
    {
        case PMSM_FOC_BUTTON_READY:
        {
            if( PMSM_FOC_SWITCH_PRESSED == buttonState )
            {
                buttonFunction();
                gMCLIB_StartStopState.debounceCounter = 0;
                gMCLIB_StartStopState.state = PMSM_FOC_BUTTON_WAIT;
            }
        }
        break;

        case PMSM_FOC_BUTTON_WAIT:
        {
            if( SW_DEBOUNCE_DLY_500MS <= gMCLIB_StartStopState.debounceCounter)
            {
                gMCLIB_StartStopState.state = PMSM_FOC_BUTTON_READY;
                gMCLIB_StartStopState.debounceCounter = 0;
            }
            else
            {
                gMCLIB_StartStopState.debounceCounter++;
            }
        }
        break;
        default:
        {
              /* Should never come here */
        }
    }
}