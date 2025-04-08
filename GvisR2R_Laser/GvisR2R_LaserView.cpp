
// GvisR2R_LaserView.cpp : CGvisR2R_LaserView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "GvisR2R_Laser.h"
#endif

#include "GvisR2R_LaserDoc.h"
#include "GvisR2R_LaserView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "MainFrm.h"

#include "Process/DataFile.h"

#include "Dialog/DlgOption01.h"
#include "Dialog/DlgMyPassword.h"
#include "Dialog/DlgMyMsgSub00.h"
#include "Dialog/DlgMyMsgSub01.h"
#include "Dialog/DlgMyMsgSub02.h"
#include "Dialog/DlgProgress.h"
#include "Dialog/DlgKeyNum1.h"

#include "Device/MDX2500DEF.h"
#include "Device/SR1000WDEF.h"


extern CMainFrame* pFrm;
extern CGvisR2R_LaserDoc* pDoc;
CGvisR2R_LaserView* pView;
extern CString PATH_WORKING_INFO;

// CGvisR2R_LaserView

IMPLEMENT_DYNCREATE(CGvisR2R_LaserView, CFormView)

BEGIN_MESSAGE_MAP(CGvisR2R_LaserView, CFormView)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DLG_INFO, OnDlgInfo)
	ON_MESSAGE(WM_MYMSG_EXIT, OnMyMsgExit)
	ON_MESSAGE(WM_CLIENT_RECEIVED_MDX, wmClientReceivedMdx)
	ON_MESSAGE(WM_CLIENT_RECEIVED_SR, wmClientReceivedSr)
	ON_MESSAGE(WM_SERVER_RECEIVED, wmServerReceived)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CGvisR2R_LaserView 생성/소멸

CGvisR2R_LaserView::CGvisR2R_LaserView()
	: CFormView(IDD_GVISR2R_LASER_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	int i = 0;

	pView = this;
	m_bJobEnd = FALSE;
	m_sMsg = _T("");

	m_bShift2Mk = FALSE;

	m_bBufEmpty[0] = FALSE;
	m_bBufEmpty[1] = FALSE;

	m_nWatiDispMain = 0;
	m_nNewLot = 0;


	m_bStopFromThread = FALSE;
	m_bBuzzerFromThread = FALSE;

	m_nWatiDispMain = 0;

	m_bStopFromThread = FALSE;
	m_bBuzzerFromThread = FALSE;

	m_nRtnMyMsgBoxIdx = -1;
	m_bDispMyMsgBox = FALSE;
	m_bDispMain = FALSE;
	m_bProbDn[0] = m_bProbDn[1] = FALSE;

	m_nSelRmap = RMAP_UP;
	m_nSelRmapInner = RMAP_INNER_UP;

	m_bTIM_INIT_VIEW = FALSE;
	m_nStepInitView = 0;
	m_nLotEndSerial = 0;

	m_nDummy[0] = 0;
	m_nDummy[1] = 0;
	m_nAoiLastSerial[0] = 0;
	m_nAoiLastSerial[1] = 0;
	m_bChkLastProcVs = FALSE;

	m_dTempPauseLen = 0.0;

	m_bInit = FALSE;
	m_bDispMsg = FALSE;
	for (i = 0; i < 10; i++)
	{
		m_bDispMsgDoAuto[i] = FALSE;
		m_nStepDispMsg[i] = 0;
	}
	m_sFixMsg[0] = _T("");
	m_sFixMsg[1] = _T("");

	m_bWaitClrDispMsg = FALSE;
	m_bOpenShareUp = TRUE;
	m_bOpenShareDn = TRUE;

	m_bStopFeeding = FALSE;

	m_pDlgMyMsg = NULL;
	m_pDlgMsgBox = NULL;

	m_pDlgInfo = NULL;
	m_pDlgFrameHigh = NULL;
	m_pDlgMenu01 = NULL;
	m_pDlgMenu02 = NULL;

	m_bTimBuzzerWarn = FALSE;

	m_bAoiFdWriteF[0] = FALSE;
	m_bAoiFdWriteF[1] = FALSE;
	m_bAoiTest[0] = FALSE;
	m_bAoiTest[1] = FALSE;


	// H/W Device 초기화.....
	m_pMotion = NULL;
	m_pLight = NULL;

#ifdef USE_VISION
	m_pVision[0] = NULL;
	m_pVision[1] = NULL;
#endif

	m_bTIM_DISP_STATUS = FALSE;

	m_bThread[0] = FALSE;
	m_dwThreadTick[0] = 0;
	m_bThread[1] = FALSE;
	m_dwThreadTick[1] = 0;
	m_bThread[2] = FALSE;
	m_dwThreadTick[2] = 0;

	m_bTIM_MPE_IO = FALSE;


	m_nStepAuto = 0;
	m_nPrevStepAuto = 0;
	m_nPrevMkStAuto = 0;

	m_sShare[0] = _T("");
	m_sBuf[0] = _T("");
	m_sShare[1] = _T("");
	m_sBuf[1] = _T("");

	m_sTick = _T("");
	m_sDispTime = _T("");

	m_bChkMpeIoOut = FALSE;

	m_bMkTmpStop = FALSE;
	m_bAoiLdRun = TRUE;
	m_bAoiLdRunF = FALSE;

	m_dwCycSt = 0;
	m_dwCycTim = 0;

	m_bTHREAD_DISP_DEF = FALSE;
	m_nStepTHREAD_DISP_DEF = 0;

	m_bTHREAD_MK[0] = FALSE;
	m_bTHREAD_MK[1] = FALSE;
	m_bTHREAD_MK[2] = FALSE;
	m_bTHREAD_MK[3] = FALSE;
	m_nMkPcs[0] = 0;
	m_nMkPcs[1] = 0;
	m_nMkPcs[2] = 0;
	m_nMkPcs[3] = 0;

	m_nErrCnt = 0;

	m_bAuto = FALSE;
	m_bManual = FALSE;
	m_bOneCycle = FALSE;

	m_bSwRun = FALSE; m_bSwRunF = FALSE;
	m_bSwStop = FALSE; m_bSwStopF = FALSE;
	m_bSwReset = FALSE; m_bSwResetF = FALSE;
	m_bSwReady = FALSE; m_bSwReadyF = FALSE;

	m_bSwStopNow = FALSE;

	for (int nAxis = 0; nAxis < MAX_AXIS; nAxis++)
		m_dEnc[nAxis] = 0.0;

	for (i = 0; i < 10; i++)
		m_sDispMsg[i] = _T("");

	m_bNewModel = FALSE;
	m_dTotVel = 0.0; m_dPartVel = 0.0;
	m_bTIM_CHK_TEMP_STOP = FALSE;
	m_bTIM_SAFTY_STOP = FALSE;
	m_bTIM_TCPIP_UPDATE = FALSE;
	m_bTIM_START_UPDATE = FALSE;
	m_bTIM_MENU01_UPDATE_WORK = FALSE;
	m_bTIM_CHK_RCV_MON_DISP_MAIN_SIG = FALSE;

	m_sMyMsg = _T("");
	m_nTypeMyMsg = IDOK;

	m_dwLotSt = 0; m_dwLotEd = 0;

	m_lFuncId = 0;

	m_bDrawGL = TRUE;
	m_bCont = FALSE;
	m_bCam = FALSE;
	m_bReview = FALSE;

	m_bChkBufIdx[0] = TRUE;
	m_nChkBufIdx[0] = 0;
	m_bChkBufIdx[1] = TRUE;
	m_nChkBufIdx[1] = 0;

	m_dwStMkDn[0] = 0;
	m_dwStMkDn[1] = 0;
	m_nVsBufLastSerial[0] = 0;
	m_nVsBufLastSerial[1] = 0;
	m_bShowModalMyPassword = FALSE;

	m_nRstNum = 0;
	m_bBufHomeDone = FALSE;
	m_bReadyDone = FALSE;

	m_Flag = 0L;
	m_AoiLdRun = 0L;

	m_bDoneDispMkInfo[0][0] = FALSE; // Cam0, Up
	m_bDoneDispMkInfo[0][1] = FALSE; // Cam0, Dn
	m_bDoneDispMkInfo[1][0] = FALSE; // Cam1, Up
	m_bDoneDispMkInfo[1][1] = FALSE; // Cam1, Dn

	m_nShareUpS = 0;
	m_nShareUpSerial[0] = 0;
	m_nShareUpSerial[1] = 0;
	m_nShareUpCnt = 0;

	m_nShareDnS = 0;
	m_nShareDnSerial[0] = 0;
	m_nShareDnSerial[1] = 0;
	m_nShareDnCnt = 0;

	m_nBufSerial[0][0] = 0; // Up-Cam0
	m_nBufSerial[0][1] = 0; // Up-Cam1
	m_nBufSerial[1][0] = 0; // Dn-Cam0
	m_nBufSerial[1][1] = 0; // Dn-Cam0

	m_bReAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bReAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][1] = FALSE; // [nCam][nPos] 

	m_bSkipAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][1] = FALSE; // [nCam][nPos] 

	m_bFailAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bFailAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][1] = FALSE; // [nCam][nPos] 

	m_bDoMk[0] = TRUE;
	m_bDoMk[1] = TRUE;
	m_bDoneMk[0] = FALSE;
	m_bDoneMk[1] = FALSE;
	m_bReMark[0] = FALSE;
	m_bReMark[1] = FALSE;

	m_nMonAlmF = 0;
	m_nClrAlmF = 0;

	m_bMkSt = FALSE;
	m_bMkStSw = FALSE;
	m_nMkStAuto = 0;

	m_bEngSt = FALSE;
	m_bEngStSw = FALSE;
	m_nEngStAuto = 0;

	m_bEng2dSt = FALSE;
	m_bEng2dStSw = FALSE;
	m_nEng2dStAuto = 0;
	m_nCntSkipError2dCode = 0;

	m_bLotEnd = FALSE;
	m_nLotEndAuto = 0;

	m_bLastProc = FALSE;
	m_bLastProcFromUp = TRUE;
	m_nLastProcAuto = 0;

	m_bLoadShare[0] = FALSE;
	m_bLoadShare[1] = FALSE;

	m_sNewLotUp = _T("");
	m_sNewLotDn = _T("");

	m_bAoiFdWrite[0] = FALSE;
	m_bAoiFdWrite[1] = FALSE;
	m_bAoiFdWriteF[0] = FALSE;
	m_bAoiFdWriteF[1] = FALSE;

	m_bCycleStop = FALSE;

	m_sDispMain = _T("");
	m_bReMk = FALSE;

	m_bWaitPcr[0] = FALSE;
	m_bWaitPcr[1] = FALSE;

	m_bShowMyMsg = FALSE;
	m_pMyMsgForeground = NULL;
	m_bContDiffLot = FALSE;

	// 	m_nMsgShiftX = 0;
	// 	m_nMsgShiftY = 0;

	for (int nAns = 0; nAns < 10; nAns++)
		m_bAnswer[nAns] = FALSE;

	m_bChkLightErr = FALSE;

	// client for SR-1000W
	m_pSr1000w = NULL;

	// client for MD-X2500
	m_pMdx2500 = NULL;

	// server for engrave
	m_pEngrave = NULL;

	m_bDestroyedView = FALSE;
	m_bContEngraveF = FALSE;

	m_bStopF_Verify = FALSE;

	m_bLoadMstInfo = FALSE;
	m_bLoadMstInfoF = FALSE;

	m_pDlgUtil01 = NULL;
	//m_pDlgUtil02 = NULL;
	m_bEngStop = FALSE;

	m_sGetItsCode = _T("");
	m_nGetItsCodeSerial = 0;

	m_bSetSig = FALSE;
	m_bSetSigF = FALSE;
	m_bSetData = FALSE;
	m_bSetDataF = FALSE;

	m_sMonDisp = _T("");
}

CGvisR2R_LaserView::~CGvisR2R_LaserView()
{
	DestroyView();
}

void CGvisR2R_LaserView::DestroyView()
{
	if (!m_bDestroyedView)
	{
		m_bDestroyedView = TRUE;

		ThreadKill();
		Sleep(30);

		DelAllDlg();
		Sleep(100);

		Buzzer(FALSE, 0);
		Buzzer(FALSE, 1);

#ifdef USE_VISION
		if (m_pVision[1])
		{
			delete m_pVision[1];
			m_pVision[1] = NULL;
		}

		if (m_pVision[0])
		{
			delete m_pVision[0];
			m_pVision[0] = NULL;
		}
#endif

		m_bTIM_MPE_IO = FALSE;
		m_bTIM_DISP_STATUS = FALSE;
		m_bTIM_INIT_VIEW = FALSE;
		Sleep(100);


		// H/W Device 소멸.....
		HwKill();

		CloseMyMsg();

		if (m_ArrayMyMsgBox.GetSize() > 0)
		{
			m_ArrayMyMsgBox.RemoveAll();
		}
	}
}

void CGvisR2R_LaserView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CGvisR2R_LaserView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CGvisR2R_LaserView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	InitMyMsg();

	pDoc->LoadWorkingInfo();
	pDoc->LoadIoInfo();
	pDoc->LoadSignalInfo();
	pDoc->LoadDataInfo();
	if (!LoadMySpec())
		LoadMySpec();
	pDoc->GetCurrentInfo();

#ifdef USE_CAM_MASTER
	CFileFind finder;
	CString sDir, sMsg;
	sDir = pDoc->WorkingInfo.System.sPathCamSpecDir;
	sDir.Delete(sDir.GetLength() - 1, 1);
	sDir.ReleaseBuffer();

	if(!pDoc->DirectoryExists(sDir))
	{
		sMsg.Format(_T("캠마스터에 스펙폴더가 없습니다. : \n 1.SpecFolder : %s"), sDir);
		pView->ClrDispMsg();
		AfxMessageBox(sMsg, MB_ICONSTOP | MB_OK);
		ExitProgram();
		return;
	}
#endif

	if (!m_bTIM_INIT_VIEW)
	{
		m_nStepInitView = 0;
		m_bTIM_INIT_VIEW = TRUE;
		SetTimer(TIM_INIT_VIEW, 300, NULL);
	}

}


// CGvisR2R_LaserView 진단

#ifdef _DEBUG
void CGvisR2R_LaserView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGvisR2R_LaserView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CGvisR2R_LaserDoc* CGvisR2R_LaserView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGvisR2R_LaserDoc)));
	return (CGvisR2R_LaserDoc*)m_pDocument;
}
#endif //_DEBUG


// CGvisR2R_LaserView 메시지 처리기


void CGvisR2R_LaserView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nInc = 0; int nSrl;
	CString str, sMsg, sPath;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (nIDEvent == TIM_INIT_VIEW)
	{
		KillTimer(TIM_INIT_VIEW);

		switch (m_nStepInitView)
		{
		case 0:
			m_nStepInitView++;
			DispMsg(_T("프로그램을 초기화합니다."), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
	
			// H/W Device 초기화.....
			HwInit();
			break;
		case 1:
			m_nStepInitView++;
			break;
		case 2:
			m_nStepInitView++;

			m_nMonAlmF = 0;
			m_nClrAlmF = 0;

			break;
		case 3:
			m_nStepInitView++;
			ThreadInit();
			break;
		case 4:
			m_nStepInitView++;
			break;
		case 5:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 1"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_02);
			break;
		case 6:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.-2"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_01);
			break;
		case 7:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 3"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			break;
		case 8:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 4"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_03);
			break;
		case 9:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 5"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_04);
			break;
		case 10:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 6"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_05);
			break;
		case 11:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 7"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_FRAME_HIGH);
			if (m_pDlgFrameHigh)
				m_pDlgFrameHigh->ChkMenu01();
			SetDualTest(pDoc->WorkingInfo.LastJob.bDualTest);
			break;
		case 12:
			Init(); // AmpReset()
			Sleep(300);
			m_nStepInitView++;
			break;
		case 13:
			m_nStepInitView++;
			DispMsg(_T("H/W를 초기화합니다."), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			InitAct();
			m_bStopFeeding = TRUE;
			Sleep(300);
			break;
		case 14:
			if (m_pMotion)
			{
				DispMsg(_T("Homming"), _T("Searching Home Position..."), RGB_GREEN, 2000, TRUE);
				m_pMotion->SearchHome();
				m_nStepInitView++;
			}
			else
			{
				m_bTIM_INIT_VIEW = FALSE;
				AfxMessageBox(_T("Motion is failed."));
				PostMessage(WM_CLOSE);
			}
			break;
		case 15:
			if (m_pMotion)
			{
				if (m_pMotion->IsHomeDone())// && m_pMotion->IsHomeDone(MS_MKFD))
				{
					m_nStepInitView++;
				}

				if (m_sMsg != sMsg)
				{
					sMsg.Format(_T("X0(%s) , Y0(%s)"), m_pMotion->IsHomeDone(MS_X0) ? _T("Done") : _T("Doing"),
						m_pMotion->IsHomeDone(MS_Y0) ? _T("Done") : _T("Doing"));
					DispMsg(sMsg, _T("Homming"), RGB_GREEN, 2000, TRUE);
				}
				Sleep(300);
			}
			else
			{
				m_bTIM_INIT_VIEW = FALSE;
				AfxMessageBox(_T("Motion is failed."));
				PostMessage(WM_CLOSE);
			}
			break;
		case 16:
			m_nStepInitView++;
			m_sMsg = _T("");
			m_bLoadMstInfo = TRUE;
			pDoc->m_bLoadMstInfo[0] = TRUE;
			pDoc->m_bLoadMstInfo[1] = TRUE;
			m_bTIM_START_UPDATE = TRUE;
			SetTimer(TIM_START_UPDATE, 50, NULL);
			break;
		case 17:
			m_nStepInitView++;
			DispMsg(_T("Completed Searching Home Pos..."), _T("Homming"), RGB_GREEN, 2000, TRUE);
			m_pMotion->ObjectMapping();
			break;
		case 18:
			TcpIpInit();
			m_nStepInitView++;
			break;
		case 19:
			if (m_bLoadMstInfo)
			{
				Sleep(100);
				break;
			}

			m_nStepInitView++;
			SetAlignPos();
			break;
		case 20:
			m_nStepInitView++;
			break;

		case 21:
			m_nStepInitView++;
			m_bStopFeeding = FALSE;
			if(m_pDlgMenu02)
				m_pDlgMenu02->SetJogSpd(_tstoi(pDoc->WorkingInfo.LastJob.sJogSpd));
			m_pMotion->SetR2RConf();
			TowerLamp(RGB_YELLOW, TRUE);

			break;
		case 22:
			m_nStepInitView++;
			m_bTIM_DISP_STATUS = TRUE;
			SetTimer(TIM_DISP_STATUS, 100, NULL);
			break;
		case 23:
			m_nStepInitView++;
			ClrDispMsg();
			if (m_pDlgMenu01)
			{
				m_pDlgMenu01->SetStripAllMk();
				m_pDlgMenu01->RefreshRmap();
				m_pDlgMenu01->ResetLastProc();
			}
			if (!MemChk())
				AfxMessageBox(_T("Memory Error - Cam Spec Data : PCR[0] or PCR[1] or Reelmap"));
			else
			{
				if (pDoc->m_pReelMap)
				{
#ifndef TEST_MODE
					ReloadRst();
					UpdateRst();
#endif
					UpdateLotTime();
				}
			}

			// Check Encoder
			if (!m_bThread[1])
				m_Thread[1].Start(GetSafeHwnd(), this, ThreadProc1);

			// DispDefImg
			if (!m_bThread[2])
				m_Thread[2].Start(GetSafeHwnd(), this, ThreadProc2);

			//MoveInitPos1();
			//Sleep(30);
			MoveInitPos0();

			SetLotLastShot();
			StartLive();

			pDoc->SetEngraveLastShot(pDoc->GetCurrentInfoEngShotNum());

			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (m_pDlgMenu01)
					m_pDlgMenu01->EnableItsMode();
			}

			SetSerialReelmap(GetLastSerialEng());	// Reelmap(좌) Display Start

			m_bTIM_MPE_IO = TRUE;
			SetTimer(TIM_MPE_IO, 50, NULL);

			m_bTIM_INIT_VIEW = FALSE;
			break;
		}

		if (m_bTIM_INIT_VIEW)
			SetTimer(TIM_INIT_VIEW, 100, NULL);
	}

	if (nIDEvent == TIM_MPE_IO)
	{
		KillTimer(TIM_MPE_IO);

		DoIO(); // DoAutoEng()

		ChkMyMsg();

		if (m_bTIM_MPE_IO)
			SetTimer(TIM_MPE_IO, 100, NULL);
	}

	if (nIDEvent == TIM_BUZZER_WARN)
	{
		KillTimer(TIM_BUZZER_WARN);
		m_nCntBz++;
		if (m_nCntBz > BUZZER_DELAY)
		{
			m_bTimBuzzerWarn = FALSE;
			Buzzer(FALSE);
		}
		if (m_bTimBuzzerWarn)
			SetTimer(TIM_BUZZER_WARN, 500, NULL);
	}

	if (nIDEvent == TIM_DISP_STATUS)
	{
		KillTimer(TIM_DISP_STATUS);

		DispStsBar();
		DoDispMain();

		if (m_bStopFromThread)
		{
			m_bStopFromThread = FALSE;
			Stop();
		}
		if (m_bBuzzerFromThread)
		{
			m_bBuzzerFromThread = FALSE;
			Buzzer(TRUE, 0);
		}

		ChkEmg();
		ChkSaftySen();
		ChkDoor();
		ChkRcvSig();

		if (m_bTIM_DISP_STATUS)
			SetTimer(TIM_DISP_STATUS, 500, NULL);
	}

	if (nIDEvent == TIM_SHOW_MENU01)
	{
		KillTimer(TIM_SHOW_MENU01);
		if (m_pDlgFrameHigh)
			m_pDlgFrameHigh->ChkMenu01();
	}

	if (nIDEvent == TIM_SHOW_MENU02)
	{
		KillTimer(TIM_SHOW_MENU02);
		if (m_pDlgFrameHigh)
			m_pDlgFrameHigh->ChkMenu02();
	}

	if (nIDEvent == TIM_CHK_TEMP_STOP)
	{
		KillTimer(TIM_CHK_TEMP_STOP);
		if (m_bTIM_CHK_TEMP_STOP)
			SetTimer(TIM_CHK_TEMP_STOP, 500, NULL);
	}

	if (nIDEvent == TIM_SAFTY_STOP)
	{
		KillTimer(TIM_SAFTY_STOP);
		MsgBox(_T("일시정지 - 마킹부 안전센서가 감지되었습니다."));
		m_bTIM_SAFTY_STOP = FALSE;
	}

	if (nIDEvent == TIM_START_UPDATE)
	{
		KillTimer(TIM_START_UPDATE);

		if (m_bLoadMstInfo && !m_bLoadMstInfoF)
		{
			if (!pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() && !pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			{
				m_bLoadMstInfoF = TRUE;
				SetTimer(TIM_TCPIP_UPDATE, 500, NULL);
			}
			else
				m_bLoadMstInfo = FALSE;
		}

		if (m_bSetSig && !m_bSetSigF)
		{
			m_bSetSigF = TRUE;

			if (m_pEngrave->m_bGetOpInfo || m_pEngrave->m_bGetInfo)
			{
				if (m_pDlgInfo)
					m_pDlgInfo->UpdateData();

				if (m_pDlgMenu01)
					m_pDlgMenu01->UpdateData();

				m_pEngrave->m_bGetOpInfo = FALSE;
				m_pEngrave->m_bGetInfo = FALSE;
			}

			m_bSetSig = FALSE;
		}
		else if (m_bSetSigF)
		{
			m_bSetSigF = FALSE;
		}

		if (m_bSetData && !m_bSetDataF)
		{
			m_bSetDataF = TRUE;

			if (m_pEngrave->m_bGetOpInfo || m_pEngrave->m_bGetInfo)
			{
				if (m_pDlgInfo)
					m_pDlgInfo->UpdateData();

				if (m_pDlgMenu01)
					m_pDlgMenu01->UpdateData();

				m_pEngrave->m_bGetOpInfo = FALSE;
				m_pEngrave->m_bGetInfo = FALSE;
			}

			if (m_pDlgMenu02)
				m_pDlgMenu02->UpdateData();
			m_bSetData = FALSE;
		}
		else if (m_bSetDataF)
		{
			m_bSetDataF = FALSE;
		}

		if (m_bTIM_START_UPDATE)
			SetTimer(TIM_START_UPDATE, 100, NULL);
	}

	if (nIDEvent == TIM_TCPIP_UPDATE)
	{
		KillTimer(TIM_TCPIP_UPDATE);
		LoadMstInfo();
		if (m_pDlgMenu01)
		{
			m_pDlgMenu01->UpdateData();
		}
		m_bLoadMstInfoF = FALSE;
		m_bLoadMstInfo = FALSE;
	}

	if (nIDEvent == TIM_MENU01_UPDATE_WORK)
	{
		KillTimer(TIM_MENU01_UPDATE_WORK);
		pDoc->GetMkMenu01();
		if (m_pDlgMenu01)
		{
			m_pDlgMenu01->UpdateData();
			m_pDlgMenu01->UpdateWorking();
			m_pDlgMenu01->DispTotRatio();
			m_pDlgMenu01->DispStripRatio();
			m_pDlgMenu01->DispDef();
			m_pDlgMenu01->DispMkCnt();
		}
	}

	CFormView::OnTimer(nIDEvent);
}

void CGvisR2R_LaserView::InitMyMsg()
{
	if (m_pDlgMyMsg)
		CloseMyMsg();

	m_pDlgMyMsg = new CDlgMyMsg(this);
	m_pDlgMyMsg->Create();
}

void CGvisR2R_LaserView::CloseMyMsg()
{
	if (m_pDlgMyMsg)
	{
		delete m_pDlgMyMsg;
		m_pDlgMyMsg = NULL;
	}
}

LRESULT CGvisR2R_LaserView::OnMyMsgExit(WPARAM wPara, LPARAM lPara)
{
	return 0L;
}

int CGvisR2R_LaserView::MsgBox(CString sMsg, int nThreadIdx, int nType, int nTimOut)
{
	int nRtnVal = -1; // Reply(-1) is None.
	if (m_pDlgMyMsg)
		nRtnVal = m_pDlgMyMsg->SyncMsgBox(sMsg, nThreadIdx, nType, nTimOut);

	return nRtnVal;
}

int CGvisR2R_LaserView::AsyncMsgBox(CString sMsg, int nThreadIdx, int nType, int nTimOut)
{
	int nRtn = -1;
	if (m_pDlgMyMsg)
		m_pDlgMyMsg->AsyncMsgBox(sMsg, nThreadIdx, nType, nTimOut);
	return nRtn;
}

int CGvisR2R_LaserView::WaitRtnVal(int nThreadIdx)
{
	int nRtn = -1;
	if (m_pDlgMyMsg)
		nRtn = m_pDlgMyMsg->WaitRtnVal(nThreadIdx);
	return nRtn;
}

void CGvisR2R_LaserView::ChkMyMsg()
{
	return;

	CWnd *pWndForeground;

	if (m_bShowMyMsg && m_pMyMsgForeground)
	{
		pWndForeground = pFrm->GetForegroundWindow();
		if (pWndForeground != m_pMyMsgForeground)
			m_pMyMsgForeground->SetForegroundWindow();
	}
}

void CGvisR2R_LaserView::UpdateLotTime()
{
	m_dwLotSt = (DWORD)pDoc->WorkingInfo.Lot.dwStTick;

	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateLotTime();
}

void CGvisR2R_LaserView::DispStsBar(CString sMsg, int nIdx)
{
	if (m_sDispMsg[nIdx] != sMsg)
		m_sDispMsg[nIdx] = sMsg;
	sMsg.Empty();
}

void CGvisR2R_LaserView::DispStsBar()
{
	DispStsMainMsg(); // 0
	DispStsMainMsg(6); // 6
	DispTime(); // 7
	ChkShare(); // 2, 4
	ChkBuf(); // 1, 3
}

BOOL CGvisR2R_LaserView::MemChk()
{
	if (!pDoc->m_pPcr[0] || !pDoc->m_pPcr[1])// || !pDoc->m_pReelMap)
		return FALSE;
	return TRUE;
}

void CGvisR2R_LaserView::ExitProgram()
{
	long lParam = 0;
	long lData = 1;
	lParam = lParam | lData;
	lData = 0x00 << 16;
	lParam = lParam | lData;
	lData = 1 << 29;
	lParam = lParam | lData;
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT);
}

