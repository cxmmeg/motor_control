/*******************************************************************************
 * User Parameters 

  File Name:
    userparams.h

 Summary:
    Header file which defines Motor Specific and Board Specific constants 

  Description:
    This file contains the motor and board specific constants. It also defines
 * switches which allows algorithm to be run in debug modes like Open Loop Mode,
 * Torque mode, etc. 

 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
//DOM-IGNORE-END

#ifndef USERPARAMS_H
#define USERPARAMS_H



#define     PWM_CLK                                             (120000000ul)   // PWM Peripheral Input Clock Frequency in Hz
#define     PWM_FREQ                                            20000           // PWM Frequency in Hz
#define     DELAY_MS                                            (float)10  // Delay in milliseconds after which Speed Ramp loop is executed
#define     SW_DEBOUNCE_DLY_MS                                  (float)500  // Switch debounce delay in mS

#define LONG_HURST

#define MCLV2
#undef ENABLE_FLUX_WEAKENING 


/*Application Configuration Switches*/

/*Defining this Macro, enables current PI tuning mode in which a step current
 *  reference is generated to observe step response of the current controller*/
#undef CURPI_TUN
#ifdef CURPI_TUN
    #define CUR_STEP_AMP  (float) 0.5
    #define CUR_STEP_TIM  (float) 0.5
#endif

/*Define macro TORQUE_MODE to run the motor in TORQUE Control Mode
 Undefine macro TORQUE_MODE to run the motor in Speed Control Mode*/
#undef TORQUE_MODE

#ifdef MCLV2
//===============================================================
// Following parameters for MCLV-2 board
// Gain of opamp = 15
// shunt resistor = 0.025 ohms
// DC offset = 1.65V
// max current = x
// (x * 0.025 * 15) + 1.65V = 3.3V
// x = 4.4Amps
#define     MAX_BOARD_CURRENT                                   (float)(4.4)            // Max Board Current in A
#define     DCBUS_SENSE_TOP_RESISTOR                            (float)30.0             // DC Bus voltage Divider - Top Side Resistor in Kohm
#define     DCBUS_SENSE_BOTTOM_RESISTOR                         (float)2.0              // DC Bus voltage Divider - Bottom Side Resistor in Kohm
#endif


#ifdef LONG_HURST
//---------------Motor Specifications : Hurst Motor With Quadrature Encoder : DMA0204024B101 (a.k.a. "Long" Hurst Motor)
#define     MOTOR_PER_PHASE_RESISTANCE                          ((float)0.285)			// Per Phase Resistance in Ohms
#define     MOTOR_PER_PHASE_INDUCTANCE                          ((float)0.00032)		// Per Phase Inductance in Henrys
#define     MOTOR_BACK_EMF_CONSTANT_Vpeak_Line_Line_KRPM_MECH   (float)7.24				// Back EMF Constant in Vpeak(L-L)/KRPM 
#define     NOPOLESPAIRS                                        ((float)5)              // Number of Pole Pairs of the PMSM Motor        
#define     STAR_CONNECTED_MOTOR                                1                       // 1 - Motor is Star Connected, 0 - Motor is Delta Connected
#define     NOMINAL_SPEED_RPM                                   (float)2804             // Nominal Rated Speed of the Motor - Value in RPM
#define     FW_SPEED_RPM                                        (float)3500             // Maximum Speed of the Motor in Flux Weakening Mode - Value in RPM
#define     ENCODER_PULSES_PER_REV                              ((float)1000)
#define     ELECTRICAL_TIME_CONSTANT                            ((float)0.001123)
#define     MECHANICAL_TIME_CONSTANT                            ((float)0.002882)
//--------------Motor Startup Behavior Configuration----------//
#define     ALIGN_TIME_IN_SEC                                   2                       // Duration of Motor Alignment in seconds
#define     OPENLOOP_RAMP_TIME_IN_SEC                           3                       // Ramp time to reach from 0 to Open Loop Speed in seconds
#define     OPENLOOP_END_SPEED_RPM                              500                     // Speed at which the motor switches from open loop to closed loop in RPM
#define     CLOSEDLOOP_RAMP_RATE_RPM_SEC                        200                     // Closed Loop Speed Ramp rate in Rev/min/Sec
#define     ALIGN_Q_CURRENT_REF                                 0.4                     // Maximum Torque Reference during Motor Alignment in A
#define     OPENLOOP_Q_CURRENT_REF                              0.4                     // Maximum Torque Reference during Open Loop Mode in A
#define     TORQUE_MODE_MAX_CUR                                 0.4                     // Maximum Torque Mode Current Reference in A
#define     MAX_MOTOR_CURRENT                                   (float)(4.4)            // Maximum Motor Current in A

