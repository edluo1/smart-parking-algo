
// smart_parking_guiDlg.cpp : implementation file
//

#include "stdafx.h"
#include "smart_parking_gui.h"
#include "smart_parking_guiDlg.h"
#include "afxdialogex.h"
#include "Cadd_Destination.h"
#include "Cadd_Lot.h"
#include "Cadd_Driver.h"
#include "Commands.h" // Used to handle commands
#include "Grid.h" // Contains the grid
#include <string>
#include <cmath>
#include <io.h>
#include <fcntl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Csmart_parking_guiDlg dialog

Csmart_parking_guiDlg::Csmart_parking_guiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Csmart_parking_guiDlg::IDD, pParent)
	, m_EchoSize(_T("Grid size: "))
	, m_EchoTime(_T("Time: "))
	, m_EchoStatus(_T("Open"))
	, m_IterationEcho(_T("0/4"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		this->world = new Grid(10, 5); // default grid
		destDrawn = false;
		lotDrawn = false;
}

void Csmart_parking_guiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ST_GRIDSIZE, m_EchoSize);
	DDX_Text(pDX, IDC_ST_TIME, m_EchoTime);
	DDX_Text(pDX, IDC_ST_STATUS, m_EchoStatus);
	DDX_Text(pDX, IDC_ITCOUNT, m_IterationEcho);
	DDX_Control(pDX, IDC_SLIDE_ITER, m_VSliderIteration);
}

BEGIN_MESSAGE_MAP(Csmart_parking_guiDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_B_OPEN_CONFIG, &Csmart_parking_guiDlg::OnBnClickedBOpenConfig)
	ON_BN_CLICKED(IDC_B_SAVECONFIG, &Csmart_parking_guiDlg::OnBnClickedBSaveconfig)
	ON_BN_CLICKED(IDC_B_NEXTEVENT, &Csmart_parking_guiDlg::OnBnClickedBNextevent)
	ON_BN_CLICKED(IDC_B_NEWDEST, &Csmart_parking_guiDlg::OnBnClickedBNewdest)
	ON_BN_CLICKED(IDC_B_NEWLOT, &Csmart_parking_guiDlg::OnBnClickedBNewlot)
	ON_BN_CLICKED(IDC_B_NEWDRIVER, &Csmart_parking_guiDlg::OnBnClickedBNewdriver)
	ON_BN_CLICKED(IDC_B_SIMEND, &Csmart_parking_guiDlg::OnBnClickedBSimend)
	ON_BN_CLICKED(IDC_B_SHOWSTATUS, &Csmart_parking_guiDlg::OnBnClickedBShowstatus)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// Csmart_parking_guiDlg message handlers

BOOL Csmart_parking_guiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	gridDrawSurface = (CWnd *)this->GetDlgItem(IDC_GRID_BOX);
	gridDraw = gridDrawSurface->GetDC();
	// add command to draw stuff here
	m_VSliderIteration.SetRange(0, (world->getIterationCount()-1), TRUE);
	m_VSliderIteration.SetPos(0);
	m_IterationEcho.Format(_T("%d/%d"), 0, world->getIterationCount()-1);
	return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Csmart_parking_guiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	DrawGrid();
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Csmart_parking_guiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Csmart_parking_guiDlg::DrawGrid() {
	CRect gridBase;
	gridDrawSurface->GetWindowRect(&gridBase);
	this->ScreenToClient(&gridBase);
	CPoint bottomRight = gridBase.TopLeft();
	bottomRight += CPoint(275, 275);
	gridBase.BottomRight() = bottomRight;
	gridBase.NormalizeRect();
	gridDraw->Rectangle(gridBase);
	int baseRectWidth = gridBase.Width();
	int baseRectHeight = gridBase.Height();
	double proportion = (baseRectWidth / world->getGridSize());
	// Draw destinations. Set boolean to check if all are drawn yet
		CBrush brushDest(RGB(165, 42, 42));
		gridBrush = gridDraw->SelectObject(&brushDest);
		CPen penBlack;
		penBlack.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		gridPen = gridDraw->SelectObject(&penBlack);
		vector <Location> destLoc = world->getDestLocations();
		int xCenter;
		int yCenter;
		for (size_t ii = 0; ii < destLoc.size(); ii++) { // draw brown circle
			xCenter = (int)round(gridBase.TopLeft().x + destLoc[ii].x*proportion);
			yCenter = (int)round(gridBase.TopLeft().y + destLoc[ii].y*proportion);
			gridDraw->Rectangle(xCenter - 2, yCenter-2, xCenter+2, yCenter+2);
		}
		gridDraw->SelectObject(gridBrush);
		destDrawn = true;
	// Draw lots.
		CBrush brushLot(RGB(35, 62, 148));
		gridBrush = gridDraw->SelectObject(&brushLot);
		vector<Location> lotLoc = world->getLotLocations();
		for (size_t ii = 0; ii < lotLoc.size(); ii++) { // draw blue circle
			xCenter = (int)round(gridBase.TopLeft().x + lotLoc[ii].x*proportion);
			yCenter = (int)round(gridBase.TopLeft().y + lotLoc[ii].y*proportion);
			gridDraw->Ellipse(xCenter-2, yCenter-2, xCenter+2, yCenter+2);
		}
		lotDrawn = true;
		gridDraw->SelectObject(gridBrush);
		gridDraw->SelectObject(gridPen);
	// Draw drivers
	vector<Location> driverLoc = world->getDriverLocations(); // get all drivers currently visible on screen
	for (size_t ii = 0; ii < driverLoc.size(); ii++) { // draw red dot
		gridDraw->SetPixel(gridBase.TopLeft().x + driverLoc[ii].x*proportion, gridBase.TopLeft().y + driverLoc[ii].y*proportion, RGB(255, 0, 0));
	}
}

