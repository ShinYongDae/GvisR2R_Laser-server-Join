
// GvisR2R_LaserView.h : CGvisR2R_LaserView 클래스의 인터페이스
//

#pragma once

#include "Global/GlobalDefine.h"

#include "Dialog/DlgMyMsg.h"
#include "Dialog/DlgMsgBox.h"
#include "Dialog/DlgFrameHigh.h"
#include "Dialog/DlgInfo.h"
#include "Dialog/DlgMenu01.h"
#include "Dialog/DlgMenu02.h"
#include "Dialog/DlgUtil01.h"
#include "Dialog/DlgUtil02.h"

#include "Device/Motion.h"
#include "Device/Light.h"
#include "Device/Vision.h"
#include "Device/SR1000W.h"
#include "Device/MDX2500.h"
#include "Device/Engrave.h"

#include "Process/PtAlign.h"
#include "Process/ThreadTask.h"

#include "GvisR2R_LaserDoc.h"



#define LAMP_DELAY				5	// 5 * 100[mSec]
#define BUZZER_DELAY			2	// 5 * 100[mSec]

#define FROM_DOMARK0			10
#define FROM_DOMARK1			50
#define FROM_DISPDEFIMG			100

#define ERR_PROC				120	// m_nMkStAuto
#define REJECT_ST				200	// m_nMkStAuto
#define ERROR_ST				250	// m_nMkStAuto

#define MK_VERIFY				200	// m_nSetpAuto
#define MK_REVIEW				220	// m_nSetpAuto
#define LAST_PROC				250	// m_nSetpAuto
#define LAST_PROC_VS_ALL		300	// m_nSetpAuto
#define LAST_PROC_VS_UP			350	// m_nSetpAuto
#define LAST_PROC_VS_DN			400	// m_nSetpAuto
#define LOT_END					500	// m_nSetpAuto

#define ENG_ST					100	// m_nEngStAuto
#define ENG_2D_ST				150	// m_nEngStAuto

#define TIM_INIT_VIEW				0
#define TIM_BUZZER_WARN				12
#define TIM_MB_TIME_OUT				13
#define TIM_DISP_STATUS				14
#define TIM_MPE_IO					15
#define TIM_SHOW_MENU01				18
#define TIM_SHOW_MENU02				19
#define TIM_CHK_TEMP_STOP			20
#define TIM_SAFTY_STOP				21
#define TIM_TCPIP_UPDATE			22
#define TIM_START_UPDATE			100
#define TIM_MENU01_UPDATE_WORK		101
#define TIM_CHK_RCV_MON_DISP_MAIN_SIG	201

#define MAX_THREAD				3

namespace Read2dIdx
{
	typedef enum Index {
		Start = 2, ChkSn = 4, InitRead = 10, Move0Cam1 = 12, Move0Cam0 = 14, Align1_0 = 17, Align0_0 = 18,
		Move1Cam1 = 21, Move1Cam0 = 23, Align1_1 = 26, Align0_1 = 27, MoveInitPt = 29, ChkElec = 32, DoRead = 35,
		Verify = 37, DoneRead = 38, LotDiff = 50
	};
}

namespace Mk1PtIdx
{
	typedef enum Index {
		Start = 2, ChkSn = 4, InitMk = 10, Move0Cam1 = 12, Move0Cam0 = 14, Align1_0 = 17, Align0_0 = 18,
		Move1Cam1 = 21, Move1Cam0 = 23, Align1_1 = 26, Align0_1 = 27, MoveInitPt = 29, ChkElec = 32, DoMk = 35,
		Verify = 37, DoneMk = 38, LotDiff = 50
	};
}

namespace Mk2PtIdx
{
	typedef enum Index {
		Start = 2, ChkSn = 4, InitMk = 10, Move0Cam1 = 12, Move0Cam0 = 14, Align1_0 = 17, Align0_0 = 18,
		Move1Cam1 = 21, Move1Cam0 = 23, Align1_1 = 26, Align0_1 = 27, MoveInitPt = 29, ChkElec = 32, DoMk = 35,
		Verify = 37, DoneMk = 38, LotDiff = 50, Shift2Mk = 60
	}; 
}