void CGvisR2R_LaserView::Init()
{
	int nAxis;
	if (m_pMotion)
	{
		for (nAxis = 0; nAxis < m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
		{
			m_pMotion->AmpReset(nAxis);
			Sleep(30);
		}
	}
}

BOOL CGvisR2R_LaserView::InitAct()
{
#ifdef USE_XMP
	if (!m_pMotion)
		return FALSE;
#endif

#ifdef USE_LIGHT
	if (!m_pLight)
		return FALSE;
#endif

	int nAxis;

	if (m_pMotion)
	{
		for (nAxis = 0; nAxis < m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
		{
			m_pMotion->ServoOnOff(nAxis, TRUE);
			Sleep(100);
		}

		double dX[2], dY[2];

		if (pDoc->m_pSpecLocal && IsPinData())
		{
			dX[0] = pDoc->m_pSpecLocal->m_dPinPosX[0];
			dY[0] = pDoc->m_pSpecLocal->m_dPinPosY[0];
			dX[1] = pDoc->m_pSpecLocal->m_dPinPosX[1];
			dY[1] = pDoc->m_pSpecLocal->m_dPinPosY[1];
		}
		else
		{
			dX[0] = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[0]);
			dY[0] = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]);
			dX[1] = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[1]);
			dY[1] = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[1]);
		}
		m_pMotion->SetPinPos(0, dX[0], dY[0]);
		m_pMotion->SetPinPos(1, dX[1], dY[1]);
		m_pMotion->m_dStBufPos = _tstof(pDoc->WorkingInfo.Motion.sStBufPos);


		CfPoint ptPnt0(dX[0], dY[0]);
		if (pDoc->m_Master[0].m_pPcsRgn)
			pDoc->m_Master[0].m_pPcsRgn->SetPinPos(0, ptPnt0);

		CfPoint ptPnt1(dX[1], dY[1]);
		if (pDoc->m_Master[0].m_pPcsRgn)
			pDoc->m_Master[0].m_pPcsRgn->SetPinPos(1, ptPnt1);

		if (pDoc->m_pSpecLocal)
		{
			pDoc->SetMkPnt(CAM_BOTH);
		}

		double dPos = _tstof(pDoc->WorkingInfo.Motion.sStBufPos);
		double dVel = _tstof(pDoc->WorkingInfo.Motion.sBufHomeSpd);
		double dAcc = _tstof(pDoc->WorkingInfo.Motion.sBufHomeAcc);
		SetBufHomeParam(dVel, dAcc);
	}

	// Light On
	if (m_pDlgMenu02)
	{
		m_pDlgMenu02->SetLight(_tstoi(pDoc->WorkingInfo.Light.sVal[0]));
		m_pDlgMenu02->SetLight2(_tstoi(pDoc->WorkingInfo.Light.sVal[1]));
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::TcpIpInit()
{
#ifdef USE_MDX2500
	if (!m_pMdx2500)
	{
		m_pMdx2500 = new CMdx2500(pDoc->WorkingInfo.System.sIpClient[ID_MDX2500], pDoc->WorkingInfo.System.sIpServer[ID_MDX2500], pDoc->WorkingInfo.System.sPort[ID_MDX2500], this);
	}
#endif

#ifdef USE_SR1000W
	if (!m_pSr1000w)
	{
		m_pSr1000w = new CSr1000w(pDoc->WorkingInfo.System.sIpClient[ID_SR1000W], pDoc->WorkingInfo.System.sIpServer[ID_SR1000W], pDoc->WorkingInfo.System.sPort[ID_SR1000W], this);
	}
#endif

#ifdef USE_TCPIP
	if (!m_pEngrave)
	{
		m_pEngrave = new CEngrave(pDoc->WorkingInfo.System.sIpClient[ID_PUNCH], pDoc->WorkingInfo.System.sIpServer[ID_ENGRAVE], pDoc->WorkingInfo.System.sPort[ID_ENGRAVE], this);
		m_pEngrave->SetHwnd(this->GetSafeHwnd());
	}
#endif

	return TRUE;
}

BOOL CGvisR2R_LaserView::HwInit()
{
	if (m_pLight)
	{
		delete m_pLight;
		m_pLight = NULL;
	}

	m_pLight = new CLight(this);
	m_pLight->Init();

	if (m_pMotion)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}
	m_pMotion = new CMotion(this);
	if (!m_pMotion->InitBoard())
	{
		MsgBox(_T("XMP 보드 초기화 실패, 다시 시작하세요.!!!"));
		PostMessage(WM_CLOSE);
		return FALSE;
	}

	return TRUE;
}

void CGvisR2R_LaserView::HwKill()
{
	if (m_pMotion)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}

	if (m_pLight)
	{
		m_pLight->Close();
		delete m_pLight;
		m_pLight = NULL;
	}

	if (m_pEngrave)
	{
		m_pEngrave->Close();
		delete m_pEngrave;
		m_pEngrave = NULL;
	}


	if (m_pSr1000w)
	{
		m_pSr1000w->Close();
		delete m_pSr1000w;
		m_pSr1000w = NULL;
	}

	if (m_pMdx2500)
	{
		m_pMdx2500->Close();
		delete m_pMdx2500;
		m_pMdx2500 = NULL;
	}
}

void CGvisR2R_LaserView::GetDispMsg(CString &strMsg, CString &strTitle)
{
	if (m_pDlgMsgBox)
		m_pDlgMsgBox->GetDispMsg(strMsg, strTitle);
}

void CGvisR2R_LaserView::DispMsg(CString strMsg, CString strTitle, COLORREF color, DWORD dwDispTime, BOOL bOverWrite)
{
	if (m_bDispMsg)
		return;

	if (m_bAuto)
	{
		return;
	}

	m_bDispMsg = TRUE;

	if (dwDispTime == 0)
	{
		dwDispTime = 24 * 3600 * 1000;
	}

	if (m_pDlgMsgBox != NULL)
	{
		if (bOverWrite)
		{
			if(m_pDlgMsgBox)
			m_pDlgMsgBox->SetDispMsg(strMsg, strTitle, dwDispTime, color);
		}
		if (m_pDlgMsgBox)
		m_pDlgMsgBox->ShowWindow(SW_SHOW);
		if (m_pDlgMsgBox)
		m_pDlgMsgBox->SetFocus();
		if (m_pDlgMsgBox)
		((CButton*)m_pDlgMsgBox->GetDlgItem(IDOK))->SetCheck(TRUE);
	}
	else
	{
		m_pDlgMsgBox = new CDlgMsgBox(this, strTitle, strMsg, dwDispTime, color);
		if (m_pDlgMsgBox->GetSafeHwnd() == 0)
		{
			m_pDlgMsgBox->Create();
			m_pDlgMsgBox->ShowWindow(SW_SHOW);
			m_pDlgMsgBox->SetDispMsg(strMsg, strTitle, dwDispTime, color);
			m_pDlgMsgBox->SetFocus();
			((CButton*)m_pDlgMsgBox->GetDlgItem(IDOK))->SetCheck(TRUE);
		}
	}

	m_bDispMsg = FALSE;
}

void CGvisR2R_LaserView::ClrDispMsg()
{
	OnQuitDispMsg(NULL, NULL);
}

BOOL CGvisR2R_LaserView::WaitClrDispMsg()
{
	m_bWaitClrDispMsg = TRUE;
	MSG message;

	DWORD dwMilliseconds = 0; // 100ms sec sleep
	while (WAIT_OBJECT_0 != ::WaitForSingleObject(m_evtWaitClrDispMsg, dwMilliseconds) && m_pDlgMsgBox != NULL)
	{
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	};
	Sleep(10);
	m_bWaitClrDispMsg = FALSE;
	return TRUE;
}

LONG CGvisR2R_LaserView::OnQuitDispMsg(UINT wParam, LONG lParam)
{
	if (m_pDlgMsgBox)
	{
		if (m_pDlgMsgBox->GetSafeHwnd())
			m_pDlgMsgBox->DestroyWindow();
		delete m_pDlgMsgBox;
		m_pDlgMsgBox = NULL;
	}

	return 0L;
}

void CGvisR2R_LaserView::ShowDlg(int nID)
{
	HideAllDlg();

	switch (nID)
	{
	case IDD_DLG_INFO:
		if (!m_pDlgInfo)
		{
			m_pDlgInfo = new CDlgInfo(this);
			if (m_pDlgInfo->GetSafeHwnd() == 0)
			{
				m_pDlgInfo->Create();
				m_pDlgInfo->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgInfo->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_FRAME_HIGH:
		if (!m_pDlgFrameHigh)
		{
			m_pDlgFrameHigh = new CDlgFrameHigh(this);
			if (m_pDlgFrameHigh->GetSafeHwnd() == 0)
			{
				m_pDlgFrameHigh->Create();
				m_pDlgFrameHigh->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgFrameHigh->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_01:
		if (!m_pDlgMenu01)
		{
			m_pDlgMenu01 = new CDlgMenu01(this);
			if (m_pDlgMenu01->GetSafeHwnd() == 0)
			{
				m_pDlgMenu01->Create();
				m_pDlgMenu01->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu01->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_02:
		if (!m_pDlgMenu02)
		{
			m_pDlgMenu02 = new CDlgMenu02(this);
			if (m_pDlgMenu02->GetSafeHwnd() == 0)
			{
				m_pDlgMenu02->Create();
				m_pDlgMenu02->ShowDebugEngSig();
				m_pDlgMenu02->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu02->ShowDebugEngSig();
			m_pDlgMenu02->ShowWindow(SW_SHOW);
		}
		break;
	}
}

void CGvisR2R_LaserView::HideAllDlg()
{
	if (m_pDlgUtil01 && m_pDlgUtil01->GetSafeHwnd())
	{
		if (m_pDlgUtil01->IsWindowVisible())
			m_pDlgUtil01->ShowWindow(SW_HIDE);
	}
	if (m_pDlgInfo && m_pDlgInfo->GetSafeHwnd())
	{
		if (m_pDlgInfo->IsWindowVisible())
			m_pDlgInfo->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu01 && m_pDlgMenu01->GetSafeHwnd())
	{
		if (m_pDlgMenu01->IsWindowVisible())
			m_pDlgMenu01->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu02 && m_pDlgMenu02->GetSafeHwnd())
	{
		if (m_pDlgMenu02->IsWindowVisible())
			m_pDlgMenu02->ShowWindow(SW_HIDE);
	}
}

void CGvisR2R_LaserView::DelAllDlg()
{

	if (m_pDlgUtil01 != NULL)
	{
		delete m_pDlgUtil01;
		m_pDlgUtil01 = NULL;
	}

	if (m_pDlgInfo != NULL)
	{
		delete m_pDlgInfo;
		m_pDlgInfo = NULL;
	}
	if (m_pDlgMenu02 != NULL)
	{
		delete m_pDlgMenu02;
		m_pDlgMenu02 = NULL;
	}
	if (m_pDlgMenu01 != NULL)
	{
		delete m_pDlgMenu01;
		m_pDlgMenu01 = NULL;
	}
	if (m_pDlgFrameHigh != NULL)
	{
		delete m_pDlgFrameHigh;
		m_pDlgFrameHigh = NULL;
	}

	if (m_pDlgMsgBox != NULL)
	{
		if (m_pDlgMsgBox->GetSafeHwnd())
			m_pDlgMsgBox->DestroyWindow();
		delete m_pDlgMsgBox;
		m_pDlgMsgBox = NULL;
	}
}

LRESULT CGvisR2R_LaserView::OnDlgInfo(WPARAM wParam, LPARAM lParam)
{
	ClrDispMsg();
	CDlgInfo Dlg;
	m_pDlgInfo = &Dlg;
	Dlg.DoModal();
	m_pDlgInfo = NULL;

	if (m_pDlgMenu01)
		m_pDlgMenu01->ChkUserInfo(FALSE);

	return 0L;
}

void CGvisR2R_LaserView::TowerLamp(COLORREF color, BOOL bOn, BOOL bWink)
{
	if (m_pEngrave)
	{
		m_pEngrave->SetTowerLamp(color, bOn, bWink);
	}
}

void CGvisR2R_LaserView::BuzzerFromThread(BOOL bOn, int nCh)
{
	m_bBuzzerFromThread = TRUE;
}

void CGvisR2R_LaserView::Buzzer(BOOL bOn, int nCh)
{
	if (m_pEngrave)
	{
		m_pEngrave->SetBuzzer(bOn, nCh);
	}
}

void CGvisR2R_LaserView::ThreadInit()
{
	if (!m_bThread[0])
		m_Thread[0].Start(GetSafeHwnd(), this, ThreadProc0);
}

void CGvisR2R_LaserView::ThreadKill()
{
	if (m_bThread[0])
	{
		m_Thread[0].Stop();
		Sleep(20);
		while (m_bThread[0])
		{
			Sleep(20);
		}
	}
	if (m_bThread[1])
	{
		m_Thread[1].Stop();
		Sleep(20);
		while (m_bThread[1])
		{
			Sleep(20);
		}
	}
	if (m_bThread[2])
	{
		m_Thread[2].Stop();
		Sleep(20);
		while (m_bThread[2])
		{
			Sleep(20);
		}
	}
}

UINT CGvisR2R_LaserView::ThreadProc0(LPVOID lpContext)
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_LaserView* pThread = reinterpret_cast<CGvisR2R_LaserView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[0] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[0].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[0] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;
			pThread->GetMonDispMainSignal();
			bLock = FALSE;
		}
		Sleep(100);
	}
	pThread->m_bThread[0] = FALSE;

	return 0;
}

UINT CGvisR2R_LaserView::ThreadProc1(LPVOID lpContext)
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_LaserView* pThread = reinterpret_cast<CGvisR2R_LaserView*>(lpContext);

	BOOL bLock = FALSE, bEStop = FALSE, bCollision = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[1] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[1].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[1] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;
			pThread->GetEnc();
			bLock = FALSE;
		}
		Sleep(100);
	}
	pThread->m_bThread[1] = FALSE;

	return 0;
}

UINT CGvisR2R_LaserView::ThreadProc2(LPVOID lpContext)
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_LaserView* pThread = reinterpret_cast<CGvisR2R_LaserView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[2] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[2].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[2] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;

			if (pThread->m_bTHREAD_DISP_DEF)
			{
				pThread->DispDefImg();
				pThread->UpdateLotTime();

				Sleep(0);
			}
			else
				Sleep(30);

			bLock = FALSE;
		}
	}
	pThread->m_bThread[2] = FALSE;

	return 0;
}


void CGvisR2R_LaserView::DispStsMainMsg(int nIdx)
{
	CString str;
	str = m_sDispMsg[nIdx];
	pFrm->DispStatusBar(str, nIdx);
}

void CGvisR2R_LaserView::SwJog(int nAxisID, int nDir, BOOL bOn)
{
	if (!pView->m_pMotion)
		return;

	double fVel, fAcc, fJerk;
	double dStep;
	if (pDoc->Status.bSwJogFast)
		dStep = 0.5;
	else
		dStep = 0.1;

	if (pDoc->Status.bSwJogStep)
	{
		if (bOn)
		{
			double dPos = pView->m_dEnc[nAxisID];
			if (nDir == M_CW)
				dPos += dStep;
			else if (nDir == M_CCW)
				dPos -= dStep;

			if (nAxisID == AXIS_X0)
			{
				if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
				{
					if (nDir == M_CW) // ▶ Jog 버튼.
						return;
				}

				if (m_pMotion->IsLimit(MS_X0, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_X0, dPos, fVel, fAcc, fAcc);
			}
			else if (nAxisID == AXIS_Y0)
			{
				if (m_pMotion->IsLimit(MS_Y0, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_Y0, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_Y0, dPos, fVel, fAcc, fAcc);
			}
		}
	}
	else	// Jog Mode
	{
		if (!m_pDlgMenu02)
			return;

		if (nAxisID == AXIS_Y0)
		{
			if (nDir == M_CCW)		// Up
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_UP);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_UP);
			}
			else if (nDir == M_CW)	// Dn
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_DN);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_DN);
			}
		}
		else if (nAxisID == AXIS_X0)
		{
			if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
			{
				if (nDir == M_CW) // ▶ Jog 버튼.
					return;
			}

			if (nDir == M_CW)		// Rt
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_RT);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_RT);
			}
			else if (nDir == M_CCW)	// Lf
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_LF);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_LF);
			}
		}
	}
}