/*________________________ D Control Loop Coefficients  ___________________________________ */
#define     D_CURRCNTR_PTERM                      0.02               /*     D axis Proportional Gain                          */ 
#define     D_CURRCNTR_ITERM                      0.0005            /*     D axis Integral Gain                              */ 
#define     D_CURRCNTR_CTERM                      0.5                /*     D axis Anti-Windup Gain                           */
#define     D_CURRCNTR_OUTMAX                     0.999              /*     D axis PI Controller Maximum Output  (Normalized) */ 

/*________________________ Q Control Loop Coefficients  ___________________________________ */
#define     Q_CURRCNTR_PTERM                      0.02               /*     Q axis Proportional Gain                          */ 
#define     Q_CURRCNTR_ITERM                      0.0005            /*     Q axis Integral Gain                              */ 
#define     Q_CURRCNTR_CTERM                      0.5                /*     Q axis Anti-Windup Gain                           */
#define     Q_CURRCNTR_OUTMAX                     0.999              /*     Q axis PI Controller Maximum Output  (Normalized) */ 
   
/*________________________Speed  control Loop Coefficients  ___________________________________ */
#define     SPEEDCNTR_PTERM                      (0.005)             /*     Speed  Proportional Gain                          */ 
#define     SPEEDCNTR_ITERM                      (0.00000020)       /*     Speed  Integral Gain                              */ 
#define     SPEEDCNTR_CTERM                       0.5                /*     Speed  Anti-Windup Gain                           */
#define     SPEEDCNTR_OUTMAX                      MAX_MOTOR_CURRENT  /*     Speed PI Controller Maximum Output  (Normalized) */                 // Q axis PI Controller Maximum Output - Max D axis Voltage (Normalized)    

/******* ********** Flux Weakening Parameter ************************/
#define     MAX_FW_NEGATIVE_ID_REF                              (float)(-3.0f)
#define     KFILTER_IDREF                                       (float)( FAST_LOOP_TIME_SEC / ( FAST_LOOP_TIME_SEC + ELECTRICAL_TIME_CONSTANT) )  
#endif



// <editor-fold defaultstate="collapsed" desc=" Derived Macros from Motor Control Board Specifications, Motor Specifications and Motor Dyanmics">