namespace Mk4PtIdx
{
	typedef enum Index {
		Start = 2, ChkSn = 4, InitMk = 10, Move0Cam1 = 12, Move0Cam0 = 14, Align1_0 = 17, Align0_0 = 18,
		Move1Cam1 = 21, Move1Cam0 = 23, Align1_1 = 26, Align0_1 = 27, Move2Cam1 = 29, Move2Cam0 = 31,
		Align1_2 = 34, Align0_2 = 35, Move3Cam1 = 37, Move3Cam0 = 39, Align1_3 = 42, Align0_3 = 43,
		MoveInitPt = 45, ChkElec = 48, DoMk = 51,
		Verify = 53, DoneMk = 54, LotDiff = 60, Shift2Mk = 70
	};
}

typedef struct _DispMain
{
	CString strMsg;
	COLORREF rgb;

	_DispMain()
	{
		Init();
	}
	_DispMain(CString sMsg, COLORREF Rgb)
	{
		strMsg = sMsg;
		rgb = Rgb;
	}

	void Init()
	{
		strMsg = _T("");
		rgb = RGB_WHITE;
	}

}stDispMain;
typedef CArray<stDispMain, stDispMain> CArDispMain;



class CGvisR2R_LaserView : public CFormView
{
	BOOL m_bEngStop;

	BOOL m_bDestroyedView;
	BOOL m_bBufEmpty[2];

	CDlgMyMsg* m_pDlgMyMsg;
	CCriticalSection m_csMyMsgBox;
	CCriticalSection m_csDispMain;
	BOOL m_bAnswer[10];
	int m_nDummy[2], m_nAoiLastSerial[2]; //[0]: Up, [1]: Dn
	BOOL m_bChkLastProcVs;
	int m_nChkBufIdx[2];

	double m_dTempPauseLen;
	DWORD m_dwCycSt, m_dwCycTim;

	BOOL m_bRtnMyMsgBox[4]; 	// [0] mk0, [1] mk1, [2] reject0, [3] reject1
	int m_nRtnMyMsgBox[4]; 	// [0] mk0, [1] mk1, [2] reject0, [3] reject1
	int m_nRtnMyMsgBoxIdx;

	int m_nPrevStepAuto, m_nPrevMkStAuto;
	int m_nMkPcs[4]; 	// [0] Auto-Left, [1] Auto-Right, [2] Manual-Left, [3] Manual-Right 
	int m_nErrCnt;
	int m_nStepInitView;

	BOOL m_bDispMsg, m_bWaitClrDispMsg;
	BOOL m_bDispMsgDoAuto[10];
	int m_nStepDispMsg[10];
	CString m_sFixMsg[2]; //[0]:up , [1]:dn

	int m_nCntBz;
	BOOL m_bTimBuzzerWarn;
	BOOL m_bTIM_DISP_STATUS, m_bTIM_MPE_IO;

	CString m_sPrevMyMsg;
	CString m_sTick, m_sDispTime;
	DWORD m_dwSetDlySt[10], m_dwSetDlyEd[10];
	DWORD m_dwSetDlySt0[10], m_dwSetDlyEd0[10];
	DWORD m_dwSetDlySt1[10], m_dwSetDlyEd1[10];

	CString m_sShare[2], m_sBuf[2]; // [0]: AOI-Up , [1]: AOI-Dn
	int m_pBufSerial[2][100], m_nBufTot[2]; // [0]: AOI-Up , [1]: AOI-Dn
	__int64 m_nBufSerialSorting[2][100]; // [0]: AOI-Up , [1]: AOI-Dn
	int m_nBufSerial[2][2]; // [0]: AOI-Up , [1]: AOI-Dn // [0]: Cam0, [1]:Cam1

	CString m_sDispMsg[10];
	double m_dTotVel, m_dPartVel;
	BOOL m_bTIM_CHK_TEMP_STOP;
	BOOL m_bTIM_SAFTY_STOP;
	BOOL m_bTIM_TCPIP_UPDATE;
	BOOL m_bTIM_START_UPDATE;
	CString m_sMyMsg; int m_nTypeMyMsg;
	int m_nVsBufLastSerial[2];
	BOOL m_bOpenShareUp, m_bOpenShareDn;

	BOOL m_bStopFeeding;
	BOOL m_bChkLightErr;

	int m_nTotMk[2], m_nCurMk[2]; // [0]: 좌 MK, [1]: 우 MK
	int m_nPrevTotMk[2], m_nPrevCurMk[2]; // [0]: 좌 MK, [1]: 우 MK

	BOOL m_bContEngraveF;
	CString m_sMsg;


	void InitMyMsg();
	void CloseMyMsg();