BOOL CGvisR2R_LaserView::ChkShareIdx(int *pBufSerial, int nBufTot, int nShareSerial)
{
	if (nBufTot < 1)
		return TRUE;
	for (int i = 0; i < nBufTot; i++)
	{
		if (pBufSerial[i] == nShareSerial)
			return FALSE;
	}
	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkVsShare(int &nSerial)
{
	int nS0, nS1;
	BOOL b0 = ChkVsShareUp(nS0);
	BOOL b1 = ChkVsShareDn(nS1);

	if (!b0 || !b1)
	{
		nSerial = -1;
		return FALSE;
	}
	else if (nS0 != nS1)
	{
		nSerial = -1;
		return FALSE;
	}

	nSerial = nS0;
	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkVsShareUp(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVsShareUp + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr파일이 존재하지 않음.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// 파일명을 얻음.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::ChkVsShareDn(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVsShareDn + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr파일이 존재하지 않음.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// 파일명을 얻음.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::ChkShare(int &nSerial)
{
	int nS0, nS1;
	BOOL b0 = ChkShareUp(nS0);
	BOOL b1 = ChkShareDn(nS1);

	if (!b0 || !b1)
	{
		nSerial = -1;
		return FALSE;
	}
	else if (nS0 != nS1)
	{
		nSerial = -1;
		return FALSE;
	}

	nSerial = nS0;
	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkShareUp(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareUp + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr파일이 존재하지 않음.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// 파일명을 얻음.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::ChkShareDn(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareDn + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr파일이 존재하지 않음.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// 파일명을 얻음.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

void CGvisR2R_LaserView::ChkShare()
{
	ChkShareUp();
	ChkShareDn();
}

void CGvisR2R_LaserView::ChkShareUp()
{
	CString str;
	int nSerial;
	if (ChkShareUp(nSerial))
	{
		str.Format(_T("US: %d"), nSerial);
		pDoc->Status.PcrShare[0].bExist = TRUE;
		pDoc->Status.PcrShare[0].nSerial = nSerial;
	}
	else
	{
		pDoc->Status.PcrShare[0].bExist = FALSE;
		pDoc->Status.PcrShare[0].nSerial = -1;
		str.Format(_T("US: "));
	}
	if (pFrm)
	{
		if (m_sShare[0] != str)
		{
			m_sShare[0] = str;
			pFrm->DispStatusBar(str, 4);
		}
	}
}

void CGvisR2R_LaserView::ChkShareDn()
{
	CString str;
	int nSerial;
	if (ChkShareDn(nSerial))
	{
		str.Format(_T("DS: %d"), nSerial);
		pDoc->Status.PcrShare[1].bExist = TRUE;
		pDoc->Status.PcrShare[1].nSerial = nSerial;
	}
	else
	{
		pDoc->Status.PcrShare[1].bExist = FALSE;
		pDoc->Status.PcrShare[1].nSerial = -1;
		str.Format(_T("DS: "));
	}
	if (pFrm)
	{
		if (m_sShare[1] != str)
		{
			m_sShare[1] = str;
			pFrm->DispStatusBar(str, 2);
		}
	}
}

BOOL CGvisR2R_LaserView::ChkBufIdx(int* pSerial, int nTot)
{
	if (nTot < 2)
		return TRUE;

	for (int i = 0; i < (nTot - 1); i++)
	{
		if (pSerial[i + 1] != pSerial[i] + 1)
			return FALSE;
	}
	return TRUE;
}


void CGvisR2R_LaserView::SwapUp(__int64 *num1, __int64 *num2) 	// 위치 바꾸는 함수
{
	__int64 temp;

	temp = *num2;
	*num2 = *num1;
	*num1 = temp;
}

BOOL CGvisR2R_LaserView::SortingInUp(CString sPath, int nIndex)
{
	struct _stat buf;
	struct tm *t;

	CString sMsg, sFileName, sSerial;
	int nPos, nSerial;
	char filename[MAX_PATH];
	StringToChar(sPath, filename);

	if (_stat(filename, &buf) != 0)
	{
		sMsg.Format(_T("일시정지 - Failed getting information."));
		pView->ClrDispMsg();
		AfxMessageBox(sMsg);
		return FALSE;
	}
	else
	{
		sFileName = sPath;
		nPos = sFileName.ReverseFind('.');
		if (nPos > 0)
		{
			sSerial = sFileName.Left(nPos);
			sSerial = sSerial.Right(4);
		}

		nSerial = _tstoi(sSerial);

		t = localtime(&buf.st_mtime);

		CString sYear, sMonth, sDay, sHour, sMin, sSec;
		sYear.Format(_T("%04d"), t->tm_year + 1900);
		sMonth.Format(_T("%02d"), t->tm_mon + 1);
		sDay.Format(_T("%02d"), t->tm_mday);
		sHour.Format(_T("%02d"), t->tm_hour);
		sMin.Format(_T("%02d"), t->tm_min);
		sSec.Format(_T("%02d"), t->tm_sec);

		__int64 nYear = _tstoi(sYear);
		__int64 nMonth = _tstoi(sMonth);
		__int64 nDay = _tstoi(sDay);
		__int64 nHour = _tstoi(sHour);
		__int64 nMin = _tstoi(sMin);
		__int64 nSec = _tstoi(sSec);

		m_nBufSerialSorting[0][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::SortingOutUp(int* pSerial, int nTot)
{
	int i, k;

	for (k = 0; k < nTot; k++) 			// 버블 정렬 소스 시작
	{
		for (i = 0; i < (nTot - 1) - k; i++)
		{

			if (m_nBufSerialSorting[0][i] > m_nBufSerialSorting[0][i + 1])
			{
				SwapUp(&m_nBufSerialSorting[0][i + 1], &m_nBufSerialSorting[0][i]);
			}
		}
	}									// 버블 정렬 소스 끝

	for (i = 0; i < nTot; i++)
	{
		pSerial[i] = (int)(m_nBufSerialSorting[0][i] % 10000);
	}
	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkBufUp(int* pSerial, int &nTot)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufUp + _T("*.pcr"));
	if (!bExist)
	{
		pDoc->m_bBufEmpty[0] = TRUE;
		if (!pDoc->m_bBufEmptyF[0])
			pDoc->m_bBufEmptyF[0] = TRUE;		// 최초 한번 버퍼가 비어있으면(초기화를 하고 난 이후) TRUE.

		return FALSE; // pcr파일이 존재하지 않음.
	}

	int nPos, nSerial;

	CString sFileName, sSerial;
	CString sNewName;

	nTot = 0;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			sFileName = cFile.GetFileName();

			if (!SortingInUp(pDoc->WorkingInfo.System.sPathVrsBufUp + sFileName, nTot))
				return FALSE;

				nTot++;
			}
		}

	BOOL bRtn = SortingOutUp(pSerial, nTot);

	if (nTot == 0)
		pDoc->m_bBufEmpty[0] = TRUE;
	else
		pDoc->m_bBufEmpty[0] = FALSE;

	return (bRtn);
}

BOOL CGvisR2R_LaserView::ChkBufDn(int* pSerial, int &nTot)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufDn + _T("*.pcr"));
	if (!bExist)
	{
		pDoc->m_bBufEmpty[1] = TRUE;
		if (!pDoc->m_bBufEmptyF[1])
			pDoc->m_bBufEmptyF[1] = TRUE;
		return FALSE; // pcr파일이 존재하지 않음.
	}

	int nPos, nSerial;

	CString sFileName, sSerial;
	nTot = 0;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			sFileName = cFile.GetFileName();

			if (!SortingInDn(pDoc->WorkingInfo.System.sPathVrsBufDn + sFileName, nTot))
				return FALSE;

			nTot++;
		}
	}


	BOOL bRtn = SortingOutDn(pSerial, nTot);

	if (nTot == 0)
		pDoc->m_bBufEmpty[1] = TRUE;
	else
		pDoc->m_bBufEmpty[1] = FALSE;

	return (bRtn);
}

void CGvisR2R_LaserView::SwapDn(__int64 *num1, __int64 *num2) 	// 위치 바꾸는 함수
{
	__int64 temp;

	temp = *num2;
	*num2 = *num1;
	*num1 = temp;
}

BOOL CGvisR2R_LaserView::SortingInDn(CString sPath, int nIndex)
{
	struct _stat buf;
	struct tm *t;

	CString sMsg, sFileName, sSerial;
	int nPos, nSerial;
	char filename[MAX_PATH];
	StringToChar(sPath, filename);

	if (_stat(filename, &buf) != 0)
	{
		sMsg.Format(_T("일시정지 - Failed getting information."));
		pView->ClrDispMsg();
		AfxMessageBox(sMsg);
		return FALSE;
	}
	else
	{
		sFileName = sPath;
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
		{
				sSerial = sFileName.Left(nPos);
			sSerial = sSerial.Right(4);
		}

			nSerial = _tstoi(sSerial);

		t = localtime(&buf.st_mtime);
		CString sYear, sMonth, sDay, sHour, sMin, sSec;
		sYear.Format(_T("%04d"), t->tm_year + 1900);
		sMonth.Format(_T("%02d"), t->tm_mon + 1);
		sDay.Format(_T("%02d"), t->tm_mday);
		sHour.Format(_T("%02d"), t->tm_hour);
		sMin.Format(_T("%02d"), t->tm_min);
		sSec.Format(_T("%02d"), t->tm_sec);

		__int64 nYear = _tstoi(sYear);
		__int64 nMonth = _tstoi(sMonth);
		__int64 nDay = _tstoi(sDay);
		__int64 nHour = _tstoi(sHour);
		__int64 nMin = _tstoi(sMin);
		__int64 nSec = _tstoi(sSec);

		m_nBufSerialSorting[1][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;

			}

	return TRUE;
}

BOOL CGvisR2R_LaserView::SortingOutDn(int* pSerial, int nTot)
{
	int i, k;

	for (k = 0; k < nTot; k++) 			// 버블 정렬 소스 시작
	{
		for (i = 0; i < (nTot - 1) - k; i++)
		{

			if (m_nBufSerialSorting[1][i] > m_nBufSerialSorting[1][i + 1])
			{
				SwapUp(&m_nBufSerialSorting[1][i + 1], &m_nBufSerialSorting[1][i]);
		}
	}
	}									// 버블 정렬 소스 끝

	for (i = 0; i < nTot; i++)
	{
		pSerial[i] = (int)(m_nBufSerialSorting[1][i] % 10000);
	}
	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkYield() // 수율 양호 : TRUE , 수율 불량 : FALSE
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nGood = 0, nBad = 0, nTot;
	double dRatio;
	CString sMsg;

	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->GetPcsNum(nGood, nBad);
		else
		{
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			sMsg.Format(_T("일시정지 - Failed ChkYield()."));
			MsgBox(sMsg);
			return FALSE;
		}
	}
	else
	{
		if (pDoc->m_pReelMapUp)
			pDoc->m_pReelMapUp->GetPcsNum(nGood, nBad);
		else
		{
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			sMsg.Format(_T("일시정지 - Failed ChkYield()."));
			MsgBox(sMsg);
			return FALSE;
		}
	}

	nTot = (nGood + nBad);
	double dTotLmt = _tstof(pDoc->WorkingInfo.LastJob.sLmtTotYld);
	if (dTotLmt > 0.0)
	{
		if (nTot > 0)
			dRatio = ((double)nGood / (double)nTot) * 100.0;
		else
			dRatio = 0.0;

		if (dRatio < dTotLmt)
		{
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			sMsg.Format(_T("일시정지 - 전체 수율 제한범위 : %.1f 미달 ( %.1f )"), dTotLmt, dRatio);
			MsgBox(sMsg);
			return FALSE;
		}
	}

	double dPrtLmt = _tstof(pDoc->WorkingInfo.LastJob.sLmtPatlYld);
	if (dPrtLmt > 0.0)
	{
		if (bDualTest)
		{
			if (pDoc->m_Yield[2].IsValid())
			{
				dRatio = pDoc->m_Yield[2].GetYield();

				if (dRatio < dPrtLmt)
				{
					Buzzer(TRUE, 0);
					TowerLamp(RGB_RED, TRUE);
					Stop();
					sMsg.Format(_T("일시정지 - 구간 수율 제한범위 : %.1f 미달 ( %.1f )"), dPrtLmt, dRatio);
					MsgBox(sMsg);
					return FALSE;
				}
			}
		}
		else
		{
			if (pDoc->m_Yield[0].IsValid())
			{
				dRatio = pDoc->m_Yield[0].GetYield();

				if (dRatio < dPrtLmt)
				{
					Buzzer(TRUE, 0);
					TowerLamp(RGB_RED, TRUE);
					Stop();
					sMsg.Format(_T("일시정지 - 구간 수율 제한범위 : %.1f 미달 ( %.1f )"), dPrtLmt, dRatio);
					MsgBox(sMsg);
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkSaftySen() // 감지 : TRUE , 비감지 : FALSE
{
	if (pDoc->WorkingInfo.LastJob.bMkSftySen)
	{
		if (pDoc->Status.bSensSaftyMk && !pDoc->Status.bSensSaftyMkF)
		{
			pDoc->Status.bSensSaftyMkF = TRUE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			DispMain(_T("정 지"), RGB_RED);			
			m_bTIM_SAFTY_STOP = TRUE;//MsgBox(_T("일시정지 - 마킹부 안전센서가 감지되었습니다."));
			SetTimer(TIM_SAFTY_STOP, 100, NULL);
		}
		else if (!pDoc->Status.bSensSaftyMk && pDoc->Status.bSensSaftyMkF)
		{
			pDoc->Status.bSensSaftyMkF = FALSE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
	}

	return (pDoc->Status.bSensSaftyMk);
}

void CGvisR2R_LaserView::ResetMotion()
{
	for (int i = 0; i < MAX_MS; i++)
	{
		ResetMotion(i);

		if (i < MAX_AXIS)
		{
			while (!m_pMotion->IsServoOn(i))
			{
				if (i == MS_X0 || i == MS_Y0)
					pView->m_pMotion->Clear(MS_X0Y0);
				else if (i == MS_X1 || i == MS_Y1)
					pView->m_pMotion->Clear(MS_X1Y1);
				else
					pView->m_pMotion->Clear(i);
				Sleep(30);
				m_pMotion->ServoOnOff(i, TRUE);
				Sleep(30);
			}
		}
	}
}

void CGvisR2R_LaserView::ResetMotion(int nMsId)
{
	if (m_pDlgMenu02)
		m_pDlgMenu02->ResetMotion(nMsId);
}

unsigned long CGvisR2R_LaserView::ChkDoor() // 0: All Closed , Open Door Index : Doesn't all closed. (Bit3: F, Bit2: L, Bit1: R, Bit0; B)
{
	unsigned long ulOpenDoor = 0;

	if (pDoc->WorkingInfo.LastJob.bAoiUpDrSen)
	{
		if (pDoc->Status.bDoorAoi[DOOR_FM_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FM_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 전면 중앙 도어 Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 전면 좌측 도어 Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP])
	{
			ulOpenDoor |= (0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 전면 우측 도어 Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BM_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BM_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 후면 중앙 도어 Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 후면 좌측 도어 Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 후면 우측 도어 Open"));
		}
	}

	if (pDoc->WorkingInfo.LastJob.bAoiDnDrSen)
	{
		if (pDoc->Status.bDoorAoi[DOOR_FM_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FM_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 전면 중앙 도어 Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 전면 좌측 도어 Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-7"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 전면 우측 도어 Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BM_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BM_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-8"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 후면 중앙 도어 Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-9"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 후면 좌측 도어 Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-10"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 후면 우측 도어 Open"));
		}
	}

	if (pDoc->WorkingInfo.LastJob.bMkDrSen)
	{
		if (pDoc->Status.bDoorMk[DOOR_FL_MK] && !pDoc->Status.bDoorMkF[DOOR_FL_MK])
		{
			ulOpenDoor |= (0x01 << 6);
			pDoc->Status.bDoorMkF[DOOR_FL_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_FL_MK] && pDoc->Status.bDoorMkF[DOOR_FL_MK])
		{
			ulOpenDoor &= ~(0x01 << 6);
			pDoc->Status.bDoorMkF[DOOR_FL_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-11"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 마킹부 전면 좌측 도어 Open"));
		}

		if (pDoc->Status.bDoorMk[DOOR_FR_MK] && !pDoc->Status.bDoorMkF[DOOR_FR_MK])
		{
			ulOpenDoor |= (0x01 << 7);
			pDoc->Status.bDoorMkF[DOOR_FR_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_FR_MK] && pDoc->Status.bDoorMkF[DOOR_FR_MK])
		{
			ulOpenDoor &= ~(0x01 << 7);
			pDoc->Status.bDoorMkF[DOOR_FR_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-12"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 마킹부 전면 우측 도어 Open"));
		}

		if (pDoc->Status.bDoorMk[DOOR_BL_MK] && !pDoc->Status.bDoorMkF[DOOR_BL_MK])
		{
			ulOpenDoor |= (0x01 << 8);
			pDoc->Status.bDoorMkF[DOOR_BL_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_BL_MK] && pDoc->Status.bDoorMkF[DOOR_BL_MK])
		{
			ulOpenDoor &= ~(0x01 << 8);
			pDoc->Status.bDoorMkF[DOOR_BL_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-13"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 마킹부 후면 좌측 도어 Open"));
		}

		if (pDoc->Status.bDoorMk[DOOR_BR_MK] && !pDoc->Status.bDoorMkF[DOOR_BR_MK])
		{
			ulOpenDoor |= (0x01 << 9);
			pDoc->Status.bDoorMkF[DOOR_BR_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_BR_MK] && pDoc->Status.bDoorMkF[DOOR_BR_MK])
		{
			ulOpenDoor &= ~(0x01 << 9);
			pDoc->Status.bDoorMkF[DOOR_BR_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-14"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 마킹부 후면 우측 도어 Open"));
		}
	}

	if (pDoc->WorkingInfo.LastJob.bEngvDrSen)
	{
		if (pDoc->Status.bDoorEngv[DOOR_FL_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_FL_ENGV])
		{
			ulOpenDoor |= (0x01 << 6);
			pDoc->Status.bDoorEngvF[DOOR_FL_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_FL_ENGV] && pDoc->Status.bDoorEngvF[DOOR_FL_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 6);
			pDoc->Status.bDoorEngvF[DOOR_FL_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-11"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 각인부 전면 좌측 도어 Open"));
		}

		if (pDoc->Status.bDoorEngv[DOOR_FR_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_FR_ENGV])
		{
			ulOpenDoor |= (0x01 << 7);
			pDoc->Status.bDoorEngvF[DOOR_FR_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_FR_ENGV] && pDoc->Status.bDoorEngvF[DOOR_FR_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 7);
			pDoc->Status.bDoorEngvF[DOOR_FR_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-12"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 각인부 전면 우측 도어 Open"));
		}

		if (pDoc->Status.bDoorEngv[DOOR_BL_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_BL_ENGV])
		{
			ulOpenDoor |= (0x01 << 8);
			pDoc->Status.bDoorEngvF[DOOR_BL_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_BL_ENGV] && pDoc->Status.bDoorEngvF[DOOR_BL_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 8);
			pDoc->Status.bDoorEngvF[DOOR_BL_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-13"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 각인부 후면 좌측 도어 Open"));
		}

		if (pDoc->Status.bDoorEngv[DOOR_BR_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_BR_ENGV])
		{
			ulOpenDoor |= (0x01 << 9);
			pDoc->Status.bDoorEngvF[DOOR_BR_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_BR_ENGV] && pDoc->Status.bDoorEngvF[DOOR_BR_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 9);
			pDoc->Status.bDoorEngvF[DOOR_BR_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-14"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 각인부 후면 우측 도어 Open"));
		}
	}

	if (pDoc->WorkingInfo.LastJob.bUclDrSen)
	{
		if (pDoc->Status.bDoorUc[DOOR_FL_UC] && !pDoc->Status.bDoorUcF[DOOR_FL_UC])
		{
			ulOpenDoor |= (0x01 << 10);
			pDoc->Status.bDoorUcF[DOOR_FL_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_FL_UC] && pDoc->Status.bDoorUcF[DOOR_FL_UC])
		{
			ulOpenDoor &= ~(0x01 << 10);
			pDoc->Status.bDoorUcF[DOOR_FL_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-15"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 언코일러부 전면 좌측 도어 Open"));
		}

		if (pDoc->Status.bDoorUc[DOOR_FR_UC] && !pDoc->Status.bDoorUcF[DOOR_FR_UC])
		{
			ulOpenDoor |= (0x01 << 11);
			pDoc->Status.bDoorUcF[DOOR_FR_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_FR_UC] && pDoc->Status.bDoorUcF[DOOR_FR_UC])
		{
			ulOpenDoor &= ~(0x01 << 11);
			pDoc->Status.bDoorUcF[DOOR_FR_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-16"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 언코일러부 측면 도어 Open"));
		}

		if (pDoc->Status.bDoorUc[DOOR_BL_UC] && !pDoc->Status.bDoorUcF[DOOR_BL_UC])
		{
			ulOpenDoor |= (0x01 << 12);
			pDoc->Status.bDoorUcF[DOOR_BL_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_BL_UC] && pDoc->Status.bDoorUcF[DOOR_BL_UC])
		{
			ulOpenDoor &= ~(0x01 << 12);
			pDoc->Status.bDoorUcF[DOOR_BL_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-17"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 언코일러부 후면 좌측 도어 Open"));
		}

		if (pDoc->Status.bDoorUc[DOOR_BR_UC] && !pDoc->Status.bDoorUcF[DOOR_BR_UC])
		{
			ulOpenDoor |= (0x01 << 13);
			pDoc->Status.bDoorUcF[DOOR_BR_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_BR_UC] && pDoc->Status.bDoorUcF[DOOR_BR_UC])
		{
			ulOpenDoor &= ~(0x01 << 13);
			pDoc->Status.bDoorUcF[DOOR_BR_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-18"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 언코일러부 후면 우측 도어 Open"));
		}
	}

	if (pDoc->WorkingInfo.LastJob.bRclDrSen)
	{
		if (pDoc->Status.bDoorRe[DOOR_FR_RC] && !pDoc->Status.bDoorReF[DOOR_FR_RC])
		{
			ulOpenDoor |= (0x01 << 15);
			pDoc->Status.bDoorReF[DOOR_FR_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_FR_RC] && pDoc->Status.bDoorReF[DOOR_FR_RC])
		{
			ulOpenDoor &= ~(0x01 << 15);
			pDoc->Status.bDoorReF[DOOR_FR_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-19"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 리코일러부 전면 우측 도어 Open"));
		}

		if (pDoc->Status.bDoorRe[DOOR_S_RC] && !pDoc->Status.bDoorReF[DOOR_S_RC])
		{
			ulOpenDoor |= (0x01 << 16);
			pDoc->Status.bDoorReF[DOOR_S_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_S_RC] && pDoc->Status.bDoorReF[DOOR_S_RC])
		{
			ulOpenDoor &= ~(0x01 << 16);
			pDoc->Status.bDoorReF[DOOR_S_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-20"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 리코일러부 측면 도어 Open"));
		}

		if (pDoc->Status.bDoorRe[DOOR_BL_RC] && !pDoc->Status.bDoorReF[DOOR_BL_RC])
		{
			ulOpenDoor |= (0x01 << 17);
			pDoc->Status.bDoorReF[DOOR_BL_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_BL_RC] && pDoc->Status.bDoorReF[DOOR_BL_RC])
		{
			ulOpenDoor &= ~(0x01 << 17);
			pDoc->Status.bDoorReF[DOOR_BL_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-21"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 리코일러부 후면 좌측 도어 Open"));
		}

		if (pDoc->Status.bDoorRe[DOOR_BR_RC] && !pDoc->Status.bDoorReF[DOOR_BR_RC])
		{
			ulOpenDoor |= (0x01 << 18);
			pDoc->Status.bDoorReF[DOOR_BR_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_BR_RC] && pDoc->Status.bDoorReF[DOOR_BR_RC])
		{
			ulOpenDoor &= ~(0x01 << 18);
			pDoc->Status.bDoorReF[DOOR_BR_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("정지-22"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 리코일러부 후면 우측 도어 Open"));
		}
	}

	return ulOpenDoor;
}

void CGvisR2R_LaserView::ChkEmg()
{
	if (pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
	{
		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		pView->DispStsBar(_T("정지-23"), 0);
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 검사부 상 전면 스위치"));
	}
	else if (!pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
	{
		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_B_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_B_AOI_UP])
	{
		pDoc->Status.bEmgAoiF[EMG_B_AOI_UP] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		pView->DispStsBar(_T("정지-24"), 0);
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 검사부 상 후면 스위치"));
	}
	else if (!pDoc->Status.bEmgAoi[EMG_B_AOI_UP] && pDoc->Status.bEmgAoiF[EMG_B_AOI_UP])
	{
		pDoc->Status.bEmgAoiF[EMG_B_AOI_UP] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_F_AOI_DN] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_DN])
	{
		pDoc->Status.bEmgAoiF[EMG_F_AOI_DN] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		pView->DispStsBar(_T("정지-23"), 0);
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 검사부 하 전면 스위치"));
	}
	else if (!pDoc->Status.bEmgAoi[EMG_F_AOI_DN] && pDoc->Status.bEmgAoiF[EMG_F_AOI_DN])
	{
		pDoc->Status.bEmgAoiF[EMG_F_AOI_DN] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_B_AOI_DN] && !pDoc->Status.bEmgAoiF[EMG_B_AOI_DN])
	{
		pDoc->Status.bEmgAoiF[EMG_B_AOI_DN] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		pView->DispStsBar(_T("정지-24"), 0);
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 검사부 하 후면 스위치"));
	}
	else if (!pDoc->Status.bEmgAoi[EMG_B_AOI_DN] && pDoc->Status.bEmgAoiF[EMG_B_AOI_DN])
	{
		pDoc->Status.bEmgAoiF[EMG_B_AOI_DN] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgMk[EMG_M_MK] && !pDoc->Status.bEmgMkF[EMG_M_MK])
	{
		pDoc->Status.bEmgMkF[EMG_M_MK] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		pView->DispStsBar(_T("정지-25"), 0);
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 마킹부 메인 스위치"));
	}
	else if (!pDoc->Status.bEmgMk[EMG_M_MK] && pDoc->Status.bEmgMkF[EMG_M_MK])
	{
		pDoc->Status.bEmgMkF[EMG_M_MK] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgMk[EMG_B_MK] && !pDoc->Status.bEmgMkF[EMG_B_MK])
	{
		pDoc->Status.bEmgMkF[EMG_B_MK] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		pView->DispStsBar(_T("정지-26"), 0);
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 마킹부 스위치"));
	}
	else if (!pDoc->Status.bEmgMk[EMG_B_MK] && pDoc->Status.bEmgMkF[EMG_B_MK])
	{
		pDoc->Status.bEmgMkF[EMG_B_MK] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgUc && !pDoc->Status.bEmgUcF)
	{
		pDoc->Status.bEmgUcF = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		pView->DispStsBar(_T("정지-27"), 0);
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 언코일러부 스위치"));
	}
	else if (!pDoc->Status.bEmgUc && pDoc->Status.bEmgUcF)
	{
		pDoc->Status.bEmgUcF = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgRc && !pDoc->Status.bEmgRcF)
	{
		pDoc->Status.bEmgRcF = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		pView->DispStsBar(_T("정지-28"), 0);
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 리코일러부 스위치"));
	}
	else if (!pDoc->Status.bEmgRc && pDoc->Status.bEmgRcF)
	{
		pDoc->Status.bEmgRcF = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgEngv[0] && !pDoc->Status.bEmgEngvF[0])
	{
		pDoc->Status.bEmgEngvF[0] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		pView->DispStsBar(_T("정지-29"), 0);
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 각인부 모니터"));
	}
	else if (!pDoc->Status.bEmgEngv[0] && pDoc->Status.bEmgEngvF[0])
	{
		pDoc->Status.bEmgEngvF[0] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgEngv[1] && !pDoc->Status.bEmgEngvF[1])
	{
		pDoc->Status.bEmgEngvF[1] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		pView->DispStsBar(_T("정지-30"), 0);
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 각인부 스위치"));
	}
	else if (!pDoc->Status.bEmgEngv[1] && pDoc->Status.bEmgEngvF[1])
	{
		pDoc->Status.bEmgEngvF[1] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}
}

int CGvisR2R_LaserView::ChkSerial() // 0: Continue, -: Previous, +: Discontinue
{
	int nSerial0 = GetBuffer0();
	int nSerial1 = GetBuffer1();
	int nLastShot = pDoc->GetLastShotMk();

	if (nSerial0 == nLastShot + 1 || nSerial1 == nLastShot + 2)
		return 0;
	if (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot)
	{
		if (nLastShot == pDoc->m_nLotLastShot && (nSerial0 == 1 || nSerial1 == 2))
			return 0;
	}
	if (m_bLastProc && nSerial0 == m_nLotEndSerial)
		return 0;

	return (nSerial0 - nLastShot);
}

void CGvisR2R_LaserView::ChkBuf()
{
	ChkBufUp();
	ChkBufDn();
}

void CGvisR2R_LaserView::DoIO()
{
	if (m_bCycleStop)
	{
		m_bCycleStop = FALSE;

		if (pDoc->m_sAlmMsg != pDoc->m_sIsAlmMsg)
		{
			MsgBox(pDoc->m_sAlmMsg);
			pDoc->m_sAlmMsg = _T("");
			pDoc->m_sIsAlmMsg = _T("");
			pDoc->m_sPrevAlmMsg = _T("");
		}
		else if (pDoc->m_sMsgBox != pDoc->m_sIsMsgBox)
		{
			MsgBox(pDoc->m_sMsgBox, 0, pDoc->m_nTypeMsgBox);
			pDoc->m_sMsgBox = _T("");
			pDoc->m_sIsMsgBox = _T("");
			pDoc->m_sPrevMsgBox = _T("");
		}

		pDoc->m_sAlmMsg = _T("");
		pDoc->m_sPrevAlmMsg = _T("");
	}

	if (pDoc->Status.bAuto)
		DoAutoEng();


	if (IsRun())
	{
		if (m_pDlgMenu01)
		{
			if (m_pDlgMenu01->IsEnableBtn())
				m_pDlgMenu01->EnableBtn(FALSE);
		}
	}
	else
	{
		if (m_pDlgMenu01)
		{
			if (!m_pDlgMenu01->IsEnableBtn())
				m_pDlgMenu01->EnableBtn(TRUE);
		}
	}

}

void CGvisR2R_LaserView::GetEnc()
{
	if (!m_pMotion)
		return;

	m_dEnc[AXIS_X0] = m_pMotion->GetActualPosition(AXIS_X0);
	m_dEnc[AXIS_Y0] = m_pMotion->GetActualPosition(AXIS_Y0);
}

void CGvisR2R_LaserView::ChkMRegOut()
{
	m_bChkMpeIoOut = TRUE;
}

int CGvisR2R_LaserView::GetTime(int nSel)
{

	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	switch (nSel)
	{
	case YEAR:
		return Tim.GetYear();
	case MONTH:
		return Tim.GetMonth();
	case DAY:
		return Tim.GetDay();
	case HOUR:
		return Tim.GetHour();
	case MINUTE:
		return Tim.GetMinute();
	case SEC:
		return Tim.GetSecond();
	}

	return 0;
}

CString CGvisR2R_LaserView::GetTime()
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	int nYear = Tim.GetYear();
	int nMonth = Tim.GetMonth();
	int nDay = Tim.GetDay();
	int nHour = Tim.GetHour();
	int nMinute = Tim.GetMinute();
	int nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d,%02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMinute, nSec);
	return strVal;
}

CString CGvisR2R_LaserView::GetTime(stLotTime &LotTime)
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	LotTime.nYear = Tim.GetYear();
	LotTime.nMonth = Tim.GetMonth();
	LotTime.nDay = Tim.GetDay();
	LotTime.nHour = Tim.GetHour();
	LotTime.nMin = Tim.GetMinute();
	LotTime.nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d,%02d:%02d:%02d"), LotTime.nYear, LotTime.nMonth, LotTime.nDay,
		LotTime.nHour, LotTime.nMin, LotTime.nSec);
	return strVal;
}


CString CGvisR2R_LaserView::GetTime(int &nHour, int &nMinute, int &nSec)
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	int nYear = Tim.GetYear();
	int nMonth = Tim.GetMonth();
	int nDay = Tim.GetDay();
	nHour = Tim.GetHour();
	nMinute = Tim.GetMinute();
	nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d,%02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMinute, nSec);
	return strVal;
}

void CGvisR2R_LaserView::DispTime()
{
	stLotTime LotTime;
	CString str;
	str = GetTime(LotTime);
	if (m_sDispTime != str)
	{
		m_sDispTime = str;
		pFrm->DispStatusBar(str, 7);

		pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
		pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
		pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
		pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
		pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;		

		if (m_pDlgMenu01)
		{
			if(pDoc->WorkingInfo.Lot.CurTime.nSec != LotTime.nSec)
				m_pDlgMenu01->DispRunTime();
		}
		pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;	
	}
}


void CGvisR2R_LaserView::SetAoiFdPitch(double dPitch)
{
	pDoc->SetAoiFdPitch(dPitch);
}

void CGvisR2R_LaserView::SetMkFdPitch(double dPitch)
{
	pDoc->SetMkFdPitch(dPitch);
}

void CGvisR2R_LaserView::SetBufHomeParam(double dVel, double dAcc)
{
	long lVel = long(dVel*1000.0);
	long lAcc = long(dAcc*1000.0);
}

//.........................................................................................

BOOL CGvisR2R_LaserView::WatiDispMain(int nDelay)
{
	if (m_nWatiDispMain % nDelay)
	{
		m_nWatiDispMain++;
		return TRUE;
	}

	m_nWatiDispMain = 0;
	m_nWatiDispMain++;
	return FALSE;
}

void CGvisR2R_LaserView::DispMain(CString sMsg, COLORREF rgb)
{
	m_sMonDisp = sMsg;

	m_csDispMain.Lock();
	m_bDispMain = FALSE;
	stDispMain stData(sMsg, rgb);
	m_ArrayDispMain.Add(stData);
	m_bDispMain = TRUE;

	if (sMsg == _T("정 지"))
		m_bStopF_Verify = TRUE;

	sMsg.Empty();
	m_csDispMain.Unlock();
}

int CGvisR2R_LaserView::DoDispMain()
{
	int nRtn = -1;

	if (!m_bDispMain)
		return nRtn;

	int nCount = m_ArrayDispMain.GetSize();
	if (nCount > 0)
	{
		stDispMain stDispMsg;

		m_csDispMain.Lock();
		stDispMsg = m_ArrayDispMain.GetAt(0);
		m_ArrayDispMain.RemoveAt(0);
		m_csDispMain.Unlock();

		if (m_pDlgMenu01)
		{
			CString sMsg = stDispMsg.strMsg;
			COLORREF rgb = stDispMsg.rgb;
			m_sDispMain = sMsg;
			m_pDlgMenu01->DispMain(sMsg, rgb);
			return 0;
		}
	}

	return nRtn;
}

BOOL CGvisR2R_LaserView::IsAuto()
{
	if (pDoc->Status.bAuto)
		return TRUE;
	return FALSE;
}

void CGvisR2R_LaserView::Shift2Buf()
{
	if (m_nShareUpS > 0)
	{
		m_bLoadShare[0] = TRUE;
		pDoc->m_ListBuf[0].Push(m_nShareUpS);

	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (m_nShareDnS > 0)
		{
			m_bLoadShare[1] = TRUE;
			pDoc->m_ListBuf[1].Push(m_nShareDnS);
		}
	}

	pDoc->CopyPcrAll();
	pDoc->DelSharePcr();
}

void CGvisR2R_LaserView::Shift2Mk()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial;

	if (bDualTest)
	{
		if (m_bLastProc && m_nBufDnSerial[0] == m_nLotEndSerial)
		{
			nSerial = m_nBufDnSerial[0];
			if (nSerial > 0 && (nSerial % 2))
			{
				pDoc->UpdateYield(nSerial);
				pDoc->Shift2Mk(nSerial);	// Cam0
				if (m_pDlgFrameHigh)
					m_pDlgFrameHigh->SetMkLastShot(nSerial);
			}
		}
		else
		{
			nSerial = m_nBufDnSerial[0];
			if (!m_bCont)
			{
				if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					pDoc->UpdateYield(nSerial + 1);
					pDoc->Shift2Mk(nSerial + 1);	// Cam1
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial + 1);
				}
				else
				{
					Stop();
				}
			}
			else
			{
				if (nSerial > 0)
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					pDoc->UpdateYield(nSerial + 1);
					pDoc->Shift2Mk(nSerial + 1);	// Cam1
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial + 1);
				}
				else
				{
					Stop();
				}
			}
		}
	}
	else
	{
		if (m_bLastProc && m_nBufUpSerial[0] == m_nLotEndSerial)
		{
			nSerial = m_nBufUpSerial[0];
			if (!m_bCont)
			{
				if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial);
				}
				else
				{
					Stop();
				}
			}
			else
			{
				if (nSerial > 0)
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial);
				}
				else
				{
					Stop();
				}
			}
		}
		else
		{
			nSerial = m_nBufUpSerial[0];
			if (!m_bCont)
			{
				if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					pDoc->UpdateYield(nSerial + 1);
					pDoc->Shift2Mk(nSerial + 1);	// Cam1
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial + 1);
				}
				else
				{
					Stop();
				}
			}
			else
			{
				if (nSerial > 0)
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					pDoc->UpdateYield(nSerial + 1);
					pDoc->Shift2Mk(nSerial + 1);	// Cam1
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial + 1);
				}
				else
				{
					Stop();
				}
			}
		}
	}
}