void Csmart_parking_guiDlg::addDestination(Destination * toAdd) {
	world->addDestination(toAdd);
}

void Csmart_parking_guiDlg::addLot(Lot * toAdd) {
	world->addLot(toAdd);
}

void Csmart_parking_guiDlg::addDriver(Driver * toAdd, int iteration) {
	world->addDriver(toAdd, iteration);
}

void Csmart_parking_guiDlg::OnBnClickedBOpenConfig()
{
	wchar_t szFilters[] = _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||");
	// Create an Open dialog
	CFileDialog fileDlg(TRUE, _T("txt"), _T("*.txt"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this); // Display the file dialog. 

	// When user clicks OK, fileDlg.DoModal() returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		CString m_strPathname = fileDlg.GetPathName();
		CT2CA converter(m_strPathname);
		std::string fileToOpen(converter);
		// TODO: Open Grid file
		open_file(*world, fileToOpen);
		m_VSliderIteration.SetRange(0, (world->getIterationCount() - 1), TRUE);
		m_VSliderIteration.SetPos(0);
		m_IterationEcho.Format(_T("%d/%d"), 0, world->getIterationCount() - 1);
		m_GridSize = world->getGridSize(); // double
		m_EchoSize.Format(_T("Grid size: %g"), m_GridSize);
		//Change the window's title to the opened file's title.
		CString fileName = fileDlg.GetFileTitle();

		SetWindowText(fileName);

	}
	UpdateData(FALSE);
}

void Csmart_parking_guiDlg::show_events() {

}

void Csmart_parking_guiDlg::OnBnClickedBSaveconfig()
{
	// TODO: Add your control notification handler code here
	// szFilters is a text string that includes two file name filters:
	// "*.my" for "MyType Files" and "*.*' for "All Files."
	TCHAR szFilters[] = _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||");

	// Create a Save dialog
	CFileDialog fileDlg(FALSE, _T("txt"), _T("*.txt"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();
		CT2CA converter(pathName);
		std::string fileToWrite(converter);
		// Implement opening and reading file in here.
		write_file(*world, fileToWrite);
		//Change the window's title to the opened file's title.
		CString fileName = fileDlg.GetFileTitle();

		SetWindowText(fileName);
	}
}


void Csmart_parking_guiDlg::OnBnClickedBNextevent()
{
	// TODO: Add your control notification handler code here
	run_simulation(*world);
	m_TimeDisplay = world->getTime(); // double
	m_EchoTime.Format(_T("Time: %g"), m_TimeDisplay);
	UpdateData(FALSE);
	Invalidate();
	GetDlgItem(IDC_ST_GRIDSIZE)->RedrawWindow();
	GetDlgItem(IDC_ST_TIME)->RedrawWindow();
	GetDlgItem(IDC_GRID_BOX)->RedrawWindow();
}

void Csmart_parking_guiDlg::OnBnClickedBSimend() // On clicking, simulation jumps to the very end.
{
	while (!world->simulationOver[world->getCurrentIteration()]) {
		run_simulation(*world);
		m_TimeDisplay = world->getTime(); // double
		m_EchoTime.Format(_T("Time: %g"), m_TimeDisplay);
		UpdateData(FALSE);
		OnPaint();
		// AfxPumpMessage(); // should prevent "not responding" from displaying
		// Sleep(50); // program stops responding at times with a sleep message
	}
}

void Csmart_parking_guiDlg::OnBnClickedBNewdest()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	Cadd_Destination Dlg(world->getDestinationCount(), this);
	
	if (Dlg.DoModal() == IDOK) { // if OK was pressed
		double destX = Dlg.m_LocationX;
		double destY = Dlg.m_LocationY;
		Destination * newDest = new Destination(world->getDestinationCount(), Location(destX, destY)); // setup new destination
		addDestination(newDest);
		destDrawn = false; // redraw destinations
	}
	UpdateData(FALSE);
}


void Csmart_parking_guiDlg::OnBnClickedBNewlot()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	Cadd_Lot Dlg(world->getLotCount(), this);
	if (Dlg.DoModal() == IDOK) { // then set up lot information
		double destX = Dlg.m_LocationX;
		double destY = Dlg.m_LocationY;
		int spaces = Dlg.m_ParkSpaceCount;
		double baseCost = Dlg.m_BaseCost;
		Lot * newLot = new Lot(world->getLotCount(), Location(destX, destY), spaces, baseCost, world);
		addLot(newLot);
		lotDrawn = false; // redraw lots next time
	}
	UpdateData(FALSE);
}


void Csmart_parking_guiDlg::OnBnClickedBNewdriver() // Opens a dialog to input a new driver. Only works with added destination.
{
	if (world->getDestinationCount() != 0) {
		UpdateData(TRUE);
		Cadd_Driver Dlg(world->getDriverCount(), this);
		if (Dlg.DoModal() == IDOK) {

		}
		UpdateData(FALSE);
	}
	else {
		AfxMessageBox(_T("No destinations available to create a new driver.\nPlease use 'New Destination' first before adding a driver."));
	}
}

void Csmart_parking_guiDlg::OnBnClickedBShowstatus()
{
	// TODO: Add your control notification handler code here

}


void Csmart_parking_guiDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (pScrollBar == (CScrollBar *)&m_VSliderIteration) {
		int value = m_VSliderIteration.GetPos();
		m_IterationEcho.Format(_T("%d/%d"), value, world->getIterationCount()-1);
		if (value != world->getCurrentIteration()) {
			world->switchIteration(value);
			m_TimeDisplay = world->getTime();
			m_EchoTime.Format(_T("Time: %g"), m_TimeDisplay);
			Invalidate();
		}
		UpdateData(FALSE);
	}
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}