	void DispStsBar();
	void ExitProgram();
	void HideAllDlg();
	void DelAllDlg();
	BOOL HwInit();
	BOOL TcpIpInit();
	void HwKill();
	void ThreadInit();
	void ThreadKill();
	void DispTime();
	void Init();
	BOOL InitAct();
	void SwJog(int nAxisID, int nDir, BOOL bOn = TRUE);
	int GetVsBufLastSerial();
	int GetVsUpBufLastSerial();
	int GetVsDnBufLastSerial();

	void DispStsMainMsg(int nIdx = 0);


	BOOL SortingInUp(CString sPath, int nIndex);
	BOOL SortingOutUp(int* pSerial, int nTot);
	void SwapUp(__int64 *num1, __int64 *num2);
	BOOL SortingInDn(CString sPath, int nIndex);
	BOOL SortingOutDn(int* pSerial, int nTot);
	void SwapDn(__int64 *num1, __int64 *num2);

	void MoveInitPos0(BOOL bWait=TRUE);

	void DoAutoEng();
	void DoAutoMarking();
	void DoAtuoGetEngStSignal();
	void DoAtuoGet2dReadStSignal();
	void DoAuto2dReading();

protected: // serialization에서만 만들어집니다.
	CGvisR2R_LaserView();
	DECLARE_DYNCREATE(CGvisR2R_LaserView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_GVISR2R_LASER_FORM };
#endif

// 특성입니다.
public:
	CGvisR2R_LaserDoc* GetDocument() const;

	BOOL m_bStopFromThread, m_bBuzzerFromThread;
	DWORD m_dwRead2dSt, m_dwRead2dEd;

	CPtAlign m_Align[2];	// [0] : LeftCam , [1] : RightCam
#ifdef USE_VISION
	CVision* m_pVision[2];	// [0] : LeftCam , [1] : RightCam
	CVision* m_pVisionInner[2];	// [0] : LeftCam , [1] : RightCam
#endif
	CMotion* m_pMotion;
	CLight* m_pLight;
	CDlgMsgBox* m_pDlgMsgBox;
	CEvent      m_evtWaitClrDispMsg;

	CDlgInfo *m_pDlgInfo;
	CDlgFrameHigh *m_pDlgFrameHigh;
	CDlgMenu01 *m_pDlgMenu01;
	CDlgMenu02 *m_pDlgMenu02;
	CDlgUtil01 *m_pDlgUtil01;


	int m_nLotEndSerial;

	BOOL m_bTIM_MENU01_UPDATE_WORK;
	BOOL m_bTIM_INIT_VIEW;
	BOOL m_bCam, m_bReview;

	BOOL m_bTIM_CHK_RCV_MON_DISP_MAIN_SIG;

	DWORD m_dwThreadTick[MAX_THREAD];
	BOOL m_bThread[MAX_THREAD];
	CThreadTask m_Thread[MAX_THREAD];

	double m_dEnc[MAX_AXIS], m_dTarget[MAX_AXIS];
	double m_dNextTarget[MAX_AXIS];
	int m_nSelRmap, m_nSelRmapInner;
	int m_nStepAuto;

	int m_nStop;
	BOOL m_bReMk;
	BOOL m_bChkMpeIoOut;

	BOOL m_bProbDn[2]; // 좌/우 .

	// Auto Sequence
	BOOL m_bAuto, m_bManual, m_bOneCycle;
	BOOL m_bMkTmpStop, m_bAoiLdRun, m_bAoiLdRunF;
	BOOL m_bTHREAD_MK[4];	// [0] Auto-Left, [1] Auto-Right, [2] Manual-Left, [3] Manual-Right
	BOOL m_bTHREAD_DISP_DEF;
	int	m_nStepTHREAD_DISP_DEF;

