
#ifndef MAC_INTERFACE_H
#define MAC_INTERFACE_H

#include <string>

extern int gWriteOption;
extern std::string gServerIp;
extern unsigned short gServerPort;
extern int gPeriod;
extern bool gShowAll;
extern bool gShowPhyKpi;
extern std::string gKpiDirectory;
extern std::string gTargetImsi;

#define SW_VERSION          "2.0.0"

#define KPI_VERSION         1007 //1008
#define KPI_VERSION_LEN     4

#define MAC_SEND_MSG_BUFFER_LENGTH 512
#define MAC_RECV_MSG_BUFFER_LENGTH 4096

typedef unsigned int UInt32;
typedef signed int SInt32;
typedef unsigned short UInt16;
typedef unsigned char UInt8;

// module id
#define KPI_MODULE_ID            0x9901
#define CLI_MODULE_ID            0x9902
#define SIM_MODULE_ID            0x9903
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
	L2_CLI_GET_KPI_REQ = 0x01,
    L3_CLI_GET_KPI_REQ = 0x02,
    L2_CLI_GET_UE_KPI_REQ = 0x03
} KPIReqAPI;

typedef enum {
    L2_CLI_SET_LOG_LEVEL_REQ = 0x01,
    L2_CLI_SET_COMM_CHAN_RAT2 = 0x02,
    L2_CLI_SET_RACH_THRESTHOLD = 0x03,
    L2_CLI_SET_MAX_UE_SCHEDULED = 0x04,
    L2_CLI_SET_TARGET_UE = 0x05,
    L2_CLI_SET_MAX_UL_RE_TX = 0x06,
    
    SIM_CLI_SET_PARAM_REQ = 0x11
} CLIReqAPI;

#define LTE_CLI_MSG_HEAD_LENGTH 12
#define CLI_VAR_SIZE(x) 1
typedef struct {
	UInt16 transactionId;
	UInt16 srcModuleId;
	UInt16 dstModuleId;
	UInt16 msgId;
	UInt16 length;
    UInt16 spare;
	char msgBody[CLI_VAR_SIZE(length)];
} LteCliMsg;

#if 0
typedef struct  S_L3MacMsgHead
{
	UInt32 mNum;          /* MagicNumber,固定值 ,0xDDCCBBAA,如果不是，则忽略接收此消息*/
	UInt32 tLen;          /* 消息包的长度（包含消息头），以BYTE为单位 */
	UInt32 sno;           /* 帧序号,从0x00开始递增，最大值为0xffffffff,发送和应答消息的对应关系是：应答SNO = 发送SNO+1 */
	UInt8  attr;          /* 操作码，ATTR（操作的应答属性，，0xff 要回复,0xfe 不需回复) */
	UInt8  common;        /* COMMOM(公共信息字段，指示消息发送者，0x01->L1; 0x02->L2; 0x03->L3; 0x04->S1; )*/
	UInt16 opc;           /* OPC(消息的具体类型，16bit)*/
	UInt32 ueId;          /* 与UE有关时使用，UE的唯一标识*/
	UInt8  cellId;        /* Cell ID */
	UInt8  rbId;          /* Rb ID 在Configuation 流程中无效，填 0xff  */
	UInt16 reserved;      /* 保留  */
	UInt32 handle;        /* 由L3随机分配，Cmac在Response时带上，唯一确认是本Request的回应 */
	UInt32 reserved1;      /* 128bit对齐保留位 */
}S_L3MacMsgHead;

typedef struct S_L3MacCfgRsp
{
	UInt32           errorCode;        /* 见interfaceMacoDef.h中消息错误类型定义*/
}S_L3MacCfgRsp;
#endif

typedef struct {
    UInt32 logLevel;
} SetLogLevelReq;

typedef struct {
	UInt32 value;
} SetRAT2Type;

typedef struct {
    UInt32 numUe;
    UInt32 numTestTime;
} SetSIMParamReq;

typedef struct {	
    UInt32 numUe;
    UInt32 rach;
    UInt32 rar;
    UInt32 msg3;
    UInt32 contResol;
    UInt32 crcValid;
    UInt32 harqAckRecvd;
    UInt32 msg3Expired;
    UInt32 crcError;
    UInt32 harqNackRecvd;  // harq nack for all dl sch including contention resolution
    UInt32 contNack;  // harq nack for contention resolution
    
	UInt32 rrcReq;
	UInt32 rrcSetup;
	UInt32 rrcSetupComplete;
	UInt32 identityReq;
	UInt32 identityResp;
	UInt32 attachReject;
	UInt32 tauReject;
	UInt32 rrcRelease;
    UInt32 rrcReestablishmentReq;
    UInt32 rrcConnReject;
    UInt32 rrcReestabReject;
    UInt32 ueInfoReq;
    UInt32 ueInfoRsp;
    UInt32 rrcReconfig;
    UInt32 rrcReconfigCompl;

    UInt32 rrcReestabCompl;
    UInt32 rrcReestab;
    UInt32 ulDCCH;
    UInt32 dlDCCH;
    
#ifndef KPI_L3
    UInt32 harqDtx;
    UInt32 harqOther;
    
    UInt32 maxActiveMacUe;
    UInt32 activeRlcUe;
    UInt32 maxActiveRlcUe;
    UInt32 activePdcpUe;
    UInt32 maxActivePdcpUe;

    UInt32 harqAckSent;
    UInt32 harqNackSent;

#ifdef DEBUG_DL_PHY
    UInt32 harqAckPUCCH;
    UInt32 harqNackPUCCH;
    UInt32 harqDtxPUCCH;
    UInt32 dlTB[10];
#endif

    UInt32 createMacUeReq;
    UInt32 createMacUeCnf;
    UInt32 deleteMacUeReq;
    UInt32 deleteMacUeCnf;

    UInt32 createRlcUeReq;
    UInt32 createRlcUeCnf;
    UInt32 deleteRlcUeReq;
    UInt32 deleteRlcUeCnf;

    UInt32 createPdcpUeReq;
    UInt32 createPdcpUeCnf;
    UInt32 deletePdcpUeReq;
    UInt32 deletePdcpUeCnf;
#if 0
    UInt32 minTa;
    UInt32 maxTa;
    UInt32 avgTa;
    UInt32 avgSnr;
#endif
#endif
} LteCounter;

typedef struct {
    UInt16 rnti; 
    UInt8 crcCorrect;
    UInt8 crcError;
    UInt8 harqACKRecvd;
    UInt8 harqNACKRecvd;
    UInt8 harqDTXRecvd;
    UInt8 contHarqVal;
    UInt32 imsi0;
    UInt32 imsi1;
    UInt16 rrcEstabDuration;
    UInt16 idRspDuration;
    UInt16 ueCtxDuration;
} UeCounter;
#define UE_COUNTER_SIZE 22 // UeCounter plus 2 bytes rnti plus 6 bytes duration
#define UE_KPI_MSG_LENGTH 4096 //(UE_COUNTER_SIZE*256)

typedef struct {

  UInt32 channelReq;
  UInt32 immediateAssign;
  UInt32 SABM;
  UInt32 RRsetupUA;
  UInt32 imsiIDReq;
  UInt32 imsiIDResp;
  UInt32 imeiIDReq;
  UInt32 imeiIDResp;
  UInt32 LacReject;
  UInt32 channelRelease;
  UInt32 DISC;
  UInt32 RRchannelUA;

} GSMCounter;

#endif