void CGvisR2R_LaserView::SetDelay(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt[nId] = GetTickCount();
	m_dwSetDlyEd[nId] = m_dwSetDlySt[nId] + mSec;
}

void CGvisR2R_LaserView::SetDelay0(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt0[nId] = GetTickCount();
	m_dwSetDlyEd0[nId] = m_dwSetDlySt0[nId] + mSec;
}

void CGvisR2R_LaserView::SetDelay1(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt1[nId] = GetTickCount();
	m_dwSetDlyEd1[nId] = m_dwSetDlySt1[nId] + mSec;
}

BOOL CGvisR2R_LaserView::WaitDelay(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::WaitDelay0(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd0[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::WaitDelay1(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd1[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::GetDelay(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt[nId]);
	if (dwCur < m_dwSetDlyEd[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::GetDelay0(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt0[nId]);
	if (dwCur < m_dwSetDlyEd0[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::GetDelay1(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt1[nId]);
	if (dwCur < m_dwSetDlyEd1[nId])
		return TRUE;
	return FALSE;
}

void CGvisR2R_LaserView::SetCycTime()
{
	DWORD dCur = GetTickCount();
	if (m_dwCycSt > 0)
	{
		m_dwCycTim = (double)(dCur - m_dwCycSt);
		if (m_dwCycTim < 0.0)
			m_dwCycTim *= (-1.0);
	}
	else
		m_dwCycTim = 0.0;
}

int CGvisR2R_LaserView::GetCycTime()
{
	if (m_dwCycTim < 0)
		m_dwCycTim = 0;

	int nTim = int(m_dwCycTim);
	return nTim;
}

void CGvisR2R_LaserView::UpdateWorking()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateWorking();
}

void CGvisR2R_LaserView::StopFromThread()
{
	m_bStopFromThread = TRUE;
}


void CGvisR2R_LaserView::EngStop(BOOL bOn)
{
	m_bEngStop = bOn;

	if (m_pEngrave)
	{
		if (bOn)
		{
			DispMain(_T("정 지"), RGB_RED);
			m_pEngrave->SwStop(TRUE);
			Sleep(100);
		}
	}
}

BOOL CGvisR2R_LaserView::IsEngStop()
{
	if (m_sDispMain != _T("운전중") && m_bEngStop)
		return TRUE;
	else if (m_sDispMain == _T("운전중") && m_bEngStop)
	{
		m_bEngStop = FALSE;
		return FALSE;
	}

	return m_bEngStop;
}

void CGvisR2R_LaserView::Stop()
{
}

BOOL CGvisR2R_LaserView::IsStop()
{
	if (m_sDispMain == _T("정 지"))
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsRun()
{
	if (m_sDispMain == _T("운전중") || m_sDispMain == _T("초기운전") || m_sDispMain == _T("단면샘플") || m_sDispMain == _T("운전준비")
		|| m_sDispMain == _T("단면검사") || m_sDispMain == _T("양면검사") || m_sDispMain == _T("양면샘플"))
	{
		if (IsEngStop())
			return FALSE;

		return TRUE;
	}
	return FALSE;
}

void CGvisR2R_LaserView::ShowLive(BOOL bShow)
{
	if (bShow)
	{
		if (!IsShowLive())
			SetTimer(TIM_SHOW_MENU02, 30, NULL);
	}
	else
	{
		if (IsShowLive())
			SetTimer(TIM_SHOW_MENU01, 30, NULL);
	}
}

BOOL CGvisR2R_LaserView::IsShowLive()
{
	if (m_pDlgMenu02)
	{
		if (m_pDlgMenu02->IsWindowVisible())
			return TRUE;
	}

	return FALSE;
}

void CGvisR2R_LaserView::SetLotSt()
{
	stLotTime LotTime;
	GetTime(LotTime);

	pDoc->WorkingInfo.Lot.StTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.StTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.StTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.StTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.StTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.StTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.EdTime.nYear = 0;
	pDoc->WorkingInfo.Lot.EdTime.nMonth = 0;
	pDoc->WorkingInfo.Lot.EdTime.nDay = 0;
	pDoc->WorkingInfo.Lot.EdTime.nHour = 0;
	pDoc->WorkingInfo.Lot.EdTime.nMin = 0;
	pDoc->WorkingInfo.Lot.EdTime.nSec = 0;

	m_dwLotSt = GetTickCount();
	pDoc->SaveLotTime(m_dwLotSt);
	DispLotTime();

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMap)
		pDoc->m_pReelMap->SetLotSt();
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetLotSt();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetLotSt();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetLotSt();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->SetLotSt();
	}
}

void CGvisR2R_LaserView::SetLotEd()
{
	stLotTime LotTime;
	GetTime(LotTime);

	pDoc->WorkingInfo.Lot.EdTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.EdTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.EdTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.EdTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.EdTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.EdTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;

	m_dwLotEd = GetTickCount();

	pDoc->SaveLotTime(pDoc->WorkingInfo.Lot.dwStTick);
	DispLotTime();

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMap)
		pDoc->m_pReelMap->SetLotEd();
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetLotEd();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetLotEd();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetLotEd();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->SetLotEd();
	}

}

void CGvisR2R_LaserView::DispLotTime()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->DispLotTime();
}

double CGvisR2R_LaserView::GetMkFdLen()
{
	int nLast = pDoc->GetLastShotMk();
	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_LaserView::GetAoiUpFdLen()
{
	int nLast = pDoc->GetLastShotUp();
	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_LaserView::GetAoiDnFdLen()
{
	int nLast = pDoc->GetLastShotDn();
	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_LaserView::GetTotVel()
{
	CString str, sPrev;
	int nDiff;
	int nHour, nMin, nSec;
	int nStYear, nStMonth, nStDay, nStHour, nStMin, nStSec;
	int nCurYear, nCurMonth, nCurDay, nCurHour, nCurMin, nCurSec;
	int nEdYear, nEdMonth, nEdDay, nEdHour, nEdMin, nEdSec;

	nStYear = pDoc->WorkingInfo.Lot.StTime.nYear;
	nStMonth = pDoc->WorkingInfo.Lot.StTime.nMonth;
	nStDay = pDoc->WorkingInfo.Lot.StTime.nDay;
	nStHour = pDoc->WorkingInfo.Lot.StTime.nHour;
	nStMin = pDoc->WorkingInfo.Lot.StTime.nMin;
	nStSec = pDoc->WorkingInfo.Lot.StTime.nSec;

	nCurYear = pDoc->WorkingInfo.Lot.CurTime.nYear;
	nCurMonth = pDoc->WorkingInfo.Lot.CurTime.nMonth;
	nCurDay = pDoc->WorkingInfo.Lot.CurTime.nDay;
	nCurHour = pDoc->WorkingInfo.Lot.CurTime.nHour;
	nCurMin = pDoc->WorkingInfo.Lot.CurTime.nMin;
	nCurSec = pDoc->WorkingInfo.Lot.CurTime.nSec;

	nEdYear = pDoc->WorkingInfo.Lot.EdTime.nYear;
	nEdMonth = pDoc->WorkingInfo.Lot.EdTime.nMonth;
	nEdDay = pDoc->WorkingInfo.Lot.EdTime.nDay;
	nEdHour = pDoc->WorkingInfo.Lot.EdTime.nHour;
	nEdMin = pDoc->WorkingInfo.Lot.EdTime.nMin;
	nEdSec = pDoc->WorkingInfo.Lot.EdTime.nSec;

	int nTotSec = 0;
	double dMkFdLen = GetMkFdLen();
	if (!nStYear && !nStMonth && !nStDay && !nStHour && !nStMin && !nStSec)
	{
		return 0.0;
	}
	else if (!nEdYear && !nEdMonth && !nEdDay && !nEdHour && !nEdMin && !nEdSec)
	{
		nDiff = (GetTickCount() - pView->m_dwLotSt) / 1000;
		nHour = int(nDiff / 3600);
		nMin = int((nDiff - 3600 * nHour) / 60);
		nSec = nDiff % 60;
	}
	else
	{
		nDiff = (pView->m_dwLotEd - pView->m_dwLotSt) / 1000;
		nHour = int(nDiff / 3600);
		nMin = int((nDiff - 3600 * nHour) / 60);
		nSec = nDiff % 60;
	}

	nTotSec = nHour * 3600 + nMin * 60 + nSec;
	double dVel = 0.0;
	if (nTotSec > 0)
		dVel = dMkFdLen / (double)nTotSec; // [mm/sec]
										   // 		dVel = (dMkFdLen*60.0) / ((double)nTotSec*1000.0); // [M/min]

	m_dTotVel = dVel;
	return dVel;
}

double CGvisR2R_LaserView::GetPartVel()
{
	double dLen = _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen) * 2.0;
	double dSec = (double)GetCycTime() / 1000.0;
	double dVel = 0.0;
	if (dSec > 0.0)
		dVel = dLen / dSec; // [mm/sec]
	m_dPartVel = dVel;
	return dVel;
}


BOOL CGvisR2R_LaserView::IsBuffer(int nNum)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (m_nBufTot[0] > nNum && m_nBufTot[1] > nNum) // [0]: AOI-Up , [1]: AOI-Dn
			return TRUE;
	}
	else
	{
		if (m_nBufTot[0] > nNum) // [0]: AOI-Up
			return TRUE;
	}
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferUp()
{
	if (m_nBufTot[0] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferDn()
{
	if (m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

int CGvisR2R_LaserView::GetBuffer(int *pPrevSerial)
{
	int nS0 = GetBufferUp(pPrevSerial);
	int nS1 = GetBufferDn(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_LaserView::GetBufferUp(int *pPrevSerial)
{
	if (IsBufferUp())
		return m_pBufSerial[0][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][0];
	return 0;
}

int CGvisR2R_LaserView::GetBufferDn(int *pPrevSerial)
{
	if (IsBufferDn())
		return m_pBufSerial[1][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][0];
	return 0;
}

BOOL CGvisR2R_LaserView::IsBuffer0()
{
	if (m_nBufTot[0] > 0 && m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferUp0()
{
	if (m_nBufTot[0] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferDn0()
{
	if (m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

int CGvisR2R_LaserView::GetBuffer0(int *pPrevSerial)
{
	int nS0 = GetBufferUp0(pPrevSerial);
	int nS1 = GetBufferDn0(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_LaserView::GetBufferUp0(int *pPrevSerial)
{
	if (IsBufferUp0())
		return m_pBufSerial[0][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][0];
	return 0;
}

int CGvisR2R_LaserView::GetBufferDn0(int *pPrevSerial)
{
	if (IsBufferDn0())
		return m_pBufSerial[1][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][0];
	return 0;
}



BOOL CGvisR2R_LaserView::IsBuffer1()
{
	if (m_nBufTot[0] > 1 && m_nBufTot[1] > 1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferUp1()
{
	if (m_nBufTot[0] > 1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferDn1()
{
	if (m_nBufTot[1] > 1)
		return TRUE;
	return FALSE;
}

int CGvisR2R_LaserView::GetBuffer1(int *pPrevSerial)
{
	int nS0 = GetBufferUp1(pPrevSerial);
	int nS1 = GetBufferDn1(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_LaserView::GetBufferUp1(int *pPrevSerial)
{
	if (IsBufferUp1())
		return m_pBufSerial[0][1];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][1];
	return 0;
}

int CGvisR2R_LaserView::GetBufferDn1(int *pPrevSerial)
{
	if (IsBufferDn1())
		return m_pBufSerial[1][1];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][1];
	return 0;
}



BOOL CGvisR2R_LaserView::IsShare()
{
	// 	if(IsShareUp() || IsShareDn())
	// 		return TRUE;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (m_bWaitPcr[0] && m_bWaitPcr[1])
		{
			if (IsShareUp() && IsShareDn())
			{
				m_bWaitPcr[0] = FALSE;
				m_bWaitPcr[1] = FALSE;
				return TRUE;
			}
		}
		else if (m_bWaitPcr[0] && !m_bWaitPcr[1])
		{
			if (IsShareUp())
			{
				m_bWaitPcr[0] = FALSE;
				return TRUE;
			}
		}
		else if (!m_bWaitPcr[0] && m_bWaitPcr[1])
		{
			if (IsShareDn())
			{
				m_bWaitPcr[1] = FALSE;
				return TRUE;
			}
		}
		else
		{
			if (IsShareUp() || IsShareDn())
				return TRUE;
		}
	}
	else
	{
		if (m_bWaitPcr[0])
		{
			if (IsShareUp())
			{
				m_bWaitPcr[0] = FALSE;
				return TRUE;
			}
		}
		else
		{
			if (IsShareUp())
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsShareUp()
{
	return pDoc->Status.PcrShare[0].bExist;
}

BOOL CGvisR2R_LaserView::IsShareDn()
{
	return pDoc->Status.PcrShare[1].bExist;
}

BOOL CGvisR2R_LaserView::IsVsShare()
{
	int nSerial;
	BOOL bExist = ChkVsShare(nSerial);
	return bExist;
}

int CGvisR2R_LaserView::GetShareUp()
{
	return pDoc->Status.PcrShare[0].nSerial;
}

int CGvisR2R_LaserView::GetShareDn()
{
	return pDoc->Status.PcrShare[1].nSerial;
}

BOOL CGvisR2R_LaserView::ChkLastProc()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = (m_pDlgMenu01->m_bLastProc);
	return bRtn;
}

BOOL CGvisR2R_LaserView::ChkLastProcFromUp()
{
	BOOL bRtn = TRUE;
	if (m_pDlgMenu01)
		bRtn = (m_pDlgMenu01->m_bLastProcFromUp);
	return bRtn;
}

BOOL CGvisR2R_LaserView::ChkLotEnd(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.25"));
		return FALSE;
	}

	BOOL bEnd0 = ChkLotEndUp(nSerial);
	BOOL bEnd1 = ChkLotEndDn(nSerial);
	if (bEnd0 || bEnd1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::ChkLotEndUp(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.26"));
		return 0;
	}

	CString sPath;
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	return pDoc->ChkLotEnd(sPath);
}

BOOL CGvisR2R_LaserView::ChkLotEndDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.27"));
		return 0;
	}

	CString sPath;
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
	return pDoc->ChkLotEnd(sPath);
}

BOOL CGvisR2R_LaserView::SetSerial(int nSerial, BOOL bDumy)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.28"));
		return 0;
	}

	if (!m_pDlgMenu01)
		return FALSE;

	int nPrevSerial = m_pDlgMenu01->GetCurSerial();

	if (nPrevSerial == nSerial)
		return TRUE;

	BOOL bRtn[2] = {1};
	bRtn[0] = m_pDlgMenu01->SetSerial(nSerial, bDumy);

	return (bRtn[0] && bRtn[1]);
}

BOOL CGvisR2R_LaserView::SetSerialReelmap(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu01)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error - SetSerialReelmap : m_pDlgMenu01 is NULL."));
		return FALSE;
	}

	return m_pDlgMenu01->SetSerialReelmap(nSerial, bDumy);
}

BOOL CGvisR2R_LaserView::SetSerialMkInfo(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu01)
		return FALSE;
	return m_pDlgMenu01->SetSerialMkInfo(nSerial, bDumy);
}

void CGvisR2R_LaserView::SetListBuf()
{
	pDoc->m_ListBuf[0].Clear();
	if (ChkBufUp(m_pBufSerial[0], m_nBufTot[0]))
	{
		for (int i = 0; i < m_nBufTot[0]; i++)
			pDoc->m_ListBuf[0].Push(m_pBufSerial[0][i]);
	}

	pDoc->m_ListBuf[1].Clear();
	if (ChkBufDn(m_pBufSerial[1], m_nBufTot[1]))
	{
		for (int i = 0; i < m_nBufTot[1]; i++)
			pDoc->m_ListBuf[1].Push(m_pBufSerial[1][i]);
	}
}

void CGvisR2R_LaserView::DispLotStTime()
{
	TCHAR szData[MAX_PATH];
	CString sPath = PATH_WORKING_INFO;
	// [Lot]
	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Tick"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.dwStTick = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.dwStTick = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Year"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nYear = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nYear = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Month"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nMonth = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nMonth = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Day"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nDay = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nDay = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Hour"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nHour = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nHour = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Minute"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nMin = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nMin = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Second"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nSec = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nSec = 0;

	m_dwLotSt = (DWORD)pDoc->WorkingInfo.Lot.dwStTick;
	DispLotTime();
}

void CGvisR2R_LaserView::ClrMkInfo()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	pDoc->ClrPcr();
	if (pDoc->m_pReelMap)
		pDoc->m_pReelMap->Clear();
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->Clear();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->Clear();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->Clear();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->Clear();
	}

	if (m_pDlgMenu01)
	{
		m_pDlgMenu01->ResetMkInfo();
		m_pDlgMenu01->SetPnlNum();
		m_pDlgMenu01->SetPnlDefNum();
		m_pDlgMenu01->RefreshRmap();
		m_pDlgMenu01->UpdateRst();
		m_pDlgMenu01->UpdateWorking();
	}
}

void CGvisR2R_LaserView::ModelChange(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn 
{
	if (nAoi == 0)
	{
		pDoc->SetModelInfoUp();
		pView->OpenReelmapUp(); // At Start...
		pView->SetPathAtBufUp();
		if (pView->m_pDlgMenu01)
		{
			pView->m_pDlgMenu01->UpdateData();
			if (pView->m_nSelRmap == RMAP_UP || pView->m_nSelRmap == RMAP_ALLUP)
				pView->m_pDlgMenu01->OpenReelmap(pView->m_nSelRmap);
		}

		pDoc->m_pSpecLocal->MakeDir(pDoc->Status.PcrShare[0].sModel, pDoc->Status.PcrShare[0].sLayer);

		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			OpenReelmapInner();
		}
	}
	else if (nAoi == 1)
	{
		pDoc->SetModelInfoDn();
		pView->OpenReelmapDn(); // At Start...
		pView->SetPathAtBufDn();
		if (pView->m_pDlgMenu01)
		{
			pView->m_pDlgMenu01->UpdateData();
			if (pView->m_nSelRmap == RMAP_DN || pView->m_nSelRmap == RMAP_ALLDN)
				pView->m_pDlgMenu01->OpenReelmap(pView->m_nSelRmap);
		}
		pDoc->m_pSpecLocal->MakeDir(pDoc->Status.PcrShare[1].sModel, pDoc->Status.PcrShare[1].sLayer);
	}
}

void CGvisR2R_LaserView::ResetMkInfo(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bDualTestInner;
	CString sLot, sLayerUp, sLayerDn;
	pDoc->GetCurrentInfo();

	// CamMst Info...
	pDoc->GetCamPxlRes();

	if (nAoi == 0 || nAoi == 2)
	{
		if (!bDualTest)
		{
			m_bDrawGL = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->ResetMkInfo();
		}

		if (IsLastJob(0)) // Up
		{
			pDoc->m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLayerUp);
			pDoc->m_Master[0].LoadMstInfo();

			if (pDoc->GetItsSerialInfo(1, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
			{
				if (pDoc->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
				{
					pDoc->m_MasterInner[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
						pDoc->WorkingInfo.LastJob.sModelUp,
						sLayerUp);
					pDoc->m_MasterInner[0].LoadMstInfo();

					if (bDualTestInner)
					{
						pDoc->m_MasterInner[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
							pDoc->WorkingInfo.LastJob.sModelUp,
							sLayerDn);
						pDoc->m_MasterInner[1].LoadMstInfo();
					}
				}
			}
		}
		else
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - IsLastJob(0)..."));
		}

		InitReelmapUp();

		if (pDoc->GetItsSerialInfo(1, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
		{
			if (pDoc->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
			{
				InitReelmapInnerUp();
				if (bDualTestInner)
					InitReelmapInnerDn();
			}
		}

		OpenReelmap();
		SetAlignPosUp();

		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->ChgModelUp();

			if (bDualTest)
				m_pDlgMenu02->ChgModelDn();
		}
		
		if (m_pDlgMenu01)
		{
			m_pDlgMenu01->InitCadImgUp();
		}

		if (m_pDlgMenu01)
		{
			if (!bDualTest)
			{
				m_pDlgMenu01->InitGL();
				m_bDrawGL = TRUE;
				m_pDlgMenu01->RefreshRmap();
			}
		}
	}


	if (bDualTest)
	{
		if (nAoi == 1 || nAoi == 2)
		{
			m_bDrawGL = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->ResetMkInfo();

			if (IsLastJob(1)) // Dn
			{
				pDoc->m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
					pDoc->WorkingInfo.LastJob.sModelUp,
					pDoc->WorkingInfo.LastJob.sLayerDn,
					pDoc->WorkingInfo.LastJob.sLayerUp);

				pDoc->m_Master[1].LoadMstInfo();
			}
			else
			{
				AfxMessageBox(_T("Error - IsLastJob(1)..."));
			}

			InitReelmapDn();
			SetAlignPosDn();

			if (m_pDlgMenu02)
				m_pDlgMenu02->ChgModelDn();

			if (m_pDlgMenu01)
			{
				m_pDlgMenu01->InitCadImgDn();
				m_pDlgMenu01->InitGL();
				m_bDrawGL = TRUE;
				m_pDlgMenu01->RefreshRmap();
			}
		}
	}
}

void CGvisR2R_LaserView::SetAlignPos()
{
	if (m_pMotion)
	{
		m_pMotion->m_dAlignPosX[0][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[0];

		m_pMotion->m_dAlignPosX[1][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[1];
	}
}

void CGvisR2R_LaserView::SetAlignPosUp()
{
	if (m_pMotion)
	{
		m_pMotion->m_dAlignPosX[0][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[0];
	}
}

void CGvisR2R_LaserView::SetAlignPosDn()
{
	if (m_pMotion)
	{
		m_pMotion->m_dAlignPosX[1][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[1];
	}
}

int CGvisR2R_LaserView::GetErrCode(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.29"));
		return 0;
	}

#ifndef	TEST_MODE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nErr[2];
	nErr[0] = GetErrCodeUp(nSerial);
	if (nErr[0] != 1)
		return nErr[0];
	if (bDualTest)
	{
		nErr[1] = GetErrCodeDn(nSerial);
		if (nErr[1] != 1)
			return nErr[1];
	}
#endif

	return 1;
}

int CGvisR2R_LaserView::GetErrCodeUp(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.30"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nErrCode = pDoc->m_pPcr[0][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_LaserView::GetErrCodeDn(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.31"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nErrCode = pDoc->m_pPcr[1][nIdx]->m_nErrPnl;
	}
#endif

	return nErrCode;
}


int CGvisR2R_LaserView::GetErrCode0(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.32"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

#ifndef	TEST_MODE
	int nErr[2];
	nErr[0] = GetErrCodeUp0(nSerial);
	if (nErr[0] != 1)
		return nErr[0];
	if (bDualTest)
	{
		nErr[1] = GetErrCodeDn0(nSerial);
		if (nErr[1] != 1)
			return nErr[1];
	}
#endif

	return 1;
}

int CGvisR2R_LaserView::GetErrCodeUp0(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.33"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nErrCode = pDoc->m_pPcr[0][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_LaserView::GetErrCodeDn0(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.34"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nErrCode = pDoc->m_pPcr[1][nIdx]->m_nErrPnl;
	}
#endif

	return nErrCode;
}


int CGvisR2R_LaserView::GetErrCode1(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.35"));
		return 0;
	}

#ifndef	TEST_MODE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nErr[2];
	nErr[0] = GetErrCodeUp1(nSerial);
	if (nErr[0] != 1)
		return nErr[0];

	if (bDualTest)
	{
		nErr[1] = GetErrCodeDn1(nSerial);
		if (nErr[1] != 1)
			return nErr[1];
	}
#endif

	return 1;
}

int CGvisR2R_LaserView::GetErrCodeUp1(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.36"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nErrCode = pDoc->m_pPcr[0][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_LaserView::GetErrCodeDn1(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.37"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nErrCode = pDoc->m_pPcr[1][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}


int CGvisR2R_LaserView::GetTotDefPcs(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.38"));
		return 0;
	}

	int nTotDef = 0;
#ifndef TEST_MODE
	nTotDef = GetTotDefPcsUp(nSerial) + GetTotDefPcsDn(nSerial);
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsUp(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.39"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.40"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nTotDef = pDoc->m_pPcr[1][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}


int CGvisR2R_LaserView::GetTotDefPcs0(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.41"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])
		{
			if (pDoc->m_pPcr[2][nIdx])
				nTotDef = pDoc->m_pPcr[2][nIdx]->m_nTotDef;
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])
		{
			if (pDoc->m_pPcr[0][nIdx])
				nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
		}
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsUp0(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.42"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsDn0(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.43"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nTotDef = pDoc->m_pPcr[1][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}


int CGvisR2R_LaserView::GetTotDefPcs1(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.44"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])
		{
			if (pDoc->m_pPcr[2][nIdx])
				nTotDef = pDoc->m_pPcr[2][nIdx]->m_nTotDef;
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])
		{
			if (pDoc->m_pPcr[0][nIdx])
				nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
		}
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsUp1(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.45"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsDn1(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.46"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nTotDef = pDoc->m_pPcr[1][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}


CfPoint CGvisR2R_LaserView::GetMkPnt(int nMkPcs)
{
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	if (pDoc->m_Master[0].m_pPcsRgn)
		ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt(nMkPcs); // Cam0의 Mk 포인트.
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

CfPoint CGvisR2R_LaserView::GetMkPnt0(int nMkPcs)
{
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	if (pDoc->m_Master[0].m_pPcsRgn)
		ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt0(nMkPcs); // Cam0의 Mk 포인트.
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

CfPoint CGvisR2R_LaserView::GetMkPnt1(int nMkPcs)
{
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	if (pDoc->m_Master[0].m_pPcsRgn)
		ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt1(nMkPcs); // Cam1의 Mk 포인트.
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

CfPoint CGvisR2R_LaserView::GetMkPnt0(int nSerial, int nMkPcs)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.47"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx0(nSerial);
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	int nDefPcsId = 0;

	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[2][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt0(nDefPcsId); // Cam0의 Mk 포인트.
					}
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[0][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt0(nDefPcsId); // Cam0의 Mk 포인트.
					}
				}
			}
		}
	}
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

int CGvisR2R_LaserView::GetMkStripIdx0(int nDefPcsId) // 0 : Fail , 1~4 : Strip Idx
{
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / 4);
	int nStripIdx = 0;//nRow, nNum, nMode, 

#ifndef TEST_MODE
	int nRow = 0, nNum = 0, nMode = 0;
	nNum = int(nDefPcsId / nNodeY);
	nMode = nDefPcsId % nNodeY;
	if (nNum % 2) 	// 홀수.
		nRow = nNodeY - (nMode + 1);
	else		// 짝수.
		nRow = nMode;

	nStripIdx = int(nRow / nStripY) + 1;
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

int CGvisR2R_LaserView::GetMkStripIdx1(int nDefPcsId) // 0 : Fail , 1~4 : Strip Idx
{
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / 4);
	int nStripIdx = 0;//nNum, nMode, nRow, 

#ifndef TEST_MODE
	int nRow = 0, nNum = 0, nMode = 0;
	nNum = int(nDefPcsId / nNodeY);
	nMode = nDefPcsId % nNodeY;
	if (nNum % 2) 	// 홀수.
		nRow = nNodeY - (nMode + 1);
	else		// 짝수.
		nRow = nMode;

	nStripIdx = int(nRow / nStripY) + 1;
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

int CGvisR2R_LaserView::GetMkStripIdx0(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.48"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / MAX_STRIP_NUM);
	int nStripIdx = 0;

#ifndef TEST_MODE
	int nDefPcsId = 0, nNum = 0, nMode = 0, nRow = 0;

	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// 홀수.
						nRow = nNodeY - (nMode + 1);
					else		// 짝수.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// 홀수.
						nRow = nNodeY - (nMode + 1);
					else		// 짝수.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

CfPoint CGvisR2R_LaserView::GetMkPnt1(int nSerial, int nMkPcs)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.50"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	int nDefPcsId = 0;
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[2][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
						{
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt1(nDefPcsId); // Cam1의 Mk 포인트.
						}
					}
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[0][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
						{
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt1(nDefPcsId); // Cam1의 Mk 포인트.
						}
					}
				}
			}
		}
	}
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

int CGvisR2R_LaserView::GetMkStripIdx1(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.51"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / MAX_STRIP_NUM);
	int nStripIdx = 0;

#ifndef TEST_MODE
	int nDefPcsId = 0, nNum = 0, nMode = 0, nRow = 0;
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// 홀수.
						nRow = nNodeY - (nMode + 1);
					else		// 짝수.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// 홀수.
						nRow = nNodeY - (nMode + 1);
					else		// 짝수.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

void CGvisR2R_LaserView::Move0(CfPoint pt, BOOL bCam)
{
	double fLen, fVel, fAcc, fJerk;
	double pPos[2];

	if (pDoc->WorkingInfo.System.bNoMk || bCam)
	{
		pPos[0] = pt.x;
		pPos[1] = pt.y;
	}
	else
	{
		pPos[0] = pt.x + _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX);
		pPos[1] = pt.y + _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetY);
	}

	if (pPos[0] < 0.0)
		pPos[0] = 0.0;
	if (pPos[1] < 0.0)
		pPos[1] = 0.0;

	double dCurrX = pView->m_dEnc[AXIS_X0];
	double dCurrY = pView->m_dEnc[AXIS_Y0];
	fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move0(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

BOOL CGvisR2R_LaserView::IsMoveDone()
{
	if (!m_pMotion)
		return FALSE;

	if (IsMoveDone0())
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsMoveDone0()
{
	if (!m_pMotion)
		return FALSE;

	if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CGvisR2R_LaserView::LoadPcrUp(int nSerial, BOOL bFromShare)
{
	return TRUE;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.52"));
		return 0;
	}

	int nHeadInfo = pDoc->LoadPCR0(nSerial); // 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	if (nHeadInfo >= 2)
	{
		MsgBox(_T("Error-LoadPCR0()"));
		return FALSE;
	}
	return TRUE;
}

BOOL CGvisR2R_LaserView::LoadPcrDn(int nSerial, BOOL bFromShare)
{
	return TRUE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.53"));
		return 0;
	}

	int nHeadInfo = pDoc->LoadPCR1(nSerial); // 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	if (nHeadInfo >= 2)
	{
		MsgBox(_T("Error-LoadPCR1()"));
		return FALSE;
	}
	return TRUE;
}

BOOL CGvisR2R_LaserView::UpdateReelmap(int nSerial)
{
	return TRUE;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.54"));
		return 0;
	}

	if (!pDoc->MakeMkDir())
		return FALSE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CString str;
	CString sPathRmap[4], sPathPcr[2]; //[Up/Dn]

	if (pDoc->m_pReelMap)
	{
		stModelInfo stInfo;
		sPathPcr[0].Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
		if (bDualTest)
			sPathPcr[1].Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);

		if (!pDoc->GetPcrInfo(sPathPcr[0], stInfo)) // Up
		{
			pView->DispStsBar(_T("E(4)"), 5);
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error-GetPcrInfo(4)"));
			return FALSE;
		}

		if (!pDoc->MakeMkDir(stInfo.sModel, stInfo.sLot, stInfo.sLayer))
			return FALSE;

		str = _T("ReelMapDataUp.txt"); // [0]:AOI-Up
		sPathRmap[0].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			stInfo.sModel,
			stInfo.sLot,
			stInfo.sLayer,
			str);

		if (bDualTest)
		{
			str = _T("ReelMapDataAll.txt"); // [2]:AOI-AllUp
			sPathRmap[2].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				stInfo.sModel,
				stInfo.sLot,
				stInfo.sLayer,
				str);

			if (!pDoc->GetPcrInfo(sPathPcr[1], stInfo)) // Dn
			{
				pView->DispStsBar(_T("E(5)"), 5);
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-GetPcrInfo(5)"));
				return FALSE;
			}

			if (!pDoc->MakeMkDir(stInfo.sModel, stInfo.sLot, stInfo.sLayer))
				return FALSE;

			str = _T("ReelMapDataDn.txt"); // [1]:AOI-Dn
			sPathRmap[1].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				stInfo.sModel,
				stInfo.sLot,
				stInfo.sLayer,
				str);
			str = _T("ReelMapDataAll.txt"); // [3]:AOI-AllDn
			sPathRmap[3].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				stInfo.sModel,
				stInfo.sLot,
				stInfo.sLayer,
				str);
		}

		m_nSerialRmapUpdate = nSerial;
		m_sPathRmapUpdate[0] = sPathRmap[0];
		m_sPathRmapUpdate[1] = sPathRmap[1];
		m_sPathRmapUpdate[2] = sPathRmap[2];
		m_sPathRmapUpdate[3] = sPathRmap[3];

		m_bTHREAD_UPDATE_REELMAP_UP = TRUE;
		if (bDualTest)
		{
			m_bTHREAD_UPDATE_REELMAP_DN = TRUE;
			m_bTHREAD_UPDATE_REELMAP_ALLUP = TRUE;
			m_bTHREAD_UPDATE_REELMAP_ALLDN = TRUE;
		}

		Sleep(100);
		return TRUE;
	}
	else
		MsgBox(_T("Error-ReelMapWrite()"));

	return FALSE;
}


void CGvisR2R_LaserView::InitInfo()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateData();
}

void CGvisR2R_LaserView::InitReelmap()
{
	pDoc->InitReelmap();
	pDoc->SetReelmap(ROT_NONE);
	pDoc->UpdateData();
}

void CGvisR2R_LaserView::InitReelmapUp()
{
	pDoc->InitReelmapUp();
	pDoc->SetReelmap(ROT_NONE);
	pDoc->UpdateData();
}

void CGvisR2R_LaserView::InitReelmapDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return;

	pDoc->InitReelmapDn();
	pDoc->SetReelmap(ROT_NONE);
	pDoc->UpdateData();
}

 void CGvisR2R_LaserView::LoadMstInfo()
 {
	 BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

#ifdef TEST_MODE
	 pDoc->GetCamPxlRes();
	 if (IsLastJob(0)) // Up
	 {
		 pDoc->m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			 pDoc->WorkingInfo.LastJob.sModelUp,
			 pDoc->WorkingInfo.LastJob.sLayerUp);
		 pDoc->m_Master[0].LoadMstInfo();
	 }
	 if (IsLastJob(1)) // Dn
	 {
		 pDoc->m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			 pDoc->WorkingInfo.LastJob.sModelDn,
			 pDoc->WorkingInfo.LastJob.sLayerDn,
			 pDoc->WorkingInfo.LastJob.sLayerUp);
		 pDoc->m_Master[1].LoadMstInfo();
	 }
#else
	pDoc->GetCamPxlRes();

	if (IsLastJob(0) && pDoc->m_bLoadMstInfo[0]) // Up
	{
		pDoc->m_bLoadMstInfo[0] = FALSE;
		pDoc->m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLayerUp);
		pDoc->m_Master[0].LoadMstInfo();
	}

	if (IsLastJob(1) && pDoc->m_bLoadMstInfo[1]) // Dn
	{
		pDoc->m_bLoadMstInfo[1] = FALSE;
		pDoc->m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			pDoc->WorkingInfo.LastJob.sLayerUp);
		pDoc->m_Master[1].LoadMstInfo();
	}

#endif
	 // Reelmap 정보 Loading.....
	 InitReelmap(); // Delete & New

	 if (m_pDlgMenu01)
	 {
		 m_pDlgMenu01->InitGL();
		 m_bDrawGL = TRUE;
		 m_pDlgMenu01->RefreshRmap();
		 m_pDlgMenu01->InitCadImg();
		 m_pDlgMenu01->SetPnlNum();
		 m_pDlgMenu01->SetPnlDefNum();
	 }

	 if (m_pDlgMenu02)
	 {
		 m_pDlgMenu02->ChgModelUp(); // PinImg, AlignImg를 Display함.
	 }

#ifndef TEST_MODE
	 if (m_pDlgMenu01)
		 m_pDlgMenu01->RedrawWindow();

	 DispMsg(_T("릴맵을 초기화합니다."), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
	 OpenReelmap();
#endif
	 SetPathAtBuf(); // Reelmap path를 설정함.
	 LoadPcrFromBuf();

#ifndef TEST_MODE
	 int nSrl = pDoc->GetLastShotMk();
	 if (nSrl >= 0)
	 {
		 if (bDualTest)
			 m_pDlgMenu01->SelMap(ALL);
		 else
			 m_pDlgMenu01->SelMap(UP);
	 }
#endif

 }

BOOL CGvisR2R_LaserView::IsPinMkData()
{
	if (pDoc->IsPinMkData())
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsPinData()
{
	return pDoc->IsPinData();
}

BOOL CGvisR2R_LaserView::CopyDefImg(int nSerial)
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.55"));
		return 0;
	}

	return pDoc->CopyDefImg(nSerial);
}

BOOL CGvisR2R_LaserView::CopyDefImg(int nSerial, CString sNewLot)
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.56"));
		return 0;
	}

	return pDoc->CopyDefImg(nSerial, sNewLot);
}

BOOL CGvisR2R_LaserView::CopyDefImgUp(int nSerial, CString sNewLot)
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.57"));
		return 0;
	}

	return pDoc->CopyDefImgUp(nSerial, sNewLot);
}

BOOL CGvisR2R_LaserView::CopyDefImgDn(int nSerial, CString sNewLot)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.58"));
		return FALSE;
	}

	return pDoc->CopyDefImgDn(nSerial, sNewLot);
}