	BOOL m_bTHREAD_UPDATAE_YIELD[2];		// [0] : Cam0, [1] : Cam1
	int	m_nSerialTHREAD_UPDATAE_YIELD[2];	// [0] : Cam0, [1] : Cam1
	BOOL m_bTHREAD_SHIFT2MK;// [2];		// [0] : Cam0, [1] : Cam1
	BOOL m_bTHREAD_UPDATE_REELMAP_UP, m_bTHREAD_UPDATE_REELMAP_ALLUP;
	BOOL m_bTHREAD_UPDATE_REELMAP_DN, m_bTHREAD_UPDATE_REELMAP_ALLDN;
	BOOL m_bTHREAD_UPDATE_RST_UP, m_bTHREAD_UPDATE_RST_ALLUP;
	BOOL m_bTHREAD_UPDATE_RST_DN, m_bTHREAD_UPDATE_RST_ALLDN;
	BOOL m_bTHREAD_RELOAD_RST_UP, m_bTHREAD_RELOAD_RST_ALLUP;
	BOOL m_bTHREAD_RELOAD_RST_DN, m_bTHREAD_RELOAD_RST_ALLDN;
	BOOL m_bTHREAD_RELOAD_RST_UP_INNER, m_bTHREAD_RELOAD_RST_ALLUP_INNER;
	BOOL m_bTHREAD_RELOAD_RST_DN_INNER, m_bTHREAD_RELOAD_RST_ALLDN_INNER;
	BOOL m_bTHREAD_RELOAD_RST_ITS, m_bTHREAD_UPDATE_RST_ITS;

	void UpdateLotTime();
	void AdjLaserOffset(CfPoint ptOffset);
	CString ShowKeypad1();

	BOOL m_bSwRun, m_bSwRunF;
	BOOL m_bSwStop, m_bSwStopF;
	BOOL m_bSwReset, m_bSwResetF;
	BOOL m_bSwReady, m_bSwReadyF;

	BOOL m_bNewModel;
	DWORD m_dwLotSt, m_dwLotEd;
	long m_lFuncId;
	BOOL m_bDrawGL;
	BOOL m_bCont;
	DWORD m_dwStMkDn[2];
	BOOL m_bInit;
	BOOL m_bSwStopNow;
	BOOL m_bShowModalMyPassword;

	int m_nRstNum;

	BOOL m_bChkBufIdx[2];
	BOOL m_bBufHomeDone, m_bReadyDone;
	BOOL m_bEngBufHomeDone;

	unsigned long m_Flag;
	unsigned long m_AoiLdRun;
	BOOL m_bDoneDispMkInfo[2][2]; // [nCam][Up/Dn]

	int m_nShareUpS, m_nShareUpSprev;
	int m_nShareUpSerial[2]; // [nCam]
	int m_nShareDnS, m_nShareDnSprev;
	int m_nShareDnSerial[2]; // [nCam]
	int m_nShareUpCnt;
	int m_nShareDnCnt;

	int m_nBufUpSerial[2]; // [nCam]
	int m_nBufDnSerial[2]; // [nCam]
	int m_nBufUpCnt;
	int m_nBufDnCnt;

	BOOL m_bFailAlign[2][4]; // [nCam][nPos] 
	BOOL m_bReAlign[2][4]; // [nCam][nPos] 
	BOOL m_bSkipAlign[2][4]; // [nCam][nPos] 

	BOOL m_bDoMk[2];			// [nCam] : TRUE(Punching), FALSE(Stop Punching)
	BOOL m_bDoneMk[2];			// [nCam] : TRUE(Punching 완료), FALSE(Punching 미완료)
	BOOL m_bReMark[2];			// [nCam] : TRUE(Punching 다시시작), FALSE(pass)

	int m_nMonAlmF, m_nClrAlmF;
	BOOL m_bLotEnd, m_bLastProc, m_bLastProcFromUp;
	BOOL m_bMkSt, m_bMkStSw;
	BOOL m_bEngSt, m_bEngStSw;
	BOOL m_bEng2dSt, m_bEng2dStSw;
	int m_nMkStAuto, m_nEngStAuto, m_nEng2dStAuto;
	int m_nLotEndAuto, m_nLastProcAuto;
	int m_nCntSkipError2dCode;
	BOOL m_bLoadShare[2]; // [Up/Dn]
	CString m_sNewLotUp, m_sNewLotDn;

	BOOL m_bAoiFdWrite[2], m_bAoiFdWriteF[2]; // [Up/Dn]
	BOOL m_bAoiTest[2], m_bAoiTestF[2], m_bWaitPcr[2]; // [Up/Dn]

	BOOL m_bCycleStop, m_bContDiffLot;
	CString m_sDispMain;
	BOOL m_bStopF_Verify;

	BOOL m_bShowMyMsg;
	CWnd *m_pMyMsgForeground;


	CString m_sDispSts[2];

	BOOL m_bDispMyMsgBox;
	CArMyMsgBox  m_ArrayMyMsgBox;
	BOOL m_bDispMain;
	CArDispMain  m_ArrayDispMain;

	int m_nWatiDispMain;

	CSr1000w* m_pSr1000w;
	CMdx2500* m_pMdx2500;
	CEngrave* m_pEngrave;

