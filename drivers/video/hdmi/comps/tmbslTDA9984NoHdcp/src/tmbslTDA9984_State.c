/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmbslTDA9984_state.c
 *
 * \version       Revision: 3
 *
 * \date          Date: 31/10/07 10:07
 *
 * \brief         BSL driver component API for the TDA9984 HDMI Transmitter
 *
 * \section refs  Reference Documents
 *
 * \section info  Change Information
 *
 * \verbatim

   History: tmbslTDA9984_state.c
 * 
 * *****************  Version 3  ****************
 * User: B.Vereecke     Date: 31/10/07   Time: 10:51
 * Updated in $/Source/tmbslTDA9984/Src
 * PR848 : Remove qmore errors
 *
 * *****************  Version 2  ****************
 * User: G.Burnouf      Date: 25/10/07   Time: 10:07
 * Updated in $/Source/tmbslTDA9984/Src
 * PR780 : change state machine for stand by mode
 *
 * *****************  Version 1  *****************
 * User: G. Burnouf    Date: 04/07/07    Time: 16:30
 * Created in $/Source/tmbslTDA9984/Src
 * initial revision
 *
   \endverbatim
 *
*/

/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/

#include "tmbslTDA9984_Functions.h"
#include "tmbslTDA9984_local.h"
#include "tmbslTDA9984_State_l.h"


/*============================================================================*/
/*                     TYPES DECLARATIONS                                     */
/*============================================================================*/


/*============================================================================*/
/*                       CONSTANTS DECLARATIONS                               */
/*============================================================================*/

/*============================================================================*/
/*                       DEFINES DECLARATIONS                               */
/*============================================================================*/


/*============================================================================*/
/*                       VARIABLES DECLARATIONS                               */
/*============================================================================*/

/*============================================================================*/
/*                       FUNCTION PROTOTYPES                                  */
/*============================================================================*/