BOOL CGvisR2R_LaserView::MovePinPos()
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->MovePinPos();
}

BOOL CGvisR2R_LaserView::OnePointAlign(CfPoint &ptPnt)
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->OnePointAlign(ptPnt);
}

BOOL CGvisR2R_LaserView::IsHomeDone(int nMsId)
{
	if (!m_pMotion)
		return FALSE;

	return m_pMotion->IsHomeDone(nMsId);
}

BOOL CGvisR2R_LaserView::GetAoiUpInfo(int nSerial, int *pNewLot, BOOL bFromBuf)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.59"));
		return 0;
	}

	return pDoc->GetAoiUpInfo(nSerial, pNewLot, bFromBuf);
}

BOOL CGvisR2R_LaserView::GetAoiDnInfo(int nSerial, int *pNewLot, BOOL bFromBuf)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.60"));
		return 0;
	}

	return pDoc->GetAoiDnInfo(nSerial, pNewLot, bFromBuf);
}

BOOL CGvisR2R_LaserView::LoadMySpec()
{
	return pDoc->LoadMySpec();
}

CString CGvisR2R_LaserView::GetProcessNum()
{
	return pDoc->GetProcessNum();
}

BOOL CGvisR2R_LaserView::GetAoiUpOffset(CfPoint &OfSt)
{
	return pDoc->GetAoiUpOffset(OfSt);
}

BOOL CGvisR2R_LaserView::GetAoiDnOffset(CfPoint &OfSt)
{
	return pDoc->GetAoiDnOffset(OfSt);
}