	int m_nNewLot;
	CString m_sMonDisp;

	CString m_sPathRmapUpdate[4];
	int m_nSerialRmapUpdate;

	BOOL m_bJobEnd;

// 작업입니다.
public:
	BOOL m_bShift2Mk;

	afx_msg LRESULT OnDlgInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMyMsgExit(WPARAM wPara, LPARAM lPara);

	int MsgBox(CString sMsg, int nThreadIdx = 0, int nType = MB_OK, int nTimOut = DEFAULT_TIME_OUT);		// SyncMsgBox
	int AsyncMsgBox(CString sMsg, int nThreadIdx = 1, int nType = MB_OK, int nTimOut = DEFAULT_TIME_OUT); // AsyncMsgBox
	int WaitRtnVal(int nThreadIdx = 1);
	void Buzzer(BOOL bOn, int nCh = 0);
	void StopFromThread();
	void BuzzerFromThread(BOOL bOn, int nCh = 0);

	BOOL WatiDispMain(int nDelay);

	void RestoreReelmap();
	CString GetProcessNum();

	void DispMain(CString sMsg, COLORREF rgb = RGB(0, 255, 0));
	int DoDispMain();
	void GetDispMsg(CString &strMsg, CString &strTitle);
	void DispMsg(CString strMsg, CString strTitle = _T(""), COLORREF color = RGB(255, 0, 0), DWORD dwDispTime = 0, BOOL bOverWrite = TRUE);
	void ClrDispMsg();
	BOOL WaitClrDispMsg();
	LONG OnQuitDispMsg(UINT wParam, LONG lParam);
	void ShowDlg(int nID);

	void TowerLamp(COLORREF color, BOOL bOn, BOOL bWink = FALSE);
	int MyPassword(CString strMsg, int nCtrlId = 0);

	void GetEnc();
	void SetAoiFdPitch(double dPitch);
	void SetMkFdPitch(double dPitch);

	void DoIO();
	void DispDefImg();
	void Option01();

	BOOL ChkVsShare(int &nSerial);
	BOOL ChkVsShareUp(int &nSerial);
	BOOL ChkVsShareDn(int &nSerial);
	void ChkShare();
	void ChkShareUp();
	void ChkShareDn();
	BOOL ChkShare(int &nSerial);
	BOOL ChkShareUp(int &nSerial);
	BOOL ChkShareDn(int &nSerial);
	BOOL ChkShareIdx(int *pBufSerial, int nBufTot, int nShareSerial);
	void ChkBuf();
	void ChkBufUp();
	void ChkBufDn();
	void ChkEmg();
	BOOL ChkBufUp(int* pSerial, int &nTot);
	BOOL ChkBufDn(int* pSerial, int &nTot);
	BOOL ChkBufIdx(int* pSerial, int nTot = 0);
	void ChkMRegOut();


	CString GetTime();
	CString GetTime(stLotTime &LotTime);
	CString GetTime(int &nHour, int &nMinute, int &nSec);
	int GetTime(int nSel);
	void SetBufHomeParam(double dVel, double dAcc);
	void DispLotStTime();
	void SetListBuf();

	static UINT ThreadProc0(LPVOID lpContext); // Safety check thread procedure
	static UINT ThreadProc1(LPVOID lpContext); // Safety check thread procedure
	static UINT ThreadProc2(LPVOID lpContext); // Safety check thread procedure

	void Shift2Buf();
	void Shift2Mk();
	void SetDelay(int mSec, int nId = 0);
	BOOL WaitDelay(int nId = 0);				// F:Done, T:On Waiting....
	void SetDelay0(int mSec, int nId = 0);
	BOOL WaitDelay0(int nId = 0);				// F:Done, T:On Waiting....
	BOOL GetDelay(int &mSec, int nId = 0);	// F:Done, T:On Waiting....
	BOOL GetDelay0(int &mSec, int nId = 0);	// F:Done, T:On Waiting....
	void UpdateWorking();
	void Stop();
	void ShowLive(BOOL bShow = TRUE);
	void SetLotSt();
	void SetLotEd();
	void DispLotTime();

	BOOL GetDelay1(int &mSec, int nId = 0);	// F:Done, T:On Waiting....
	void SetDelay1(int mSec, int nId = 0);
	BOOL WaitDelay1(int nId = 0);				// F:Done, T:On Waiting....

	BOOL IsStop();
	BOOL IsRun();