#define     PWM_PERIOD_COUNT                                    (((PWM_CLK/PWM_FREQ)/2))
#define     PWM_HALF_PERIOD_COUNT                               PWM_PERIOD_COUNT>>1
#define     PWM_MAX_DUTY_COUNT                                  (uint32_t)( 0.98 * PWM_PERIOD_COUNT)
#define     MAX_MOTOR_CURRENT_SQUARED                           (float)((float)MAX_MOTOR_CURRENT*(float)MAX_MOTOR_CURRENT)
#define     VREF_DAC_VALUE                                      (int) 2048
#define     ADC_CURRENT_SCALE                                   (float)(MAX_BOARD_CURRENT/(float)2048)
#define     CURRENT_LIMIT_CMP_REF                               (int)(((float)2048*(MAX_MOTOR_CURRENT/MAX_BOARD_CURRENT))+VREF_DAC_VALUE)
#define     MOTOR_PER_PHASE_INDUCTANCE_DIV_2_PI                 ((float)(MOTOR_PER_PHASE_INDUCTANCE/(2*M_PI)))	
#define     MAX_ADC_COUNT                                       (float)4095     // for 12-bit ADC
#define     MAX_ADC_INPUT_VOLTAGE                               (float)3.3      // volts
#define     DCBUS_SENSE_RATIO                                   (float)(DCBUS_SENSE_BOTTOM_RESISTOR/(DCBUS_SENSE_BOTTOM_RESISTOR + DCBUS_SENSE_TOP_RESISTOR))
#define     VOLTAGE_ADC_TO_PHY_RATIO                            (float)(MAX_ADC_INPUT_VOLTAGE/(MAX_ADC_COUNT * DCBUS_SENSE_RATIO))
#define     SINGLE_ELEC_ROT_RADS_PER_SEC                        (float)(2*M_PI)
#define     MAX_DUTY                                            (PWM_PERIOD_COUNT)
#define     LOOPTIME_SEC                                        (float)(1/((float)PWM_FREQ))           // PWM Period - 50 uSec, 20Khz PWM
#define     COUNT_FOR_ALIGN_TIME                                (unsigned int)((float)ALIGN_TIME_IN_SEC/(float)LOOPTIME_SEC)
#define     ALIGN_CURRENT_STEP                                  (float)(2*ALIGN_Q_CURRENT_REF/COUNT_FOR_ALIGN_TIME) // Current reference during aligning is ramped up for 50% of align time.
#define     OPENLOOP_END_SPEED_RPS                              ((float)OPENLOOP_END_SPEED_RPM/60)
#define     OPENLOOP_END_SPEED_RADS_PER_SEC_MECH                (float)(OPENLOOP_END_SPEED_RPS * SINGLE_ELEC_ROT_RADS_PER_SEC)
#define     OPENLOOP_END_SPEED_RADS_PER_SEC_ELEC                (float)(OPENLOOP_END_SPEED_RADS_PER_SEC_MECH * NOPOLESPAIRS)
#define     OPENLOOP_END_SPEED_RADS_PER_SEC_ELEC_IN_LOOPTIME    (float)(OPENLOOP_END_SPEED_RADS_PER_SEC_ELEC * LOOPTIME_SEC)
#define     OPENLOOP_RAMPSPEED_INCREASERATE                     (float)(OPENLOOP_END_SPEED_RADS_PER_SEC_ELEC_IN_LOOPTIME/(OPENLOOP_RAMP_TIME_IN_SEC/LOOPTIME_SEC))
#define     CLOSEDLOOP_RAMP_RATE_RPS_SEC                        ((float)CLOSEDLOOP_RAMP_RATE_RPM_SEC/60) // CLosed Loop  Speed Ramp rate in Rev/sec^2 
#define     CLOSEDLOOP_RAMP_RATE_RADS_PER_SEC2_MECH             (float)(CLOSEDLOOP_RAMP_RATE_RPS_SEC*2*M_PI) // CLosed Loop  Speed Ramp Rate in Mechanical Radians/Sec^2
#define     CLOSEDLOOP_RAMP_RATE_RADS_PER_SEC2_ELEC             (float)(CLOSEDLOOP_RAMP_RATE_RADS_PER_SEC2_MECH*NOPOLESPAIRS) // CLosed Loop  Speed Ramp rate in Electrical Radians/Sec^2
#define     CLOSEDLOOP_SPEED_RAMP_RATE_DELTA                    (float)(CLOSEDLOOP_RAMP_RATE_RADS_PER_SEC2_ELEC*DELAY_MS*0.001) // CLosed Loop  Speed Ramp Rate in Electrical Radians/sec^2 in each control loop time
#define     CLOSEDLOOP_SPEED_HYSTERESIS                         (float)(5*CLOSEDLOOP_SPEED_RAMP_RATE_DELTA)
#define     NOMINAL_SPEED_RAD_PER_SEC_ELEC                      (float)(((NOMINAL_SPEED_RPM/60)*2*M_PI)*NOPOLESPAIRS) // Value in RPM
#define     MAXIMUM_SPEED_RAD_PER_SEC_ELEC                      (float)(((FW_SPEED_RPM/60)*2*M_PI)*NOPOLESPAIRS)
#if(STAR_CONNECTED_MOTOR==1)
    #define     MOTOR_BACK_EMF_CONSTANT_Vpeak_PHASE_RPM_MECH    (float)((MOTOR_BACK_EMF_CONSTANT_Vpeak_Line_Line_KRPM_MECH/1.732)/1000)