BOOL CGvisR2R_LaserView::GetMkOffset(CfPoint &OfSt)
{
	if (m_pDlgMenu02)
	{
		OfSt.x = m_pDlgMenu02->m_dMkFdOffsetX[0][0]; // -: 제품 덜옴, +: 제품 더옴.
		OfSt.y = m_pDlgMenu02->m_dMkFdOffsetY[0][0]; // -: 제품 덜옴, +: 제품 더옴.
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::IsAoiLdRun()
{
	return TRUE;
}

BOOL CGvisR2R_LaserView::IsInitPos0()
{
	if (!m_pMotion)
		return FALSE;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
		return FALSE;
	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
		return FALSE;

	return TRUE;
}

void CGvisR2R_LaserView::MoveInitPos0(BOOL bWait)
{
	if (!m_pMotion)
		return;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		if(bWait)
			m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, WAIT);
		else
			m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

BOOL CGvisR2R_LaserView::IsSetLotEnd()
{
	if (m_nLotEndSerial > 0)
		return TRUE;
	return FALSE;
}

void CGvisR2R_LaserView::SetLotEnd(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.61"));
		return;
	}
	m_nLotEndSerial = nSerial;

	CString str;
	str.Format(_T("%d"), m_nLotEndSerial);
	DispStsBar(str, 0);
}

int CGvisR2R_LaserView::GetLotEndSerial()
{
	return m_nLotEndSerial; // 테이블상에 정지하는 Serial.
}

BOOL CGvisR2R_LaserView::StartLive()
{
	if (StartLive0())	// && StartLive1())
		return TRUE;

	return  FALSE;
}

BOOL CGvisR2R_LaserView::StartLive0()
{
	BOOL bRtn0 = FALSE;

#ifdef USE_VISION
	if (m_pVision[0])
		bRtn0 = m_pVision[0]->StartLive();
#endif
	if (bRtn0)
		return TRUE;

	return  FALSE;
}

BOOL CGvisR2R_LaserView::StopLive()
{
#ifdef TEST_MODE
	return TRUE;
#endif

	if (StopLive0())	// && StopLive1())
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_LaserView::StopLive0()
{
#ifdef TEST_MODE
	return TRUE;
#endif

	BOOL bRtn0 = FALSE;

#ifdef USE_VISION
	if (m_pVision[0])
		bRtn0 = m_pVision[0]->StopLive();
#endif
	if (bRtn0)
		return TRUE;

	return FALSE;
}

void CGvisR2R_LaserView::UpdateRst()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateRst();
}

BOOL CGvisR2R_LaserView::IsChkTmpStop()
{
	if (pDoc->WorkingInfo.LastJob.bTempPause)
	{
		double dFdLen = GetMkFdLen();
		if (dFdLen >= _tstof(pDoc->WorkingInfo.LastJob.sTempPauseLen)*1000.0)
		{
			pDoc->WorkingInfo.LastJob.bTempPause = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsVerify()
{
	BOOL bVerify = FALSE;
	int nSerial0 = m_nBufUpSerial[0];
	int nSerial1 = m_nBufUpSerial[1];
	int nPeriod = pDoc->WorkingInfo.LastJob.nVerifyPeriod;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		nSerial0 = m_nBufDnSerial[0];
		nSerial1 = m_nBufDnSerial[1];
	}


	if (pDoc->WorkingInfo.LastJob.bVerify)
	{
		double dFdLen = GetMkFdLen();
		double dVerifyLen = _tstof(pDoc->WorkingInfo.LastJob.sVerifyLen)*1000.0;
		if (dFdLen < dVerifyLen)
		{
			if (nSerial0 == 1 || nPeriod == 0 || nPeriod == 1 || nPeriod == 2 || m_bStopF_Verify)
			{
				m_bStopF_Verify = FALSE;
				bVerify = TRUE;
			}
			else
			{
				if (!(nSerial0 % nPeriod) || !(nSerial1 % nPeriod))
					bVerify = TRUE;
			}
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bVerify = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();
		}
	}

	return bVerify;
}

int CGvisR2R_LaserView::GetVsBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotMk();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

int CGvisR2R_LaserView::GetVsUpBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotUp();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

int CGvisR2R_LaserView::GetVsDnBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotDn();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

BOOL CGvisR2R_LaserView::IsFixPcsUp(int nSerial)
{
	if (!pDoc->m_pReelMapUp)
		return FALSE;

	CString sMsg = _T(""), str = _T("");
	int nStrip, pCol[2500], pRow[2500], nTot;

	BOOL bIsFixPcs = FALSE;
	if (pDoc->m_pReelMapUp)
		bIsFixPcs = pDoc->m_pReelMapUp->IsFixPcs(nSerial, pCol, pRow, nTot);

	if (bIsFixPcs)
	{
		int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
		int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
		int nStPcsY = nNodeY / MAX_STRIP_NUM;

		sMsg.Format(_T("상면 고정불량 발생"));
		for (int i = 0; i < nTot; i++)
		{
			nStrip = int(pRow[i] / nStPcsY);
			if (!(i % 5))
				str.Format(_T("\r\n[%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			else
				str.Format(_T(" , [%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);

			sMsg += str;
		}
		m_sFixMsg[0] = sMsg;
		return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsFixPcsDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (!pDoc->m_pReelMapUp)
		return FALSE;

	CString sMsg = _T(""), str = _T("");
	int nStrip, pCol[2500], pRow[2500], nTot;

	BOOL bIsFixPcs = FALSE;
	if (pDoc->m_pReelMapDn)
		bIsFixPcs = pDoc->m_pReelMapDn->IsFixPcs(nSerial, pCol, pRow, nTot);

	if (bIsFixPcs)
	{
		int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
		int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
		int nStPcsY = nNodeY / 4;

		sMsg.Format(_T("하면 고정불량 발생"));
		for (int i = 0; i < nTot; i++)
		{
			nStrip = int(pRow[i] / nStPcsY);
			if (!(i % 5))
				str.Format(_T("\r\n[%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			else
				str.Format(_T(" , [%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);

			sMsg += str;
		}
		m_sFixMsg[1] = sMsg;
		return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsReview()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CGvisR2R_LaserView::IsReview0()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CGvisR2R_LaserView::IsReview1()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

void CGvisR2R_LaserView::OpenShareUp(BOOL bOpen)
{
	m_bOpenShareUp = bOpen;
}

BOOL CGvisR2R_LaserView::IsOpenShareUp()
{
	return m_bOpenShareUp;
}

void CGvisR2R_LaserView::OpenShareDn(BOOL bOpen)
{
	m_bOpenShareDn = bOpen;
}

BOOL CGvisR2R_LaserView::IsOpenShareDn()
{
	return m_bOpenShareDn;
}


void CGvisR2R_LaserView::SwAoiEmg(BOOL bOn)
{
}

BOOL CGvisR2R_LaserView::IsVs()
{
	if (!m_bChkLastProcVs)
	{
		BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
		if (bDualTest)
		{
			if (GetAoiDnVsStatus())
				return TRUE;
			else if (GetAoiUpVsStatus())
				return TRUE;
		}
		else
		{
			if (GetAoiUpVsStatus())
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsVsUp()
{
	return GetAoiUpVsStatus();
}

BOOL CGvisR2R_LaserView::IsVsDn()
{
	return GetAoiDnVsStatus();
}

BOOL CGvisR2R_LaserView::GetAoiUpVsStatus()
{
	BOOL bVsStatus = FALSE;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current VS Status"), NULL, szData, sizeof(szData), sPath))
		bVsStatus = _tstoi(szData) > 0 ? TRUE : FALSE;

	return bVsStatus;
}

BOOL CGvisR2R_LaserView::GetAoiDnVsStatus()
{
	BOOL bVsStatus = FALSE;
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current VS Status"), NULL, szData, sizeof(szData), sPath))
		bVsStatus = _tstoi(szData) > 0 ? TRUE : FALSE;

	return bVsStatus;
}

BOOL CGvisR2R_LaserView::IsDoneDispMkInfo()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = m_pDlgMenu01->IsDoneDispMkInfo();
	return bRtn;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CGvisR2R_LaserView::DispDefImg()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	int nBreak = 0;

	switch (m_nStepTHREAD_DISP_DEF)
	{
		// CopyDefImg Start ============================================
	case 0:
		m_nStepTHREAD_DISP_DEF++;
		m_nBufUpSerial[0] = m_nBufDnSerial[0] = _ttoi(pView->GetMkMenu01(_T("DispDefImg"), _T("SerialL")));
		m_nBufUpSerial[1] = m_nBufDnSerial[1] = _ttoi(pView->GetMkMenu01(_T("DispDefImg"), _T("SerialR")));

		if (bDualTest)
		{
			nSerial = m_nBufDnSerial[0];
			sNewLot = m_sNewLotDn;
		}
		else
		{
			nSerial = m_nBufUpSerial[0];
			sNewLot = m_sNewLotUp;
		}
		break;
	case 1:
		Sleep(300);
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 2:
		if (bDualTest)
			nSerial = m_nBufDnSerial[0];
		else
			nSerial = m_nBufUpSerial[0];

		if (IsDoneDispMkInfo())	 // Check 불량이미지 Display End
		{
			if (ChkLastProc() && (nSerial > m_nLotEndSerial))
			{
				SetSerialReelmap(nSerial, TRUE);	// Reelmap(좌) Display Start
				SetSerialMkInfo(nSerial, TRUE);		// 불량이미지(좌) Display Start
			}
			else if (ChkLastProc() && nSerial < 1)
			{
				SetSerialReelmap(m_nLotEndSerial + 1, TRUE);		// Reelmap(좌) Display Start
				SetSerialMkInfo(m_nLotEndSerial + 1, TRUE);		// 불량이미지(좌) Display Start
			}
			else
			{
				SetFixPcs(nSerial);
				SetSerialReelmap(nSerial);	// Reelmap(좌) Display Start
				SetSerialMkInfo(nSerial);	// 불량이미지(좌) Display Start
			}

			m_nStepTHREAD_DISP_DEF++;
		}

		break;

	case 3:
		if (IsDoneDispMkInfo())	 // Check 불량이미지(좌) Display End
		{
			if (bDualTest)
				nSerial = m_nBufDnSerial[1];
			else
				nSerial = m_nBufUpSerial[1];

			if (nSerial > 0)
			{
				if (ChkLastProc() && (nSerial > m_nLotEndSerial))
					SetSerialMkInfo(nSerial, TRUE);	// 불량이미지(우) Display Start
				else
					SetSerialMkInfo(nSerial);		// 불량이미지(우) Display Start
			}
			else
			{
				if (ChkLastProc())
					SetSerialMkInfo(m_nLotEndSerial + 1, TRUE);	// 불량이미지(우) Display Start
			}
			m_nStepTHREAD_DISP_DEF++;
		}
		break;
	case 4:
		if (bDualTest)
			nSerial = m_nBufDnSerial[1];
		else
			nSerial = m_nBufUpSerial[1];

		if (nSerial > 0)
		{
			m_nStepTHREAD_DISP_DEF++;

			if (ChkLastProc() && (nSerial > m_nLotEndSerial))
			{
				SetSerialReelmap(nSerial, TRUE);	// Reelmap(우) Display Start
			}
			else
			{
				SetFixPcs(nSerial);
				SetSerialReelmap(nSerial);			// Reelmap(우) Display Start
			}
		}
		else
		{
			if (ChkLastProc())
			{
				m_nStepTHREAD_DISP_DEF++;
				SetSerialReelmap(m_nLotEndSerial + 1, TRUE);	// 불량이미지(우) Display Start
			}
			else
			{
				m_nStepTHREAD_DISP_DEF++;
			}
		}
		break;
	case 5:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 6:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 7:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 8:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 9:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 10:
		m_nStepTHREAD_DISP_DEF++;
		break;

	case 11:
		if (IsDoneDispMkInfo() && IsRun())	 // Check 불량이미지(우) Display End
			m_nStepTHREAD_DISP_DEF++;
		break;
	case 12:
		m_bTHREAD_DISP_DEF = FALSE;
		break;
		// CopyDefImg End ============================================
	}

}

BOOL CGvisR2R_LaserView::IsSameUpDnLot()
{
	if (pDoc->Status.PcrShare[0].sLot == pDoc->Status.PcrShare[1].sLot)
		return TRUE;

	return FALSE;
}

int CGvisR2R_LaserView::GetAutoStep()
{
	return m_nStepAuto;
}

void CGvisR2R_LaserView::MoveMkInitPos()
{
	MoveMk0InitPos();
}

void CGvisR2R_LaserView::MoveMk0InitPos()
{
	double pTgtPos[2];
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	double dCurrX = pView->m_dEnc[AXIS_X0];
	double dCurrY = pView->m_dEnc[AXIS_Y0];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		pView->m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		if (!pView->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
		{
			if (!pView->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
				AfxMessageBox(_T("Move X0Y0 Error..."));
		}
	}
}


BOOL CGvisR2R_LaserView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message != WM_KEYDOWN)
		return CFormView::PreTranslateMessage(pMsg);

	if ((pMsg->lParam & 0x40000000) == 0)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;
		case VK_ESCAPE:
			return TRUE;
		case 'S':
		case 's':
			if (GetKeyState(VK_CONTROL) < 0) // Ctrl 키가 눌려진 상태
			{
				WINDOWPLACEMENT wndPlace;
				AfxGetMainWnd()->GetWindowPlacement(&wndPlace);
				wndPlace.showCmd |= SW_MAXIMIZE;
				AfxGetMainWnd()->SetWindowPlacement(&wndPlace);
			}
			break;
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}


int CGvisR2R_LaserView::MyPassword(CString strMsg, int nCtrlId)
{
	CDlgMyPassword dlg1(this);
	dlg1.SetMsg(strMsg, nCtrlId);
	dlg1.DoModal();
	return (dlg1.m_nRtnVal);

}

BOOL CGvisR2R_LaserView::ReloadRst()
{
	return TRUE;

	double dRatio = 0.0;
	CString sVal = _T("");
	CDlgProgress dlg;
	sVal.Format(_T("On Reloading Reelmap."));
	dlg.Create(sVal);

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	BOOL bRtn[5];
	if (pDoc->m_pReelMap)
		bRtn[0] = pDoc->m_pReelMap->ReloadRst();
	if (pDoc->m_pReelMapUp)
		bRtn[1] = pDoc->m_pReelMapUp->ReloadRst();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			bRtn[2] = pDoc->m_pReelMapDn->ReloadRst();
		if (pDoc->m_pReelMapAllUp)
			bRtn[3] = pDoc->m_pReelMapAllUp->ReloadRst();
		if (pDoc->m_pReelMapAllDn)
			bRtn[4] = pDoc->m_pReelMapAllDn->ReloadRst();
	}

	int nRatio[5] = { 0 };
	BOOL bDone[5] = { 0 };
	int nTo = 0;
	if (bDualTest)
		nTo = 500; //[%]
	else
		nTo = 200; //[%]

	dlg.SetRange(0, nTo);

	for (int nProc = 0; nProc < nTo;)
	{
		if (pDoc->m_pReelMap)
		{
			nRatio[0] = pDoc->m_pReelMap->GetProgressReloadRst();
			bDone[0] = pDoc->m_pReelMap->IsDoneReloadRst();
		}
		else
			bDone[0] = TRUE;
		if (!bRtn[0])
			bDone[0] = TRUE;

		if (pDoc->m_pReelMapUp)
		{
			nRatio[1] = pDoc->m_pReelMapUp->GetProgressReloadRst();
			bDone[1] = pDoc->m_pReelMapUp->IsDoneReloadRst();
		}
		else
			bDone[1] = TRUE;
		if (!bRtn[1])
			bDone[1] = TRUE;

		if (bDualTest)
		{
			if (pDoc->m_pReelMapDn)
			{
				nRatio[2] = pDoc->m_pReelMapDn->GetProgressReloadRst();
				bDone[2] = pDoc->m_pReelMapDn->IsDoneReloadRst();
			}
			else
				bDone[2] = TRUE;
			if (!bRtn[2])
				bDone[2] = TRUE;

			if (pDoc->m_pReelMapAllUp)
			{
				nRatio[3] = pDoc->m_pReelMapAllUp->GetProgressReloadRst();
				bDone[3] = pDoc->m_pReelMapAllUp->IsDoneReloadRst();
			}
			else
				bDone[3] = TRUE;
			if (!bRtn[3])
				bDone[3] = TRUE;

			if (pDoc->m_pReelMapAllDn)
			{
				nRatio[4] = pDoc->m_pReelMapAllDn->GetProgressReloadRst();
				bDone[4] = pDoc->m_pReelMapAllDn->IsDoneReloadRst();
			}
			else
				bDone[4] = TRUE;
			if (!bRtn[4])
				bDone[4] = TRUE;

		}
		else
		{
			bDone[2] = TRUE;
			bDone[3] = TRUE;
			bDone[4] = TRUE;
		}

		nProc = nRatio[0] + nRatio[1] + nRatio[2] + nRatio[3] + nRatio[4];

		if (bDone[0] && bDone[1] && bDone[2] && bDone[3] && bDone[4])
			break;
		else
		{
			dlg.SetPos(nProc);
			Sleep(30);
		}
	}

	dlg.DestroyWindow();

	if (bDualTest)
	{
		for (int i = 0; i < 5; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		return ReloadRstInner();
	}

	return TRUE;
}
void CGvisR2R_LaserView::ReloadRstUp()
{
return;
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->ReloadRst();
}

void CGvisR2R_LaserView::ReloadRstAllUp()
{
	return;

	if (pDoc->m_pReelMapAllUp)
		pDoc->m_pReelMapAllUp->ReloadRst();
}

void CGvisR2R_LaserView::ReloadRstDn()
{
	return;
	if (pDoc->m_pReelMapDn)
		pDoc->m_pReelMapDn->ReloadRst();
}

void CGvisR2R_LaserView::ReloadRstAllDn()
{
	return;
	if (pDoc->m_pReelMapAllDn)
		pDoc->m_pReelMapAllDn->ReloadRst();
}

BOOL CGvisR2R_LaserView::ReloadRst(int nSerial)
{
	return TRUE;

	m_nReloadRstSerial = nSerial;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	m_bTHREAD_RELOAD_RST_UP = TRUE;
	if (bDualTest)
	{
		m_bTHREAD_RELOAD_RST_DN = TRUE;
		m_bTHREAD_RELOAD_RST_ALLUP = TRUE;
		m_bTHREAD_RELOAD_RST_ALLDN = TRUE;
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		m_bTHREAD_RELOAD_RST_UP_INNER = TRUE;
		m_bTHREAD_RELOAD_RST_ITS = TRUE;
		if (pDoc->WorkingInfo.LastJob.bDualTestInner)
		{
			m_bTHREAD_RELOAD_RST_DN_INNER = TRUE;
			m_bTHREAD_RELOAD_RST_ALLUP_INNER = TRUE;
			m_bTHREAD_RELOAD_RST_ALLDN_INNER = TRUE;
		}
	}

	Sleep(100);


	return TRUE;
}

void CGvisR2R_LaserView::ReloadRstUpInner()
{
return;
	if (pDoc->m_pReelMapInnerUp)
		pDoc->m_pReelMapInnerUp->ReloadRst();
}

void CGvisR2R_LaserView::ReloadRstAllUpInner()
{
return;
	if (pDoc->m_pReelMapInnerAllUp)
		pDoc->m_pReelMapInnerAllUp->ReloadRst();
}

void CGvisR2R_LaserView::ReloadRstDnInner()
{
	return;
	if (pDoc->m_pReelMapInnerDn)
		pDoc->m_pReelMapInnerDn->ReloadRst();
}

BOOL CGvisR2R_LaserView::OpenReelmapFromBuf(int nSerial)
{
	return TRUE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	stModelInfo stInfoUp, stInfoDn;
	CString sSrc;
	sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	if (!pDoc->GetPcrInfo(sSrc, stInfoUp))
	{
		pView->DispStsBar(_T("E(6)"), 5);
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-GetPcrInfo(6)"));
		return FALSE;
	}
	if (bDualTest)
	{
		sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
		if (!pDoc->GetPcrInfo(sSrc, stInfoDn))
		{
			pView->DispStsBar(_T("E(7)"), 5);
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error-GetPcrInfo(7)"));
			return FALSE;
		}
	}

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->Open();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->Open();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->Open();
	}

	return TRUE;
}

void CGvisR2R_LaserView::OpenReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->Open();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->Open();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->Open();
	}
}

void CGvisR2R_LaserView::OpenReelmapUp()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->Open();
	}

	if (pDoc->m_pReelMap)
	{
		if (bDualTest)
		{
			if (pDoc->m_pReelMap->m_nLayer == RMAP_UP || pDoc->m_pReelMap->m_nLayer == RMAP_ALLUP)
				pDoc->m_pReelMap->Open();
		}
		else
		{
			if (pDoc->m_pReelMap->m_nLayer == RMAP_UP)
				pDoc->m_pReelMap->Open();
		}
	}
}

void CGvisR2R_LaserView::OpenReelmapDn()
{
	return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return;

	if (pDoc->m_pReelMapDn)
		pDoc->m_pReelMapDn->Open();
	if (pDoc->m_pReelMapAllDn)
		pDoc->m_pReelMapAllDn->Open();
}

void CGvisR2R_LaserView::EStop()
{
	if (m_pMotion)
	{
		m_pMotion->EStop(MS_X0Y0);
		Sleep(30);
		ResetMotion(MS_X0Y0);
		Sleep(30);
		AfxMessageBox(_T("X축 충돌 범위에 의한 정지입니다."));
	}
}

BOOL CGvisR2R_LaserView::IsRunAxisX()
{
	if (m_pMotion->IsMotionDone(MS_X0))
		return FALSE;
	return TRUE;
}

BOOL CGvisR2R_LaserView::MoveAlign0(int nPos)
{
	if (!m_pMotion)
		return FALSE;

	if (m_pDlgMenu02)
		m_pDlgMenu02->SetLight();

	if (m_pMotion->m_dPinPosY[0] > -5.0 && m_pMotion->m_dPinPosX[0] > -5.0)
	{
		double dCurrX = m_dEnc[AXIS_X0];
		double dCurrY = m_dEnc[AXIS_Y0];

		double pPos[2];
		if (nPos == 0)
		{
			pPos[0] = pDoc->m_Master[0].m_stAlignMk.X0 + m_pMotion->m_dPinPosX[0];
			pPos[1] = pDoc->m_Master[0].m_stAlignMk.Y0 + m_pMotion->m_dPinPosY[0];
		}
		else if (nPos == 1)
		{
			pPos[0] = pDoc->m_Master[0].m_stAlignMk.X1 + m_pMotion->m_dPinPosX[0];
			pPos[1] = pDoc->m_Master[0].m_stAlignMk.Y1 + m_pMotion->m_dPinPosY[0];
		}

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			pView->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
			if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
			{
				if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Error - Move MoveAlign0 ..."));
					return FALSE;
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

void CGvisR2R_LaserView::SetLastProc()
{

	if (m_pDlgMenu01)
		m_pDlgMenu01->SetLastProc();
}

BOOL CGvisR2R_LaserView::IsLastProc()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = m_pDlgMenu01->IsLastProc();
	else
		bRtn = FALSE;

	return bRtn;
}

BOOL CGvisR2R_LaserView::IsLastJob(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
{
	switch (nAoi)
	{
	case 0: // AOI-Up
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			return FALSE;
		break;
	case 1: // AOI-Dn
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerDn.IsEmpty())
			return FALSE;
		break;
	case 2: // AOI-UpDn
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			return FALSE;
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerDn.IsEmpty())
			return FALSE;
		break;
	}

	return TRUE;
}

void CGvisR2R_LaserView::MonDispMain()
{
	CString sDisp = pDoc->GetMonDispMain();

	if (sDisp == _T("운전중") && m_sMonDisp != _T("운전중"))
	{		
		DispMain(_T("운전중"), RGB_GREEN);
	}

	if (sDisp == _T("운전준비") && m_sMonDisp != _T("운전준비"))
	{
		DispMain(_T("운전준비"), RGB_GREEN);
	}

	if (sDisp == _T("양면샘플") && m_sMonDisp != _T("양면샘플"))
	{
		DispMain(_T("양면샘플"), RGB_GREEN);
	}

	if (sDisp == _T("양면검사") && m_sMonDisp != _T("양면검사"))
	{
		DispMain(_T("양면검사"), RGB_GREEN);
	}

	if (sDisp == _T("단면검사") && m_sMonDisp != _T("단면검사"))
	{
		DispMain(_T("단면검사"), RGB_GREEN);
	}

	if (sDisp == _T("내층검사") && m_sMonDisp != _T("내층검사"))
	{
		DispMain(_T("내층검사"), RGB_GREEN);
	}

	if (sDisp == _T("외층검사") && m_sMonDisp != _T("외층검사"))
	{
		DispMain(_T("외층검사"), RGB_GREEN);
	}

	if (sDisp == _T("정 지") && m_sMonDisp != _T("정 지"))
	{
		pView->DispStsBar(_T("정지-44"), 0);
		DispMain(_T("정 지"), RGB_RED);
	}
}

void CGvisR2R_LaserView::ChkTempStop(BOOL bChk)
{
	if (bChk)
	{
		if (!m_bTIM_CHK_TEMP_STOP)
		{
			m_bTIM_CHK_TEMP_STOP = TRUE;
			SetTimer(TIM_CHK_TEMP_STOP, 100, NULL);
		}
	}
	else
	{
		m_bTIM_CHK_TEMP_STOP = FALSE;
	}
}

void CGvisR2R_LaserView::ChgLot()
{
	pDoc->WorkingInfo.LastJob.sLotUp = pDoc->Status.PcrShare[0].sLot;
	pDoc->SetModelInfoUp();

	pDoc->WorkingInfo.LastJob.sLotDn = pDoc->Status.PcrShare[1].sLot;
	pDoc->SetModelInfoDn();
	SetPathAtBuf();
}

void CGvisR2R_LaserView::LoadPcrFromBuf()
{
	return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString str, sTemp;

	if (ChkBufUp(m_pBufSerial[0], m_nBufTot[0]))
	{
		for (int i = 0; i < m_nBufTot[0]; i++)
		{
			LoadPcrUp(m_pBufSerial[0][i]);
			if (!bDualTest)
				UpdateReelmap(m_pBufSerial[0][i]);
		}
	}

	if (bDualTest)
	{
		if (ChkBufDn(m_pBufSerial[1], m_nBufTot[1]))
		{
			for (int i = 0; i < m_nBufTot[1]; i++)
			{
				LoadPcrDn(m_pBufSerial[1][i]);
				UpdateReelmap(m_pBufSerial[1][i]); // After inspect bottom side.
			}
		}
	}
}

void CGvisR2R_LaserView::SetPathAtBuf()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetPathAtBuf();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetPathAtBuf();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetPathAtBuf();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->SetPathAtBuf();
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (pDoc->m_pReelMapIts)
			pDoc->m_pReelMapIts->SetPathAtBuf();
	}
}

void CGvisR2R_LaserView::SetPathAtBufUp()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetPathAtBuf();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetPathAtBuf();
	}
}

void CGvisR2R_LaserView::SetPathAtBufDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return;

	if (pDoc->m_pReelMapDn)
		pDoc->m_pReelMapDn->SetPathAtBuf();
	if (pDoc->m_pReelMapAllDn)
		pDoc->m_pReelMapAllDn->SetPathAtBuf();
}


void  CGvisR2R_LaserView::SetLotLastShot()
{
	pDoc->m_nLotLastShot = pDoc->m_nBufLastShot = int(_tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0 / _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));
}

BOOL CGvisR2R_LaserView::IsMkStrip(int nStripIdx)
{
	if (!m_pDlgMenu01 || nStripIdx < 1 || nStripIdx > MAX_STRIP_NUM)
		return TRUE;

	return (m_pDlgMenu01->GetChkStrip(nStripIdx - 1));
}

void CGvisR2R_LaserView::CycleStop()
{
	m_bCycleStop = TRUE;
}

BOOL CGvisR2R_LaserView::ChkLotCutPos()
{
	if (pDoc->WorkingInfo.LastJob.bLotSep && pDoc->m_bDoneChgLot)
	{
		double dFdTotLen = GetMkFdLen();
		double dLotCutPos = _tstof(pDoc->WorkingInfo.LastJob.sLotCutPosLen)*1000.0;
		if (dFdTotLen >= dLotCutPos)
		{
			pDoc->WorkingInfo.LastJob.bLotSep = FALSE;
			if (pDoc->m_pReelMap)
				pDoc->m_pReelMap->m_bUseLotSep = FALSE;

			::WritePrivateProfileString(_T("Last Job"), _T("Use Lot seperate"), _T("0"), PATH_WORKING_INFO);

			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::ChkStShotNum()
{
	CString sMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_ListBuf[0].nTot == 0)
	{
		if (m_nShareUpS > 0 && !(m_nShareUpS % 2))
		{
			sMsg.Format(_T("AOI 상면의 시리얼이 짝수로 시작하였습니다.\r\n- 시리얼 번호: %d"), m_nShareUpS);
			MsgBox(sMsg);
			return FALSE;
		}
	}

	if (bDualTest)
	{
		if (pDoc->m_ListBuf[1].nTot == 0)
		{
			if (m_nShareDnS > 0 && !(m_nShareDnS % 2))
			{
				sMsg.Format(_T("AOI 하면의 시리얼이 짝수로 시작하였습니다.\r\n- 시리얼 번호: %d"), m_nShareDnS);
				MsgBox(sMsg);
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkContShotNum()
{
	CString sMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!m_pDlgFrameHigh)
		return FALSE;

	if (pDoc->m_ListBuf[0].nTot == 0)
	{
		if (m_nShareUpS > 0 && m_pDlgFrameHigh->m_nMkLastShot + 1 != m_nShareUpS)
		{
			sMsg.Format(_T("AOI 상면의 시작Shot(%d)이 마지막Shot(%d)과 불연속입니다.\r\n계속 진행하시겠습니까?"), m_nShareUpS, m_pDlgFrameHigh->m_nMkLastShot);
			if (IDNO == MsgBox(sMsg, 0, MB_YESNO))
				return FALSE;
		}
	}

	return TRUE;
}

void CGvisR2R_LaserView::SetFixPcs(int nSerial)
{
	return;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetFixPcs(nSerial);
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetFixPcs(nSerial);
	}
}

BOOL CGvisR2R_LaserView::RemakeReelmap()
{
	return TRUE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sReelmapSrc, str;

	str = _T("ReelMapDataUp.txt");
	sReelmapSrc.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		str);
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->RemakeReelmap(sReelmapSrc);

	if (bDualTest)
	{
		str = _T("ReelMapDataDn.txt");
		sReelmapSrc.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->RemakeReelmap(sReelmapSrc);

		str = _T("ReelMapDataAll.txt");
		sReelmapSrc.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->RemakeReelmap(sReelmapSrc);
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::IsDoneRemakeReelmap()
{
	return TRUE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bSuccess[3] = { FALSE };
	DWORD dwSt = GetTickCount();
	BOOL bThreadAliveRemakeReelmapUp = FALSE;
	BOOL bThreadAliveRemakeReelmapDn = FALSE;
	BOOL bThreadAliveRemakeReelmapAllUp = FALSE;
	BOOL bThreadAliveRemakeReelmapAllDn = FALSE;

	do
	{
		if (GetTickCount() - dwSt > 600000)
			break;
		if (pDoc->m_pReelMapUp)
			bThreadAliveRemakeReelmapUp = pDoc->m_pReelMapUp->m_bThreadAliveRemakeReelmap;
		if (pDoc->m_pReelMapDn)
			bThreadAliveRemakeReelmapDn = pDoc->m_pReelMapDn->m_bThreadAliveRemakeReelmap;
		if (pDoc->m_pReelMapAllUp)
			bThreadAliveRemakeReelmapAllUp = pDoc->m_pReelMapAllUp->m_bThreadAliveRemakeReelmap;
	} while (bThreadAliveRemakeReelmapUp || bThreadAliveRemakeReelmapDn || bThreadAliveRemakeReelmapAllUp);

	if (bDualTest)
	{
		if (pDoc->m_pReelMapUp)
			bSuccess[0] = pDoc->m_pReelMapUp->m_bRtnThreadRemakeReelmap;
		if (pDoc->m_pReelMapDn)
			bSuccess[1] = pDoc->m_pReelMapDn->m_bRtnThreadRemakeReelmap;
		if (pDoc->m_pReelMapAllUp)
			bSuccess[2] = pDoc->m_pReelMapAllUp->m_bRtnThreadRemakeReelmap;

		if (!bSuccess[0] || !bSuccess[2] || !bSuccess[1])
		{
			MsgBox(_T("ReelMap Converting Failed."));
			return FALSE;
		}
	}
	else
	{
		if (pDoc->m_pReelMapUp)
		{
			if (!pDoc->m_pReelMapUp->m_bRtnThreadRemakeReelmap)
			{
				MsgBox(_T("ReelMap Converting Failed."));
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkLightErr()
{
	int nSerial, nErrCode;
	BOOL bError = FALSE;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
		nSerial = m_nBufDnSerial[0];//GetBuffer0();
	else
		nSerial = m_nBufUpSerial[0];//GetBuffer0();

	if (nSerial > 0)
	{
		if ((nErrCode = GetErrCode0(nSerial)) != 1)
		{
			if (nErrCode == -1) // -1(Align Error, 노광불량)
				bError = TRUE;
		}
	}

	if (bDualTest)
		nSerial = m_nBufDnSerial[1];
	else
		nSerial = m_nBufUpSerial[1];

	if (nSerial > 0)
	{
		if ((nErrCode = GetErrCode1(nSerial)) != 1)
		{
			if (nErrCode == -1) // -1(Align Error, 노광불량)
				bError = TRUE;
		}
	}

	if (bError)
	{
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		pView->DispStsBar(_T("정지-45"), 0);
		DispMain(_T("정 지"), RGB_RED);
	}

	return bError;
}

void CGvisR2R_LaserView::CntMk()
{
}

BOOL CGvisR2R_LaserView::IsOnMarking0()
{
	if (m_nMkPcs[0] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsOnMarking1()
{
	if (m_nMkPcs[1] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		return TRUE;

	return FALSE;
}

void CGvisR2R_LaserView::SetDualTest(BOOL bOn)
{
	if (m_pDlgFrameHigh)
		m_pDlgFrameHigh->SetDualTest(bOn);
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetDualTest(bOn);
}

void CGvisR2R_LaserView::SetTwoMetal(BOOL bSel, BOOL bOn)
{
	if (bSel)
	{
		if (bOn)
		{
			pDoc->WorkingInfo.LastJob.bTwoMetal = TRUE;
			::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_TWO_METAL - Uncoiler\r역방향 ON : TRUE	
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bTwoMetal = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), _T("0"), PATH_WORKING_INFO);// IDC_CHK_TWO_METAL - Uncoiler\r정방향 ON : TRUE	
		}
	}
	else
	{
		if (bOn)
		{
			pDoc->WorkingInfo.LastJob.bOneMetal = TRUE;
			::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r정방향 CW : FALSE
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bOneMetal = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), _T("0"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r정방향 CW : FALSE
		}
	}
}

void CGvisR2R_LaserView::RestoreReelmap()
{
	return;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->RestoreReelmap();
}

void CGvisR2R_LaserView::AdjPinPos()
{
	double dOfsX, dOfsY;
	if (m_pDlgMenu02)
	{
		dOfsX = m_pDlgMenu02->m_dMkFdOffsetX[0][0];
		dOfsY = m_pDlgMenu02->m_dMkFdOffsetY[0][0];

		if (dOfsY > -2.0 && dOfsY < 2.0)
		{
			double dOffsetY = -1.0 * dOfsY;
			dOffsetY *= pDoc->m_dShiftAdjustRatio;

			CfPoint ptPnt[2];
			ptPnt[0].x = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[0]);
			ptPnt[0].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]) + dOffsetY;
			m_pDlgMenu02->SetPinPos(0, ptPnt[0]);
		}
	}
}

void CGvisR2R_LaserView::SetEngraveSts(int nStep)
{
}

void CGvisR2R_LaserView::SetEngraveStopSts()
{
}

void CGvisR2R_LaserView::SetEngraveFdSts()
{
}

BOOL CGvisR2R_LaserView::IsEngraveFdSts()
{
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsEngraveFd()
{
	if (m_nShareDnCnt > 0)
	{
		if (!(m_nShareDnCnt % 2))
		{
			return FALSE;
		}
	}
	else
	{
		if (m_nShareUpCnt > 0)
		{
			if (!(m_nShareUpCnt % 2))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

double CGvisR2R_LaserView::GetEngraveFdLen()
{
	int nLast = pDoc->GetLastShotEngrave();

	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

LRESULT CGvisR2R_LaserView::wmClientReceivedMdx(WPARAM wParam, LPARAM lParam)
{
	int nCmd = (int)wParam;
	CString* sReceived = (CString*)lParam;
	switch (nCmd)
	{
	case MdxIsReady:
		break;
	case GetCharacterString:
		break;
	default:
		break;
	}

	return (LRESULT)1;
}

LRESULT CGvisR2R_LaserView::wmClientReceivedSr(WPARAM wParam, LPARAM lParam)
{
	int nCmd = (int)wParam;
	CString* sReceived = (CString*)lParam;
	switch (nCmd)
	{
	case SrTriggerInputOn:
		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->Disp2dCode();
		}
		break;
	default:
		break;
	}

	return (LRESULT)1;
}

LRESULT CGvisR2R_LaserView::wmServerReceived(WPARAM wParam, LPARAM lParam)
{
	if(!m_pEngrave)
		return (LRESULT)0;

	int nAcceptId = (int)wParam;
	SOCKET_DATA sSockData;
	SOCKET_DATA *pSocketData = (SOCKET_DATA*)lParam;
	SOCKET_DATA rSockData = *pSocketData;
	int nCmdCode = rSockData.nCmdCode;
	int nMsgId = rSockData.nMsgID;
	switch (nCmdCode)
	{
	case _GetSig:
		break;
	case _GetData:
		break;
	case _SetSig:
		pView->m_bSetSig = TRUE;
		break;
	case _SetData:
		if (m_pEngrave && m_pEngrave->IsConnected())
			m_pEngrave->GetSysData(rSockData);

		pView->m_bSetData = TRUE;
		break;
	default:
		break;
	}


	return (LRESULT)1;
}

void CGvisR2R_LaserView::SetEngraveFdPitch(double dPitch)
{
	pDoc->SetEngraveFdPitch(dPitch);
}


BOOL CGvisR2R_LaserView::IsConnected()
{
#ifdef USE_ENGRAVE
	if (m_pEngrave)
	{
		if (m_pEngrave->IsConnected())
		{
			if (!m_bContEngraveF)
			{
				m_bContEngraveF = TRUE;
			}
			return TRUE;
		}
		else
		{
			if (m_bContEngraveF)
			{
				m_bContEngraveF = FALSE;
			}
			return FALSE;
		}
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsConnectedMdx()
{
	if (m_pMdx2500)
	{
		return m_pMdx2500->IsConnected();
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsConnectedSr()
{
	if (m_pSr1000w)
	{
		return m_pSr1000w->IsConnected();
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsConnectedMk()
{
	if (m_pEngrave)
	{
		return m_pEngrave->IsConnected();
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsDispContRun()
{
	if (m_pEngrave)
	{
		return m_pEngrave->IsDispContRun();
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsDispLotEnd()
{
	if (m_pEngrave)
	{
		return m_pEngrave->IsDispLotEnd();
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsPinPos0()
{
	if (!m_pMotion)
		return FALSE;

	double pPos[2];
	pPos[0] = m_pMotion->m_dPinPosX[0];
	pPos[1] = m_pMotion->m_dPinPosY[0];

	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	if (dCurrX < pPos[0] - 4.0 || dCurrX > pPos[0] + 4.0)
		return FALSE;
	if (dCurrY < pPos[1] - 4.0 || dCurrY > pPos[1] + 4.0)
		return FALSE;

	return TRUE;
}


void CGvisR2R_LaserView::InitAutoEng()
{
	m_bJobEnd = FALSE;

	m_bMkSt = FALSE;
	m_bMkStSw = FALSE;
	m_nMkStAuto = 0;

	m_bEngSt = FALSE;
	m_bEngStSw = FALSE;
	m_nEngStAuto = 0;

	m_bEng2dSt = FALSE;
	m_bEng2dStSw = FALSE;
	m_nEng2dStAuto = 0;
	m_nCntSkipError2dCode = 0;

	m_nGetItsCodeSerial = 0;
	pDoc->m_nShotNum = 0;
	//pDoc->m_bUploadPinImg = FALSE;
	pDoc->BtnStatus.EngAuto._Init();
	InitAutoEngSignal();

	pDoc->WorkingInfo.LastJob.dEngraveOrgX = _tstof(pDoc->WorkingInfo.LastJob.sEngraveOrgX);					// X_org
	pDoc->WorkingInfo.LastJob.dEngraveOrgY = _tstof(pDoc->WorkingInfo.LastJob.sEngraveOrgY);					// Y_org
	pDoc->WorkingInfo.LastJob.dEngravePosOffsetX = _tstof(pDoc->WorkingInfo.LastJob.sEngravePosOffsetX);		// X_offset
	pDoc->WorkingInfo.LastJob.dEngravePosOffsetY = _tstof(pDoc->WorkingInfo.LastJob.sEngravePosOffsetY);		// Y_offset
	pDoc->WorkingInfo.LastJob.dEngravePosTheta = _tstof(pDoc->WorkingInfo.LastJob.sEngravePosTheta);			// Theta_offset

	pDoc->m_AlignOffset.x = 0.0;
	pDoc->m_AlignOffset.y = 0.0;
	pDoc->m_TotalAlignOffset.x = 0.0;
	pDoc->m_TotalAlignOffset.y = 0.0;

	if (m_pDlgMenu01)
		m_pDlgMenu01->ResetLotTime();
}

// DoAuto
void CGvisR2R_LaserView::DoAutoEng()
{
	if (!IsAuto() || (MODE_INNER != pDoc->WorkingInfo.LastJob.nTestMode && MODE_LASER != pDoc->WorkingInfo.LastJob.nTestMode) || m_bJobEnd)
		return;

	CString str;
	str.Format(_T("%d : %d"), m_nStepTHREAD_DISP_DEF, m_bTHREAD_DISP_DEF ? 1 : 0);
	pView->DispStsBar(str, 6);

	// 각인부 마킹시작 신호를 확인
	DoAtuoGetEngStSignal();

	// 각인부 2D 코드 Reading신호를 확인
	DoAtuoGet2dReadStSignal();

	// Engrave Auto Sequence
	// 각인부 Marking Start
	DoAutoMarking();

	// 각인부 2D 코드 Reading Start
	DoAuto2dReading();
}

void CGvisR2R_LaserView::DoAtuoGetEngStSignal()
{
	if (!m_bEngSt)
	{
		if ((pDoc->BtnStatus.EngAuto.MkSt || m_bMkStSw) && !pDoc->BtnStatus.EngAuto.MkStF)  // AlignTest		// 마킹시작(PC가 확인하고 Reset시킴.)-20141029
		{
			m_bEngStSw = FALSE;
			pDoc->BtnStatus.EngAuto.MkStF = TRUE;

			m_bEngSt = TRUE;
			m_nEngStAuto = ENG_ST;

			pDoc->BtnStatus.EngAuto.IsMkSt = FALSE;
			pDoc->BtnStatus.EngAuto.OnMking = FALSE;
			pDoc->BtnStatus.EngAuto.IsOnMking = FALSE;
			pDoc->BtnStatus.EngAuto.MkDone = FALSE;
			pDoc->BtnStatus.EngAuto.IsMkDone = FALSE;
				
			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(0, TRUE);
		}
	}

	if (!pDoc->BtnStatus.EngAuto.IsMkSt && pDoc->BtnStatus.EngAuto.MkStF)
	{
		pDoc->BtnStatus.EngAuto.MkSt = FALSE;
		pDoc->BtnStatus.EngAuto.MkStF = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, FALSE);
		if (m_pDlgMenu02)
			m_pDlgMenu02->SetLed(0, FALSE);
	}
}

void CGvisR2R_LaserView::DoAtuoGet2dReadStSignal()
{
	if (!m_bEng2dSt)
	{
		if ((pDoc->BtnStatus.EngAuto.Read2dSt || m_bMkStSw) && !pDoc->BtnStatus.EngAuto.Read2dStF)  // 2D(GUI) Reading 동작 Start신호(PLC On->PC Off)
		{
			m_bEng2dStSw = FALSE;
			pDoc->BtnStatus.EngAuto.Read2dStF = TRUE;

			pDoc->BtnStatus.EngAuto.IsRead2dSt = FALSE;
			pDoc->BtnStatus.EngAuto.OnRead2d = FALSE;
			pDoc->BtnStatus.EngAuto.IsOnRead2d = FALSE;
			pDoc->BtnStatus.EngAuto.Read2dDone = FALSE;
			pDoc->BtnStatus.EngAuto.IsRead2dDone = FALSE;

			m_bEng2dSt = TRUE;
			m_nEng2dStAuto = ENG_2D_ST;

			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(3, TRUE);
		}
	}

	if (!pDoc->BtnStatus.EngAuto.IsRead2dSt && pDoc->BtnStatus.EngAuto.Read2dStF)
	{
		pDoc->BtnStatus.EngAuto.Read2dSt = FALSE;
		pDoc->BtnStatus.EngAuto.Read2dStF = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, FALSE);
		if (m_pDlgMenu02)
			m_pDlgMenu02->SetLed(3, FALSE);
	}
}

void CGvisR2R_LaserView::DoAuto2dReading()
{
	if (MODE_INNER == pDoc->WorkingInfo.LastJob.nTestMode || MODE_LASER == pDoc->WorkingInfo.LastJob.nTestMode)// || MODE_OUTER == pDoc->WorkingInfo.LastJob.nTestMode)
	{
		Eng2dRead();
	}
}

void CGvisR2R_LaserView::DoAutoMarking()
{
	if (MODE_INNER == pDoc->WorkingInfo.LastJob.nTestMode || MODE_LASER == pDoc->WorkingInfo.LastJob.nTestMode)
	{
		MarkingWith1PointAlign();
	}
}

void CGvisR2R_LaserView::MarkingWith1PointAlign()
{
	Eng1PtReady();
	Eng1PtInit();
	Eng1PtAlignPt0();
	Eng1PtDoMarking();
}

void CGvisR2R_LaserView::Eng1PtReady()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial = pDoc->GetLastShotEngrave() + 1;

	if (m_bEngSt)
	{
		switch (m_nEngStAuto)
		{
		case ENG_ST:	// PLC MK 신호 확인	
			if (IsRun())
			{
				m_nEngStAuto++;
			}
			break;
		case ENG_ST + 1:
			if (pDoc->m_bUploadPinImg)
			{
				pDoc->BtnStatus.EngAuto.IsOnMking = FALSE;
				pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, TRUE);
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(1, TRUE);
				m_nEngStAuto = ENG_ST + (Mk1PtIdx::Start);
			}
			else
			{
				m_nEngStAuto = ENG_ST;
				Buzzer(TRUE, 0);
				TowerLamp(RGB_YELLOW, TRUE);
				pView->MsgBox(_T("카메라 정렬이미지가 저장되지 않았습니다.\r\n핀위치를 저장하세요."), 0, MB_OK);
				EngStop(TRUE);
			}
			break;
		case ENG_ST + (Mk1PtIdx::Start) :	// 2
			if(pDoc->BtnStatus.EngAuto.IsOnMking) // 펀칭부에서 각인중 신호를 받았는 확인 신호
				m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::Start) + 1:
			pDoc->BtnStatus.EngAuto.IsOnMking = FALSE;
			m_nEngStAuto = ENG_ST + (Mk1PtIdx::InitMk);			// InitMk()
			break;
		}
	}
}

void CGvisR2R_LaserView::Eng1PtInit()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial = pDoc->GetLastShotEngrave() + 1;

	if (m_bEngSt)
	{
		switch (m_nEngStAuto)
		{
		case ENG_ST + (Mk1PtIdx::InitMk) :
			m_nEngStAuto++;
			if (nSerial > 2)
			{
				AdjPinPosEng();
				// 각인부 작업완료.(PC가 On, PLC가 확인 후 Off) - ?
			}
			break;

		case ENG_ST + (Mk1PtIdx::InitMk) + 1:
			m_nEngStAuto = ENG_ST + (Mk1PtIdx::Move0Cam0);	// Move - Cam1 - Pt0
			break;
		}
	}
}

void CGvisR2R_LaserView::Eng1PtAlignPt0()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial = pDoc->GetLastShotEngrave() + 1;

	if (m_bEngSt)
	{
		switch (m_nEngStAuto)
		{
		case ENG_ST + (Mk1PtIdx::Move0Cam0) :	// Move - Cam1 - Pt0
			MovePinPos();
			m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::Move0Cam0) + 1:
			m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::Move0Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nEngStAuto++;
			}
			break;
		case ENG_ST + (Mk1PtIdx::Move0Cam0) + 3:
			m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::Align0_0) :		// 1PtAlign - Cam0 - Pt0
			m_nEngStAuto++;
			if (!m_bSkipAlign[0][0])
			{
				if (OnePointAlign0(0))
					m_bFailAlign[0][0] = FALSE;
				else
					m_bFailAlign[0][0] = TRUE;
			}
			break;
		case ENG_ST + (Mk1PtIdx::Align0_0) + 1:
			if (!IsRun())
				break;

			if (m_bFailAlign[0][0])
			{
				SetErrorRead2dCode(_PcId::_Engrave);
				Buzzer(TRUE, 0);
				TowerLamp(RGB_YELLOW, TRUE);
				if (IDNO == MsgBox(_T("카메라의 원점 정렬을 다시하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);
					if (IDYES == MsgBox(_T("카메라의 원점 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						m_bReAlign[0][0] = FALSE;
						m_bSkipAlign[0][0] = TRUE;
						m_bSkipAlign[0][1] = TRUE;
						m_bSkipAlign[0][2] = TRUE;
						m_bSkipAlign[0][3] = TRUE;
						if (IDNO == MsgBox(_T("판넬에 레이저 2D 마킹을 하시겠습니까?"), 0, MB_YESNO))
						{
							m_bDoMk[0] = FALSE;
							m_bDoneMk[0] = TRUE;
							EngStop(TRUE);
						}
						else
						{
							m_bDoMk[0] = TRUE;
							m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						// 원점 정렬을 다시 함.
						m_bReAlign[0][0] = TRUE;
						m_bSkipAlign[0][0] = FALSE;
						m_bSkipAlign[0][1] = FALSE;
						m_bSkipAlign[0][2] = FALSE;
						m_bSkipAlign[0][3] = FALSE;
						m_nEngStAuto = ENG_ST + (Mk1PtIdx::Move0Cam0); // OnePointAlign0(0) 으로 진행. - 카메라 재정렬
						EngStop(TRUE);
					}
				}
				else
				{ 
					// 원점 정렬을 다시 함.
					Buzzer(FALSE, 0);

					m_bReAlign[0][0] = TRUE;
					m_bSkipAlign[0][0] = FALSE;
					m_bSkipAlign[0][1] = FALSE;
					m_nEngStAuto = ENG_ST + (Mk1PtIdx::Move0Cam0); // OnePointAlign0(0) 으로 진행. - 카메라 재정렬
					EngStop(TRUE);
				}
			}

			if (m_bFailAlign[0][0])
			{
				if (!m_bReAlign[0][0])
				{
					if (m_bDoMk[0])
						m_nEngStAuto++; // DoMk
					else
					{
						//MovePinPos(); // 2D 코드 위치
						m_nEngStAuto = ENG_ST + (Mk1PtIdx::DoneMk); // Align변수 초기화 (Skip 65 : Mk())
					}
				}
				else
				{
					m_nEngStAuto = ENG_ST + (Mk1PtIdx::Move0Cam0); // OnePointAlign0(0) 으로 진행. - 카메라 재정렬
				}
			}
			else
			{
				//AdjLaserOffset(pDoc->m_AlignOffset);
				m_nEngStAuto++; // DoMk
			}

			break;
		case ENG_ST + (Mk1PtIdx::Align0_0) + 2:
			if (IsRun())
			{
				//MovePinPos(); // 2D 코드 위치
				//pDoc->m_nShotNum++;							// 각인할 시리얼 증가
				m_nEngStAuto = ENG_ST + (Mk1PtIdx::DoMk);
			}
			break;
		}
	}
}

void CGvisR2R_LaserView::Eng1PtDoMarking()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nLastSerialNext = pDoc->GetLastShotEngrave() + 1;
	int nEngraveSerial = m_nGetItsCodeSerial + 1;

	if (m_bEngSt)
	{
		switch (m_nEngStAuto)
		{
		case ENG_ST + (Mk1PtIdx::DoMk) :				// Mk 마킹 시작
			if (!IsRun())
				break;

			if (!pDoc->WorkingInfo.System.bNoMk)
			{
				if (!SetMdxLotAndShotNum(pDoc->m_sItsCode, nLastSerialNext))
				{
					Buzzer(TRUE, 0);
					TowerLamp(RGB_RED, TRUE);
					EngStop(TRUE);
					break;
				}
			}
			Sleep(100);
			m_nEngStAuto++;
			break;

		case ENG_ST + (Mk1PtIdx::DoMk) + 1:
			if (!IsRun())
				break;

			if (!pDoc->WorkingInfo.System.bNoMk)
			{
				if(m_nGetItsCodeSerial == 0 || nEngraveSerial == nLastSerialNext)
					SetMk(TRUE);	// Mk 마킹 시작
				else if (nEngraveSerial > nLastSerialNext)
				{
					pView->SetErrorRead2dCode(_PcId::_Engrave);
					Buzzer(TRUE, 0);
					TowerLamp(RGB_RED, TRUE);
					EngStop(TRUE);
					MsgBox(_T("정지 - 2D바코드의 각인된 시리얼이 각인할 시리얼보다 큽니다."));
					break;
				}
			}
			SetCurrentInfoEngShotNum(nEngraveSerial);
			m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::DoMk) + 2:
			if (IsMkDone())
			{
				Sleep(300);
				m_nEngStAuto = ENG_ST + (Mk1PtIdx::DoneMk);	// Mk 마킹 완료
			}
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) :
			pDoc->BtnStatus.EngAuto.IsOnMking = FALSE;
			pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, FALSE);
			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(1, FALSE);
			m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) + 1:
			if (pDoc->BtnStatus.EngAuto.IsOnMking) // 펀칭부 신호 확인 후
			{
				pDoc->BtnStatus.EngAuto.IsMkDone = FALSE;
				pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, TRUE);
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(2, TRUE);
				m_nEngStAuto++;
			}
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) + 2:
			if (pDoc->BtnStatus.EngAuto.IsMkDone)
			{
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(2, FALSE);
				Sleep(300);
				m_nEngStAuto++;
			}
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) + 3:
			if(pDoc->BtnStatus.EngAuto.FdDone)
			{
				if (m_nGetItsCodeSerial == 0 || nEngraveSerial == nLastSerialNext)
					SetLastSerialEng(nLastSerialNext); // (_ttoi(pDoc->m_sShotNum));
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(6, TRUE);
				m_nEngStAuto++;
			}
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) + 4:
			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(6, FALSE);
			m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) + 5:
			pDoc->BtnStatus.EngAuto.IsMkSt = FALSE;
			pDoc->BtnStatus.EngAuto.OnMking = FALSE;
			pDoc->BtnStatus.EngAuto.IsOnMking = FALSE;
			pDoc->BtnStatus.EngAuto.MkDone = FALSE;
			pDoc->BtnStatus.EngAuto.IsMkDone = FALSE;

			pDoc->BtnStatus.EngAuto.FdDone = FALSE;
			pDoc->BtnStatus.EngAuto.IsFdDone = FALSE;

			m_nEngStAuto = 0;
			m_bEngSt = FALSE;
			break;
		}
	}
}

BOOL CGvisR2R_LaserView::IsMkDone()
{
	if (!pView || !pView->m_pMdx2500)
		return FALSE;

	return (!pView->m_pMdx2500->IsRunning());
}

BOOL CGvisR2R_LaserView::SetMk(BOOL bRun)	// Marking Start
{
	if (!pView || !pView->m_pMdx2500)
		return FALSE;

	return (pView->m_pMdx2500->LaserMarking());
}

BOOL CGvisR2R_LaserView::SetMdxLotAndShotNum(CString sLot, int nSerial)
{
	if (!pView || !pView->m_pMdx2500)
		return FALSE;

	CString sSerial;
	sSerial.Format(_T("%03d"), nSerial);
	return m_pMdx2500->SetMdxOrderShotNum(sLot, sSerial);
}

void CGvisR2R_LaserView::AdjPinPosEng()
{
	if (m_pDlgMenu02)
	{
		if (m_pDlgMenu02->m_dMkFdOffsetY[0][0] > -2.0 &&
			m_pDlgMenu02->m_dMkFdOffsetY[0][0] < 2.0)
		{
			double dOffsetY = -1.0*(m_pDlgMenu02->m_dMkFdOffsetY[0][0]);
			dOffsetY *= pDoc->m_dShiftAdjustRatio;

			CfPoint ptPnt[2];
			ptPnt[0].x = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[0]);
			ptPnt[0].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]) + dOffsetY;

			m_pDlgMenu02->SetPinPos(0, ptPnt[0]);
		}
	}
}

BOOL CGvisR2R_LaserView::OnePointAlign0(int nPos)
{
	if (!m_pDlgMenu02)
		return FALSE;
	BOOL bRtn;
	CfPoint ptPnt(0.0, 0.0);
	CfPoint _ptPnt(0.0, 0.0);
	bRtn = m_pDlgMenu02->OnePointAlign(ptPnt); // 비전으로 확인한 원점위치 (Motion의 절대좌표계).
	if (bRtn)
	{
		SetEngOffset(ptPnt);
	}
	else
	{
		SetEngOffset(_ptPnt);
	}

	return bRtn;
}


// DoAutoReading

void CGvisR2R_LaserView::Eng2dRead()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial;
	CString sMsg;

	if (m_bEng2dSt)
	{
		switch (m_nEng2dStAuto)
		{
		case ENG_2D_ST:	// PLC MK 신호 확인	
			if (IsRun())
			{
				MoveInitPos0();
				m_nEng2dStAuto++;
			}
			break;
		case ENG_2D_ST + 1:
			pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, TRUE);
			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(4, TRUE);
			m_nEng2dStAuto = ENG_2D_ST + (Read2dIdx::Start);
			break;
		case ENG_2D_ST + (Read2dIdx::Start) :	// 2
			m_nEng2dStAuto++;
			break;
		case ENG_2D_ST + (Read2dIdx::Start) + 1:
			m_nEng2dStAuto = ENG_2D_ST + (Read2dIdx::DoRead);
			break;
		case ENG_2D_ST + (Read2dIdx::DoRead) :			// 2D Reading 시작
			if (!pDoc->WorkingInfo.System.bNoMk)
				Set2dRead(TRUE);							// 2D Reading 시작
			m_dwRead2dSt = GetTickCount();
			m_nEng2dStAuto++;
			break;
		case ENG_2D_ST + (Read2dIdx::DoRead) + 1:
			Sleep(300);
			m_nEng2dStAuto++;
			break;
		case ENG_2D_ST + (Read2dIdx::DoRead) + 2:
			if (IsRun())
			{
				if (Is2dReadDone())
				{
					if (!pDoc->WorkingInfo.System.bNoMk)
					{
						Get2dCode(m_sGetItsCode, m_nGetItsCodeSerial);
						if (m_sGetItsCode != pDoc->m_sItsCode)
						{
							pView->SetErrorRead2dCode(_PcId::_Engrave);
							sMsg.Format(_T("설정된 ITS CODE : %s \r\n각인된 ITS CODE : \n\r일치하지 않습니다."), pDoc->m_sItsCode, m_sGetItsCode);
							pView->MsgBox(sMsg);
						}
					}
					else
					{
						m_nGetItsCodeSerial++;
					}
					if (pDoc->m_bUseSkipError2dCode)
						m_nCntSkipError2dCode = 0;
					SetCurrentInfoReadShotNum(m_nGetItsCodeSerial);
					Sleep(300);
					m_nEng2dStAuto = ENG_2D_ST + (Read2dIdx::DoneRead);	// 2D Reading 완료
				}
				else
				{
					m_dwRead2dEd = GetTickCount();
					if ((m_dwRead2dEd - m_dwRead2dSt) > 30000)
					{
						if (pDoc->m_bUseSkipError2dCode && m_nCntSkipError2dCode < pDoc->m_nSkipError2dCode)
						{
							//m_sGetItsCode
							m_nGetItsCodeSerial++;
							pDoc->m_sShotNum.Format(_T("%03d"), m_nGetItsCodeSerial);
							pDoc->m_nShotNum = m_nGetItsCodeSerial;
							m_nCntSkipError2dCode++;
							m_nEng2dStAuto = ENG_2D_ST + (Read2dIdx::DoneRead);	// 2D Reading 완료
						}
						else
						{
							// nParam = _PcId::_Engrave
							pView->SetErrorRead2dCode(_PcId::_Engrave);
							Buzzer(TRUE, 0);
							TowerLamp(RGB_RED, TRUE);
							EngStop(TRUE);
							if (IDYES == MsgBox(_T("정지 - 2D바코드의 각인된 코드를 읽을 수 없습니다.\r\n운전을 누르시고, 다음 Shot으로 진행합니까?\r\n \"아니요\"를 누르시고 운전을 누르시면 2D코드를 다시 읽습니다."), 0, MB_YESNO))
							{
								m_nGetItsCodeSerial = _ttoi(ShowKeypad1());
								SetCurrentInfoReadShotNum(m_nGetItsCodeSerial);
								m_nEng2dStAuto = ENG_2D_ST + (Read2dIdx::DoneRead);	// 2D Reading 완료
							}
							else
							{
								// 운전을 누르면 다시 2D 코드를 읽기를 시작합니다.
								m_nEng2dStAuto = ENG_2D_ST + (Read2dIdx::DoRead); // 2D Reading 시작
							}
						}
					}
				}
			}
			break;
		case ENG_2D_ST + (Read2dIdx::DoneRead) :
			if (IsRun())
			{
				m_nEng2dStAuto++;
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(4, FALSE);
				MovePinPos();
			}
			break;
		case ENG_2D_ST + (Read2dIdx::DoneRead) + 1:
			pDoc->BtnStatus.EngAuto.IsRead2dDone = FALSE;
			pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, TRUE);
			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(5, TRUE);
			m_nEng2dStAuto++;
			break;
		case ENG_2D_ST + (Read2dIdx::DoneRead) + 2:
			if (pDoc->BtnStatus.EngAuto.IsRead2dDone)
			{
				m_nEng2dStAuto++;
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(5, FALSE);
				Sleep(300);
			}
			break;
		case ENG_2D_ST + (Read2dIdx::DoneRead) + 3:
			pDoc->BtnStatus.EngAuto.IsRead2dSt = FALSE;
			pDoc->BtnStatus.EngAuto.OnRead2d = FALSE;
			pDoc->BtnStatus.EngAuto.IsOnRead2d = FALSE;
			pDoc->BtnStatus.EngAuto.Read2dDone = FALSE;
			pDoc->BtnStatus.EngAuto.IsRead2dDone = FALSE;

			m_nEng2dStAuto = 0;
			m_bEng2dSt = FALSE;
			break;
		}
	}
}

CString CGvisR2R_LaserView::ShowKeypad1()
{
	BOOL bAdj = TRUE;
	CString strData, strPrev;

	CRect rect(0, 0, 0, 0);
	CDlgKeyNum1 *pDlg = new CDlgKeyNum1(&strData, _T(""), this);
	pDlg->DoModal();
	delete pDlg;

	return strData;
}


BOOL  CGvisR2R_LaserView::Is2dReadDone()
{
	if (!pView || !pView->m_pSr1000w)
		return FALSE;

	return (!pView->m_pSr1000w->IsRunning());
}

BOOL CGvisR2R_LaserView::Set2dRead(BOOL bRun)	// Marking Start
{
	if (!pView || !pView->m_pSr1000w)
		return FALSE;

	return (pView->m_pSr1000w->DoRead2DCode());
}

BOOL CGvisR2R_LaserView::SetEngOffset(CfPoint &OfSt)
{
	// Write Feeding Offset data....
	if(m_pDlgFrameHigh)
		m_pDlgFrameHigh->SetEngOffset(OfSt);
	return pDoc->SetEngOffset(OfSt);
}

void CGvisR2R_LaserView::SetMyMsgYes()
{
	if (m_pDlgMyMsg)
	{
		if (m_pDlgMyMsg->m_pDlgMyMsgSub01)
		{
			((CDlgMyMsgSub01*)(m_pDlgMyMsg->m_pDlgMyMsgSub01))->ClickYes();
		}
	}
}

void CGvisR2R_LaserView::SetMyMsgNo()
{
	//if (pDoc->GetCurrentInfoSignal(_SigInx::_MyMsgNo))
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_MyMsgNo, FALSE);

		if (m_pDlgMyMsg)
		{
			if (m_pDlgMyMsg->m_pDlgMyMsgSub01)
			{
				((CDlgMyMsgSub01*)(m_pDlgMyMsg->m_pDlgMyMsgSub01))->ClickNo();
			}
		}
	}
}

void CGvisR2R_LaserView::SetMyMsgOk()
{
	pDoc->SetCurrentInfoSignal(_SigInx::_MyMsgOk, FALSE);

	if (m_pDlgMyMsg)
	{
		if (m_pDlgMyMsg->m_pDlgMyMsgSub02)
		{
			((CDlgMyMsgSub02*)(m_pDlgMyMsg->m_pDlgMyMsgSub02))->ClickOk();
		}
	}
}

void CGvisR2R_LaserView::CheckCurrentInfoSignal(int nMsgID, int nData)
{
	if (m_pEngrave)
		m_pEngrave->SetCurrentInfoSignal(nMsgID, nData);
}

void CGvisR2R_LaserView::CheckMonDispMainSignal()
{
	if (m_pEngrave)
		m_pEngrave->SetMonDispMainSignal();
}

void CGvisR2R_LaserView::InitAutoEngSignal()
{
	if (m_pDlgMenu02)
	{
		m_pDlgMenu02->SetLed(0, FALSE); // _SigInx::_EngAutoSeqMkSt
		m_pDlgMenu02->SetLed(1, FALSE); // _SigInx::_EngAutoSeqOnMkIng
		m_pDlgMenu02->SetLed(2, FALSE); // _SigInx::_EngAutoSeqMkDone
		m_pDlgMenu02->SetLed(3, FALSE); // _SigInx::_EngAutoSeq2dReadSt
		m_pDlgMenu02->SetLed(4, FALSE); // _SigInx::_EngAutoSeqOnReading2d
		m_pDlgMenu02->SetLed(5, FALSE); // _SigInx::_EngAutoSeq2dReadDone
		m_pDlgMenu02->SetLed(6, FALSE); // _SigInx::_EngAutoSeqFdDone
	}
}

BOOL CGvisR2R_LaserView::GetCurrentInfoSignal()
{
	if (!m_pEngrave) return FALSE;

	if (m_pEngrave->m_bRcvSig[_SigInx::_EngAutoInit])
	{
		pDoc->BtnStatus.EngAuto.IsInit = TRUE;
	}
	if (m_pEngrave->m_bRcvSig[_SigInx::_EngAutoSeqMkSt])
	{
		pDoc->BtnStatus.EngAuto.IsMkSt = TRUE;
	}
	if (m_pEngrave->m_bRcvSig[_SigInx::_IsEngAutoSeqOnMkIng])
	{
		pDoc->BtnStatus.EngAuto.IsOnMking = TRUE;
	}
	if (m_pEngrave->m_bRcvSig[_SigInx::_IsEngAutoSeqMkDone])
	{
		pDoc->BtnStatus.EngAuto.IsMkDone = TRUE;
	}
	if (m_pEngrave->m_bRcvSig[_SigInx::_EngAutoSeq2dReadSt])
	{
		pDoc->BtnStatus.EngAuto.IsRead2dSt = TRUE;
	}
	if (m_pEngrave->m_bRcvSig[_SigInx::_IsEngAutoSeqOnReading2d])
	{
		pDoc->BtnStatus.EngAuto.IsOnRead2d = TRUE;
	}
	if (m_pEngrave->m_bRcvSig[_SigInx::_IsEngAutoSeq2dReadDone])
	{
		pDoc->BtnStatus.EngAuto.IsRead2dDone = TRUE;
	}
	if (m_pEngrave->m_bRcvSig[_SigInx::_EngAutoSeqFdDone])
	{
		pDoc->BtnStatus.EngAuto.IsFdDone = TRUE;
	}

	if (m_pDlgMenu02)
	{
		CString sVal;
		sVal.Format(_T("%d"), pDoc->BtnStatus.EngAuto.IsMkSt ? 1 : 0);
		m_pDlgMenu02->GetDlgItem(IDC_STATIC_ENG_START)->SetWindowText(sVal);
		sVal.Format(_T("%d"), pDoc->BtnStatus.EngAuto.IsOnMking ? 1 : 0);
		m_pDlgMenu02->GetDlgItem(IDC_STATIC_ENG_ON)->SetWindowText(sVal);
		sVal.Format(_T("%d"), pDoc->BtnStatus.EngAuto.IsMkDone ? 1 : 0);
		m_pDlgMenu02->GetDlgItem(IDC_STATIC_ENG_DONE)->SetWindowText(sVal);
		sVal.Format(_T("%d"), pDoc->BtnStatus.EngAuto.IsRead2dSt ? 1 : 0);
		m_pDlgMenu02->GetDlgItem(IDC_STATIC_RD_START)->SetWindowText(sVal);
		sVal.Format(_T("%d"), pDoc->BtnStatus.EngAuto.IsOnRead2d ? 1 : 0);
		m_pDlgMenu02->GetDlgItem(IDC_STATIC_RD_ON)->SetWindowText(sVal);
		sVal.Format(_T("%d"), pDoc->BtnStatus.EngAuto.IsRead2dDone ? 1 : 0);
		m_pDlgMenu02->GetDlgItem(IDC_STATIC_RD_DONE)->SetWindowText(sVal);
		sVal.Format(_T("%d"), pDoc->BtnStatus.EngAuto.IsFdDone ? 1 : 0);
		m_pDlgMenu02->GetDlgItem(IDC_STATIC_FD_DONE)->SetWindowText(sVal);
	}

	// 상대방의 로컬파일에서 확인
	if (pDoc->GetCurrentInfoSignal(_SigInx::_MyMsgYes) && !m_pEngrave->m_bRcvSig[_SigInx::_MyMsgYes])
	{
		m_pEngrave->m_bRcvSig[_SigInx::_MyMsgYes] = TRUE;
		pDoc->ResetCurrentInfoSignal(_SigInx::_MyMsgYes);
	}
	if (pDoc->GetCurrentInfoSignal(_SigInx::_MyMsgNo) && !m_pEngrave->m_bRcvSig[_SigInx::_MyMsgNo])
	{
		m_pEngrave->m_bRcvSig[_SigInx::_MyMsgNo] = TRUE;
		pDoc->ResetCurrentInfoSignal(_SigInx::_MyMsgNo);
	}
	if (pDoc->GetCurrentInfoSignal(_SigInx::_MyMsgOk) && !m_pEngrave->m_bRcvSig[_SigInx::_MyMsgOk])
	{
		m_pEngrave->m_bRcvSig[_SigInx::_MyMsgOk] = TRUE;
		pDoc->ResetCurrentInfoSignal(_SigInx::_MyMsgOk);
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::GetMonDispMainSignal()
{
	if (!m_pEngrave) return FALSE;

	if (m_pEngrave->m_bRcvSig[_SigInx::_GetMonDispMainSignal])
	{
		m_pEngrave->m_bRcvSig[_SigInx::_GetMonDispMainSignal] = FALSE;
		MonDispMain();
	}

	return TRUE;
}

void CGvisR2R_LaserView::SetLastSerialEng(int nSerial)
{
	if (m_pDlgFrameHigh)
		m_pDlgFrameHigh->SetEngraveLastShot(nSerial);
}

int CGvisR2R_LaserView::GetLastSerialEng()
{
	int nSerial = 0;
	if (pDoc)
		nSerial = pDoc->GetLastSerialEng();

	return nSerial;
}

CString CGvisR2R_LaserView::GetCurrentInfoBufUp()
{
	return pDoc->GetCurrentInfoBufUp();
}

CString CGvisR2R_LaserView::GetCurrentInfoBufDn()
{
	return pDoc->GetCurrentInfoBufDn();
}

void CGvisR2R_LaserView::ChkBufUp()
{
	CString str = GetCurrentInfoBufUp();

	if (pFrm)
	{
		if (m_sBuf[0] != str)
		{
			m_sBuf[0] = str;
			pFrm->DispStatusBar(str, 3);
		}
	}
}

void CGvisR2R_LaserView::ChkBufDn()
{
	CString str = GetCurrentInfoBufDn();

	if (pFrm)
	{
		if (m_sBuf[1] != str)
		{
			m_sBuf[1] = str;
			pFrm->DispStatusBar(str, 1);
		}
	}
}

void CGvisR2R_LaserView::SetCurrentInfoEngShotNum(int nSerial)
{
	pDoc->SetCurrentInfoEngShotNum(nSerial);
}

void CGvisR2R_LaserView::SetCurrentInfoReadShotNum(int nSerial)
{
	pDoc->SetCurrentInfoReadShotNum(nSerial);
}


BOOL CGvisR2R_LaserView::Get2dCode(CString &sItsCode, int &nSerial)
{
	if (!m_pSr1000w)
		return FALSE;

	CString sData, sMsg = _T("");
	DispStsBar(sMsg, 6);

	if (m_pSr1000w->Get2DCode(sData))
	{
		int nPos = sData.ReverseFind('-');
		if (nPos != -1)
		{
			pDoc->m_sOrderNum = sData.Left(nPos);
			pDoc->m_sShotNum = sData.Right(sData.GetLength() - nPos - 1);
			pDoc->m_nShotNum = _tstoi(pDoc->m_sShotNum);
			sItsCode = pDoc->m_sOrderNum;
			nSerial = pDoc->m_nShotNum;
			DispStsBar(sData, 6);
		}
		else
		{
			pView->MsgBox(sData);
		}

		return TRUE;
	}

	return FALSE;
}

void CGvisR2R_LaserView::SetTotOpRto(CString sVal)		// 전체진행율
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetTotOpRto(sVal);
}

void CGvisR2R_LaserView::SetTotVel(CString sVal)		// 전체속도
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetTotVel(sVal);
}

void CGvisR2R_LaserView::SetPartVel(CString sVal)		// 구간속도
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetPartVel(sVal);
}

void CGvisR2R_LaserView::SetMkDoneLen(CString sVal)		// 마킹부 : Distance (FdDone) [M]
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetMkDoneLen(sVal);
}

void CGvisR2R_LaserView::SetAoiDnDoneLen(CString sVal)	// 검사부(하) : Distance (FdDone) [M]
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetAoiDnDoneLen(sVal);
}

void CGvisR2R_LaserView::SetAoiUpDoneLen(CString sVal)	// 검사부(상) : Distance (FdDone) [M]
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetAoiUpDoneLen(sVal);
}

void CGvisR2R_LaserView::SetEngDoneLen(CString sVal)	// 검사부(상) : Distance (FdDone) [M]
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetEngDoneLen(sVal);
}