	double GetMkFdLen();
	double GetTotVel();
	double GetPartVel();
	void SetCycTime();
	int GetCycTime(); // [mSec]

	BOOL IsShare();
	BOOL IsShareUp();
	BOOL IsShareDn();
	BOOL IsVsShare();
	int GetShareUp();
	int GetShareDn();

	BOOL IsBuffer(int nNum = 0);
	BOOL IsBufferUp();
	BOOL IsBufferDn();
	int GetBuffer(int *pPrevSerial = NULL);
	int GetBufferUp(int *pPrevSerial = NULL);
	int GetBufferDn(int *pPrevSerial = NULL);

	BOOL IsBuffer0();
	BOOL IsBufferUp0();
	BOOL IsBufferDn0();
	int GetBuffer0(int *pPrevSerial = NULL);
	int GetBufferUp0(int *pPrevSerial = NULL);
	int GetBufferDn0(int *pPrevSerial = NULL);

	BOOL IsBuffer1();
	BOOL IsBufferUp1();
	BOOL IsBufferDn1();
	int GetBuffer1(int *pPrevSerial = NULL);
	int GetBufferUp1(int *pPrevSerial = NULL);
	int GetBufferDn1(int *pPrevSerial = NULL);

	BOOL IsAuto();
	BOOL SetSerial(int nSerial, BOOL bDumy = FALSE);

	int GetErrCode(int nSerial);
	int GetErrCodeUp(int nSerial);
	int GetErrCodeDn(int nSerial);

	int GetErrCode0(int nSerial);
	int GetErrCodeUp0(int nSerial);
	int GetErrCodeDn0(int nSerial);

	int GetErrCode1(int nSerial);
	int GetErrCodeUp1(int nSerial);
	int GetErrCodeDn1(int nSerial);

	int GetTotDefPcs(int nSerial);
	int GetTotDefPcsUp(int nSerial);
	int GetTotDefPcsDn(int nSerial);

	int GetTotDefPcs0(int nSerial);
	int GetTotDefPcsUp0(int nSerial);
	int GetTotDefPcsDn0(int nSerial);

	int GetTotDefPcs1(int nSerial);
	int GetTotDefPcsUp1(int nSerial);
	int GetTotDefPcsDn1(int nSerial);

	CfPoint GetMkPnt(int nMkPcs);
	CfPoint GetMkPnt0(int nMkPcs);
	CfPoint GetMkPnt1(int nMkPcs);
	CfPoint GetMkPnt0(int nSerial, int nMkPcs);
	CfPoint GetMkPnt1(int nSerial, int nMkPcs);

	void Move0(CfPoint pt, BOOL bCam = FALSE);
	BOOL IsMoveDone();
	BOOL IsMoveDone0();
	BOOL UpdateReelmap(int nSerial);

	void InitInfo();
	void InitReelmap();
	void InitReelmapUp();
	void InitReelmapDn();
	BOOL IsPinMkData();
	BOOL IsPinData();
	BOOL CopyDefImg(int nSerial);
	BOOL CopyDefImg(int nSerial, CString sNewLot);
	BOOL CopyDefImgUp(int nSerial, CString sNewLot = _T(""));
	BOOL CopyDefImgDn(int nSerial, CString sNewLot = _T(""));

	BOOL MovePinPos();
	BOOL OnePointAlign(CfPoint &ptPnt);
	BOOL StartLive();
	BOOL StopLive();

	void MoveMkInitPos();
	void ResetMkInfo(int nAoi = 0); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
	void ClrMkInfo();