/*============================================================================*/
/* setState                                                                   */
/*============================================================================*/
tmErrorCode_t
setState
(
    tmHdmiTxobject_t   *pDis,
    tmbslTDA9984Event_t  event
)
{
    tmbslTDA9984State_t state = pDis->state;
    UInt8 nIgnoredEvents = pDis->nIgnoredEvents;

    switch (state)
    {
    case ST_UNINITIALIZED:
        switch (event)
        {
        case EV_UNPLUGGED:          state = ST_DISCONNECTED;      break;
        case EV_PLUGGEDIN:          state = ST_AWAIT_EDID;        break;
        default:                    nIgnoredEvents++;             break;
        }                                                               
        break;                                                          
    case ST_DISCONNECTED:                                               
        switch (event)                                                  
        {                                                               
        case EV_DEINIT:             state = ST_UNINITIALIZED;     break;
        case EV_PLUGGEDIN:          state = ST_AWAIT_EDID;        break;
        case EV_STANDBY:            state = ST_STANDBY;           break;
        default:                    nIgnoredEvents++;             break;
        }                                                               
        break;                                                          
    case ST_AWAIT_EDID:                                                 
        switch (event)                                                  
        {                                                               
        case EV_DEINIT:             state = ST_UNINITIALIZED;     break;
        case EV_UNPLUGGED:          state = ST_DISCONNECTED;      break;
        case EV_STANDBY:            state = ST_STANDBY;           break;
		case EV_GETBLOCKDATA:       state = ST_AWAIT_RX_SENSE;	  break;
        default:                    nIgnoredEvents++;             break;
        }
        break;

    case ST_AWAIT_RX_SENSE:                                                 
        switch (event)                                                  
        {                                                               
        case EV_DEINIT:             state = ST_UNINITIALIZED;     break;
        case EV_UNPLUGGED:          state = ST_DISCONNECTED;      break;
        case EV_STANDBY:            state = ST_STANDBY;           break;
        case EV_SINKON:             state = ST_SINK_CONNECTED;    break;
        default:                    nIgnoredEvents++;             break;
        }
        break;

    case ST_SINK_CONNECTED:
        switch (event)
        {
        case EV_DEINIT:             state = ST_UNINITIALIZED;     break;
        case EV_UNPLUGGED:          state = ST_DISCONNECTED;      break;
        case EV_STANDBY:            state = ST_STANDBY;           break;
        case EV_SETINOUT:           state = ST_VIDEO_NO_HDCP;     break;
        case EV_SINKOFF:            state = ST_AWAIT_RX_SENSE;    break;
        default:                    nIgnoredEvents++;             break;
        }                                                               
        break;                                                          
    case ST_VIDEO_NO_HDCP:                                              
        switch (event)                                                  
        {                                                               
        case EV_DEINIT:             state = ST_UNINITIALIZED;     break;
        case EV_UNPLUGGED:          state = ST_DISCONNECTED;      break;
        case EV_STANDBY:            state = ST_STANDBY;           break;
        case EV_OUTDISABLE:         state = ST_SINK_CONNECTED;    break;
        case EV_HDCP_RUN:           state = ST_HDCP_WAIT_RX;      break;
        case EV_SINKOFF:            state = ST_AWAIT_RX_SENSE;    break;
        default:                    nIgnoredEvents++;             break;
        }                                                               
        break;                                                          
    case ST_HDCP_WAIT_RX:                                               
        switch (event)                                                  
        {                                                               
        case EV_DEINIT:             state = ST_UNINITIALIZED;     break;
        case EV_UNPLUGGED:          state = ST_DISCONNECTED;      break;
        case EV_STANDBY:            state = ST_STANDBY;           break;
        case EV_HDCP_BKSV_NREPEAT:  state = ST_HDCP_AUTHENTICATED;break;
        case EV_HDCP_BKSV_REPEAT:   state = ST_HDCP_WAIT_BSTATUS; break;
        case EV_HDCP_BKSV_NSECURE:  state = ST_HDCP_WAIT_RX;      break;
        case EV_HDCP_T0:            state = ST_HDCP_WAIT_RX;      break;
        case EV_HDCP_STOP:          state = ST_VIDEO_NO_HDCP;     break;
        case EV_SINKOFF:            state = ST_AWAIT_RX_SENSE;    break;
        default:                    nIgnoredEvents++;             break;
        }
        break;
    case ST_HDCP_WAIT_BSTATUS:
        switch (event)
        {
        case EV_DEINIT:             state = ST_UNINITIALIZED;     break;
        case EV_UNPLUGGED:          state = ST_DISCONNECTED;      break;
        case EV_STANDBY:            state = ST_STANDBY;           break;
        case EV_HDCP_BSTATUS_GOOD:  state = ST_HDCP_WAIT_SHA_1;   break;
        case EV_HDCP_T0:            state = ST_HDCP_WAIT_RX;      break;
        case EV_HDCP_STOP:          state = ST_VIDEO_NO_HDCP;     break;
        case EV_SINKOFF:            state = ST_AWAIT_RX_SENSE;    break;
        default:                    nIgnoredEvents++;             break;
        }
        break;
    case ST_HDCP_WAIT_SHA_1:
        switch (event)
        {
        case EV_DEINIT:             state = ST_UNINITIALIZED;     break;
        case EV_UNPLUGGED:          state = ST_DISCONNECTED;      break;
        case EV_STANDBY:            state = ST_STANDBY;           break;
        case EV_HDCP_KSV_SECURE:    state = ST_HDCP_AUTHENTICATED;break;
        case EV_HDCP_T0:            state = ST_HDCP_WAIT_RX;      break;
        case EV_HDCP_STOP:          state = ST_VIDEO_NO_HDCP;     break;
        case EV_SINKOFF:            state = ST_AWAIT_RX_SENSE;    break;
        default:                    nIgnoredEvents++;             break;
        }
        break;
    case ST_HDCP_AUTHENTICATED:
        switch (event)
        {
        case EV_DEINIT:             state = ST_UNINITIALIZED;     break;
        case EV_UNPLUGGED:          state = ST_DISCONNECTED;      break;
        case EV_STANDBY:            state = ST_STANDBY;           break;
        case EV_HDCP_T0:            state = ST_HDCP_WAIT_RX;      break;
        case EV_HDCP_STOP:          state = ST_VIDEO_NO_HDCP;     break;
        case EV_SINKOFF:            state = ST_AWAIT_RX_SENSE;    break;
        default:                    nIgnoredEvents++;             break;
        }
        break;
    case ST_STANDBY:
        switch (event)
        {
        case EV_DEINIT:             state = ST_UNINITIALIZED;     break;
        case EV_RESUME_UNPLUGGED:   state = ST_DISCONNECTED;      break;
        case EV_RESUME_PLUGGEDIN:   state = ST_AWAIT_RX_SENSE;    break;
        case EV_RESUME_AWAIT_EDID:  state = ST_AWAIT_EDID;        break;
        default:                    nIgnoredEvents++;             break;
        }
        break;
    default:
        nIgnoredEvents++;
        break;
    }
    pDis->state = state;
    pDis->nIgnoredEvents = nIgnoredEvents;
    return TM_OK;
}

/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/