void CGvisR2R_LaserView::DispStatusBar(CString strMsg, int nStatusBarID)
{
	if (pFrm)
		pFrm->DispStatusBar(strMsg, nStatusBarID);
}

void CGvisR2R_LaserView::GetMkMenu01()
{
	m_bTIM_MENU01_UPDATE_WORK = TRUE;
	SetTimer(TIM_MENU01_UPDATE_WORK, 500, NULL);
}

CString CGvisR2R_LaserView::GetMkMenu01(CString sMenu, CString sItem)
{
	CString sPath = pDoc->WorkingInfo.System.sPathMkMenu01;
	TCHAR szData[512];

	if (sPath.IsEmpty())
		return _T("");

	if (0 < ::GetPrivateProfileString(sMenu, sItem, NULL, szData, sizeof(szData), sPath))
		return CString(szData);

	return _T("");
}


CString CGvisR2R_LaserView::GetMkDispInfoUp(CString sMenu, CString sItem, int nSerial)
{
	TCHAR szData[512];
	CString sPath;
	sPath.Format(_T("%s%s\\%s\\%s\\DefImage\\%d\\Disp.txt"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		nSerial);

	if (sPath.IsEmpty())
		return _T("");

	if (0 < ::GetPrivateProfileString(sMenu, sItem, NULL, szData, sizeof(szData), sPath))
		return CString(szData);

	return _T("");
}

CString CGvisR2R_LaserView::GetMkDispInfoDn(CString sMenu, CString sItem, int nSerial)
{
	TCHAR szData[512];
	CString sPath;
	sPath.Format(_T("%s%s\\%s\\%s\\DefImage\\%d\\Disp.txt"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		nSerial);

	if (sPath.IsEmpty())
		return _T("");

	if (0 < ::GetPrivateProfileString(sMenu, sItem, NULL, szData, sizeof(szData), sPath))
		return CString(szData);

	return _T("");
}

CString CGvisR2R_LaserView::GetTimeIts()
{
	stLotTime ItsTime;

	CString strVal;
	time_t osBinTime;		// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 

	// operating system.
	CTime Tim(osBinTime);

	ItsTime.nYear = Tim.GetYear();
	ItsTime.nMonth = Tim.GetMonth();
	ItsTime.nDay = Tim.GetDay();
	ItsTime.nHour = Tim.GetHour();
	ItsTime.nMin = Tim.GetMinute();
	ItsTime.nSec = Tim.GetSecond();

	strVal.Format(_T("%04d%02d%02d%02d%02d%02d"),
		ItsTime.nYear, ItsTime.nMonth, ItsTime.nDay,
		ItsTime.nHour, ItsTime.nMin, ItsTime.nSec);

	return strVal;
}

BOOL CGvisR2R_LaserView::ReloadRstInner()
{
	double dRatio = 0.0;
	CString sVal = _T("");
	CDlgProgress dlg;
	sVal.Format(_T("On Reloading InnerReelmap."));
	dlg.Create(sVal);
	//dlg.SetRange(0, 500);

	//GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;


	BOOL bRtn[7];
	if (pDoc->m_pReelMapInner)
		bRtn[0] = pDoc->m_pReelMapInner->ReloadRst();

	if (pDoc->m_pReelMapInnerUp)
		bRtn[1] = pDoc->m_pReelMapInnerUp->ReloadRst();

	if (pDoc->m_pReelMapIts)
		bRtn[2] = pDoc->m_pReelMapIts->ReloadRst();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerDn)
			bRtn[4] = pDoc->m_pReelMapInnerDn->ReloadRst();
		if (pDoc->m_pReelMapInnerAllUp)
			bRtn[5] = pDoc->m_pReelMapInnerAllUp->ReloadRst();
		if (pDoc->m_pReelMapInnerAllDn)
			bRtn[6] = pDoc->m_pReelMapInnerAllDn->ReloadRst();
	}

	int nRatio[7] = { 0 };
	BOOL bDone[7] = { 0 };
	int nTo = 0;
	if (bDualTest)
		nTo = 600; //[%]
	else
		nTo = 300; //[%]

	dlg.SetRange(0, nTo);

	for (int nProc = 0; nProc < nTo;)
	{
		if (pDoc->m_pReelMapInner)
		{
			nRatio[0] = pDoc->m_pReelMapInner->GetProgressReloadRst();
			bDone[0] = pDoc->m_pReelMapInner->IsDoneReloadRst();
		}
		else
			bDone[0] = TRUE;
		if (!bRtn[0])
			bDone[0] = TRUE;

		if (pDoc->m_pReelMapInnerUp)
		{
			nRatio[1] = pDoc->m_pReelMapInnerUp->GetProgressReloadRst();
			bDone[1] = pDoc->m_pReelMapInnerUp->IsDoneReloadRst();
		}
		else
			bDone[1] = TRUE;
		if (!bRtn[1])
			bDone[1] = TRUE;
		bDone[3] = TRUE;

		if (pDoc->m_pReelMapIts)
		{
			nRatio[2] = pDoc->m_pReelMapIts->GetProgressReloadRst();
			bDone[2] = pDoc->m_pReelMapIts->IsDoneReloadRst();
		}
		else
			bDone[2] = TRUE;
		if (!bRtn[2])
			bDone[2] = TRUE;

		if (bDualTest)
		{
			if (pDoc->m_pReelMapInnerDn)
			{
				nRatio[4] = pDoc->m_pReelMapInnerDn->GetProgressReloadRst();
				bDone[4] = pDoc->m_pReelMapInnerDn->IsDoneReloadRst();
			}
			else
				bDone[4] = TRUE;
			if (!bRtn[4])
				bDone[4] = TRUE;

			if (pDoc->m_pReelMapInnerAllUp)
			{
				nRatio[5] = pDoc->m_pReelMapInnerAllUp->GetProgressReloadRst();
				bDone[5] = pDoc->m_pReelMapInnerAllUp->IsDoneReloadRst();
			}
			else
				bDone[5] = TRUE;
			if (!bRtn[5])
				bDone[5] = TRUE;

			if (pDoc->m_pReelMapAllDn)
			{
				nRatio[6] = pDoc->m_pReelMapInnerAllDn->GetProgressReloadRst();
				bDone[6] = pDoc->m_pReelMapInnerAllDn->IsDoneReloadRst();
			}
			else
				bDone[6] = TRUE;
			if (!bRtn[6])
				bDone[6] = TRUE;

		}
		else
		{
			bDone[4] = TRUE;
			bDone[5] = TRUE;
			bDone[6] = TRUE;
		}

		nProc = nRatio[0] + nRatio[1] + nRatio[2] + nRatio[3] + nRatio[4] + nRatio[5] + nRatio[6];

		if (bDone[0] && bDone[1] && bDone[2] && bDone[3] && bDone[4] && bDone[5] && bDone[6])
			break;
		else
		{
			dlg.SetPos(nProc);
			Sleep(30);
		}
	}

	dlg.DestroyWindow();

	if (bDualTest)
	{
		for (int i = 0; i < 7; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::ReloadRstInner(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	BOOL bRtn[7] = { 0 };
	if (pDoc->m_pReelMapInner)
		bRtn[0] = pDoc->m_pReelMapInner->ReloadRst(nSerial);
	if (pDoc->m_pReelMapInnerUp)
		bRtn[1] = pDoc->m_pReelMapInnerUp->ReloadRst(nSerial);

	bRtn[3] = TRUE;

	if (pDoc->m_pReelMapIts)
		bRtn[2] = pDoc->m_pReelMapIts->ReloadRst(nSerial);

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerDn)
			bRtn[4] = pDoc->m_pReelMapInnerDn->ReloadRst(nSerial);
		if (pDoc->m_pReelMapInnerAllUp)
			bRtn[5] = pDoc->m_pReelMapInnerAllUp->ReloadRst(nSerial);
		if (pDoc->m_pReelMapInnerAllDn)
			bRtn[6] = pDoc->m_pReelMapInnerAllDn->ReloadRst(nSerial);

		for (int i = 0; i < 7; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}

	return TRUE;
}

void CGvisR2R_LaserView::OpenReelmapInner()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (pDoc->m_pReelMapInnerUp)
		pDoc->m_pReelMapInnerUp->Open();

	if (pDoc->m_pReelMapIts)
		pDoc->m_pReelMapIts->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerDn)
			pDoc->m_pReelMapInnerDn->Open();
		if (pDoc->m_pReelMapInnerAllUp)
			pDoc->m_pReelMapInnerAllUp->Open();
		if (pDoc->m_pReelMapInnerAllDn)
			pDoc->m_pReelMapInnerAllDn->Open();
	}

	if (pDoc->m_pReelMapInner)
	{
		if (pDoc->m_pReelMapInner->m_nLayer < 0)
			pDoc->m_pReelMapInner->m_nLayer = pView->m_nSelRmapInner;
		pDoc->m_pReelMapInner->Open();
	}
}