	BOOL IsHomeDone(int nMsId);
	BOOL GetAoiUpInfo(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	BOOL GetAoiDnInfo(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	BOOL ChkLotEnd(int nSerial);
	BOOL ChkLotEndUp(int nSerial);
	BOOL ChkLotEndDn(int nSerial);
	BOOL LoadMySpec();
	BOOL MemChk();
	BOOL GetAoiUpOffset(CfPoint &OfSt);
	BOOL GetAoiDnOffset(CfPoint &OfSt);
	BOOL GetMkOffset(CfPoint &OfSt);
	BOOL IsAoiLdRun();

	void SetLotEnd(int nSerial);
	int GetLotEndSerial();
	void ModelChange(int nAoi = 0); // 0 : AOI-Up , 1 : AOI-Dn
	void UpdateRst();
	int GetAutoStep();
	BOOL IsShowLive();
	BOOL IsChkTmpStop();
	BOOL ChkLastProc();
	double GetAoiUpFdLen();
	double GetAoiDnFdLen();
	BOOL IsVerify();
	BOOL IsFixPcsUp(int nSerial);
	BOOL IsFixPcsDn(int nSerial);
	BOOL IsReview();
	BOOL IsReview0();
	BOOL IsReview1();
	void OpenShareUp(BOOL bOpen = TRUE);
	void OpenShareDn(BOOL bOpen = TRUE);
	BOOL IsOpenShareUp();
	BOOL IsOpenShareDn();
	void ResetMotion();
	void ResetMotion(int nMsId);
	unsigned long ChkDoor(); // 0: All Closed , Open Door Index : Doesn't all closed. (Bit3: F, Bit2: L, Bit1: R, Bit0; B)
	BOOL ChkSaftySen();
	BOOL ChkYield();
	void SwAoiEmg(BOOL bOn);
	BOOL IsVs();
	BOOL IsVsUp();
	BOOL IsVsDn();
	BOOL GetAoiUpVsStatus();
	BOOL GetAoiDnVsStatus();
	BOOL IsDoneDispMkInfo();
	BOOL IsSetLotEnd();
	void ChkRcvSig();
	void ChkErrorRead2dCode();

	int ChkSerial(); // 0: Continue, -: Previous, +:Discontinue
	BOOL ReloadRst();
	void OpenReelmap();
	void OpenReelmapUp();
	void OpenReelmapDn();
	BOOL IsRunAxisX();
	void EStop();
	void SetAlignPos();
	void SetAlignPosUp();
	void SetAlignPosDn();
	void IoWrite(CString sMReg, long lData);


	BOOL LoadPcrUp(int nSerial, BOOL bFromShare = FALSE);
	BOOL LoadPcrDn(int nSerial, BOOL bFromShare = FALSE);

	void SetLastProc();
	BOOL IsLastProc();
	BOOL IsLastJob(int nAoi); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn

	void MonDispMain();

	void ChkTempStop(BOOL bChk);
	void ChgLot();

	void SetLotLastShot();

	BOOL IsMkStrip(int nStripIdx);
	void CycleStop();
	BOOL ChkLotCutPos();
	BOOL OpenReelmapFromBuf(int nSerial);

	void SetPathAtBuf();
	void SetPathAtBufUp();
	void SetPathAtBufDn();
	void LoadPcrFromBuf();

	BOOL SetSerialReelmap(int nSerial, BOOL bDumy = FALSE);
	BOOL SetSerialMkInfo(int nSerial, BOOL bDumy = FALSE);
	BOOL ChkLastProcFromUp();

	void CntMk();
	void ChkMyMsg();
	BOOL ReloadRst(int nSerial);
	BOOL IsSameUpDnLot();
	BOOL ChkStShotNum();
	BOOL ChkContShotNum();
	void SetFixPcs(int nSerial);

	BOOL RemakeReelmap();
	BOOL IsDoneRemakeReelmap();

	BOOL ChkLightErr();
	BOOL IsOnMarking0();
	BOOL IsOnMarking1();

	void SetDualTest(BOOL bOn = TRUE);
	void SetTwoMetal(BOOL bSel, BOOL bOn = TRUE);
	void DispStsBar(CString sMsg, int nIdx = 0);
	void AdjPinPos();

	int GetMkStripIdx0(int nDefPcsId); // 0 : Fail , 1~4 : Strip Idx
	int GetMkStripIdx1(int nDefPcsId); // 0 : Fail , 1~4 : Strip Idx
	int GetMkStripIdx0(int nSerial, int nMkPcs); // 0 : Fail , 1~4 : Strip Idx
	int GetMkStripIdx1(int nSerial, int nMkPcs); // 0 : Fail , 1~4 : Strip Idx

	void MoveMk0InitPos();
	BOOL MoveAlign0(int nPos);
	BOOL IsInitPos0();
	BOOL StartLive0();
	BOOL StopLive0();

	BOOL IsEngraveFdSts();
	BOOL IsEngraveFd();
	void SetEngraveFdSts();
	void SetEngraveStopSts();
	void SetEngraveSts(int nStep);

	double GetEngraveFdLen();
	void SetEngraveFdPitch(double dPitch);
	BOOL IsConnected();
	void DestroyView();

	BOOL IsConnectedMdx();
	BOOL IsConnectedSr();
	BOOL IsConnectedMk();
	BOOL IsDispContRun();
	BOOL IsDispLotEnd();

	BOOL IsPinPos0();

	void InitAutoEng();
	void InitAutoEngSignal();
	void MarkingWith1PointAlign();
	void CheckCurrentInfoSignal(int nMsgID, int nData);
	void CheckMonDispMainSignal();

	void Eng1PtReady();
	void Eng1PtInit();
	void Eng1PtAlignPt0();
	void Eng1PtDoMarking();

	void Eng2dRead();

	void AdjPinPosEng();
	BOOL OnePointAlign0(int nPos);
	BOOL SetMk(BOOL bRun = TRUE);
	BOOL SetMdxLotAndShotNum(CString sLot, int nSerial);
	BOOL IsMkDone();

	BOOL Set2dRead(BOOL bRun = TRUE);
	BOOL Is2dReadDone();


	int m_nReloadRstSerial;
	void ReloadRstUp();
	void ReloadRstAllUp();
	void ReloadRstDn();
	void ReloadRstAllDn();

	void ReloadRstUpInner();
	void ReloadRstAllUpInner();
	void ReloadRstDnInner();


	BOOL m_bSetSig, m_bSetSigF, m_bSetData, m_bSetDataF;
	BOOL m_bLoadMstInfo, m_bLoadMstInfoF;
	void LoadMstInfo();

	//void CompletedMk(int nCam); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None

	void SetMyMsgYes();
	void SetMyMsgNo();
	void SetMyMsgOk();

	BOOL SetEngOffset(CfPoint &OfSt);
	void EngStop(BOOL bOn);
	BOOL IsEngStop();
	BOOL GetCurrentInfoSignal();
	BOOL GetMonDispMainSignal();
	int GetLastSerialEng();
	void SetLastSerialEng(int nSerial);
	CString GetCurrentInfoBufUp();
	CString GetCurrentInfoBufDn();
	void SetCurrentInfoEngShotNum(int nSerial);
	void SetCurrentInfoReadShotNum(int nSerial);

	CString m_sGetItsCode;
	int m_nGetItsCodeSerial;
	BOOL Get2dCode(CString &sItsCode, int &nSerial);

	void SetTotOpRto(CString sVal);		// 전체진행율
	void SetTotVel(CString sVal);		// 전체속도
	void SetPartVel(CString sVal);		// 구간속도
	void SetMkDoneLen(CString sVal);	// 마킹부 : Distance (FdDone) [M]
	void SetAoiDnDoneLen(CString sVal);	// 검사부(하) : Distance (FdDone) [M]
	void SetAoiUpDoneLen(CString sVal);	// 검사부(상) : Distance (FdDone) [M]
	void SetEngDoneLen(CString sVal);	// 각인부 : Distance (FdDone) [M]

	void DispStatusBar(CString strMsg, int nStatusBarID);
	void GetMkMenu01();
	CString GetMkMenu01(CString sMenu, CString sItem);
	CString GetMkDispInfoUp(CString sMenu, CString sItem, int nSerial);
	CString GetMkDispInfoDn(CString sMenu, CString sItem, int nSerial);

	BOOL m_bTHREAD_DISP_DEF_INNER;
	int	m_nStepTHREAD_DISP_DEF_INNER;

	void InitReelmapInner();
	void InitReelmapInnerUp();
	void InitReelmapInnerDn();
	BOOL ReloadRstInner();
	BOOL ReloadRstInner(int nSerial);
	void UpdateRstInner();
	void OpenReelmapInner();
	void OpenReelmapInnerUp();
	void OpenReelmapInnerDn();
	void DispDefImgInner();
	BOOL SetSerialReelmapInner(int nSerial, BOOL bDumy = FALSE);
	BOOL SetSerialMkInfoInner(int nSerial, BOOL bDumy = FALSE);

	void SetErrorRead2dCode(int nMcId); // PLC에 각인부 알람상태 ON

	CString GetTimeIts();

	void SwReset();
	BOOL DoReset();
	void EngAutoInit();
	void EngAutoInitCont();

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

	afx_msg LRESULT wmClientReceivedMdx(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT wmClientReceivedSr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT wmServerReceived(WPARAM wParam, LPARAM lParam);

// 구현입니다.
public:
	virtual ~CGvisR2R_LaserView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // GvisR2R_LaserView.cpp의 디버그 버전
inline CGvisR2R_LaserDoc* CGvisR2R_LaserView::GetDocument() const
   { return reinterpret_cast<CGvisR2R_LaserDoc*>(m_pDocument); }
#endif