#else
    #define     MOTOR_BACK_EMF_CONSTANT_Vpeak_PHASE_RPM_MECH    (float)((MOTOR_BACK_EMF_CONSTANT_Vpeak_Line_Line_KRPM_MECH)/1000)
#endif 
#define     MOTOR_BACK_EMF_CONSTANT_Vpeak_PHASE_RPS_MECH         (float)(MOTOR_BACK_EMF_CONSTANT_Vpeak_PHASE_RPM_MECH * 60)
#define     MOTOR_BACK_EMF_CONSTANT_Vpeak_PHASE_RAD_PER_SEC_MECH (float)(MOTOR_BACK_EMF_CONSTANT_Vpeak_PHASE_RPS_MECH/(2*M_PI))
#define     MOTOR_BACK_EMF_CONSTANT_Vpeak_PHASE_RAD_PER_SEC_ELEC (float)(MOTOR_BACK_EMF_CONSTANT_Vpeak_PHASE_RAD_PER_SEC_MECH/NOPOLESPAIRS)
#define     INVKFi_BELOW_BASE_SPEED                              (float)(1/MOTOR_BACK_EMF_CONSTANT_Vpeak_PHASE_RAD_PER_SEC_ELEC)
#define     DELAY_10MS_COUNT                                     (float)(PWM_FREQ*DELAY_MS*(float)0.001)
#define     SW_DEBOUNCE_DLY_500MS                                (uint32_t)(SW_DEBOUNCE_DLY_MS/DELAY_MS)  // Switch debounce duration in multiple of 10mS
#define     FW_SPEED_RAD_PER_SEC_ELEC                            (float)(((FW_SPEED_RPM/60)*2*M_PI)*NOPOLESPAIRS)
#define     MAX_NORM                                             (float) 0.99
#define     MAX_NORM_SQ                                          (float) (MAX_NORM*MAX_NORM)
#define     TORQUE_MODE_POT_ADC_RATIO                            (float) (TORQUE_MODE_MAX_CUR/MAX_ADC_COUNT)

#ifndef  ENABLE_FLUX_WEAKENING
#define     POT_ADC_COUNT_FW_SPEED_RATIO                    (float)(NOMINAL_SPEED_RAD_PER_SEC_ELEC/MAX_ADC_COUNT)
#else 
#define     POT_ADC_COUNT_FW_SPEED_RATIO                    (float)(MAXIMUM_SPEED_RAD_PER_SEC_ELEC/MAX_ADC_COUNT)
#endif 


#define 	DECIMATE_NOMINAL_SPEED                              ((NOMINAL_SPEED_RPM *(M_PI/30))*NOPOLESPAIRS/10)
#ifdef      CURPI_TUN
    #define CPT_CNT_VAL   (float)(CUR_STEP_TIM*PWM_FREQ)
#endif 



//--------------PDEC Configuration----------//
#define ENCODER_PULSES_PER_EREV                      (uint16_t)(ENCODER_PULSES_PER_REV/NOPOLESPAIRS)
#define QDEC_RC 65536u
#define QDEC_UPPER_THRESHOLD 49151u   
#define QDEC_LOWER_THRESHOLD 16384u  
#define QDEC_OVERFLOW  (uint16_t)(QDEC_RC % ENCODER_PULSES_PER_EREV) 
#define QDEC_UNDERFLOW  (uint16_t)(ENCODER_PULSES_PER_EREV - QDEC_OVERFLOW)
#define FAST_LOOP_TIME_SEC          (float)(1/(float)PWM_FREQ) /* Always runs in sync with PWM */
#define SLOW_LOOP_TIME_SEC          (float)(FAST_LOOP_TIME_SEC * 100) /* 100 times slower than Fast Loop */
 
#endif
// </editor-fold>