void CGvisR2R_LaserView::OpenReelmapInnerUp()
{
	//GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (pDoc->m_pReelMapInnerUp)
		pDoc->m_pReelMapInnerUp->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerAllUp)
			pDoc->m_pReelMapInnerAllUp->Open();
	}

	if (pDoc->m_pReelMapInner)
	{
		if (pDoc->m_pReelMapInner->m_nLayer < 0)
			pDoc->m_pReelMapInner->m_nLayer = pView->m_nSelRmapInner;

		if (bDualTest)
		{
			if (pDoc->m_pReelMapInner->m_nLayer == RMAP_UP || pDoc->m_pReelMapInner->m_nLayer == RMAP_ALLUP)
				pDoc->m_pReelMapInner->Open();
		}
		else
		{
			if (pDoc->m_pReelMapInner->m_nLayer == RMAP_UP)
				pDoc->m_pReelMapInner->Open();
		}
	}
}

void CGvisR2R_LaserView::OpenReelmapInnerDn()
{
	//GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (!bDualTest)
		return;

	if (pDoc->m_pReelMapInnerDn)
		pDoc->m_pReelMapInnerDn->Open();
	if (pDoc->m_pReelMapInnerAllDn)
		pDoc->m_pReelMapInnerAllDn->Open();

	if (pDoc->m_pReelMapInner)
	{
		if (pDoc->m_pReelMapInner->m_nLayer < 0)
			pDoc->m_pReelMapInner->m_nLayer = pView->m_nSelRmapInner;

		if (pDoc->m_pReelMapInner->m_nLayer == RMAP_DN || pDoc->m_pReelMapInner->m_nLayer == RMAP_ALLDN)
			pDoc->m_pReelMapInner->Open();
	}
}


void CGvisR2R_LaserView::UpdateRstInner()
{
	//if (m_pDlgMenu06)
	//	m_pDlgMenu06->UpdateRst();
}

void CGvisR2R_LaserView::InitReelmapInner()
{
	pDoc->InitReelmapInner();
	pDoc->SetReelmapInner(ROT_NONE);
}

void CGvisR2R_LaserView::InitReelmapInnerUp()
{
	pDoc->InitReelmapInnerUp();
	pDoc->SetReelmapInner(ROT_NONE);
}

void CGvisR2R_LaserView::InitReelmapInnerDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;
	if (bDualTest)
	{
		pDoc->InitReelmapInnerDn();
		pDoc->SetReelmapInner(ROT_NONE);
	}
}

void CGvisR2R_LaserView::DispDefImgInner()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	int nBreak = 0;

	switch (m_nStepTHREAD_DISP_DEF_INNER)
	{
		// CopyDefImg Start ============================================
	case 0:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 1:
		Sleep(300);
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 2:
		if (bDualTest)
			nSerial = m_nBufDnSerial[0]; // 좌측 Camera
		else
			nSerial = m_nBufUpSerial[0]; // 좌측 Camera

		m_nStepTHREAD_DISP_DEF_INNER++;
		break;

	case 3:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 4:
		if (bDualTest)
			nSerial = m_nBufDnSerial[1]; // 우측 Camera
		else
			nSerial = m_nBufUpSerial[1]; // 우측 Camera

		if (nSerial > 0)
		{
			m_nStepTHREAD_DISP_DEF_INNER++;

			if (ChkLastProc() && (nSerial > m_nLotEndSerial))
			{
				SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(우) Display Start
			}
			else
			{
				SetSerialReelmapInner(nSerial);			// Reelmap(우) Display Start
			}
		}
		else
		{
			if (ChkLastProc())
			{
				m_nStepTHREAD_DISP_DEF_INNER++;
				SetSerialReelmapInner(m_nLotEndSerial + 1, TRUE);	// 불량이미지(우) Display Start
			}
			else
			{
				if (bDualTest)
				{
					if (m_bLastProc && m_nBufDnSerial[0] == m_nLotEndSerial)
						m_nStepTHREAD_DISP_DEF_INNER++;
					else
					{
						m_nStepTHREAD_DISP_DEF_INNER++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] == m_nLotEndSerial)
						m_nStepTHREAD_DISP_DEF_INNER++;
					else
					{
						m_nStepTHREAD_DISP_DEF_INNER++;
					}
				}
			}
		}
		break;
	case 5:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 6:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 7:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 8:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 9:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 10:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;

	case 11:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 12:
		m_bTHREAD_DISP_DEF_INNER = FALSE;
		break;
		// Disp DefImg End ============================================
	}

}

BOOL CGvisR2R_LaserView::SetSerialReelmapInner(int nSerial, BOOL bDumy)
{
	return TRUE;
}

BOOL CGvisR2R_LaserView::SetSerialMkInfoInner(int nSerial, BOOL bDumy)
{
	return TRUE;
}

void CGvisR2R_LaserView::ChkRcvSig()
{
	if (!m_pEngrave) return;

	int nMsgID = 0;
	for (nMsgID = 0; nMsgID < _SigInx::_EndIdx; nMsgID++)
	{
		GetCurrentInfoSignal();

		if (m_pEngrave->m_bRcvSig[nMsgID])
		{
			m_pEngrave->m_bRcvSig[nMsgID] = FALSE;
			switch (nMsgID)
			{
			case _SigInx::_UpdateWork:
				break;
			case _SigInx::_Stop:
				break;
			case _SigInx::_EngAutoInit:
				break;
			case _SigInx::_MyMsgYes:
				SetMyMsgYes();
				break;
			case _SigInx::_MyMsgNo:
				SetMyMsgNo();
				break;
			case _SigInx::_MyMsgOk:
				SetMyMsgOk();
				break;
			case _SigInx::_TempPause:
				::WritePrivateProfileString(_T("Last Job"), _T("Use Temporary Pause"), pDoc->WorkingInfo.LastJob.bTempPause ? _T("1") : _T("0"), PATH_WORKING_INFO);
				break;
			default:
				break;
			}
		}
	}
}

void CGvisR2R_LaserView::AdjLaserOffset(CfPoint ptOffset)
{
	if (!m_pMdx2500)
		return;

	if (!pDoc->m_bUseAdjustLaser)
		return;

	double pData[5]; // X_org,Y_org,X_offset,Y_offset,Theta_offset

	if ((ptOffset.x > 0.01 || ptOffset.x < -0.01) && (ptOffset.y > 0.01 || ptOffset.y < -0.01))
	{
		pDoc->WorkingInfo.LastJob.dEngravePosOffsetX = ptOffset.x;
		pDoc->WorkingInfo.LastJob.dEngravePosOffsetY = ptOffset.y;

		pData[0] = pDoc->WorkingInfo.LastJob.dEngraveOrgX;						// X_org
		pData[1] = pDoc->WorkingInfo.LastJob.dEngraveOrgY;						// Y_org
		pData[2] = pDoc->WorkingInfo.LastJob.dEngravePosOffsetX;				// X_offset
		pData[3] = pDoc->WorkingInfo.LastJob.dEngravePosOffsetY;				// Y_offset
		pData[4] = pDoc->WorkingInfo.LastJob.dEngravePosTheta;					// Theta_offset

		if (m_pMdx2500->SetLaserPos(pData))
		{
			MSG message;

			while (m_pMdx2500->IsRunning())
			{
				if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&message);
					::DispatchMessage(&message);
				}
				Sleep(10);
			}
		}

	}
	else if (ptOffset.x > 0.01 || ptOffset.x < -0.01)
	{
		pDoc->WorkingInfo.LastJob.dEngravePosOffsetX = ptOffset.x;

		pData[0] = pDoc->WorkingInfo.LastJob.dEngraveOrgX;						// X_org
		pData[1] = pDoc->WorkingInfo.LastJob.dEngraveOrgY;						// Y_org
		pData[2] = pDoc->WorkingInfo.LastJob.dEngravePosOffsetX;				// X_offset
		pData[3] = pDoc->WorkingInfo.LastJob.dEngravePosOffsetY;				// Y_offset
		pData[4] = pDoc->WorkingInfo.LastJob.dEngravePosTheta;					// Theta_offset

		if (m_pMdx2500->SetLaserPos(pData))
		{
			MSG message;

			while (m_pMdx2500->IsRunning())
			{
				if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&message);
					::DispatchMessage(&message);
				}
				Sleep(10);
			}

		}
	}
	else if (ptOffset.y > 0.01 || ptOffset.y < -0.01)
	{
		pDoc->WorkingInfo.LastJob.dEngravePosOffsetY = ptOffset.y;

		pData[0] = pDoc->WorkingInfo.LastJob.dEngraveOrgX;						// X_org
		pData[1] = pDoc->WorkingInfo.LastJob.dEngraveOrgY;						// Y_org
		pData[2] = pDoc->WorkingInfo.LastJob.dEngravePosOffsetX;				// X_offset
		pData[3] = pDoc->WorkingInfo.LastJob.dEngravePosOffsetY;				// Y_offset
		pData[4] = pDoc->WorkingInfo.LastJob.dEngravePosTheta;					// Theta_offset

		if (m_pMdx2500->SetLaserPos(pData))
		{
			MSG message;

			while (m_pMdx2500->IsRunning())
			{
				if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&message);
					::DispatchMessage(&message);
				}
				Sleep(10);
			}
		}
	}
	else
		return;
}

void CGvisR2R_LaserView::Option01()
{
	ClrDispMsg();
	CDlgOption01 Dlg;
	Dlg.DoModal();
}

void CGvisR2R_LaserView::SetErrorRead2dCode(int nMcId)
{
	if (m_pEngrave)
		m_pEngrave->SetErrorRead2dCode(nMcId);
}

void CGvisR2R_LaserView::ChkErrorRead2dCode()
{
	if (IsAuto())
	{
		if (MODE_INNER == pDoc->WorkingInfo.LastJob.nTestMode || MODE_LASER == pDoc->WorkingInfo.LastJob.nTestMode)
		{
			if (pDoc->GetSignalAoiUp())
			{
				if (m_pEngrave)
					m_pEngrave->SetErrorRead2dCode(_PcId::_AoiUp);
				pDoc->SetSignalAoiUp();
			}
			else if (pDoc->GetSignalAoiDn())
			{
				if (m_pEngrave)
					m_pEngrave->SetErrorRead2dCode(_PcId::_AoiDn);
				pDoc->SetSignalAoiDn();
			}
		}
	}
}

void CGvisR2R_LaserView::EngAutoInit()
{
	if (pDoc->BtnStatus.EngAuto.Init)
	{
		pView->m_bCont = FALSE;

		pDoc->m_bDoneChgLot = FALSE;
		pView->m_nNewLot = 0;
		pDoc->SetEngraveLastShot(0);
		pDoc->SetCurrentInfoEngShotNum(0);
		pDoc->SetCurrentInfoReadShotNum(0);
		InitAutoEng();
	}
}

void CGvisR2R_LaserView::EngAutoInitCont()
{
	pView->m_bCont = TRUE;

	pDoc->m_bDoneChgLot = FALSE;
	pView->m_nNewLot = 0;
	InitAutoEng();
}

void CGvisR2R_LaserView::SwReset()
{
	ClrDispMsg();

	if (!DoReset())
		return;

	m_bSwRun = FALSE;
	m_bSwStop = FALSE;
	m_bSwReady = FALSE;
	m_bSwReset = TRUE;
}

BOOL CGvisR2R_LaserView::DoReset()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	// 자신의 로컬파일에 설정
	pDoc->SetCurrentInfoSignal(_SigInx::_MyMsgYes, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_MyMsgNo, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_MyMsgOk, FALSE);


	if (pDoc->Status.bManual)
	{
		BOOL bInit = TRUE;

		pView->ClrDispMsg();

		if (IDNO == pView->MsgBox(_T("초기화를 하시겠습니까?"), 0, MB_YESNO))
			bInit = FALSE;
		else
		{
			pDoc->m_bDoneChgLot = FALSE;
			pView->m_nNewLot = 0;
			pDoc->SetEngraveLastShot(0);
			pDoc->SetCurrentInfoEngShotNum(0);
			pDoc->SetCurrentInfoReadShotNum(0);
		}
			pView->m_bCont = FALSE;
			if (!bInit)
			{
				if (IDNO == pView->MsgBox(_T("이어가기를 하시겠습니까?"), 0, MB_YESNO))
				{
					pView->m_bCont = FALSE;
					return FALSE;
				}
				pView->m_bCont = TRUE;
			}

		InitAutoEng();
		return TRUE;
	}

	return FALSE;
}
