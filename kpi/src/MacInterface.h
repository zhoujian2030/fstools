
#ifndef MAC_INTERFACE_H
#define MAC_INTERFACE_H

#define MAC_SEND_MSG_BUFFER_LENGTH 512
#define MAC_RECV_MSG_BUFFER_LENGTH 2048

#define KPI_SERVER_IP "192.168.1.166"
#define KPI_SERVER_PORT 50001

typedef unsigned int UInt32;
typedef signed int SInt32;
typedef unsigned short UInt16;
typedef unsigned char UInt8;

// module id
#define KPI_MODULE_ID            0x9901
#define MAC_MODULE_ID            7
#define RRC_MODULE_ID            3 
#define RLC_MODULE_ID            6
#define OAM_MODULE_ID            1
#define PDCP_MODULE_ID           5
#define PACKET_RELAY_MODULE_ID   4
#define RRM_MODULE_ID            2
#define PHY_MODULE_ID            8

// msg id
typedef enum {
	MAC_KPI_REQ = 0x01
} KPIReqAPI;

#define COLLECT_PERIOD_MS 10000

#define LTE_MSG_HEAD_LENGTH 10
#define MAC_VAR_SIZE(x) 1
typedef struct {
	UInt16 transactionId;
	UInt16 srcModuleId;
	UInt16 dstModuleId;
	UInt16 msgId;
	UInt16 length;
	char msgBody[MAC_VAR_SIZE(length)];
} LteMacMsg;

typedef struct {	
    UInt32 activeUe;
    UInt32 rach;
    UInt32 rar;
    UInt32 msg3;
    UInt32 contResol;
    UInt32 crcValid;
    UInt32 harqAck;
    UInt32 msg3Expired;
    UInt32 crcError;
    UInt32 harqNack;  // harq nack for all dl sch including contention resolution
    UInt32 contNack;  // harq nack for contention resolution
    
	UInt32 rrcReq;
	UInt32 rrcSetup;
	UInt32 rrcSetupComplete;
	UInt32 identityReq;
	UInt32 idnetityResp;
	UInt32 attachReject;
	UInt32 tauReject;
	UInt32 rrcRelease;
    UInt32 rrcReestablishmentReq;
    UInt32 rrcConnReject;
    UInt32 rrcReestabReject;
    UInt32 ueInfoReq;
    UInt32 ueInfoRsp;

    UInt32 ulCCCH;
    UInt32 dlCCCH;
    UInt32 ulDCCH;
    UInt32 dlDCCH;
} LteCounter;

#endif
