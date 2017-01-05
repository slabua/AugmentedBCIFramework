#ifndef GUI_H
#define GUI_H

#include <afxmt.h>
#include <string>
#include "KinectListener.h"
#include "TobiiTracker.h"

// SLB QT4
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>			// SLB
#include <QtGui/QRadioButton>
#include <QtGui/QGroupBox>
#include <QtGui/QTextEdit>
#include <QtGui/qprogressbar.h>		// SLB
#include <QtGui/qstatusbar.h>		// SLB
#include <QtGui/qmainwindow.h>		// SLB
//
/* SLB QT4
#include <Qt/qdialog.h>
#include <Qt/qlabel.h>
#include <Qt/qlineedit.h>
#include <Qt/qboxlayout.h>
#include <Qt/qpushbutton.h>
#include <Qt/qcombobox.h>
#include <Qt/qcheckbox.h> // SLB
#include <Qt/qradiobutton.h>
#include <Qt/qgroupbox.h>
#include <Qt/qmessagebox.h>
#include <Qt/qtextedit.h>
#include <Qt/qscrollbar.h>
#include <Qt/qthread.h>
*/
/* SLB QT5
#include <QtWidgets\qdialog.h>
#include <QtWidgets\qlabel.h>
#include <QtWidgets\qlineedit.h>
#include <QtWidgets\qboxlayout.h>
#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qcombobox.h>
#include <QtWidgets\qcheckbox.h> // SLB
#include <QtWidgets\qradiobutton.h>
#include <QtWidgets\qgroupbox.h>
#include <QtWidgets\qmessagebox.h>
#include <QtWidgets\qtextedit.h>
#include <QtWidgets\qscrollbar.h>
#include <QtCore\qthread.h>
*/

#include "Worker.h"
#include "qcustomplot.h"

using std::string;
using std::map;
using std::vector;

// SLB
#define TIMER_INTERVAL			50
#define FRAMEWORKNAME			"UniPA BCI Framework"
#define SERVICE_CHANNELS		3
#define NOTIFICATION_SOUND		"..\\bin\\audio\\notification.wav"
#define FRAMEWORKREDICONPATH	"./icons/UniPABCIGui.bin"
#define FRAMEWORKBLUEICONPATH	"./icons/UniPABCISpeller.bin"
#define FRAMEWORKBGIMAGE		"./icons/bg.jpg"
#define CREDITSBGIMAGE			"./icons/unipabg.png"
#define EYEICONPATH				"./icons/iconeye.png"
#define BRAINICONPATH			"./icons/iconbrain.png"
#define EYECLOSEDICONPATH		"./icons/eyeclosed.png"
#define EYEOPENICONPATH			"./icons/eyeopen.png"
#define EYESCLOSED				0
#define EYESOPEN				1
//#define CLOSEDEYES_START		100
#define CLOSEDEYES_STOP			101
//#define OPENEYES_START		200
//#define OPENEYES_STOP			201
#define CHSTATUS_GREY			"#C8C8C8"
#define CHSTATUS_GREEN			"#50C95D"//2EC940"
#define CHSTATUS_YELLOW			"#E5E05C"//E5DE40"
#define CHSTATUS_RED			"#E93A44"//E90814"
#define ocular_filter			60
//#define FILETYPE_SPELLING		-1
#define FILETYPE_BASELINE		-2
#define FILETYPE_GAME			-3
#define GAMEUSER_LOOKING		1
#define GAMETOBII_DISABLED		0
#define GAMEUSER_NOTLOOKING		-1
#define GAMEICON_ROCK			"./icons/rock.png"
#define GAMEICON_PAPER			"./icons/paper.png"
#define GAMEICON_SCISSORS		"./icons/scissors.png"
#define GAMEICON_ROCK_G			"./icons/rockg.png"
#define GAMEICON_PAPER_G		"./icons/paperg.png"
#define GAMEICON_SCISSORS_G		"./icons/scissorsg.png"
#define GAMEMODE_ISCHEATTRIAL	-4
#define GAMEMODE_NEXT			1
#define GAMEMODE_SKIP			-1
#define GAMEMODE_BASELINE		0
#define GAMEMODE_TRIALEND		6
#define GAMEMODE_INIT			1
#define GAMEMODE_ACTION			2
#define GAMEMODE_FAIR			5
#define GAMEMODE_CHEATWIN		3
#define GAMEMODE_CHEATLOSE		4
#define GAME_UNDEFINED			-1
#define GAME_ROCK				0
#define GAME_PAPER				1
#define GAME_SCISSORS			2
#define GAMERESULT_ROBOTWIN		0
#define GAMERESULT_TIE			1
#define GAMERESULT_PLAYERWIN	2
#define KINECT_EXECUTABLE		"KinectHandTracking.exe"


//class Gui : public QDialog {
class Gui : public QMainWindow {

	Q_OBJECT
	public:
		Gui(QWidget *parent = 0);
		virtual ~Gui();					// SLB
		void openLogFile();				// SLB
		void writeLogFile(string);		// SLB
		void initVariables();			// SLB
		void initReceivedData(int);		// SLB
		void initReceivedData();		// SLB
		void initiateConfiguration();	// SLB
		void readConfiguration();
		void updateConfiguration();
		void initiateOperation();		// SLB
		void terminateOperation();		// SLB
		void initmapping(int);
		void initMapVel();
		bool isRunning();
		CEvent& getFinishInitializationEvent();
		CEvent& getSignalCheckStartEvent();			// SLB
		CEvent& getGameSessionStartEvent();			// SLB
		CEvent& getBaselineAcquisitionStartEvent();	// SLB
		CEvent& getCalibrationStartEvent();
		CEvent& getSpellerStartEvent();
		CEvent& getFlashOptimizationEvent();
		CEvent& getFlashAnalysisStartEvent();
		CEvent& getSpellerFileStartEvent();
		CEvent& getOnlineFlashAnalysisStartEvent();
		CEvent& getCalibrationAnalysisStartEvent();
		//CEvent& getEyeTrackingEvent(); // SLB
		CEvent& getKillEvent();		// SLB
		bool exists_file(string);
		void check_folder(string);	// SLB
		void setIndexSliders();		// SLB
		void setPlotBars();			// SLB
		void insertNewFlash(string);
		string BoolToString(bool);	// SLB
		QRect buildScreenGeometry();						// SLB
		void showGameSessionDialogControl();				// SLB
		void showBaselineAcquisitionDialogControl(void);	// SLB
		void showBaselineAcquisitionDialogUser(void);		// SLB
		bool isAcqReady();						// SLB
		bool isAcqDone();						// SLB
		void setEyesAcquisitionStatus(int);		// SLB
		bool isCurrentRunDone();				// SLB
		bool isGameRunDone();					// SLB
		bool isGameRunStarted();				// SLB
		bool isGameUseBCI();					// SLB
		void setCurrentRunDone(bool);			// SLB
		void changeChannelStatus(int);			// SLB
		void setReceivedData(float*);			// SLB
		float getReceivedDataN();				// SLB
		float getReceivedDataN1();				// SLB
		void changeChannelStatusColour();		// SLB
		void guiElementsSetEnabled(bool);		// SLB
		void gameSetChoiceIcons(int);			// SLB
		void gamePlaySaishoWaGuu();				// SLB
		void gamePlayJanKenPon();				// SLB
		void gamePlayMove();					// SLB
		void gameTrialClosing();				// SLB
		void gamePlayRock();					// SLB
		void gamePlayPaper();					// SLB
		void gamePlayScissors();				// SLB
		void gamePlayPaper2Rock();				// SLB
		void gamePlayScissors2Rock();			// SLB
		void gamePlayRock2Paper();				// SLB
		void gamePlayScissors2Paper();			// SLB
		void gamePlayRock2Scissors();			// SLB
		void gamePlayPaper2Scissors();			// SLB
		void gamePlayTieSentence();				// SLB
		void gamePlayWinSentence();				// SLB
		void gamePlayLoseSentence();			// SLB
		void gameUpdateCounterLabel();			// SLB
		void gameDelay();						// SLB
		void robotRawCommand(string);			// SLB
		void robotPlaymotion(string);			// SLB
		void robotPlayvoice(string);			// SLB
		void robotSpeak(string, float);			// SLB
		void playSound(string);					// SLB
		void initKinect();						// SLB Kinect
		void initTobii();						// SLB Tobii
		void destroyTobii();					// SLB
		TobiiTracker *gameEyeT;					// SLB
	public slots:
		void clickedSignalCheckButton();			// SLB
		void clickedGameSessionButton();			// SLB
		void clickedBaselineAcquisitionButton();	// SLB
		void clickedBaselineAcqButton1();			// SLB
		void clickedBaselineAcqButton2();			// SLB
		void menubar1helpSlot();					// SLB
		void menubar1creditsSlot();					// SLB
		void setAcqDone();							// SLB
		int getBaselineAcqDuration();				// SLB
		void clickedCalibrationButton();
		void clickedSpellerButton();
		void clickedSpellerFileButton();
		//void enableSpellerButton();				// SLB removed, was never used
		void clickedFlashAnalysisButton();
		void clickedOnlineFlashAnalysisButton();
		void clickedStartCalibrationAnalysisButton();
		void updateStamp(string);
		//void gameSetChoiceIcons(int);				// SLB slot for worker class
	signals:
	private slots:
		void okClicked();
		void configChanged();						// SLB
		//void resetClicked();						// SLB TODO
		void closeClicked();						// SLB
		void closeEvent(QCloseEvent *event);		// SLB
		//void enableokButton();					// SLB not needed
		void changeComboTarget();
		void changeTagComboTarget();
		void changeTextureCombo();					// SLB
		void changePaletteString();					// SLB
		void changeLineFlash();						// SLB
		//void changeLineFlashTime();				// SLB
		void changeChannel1();						// SLB
		void changeChannel2();						// SLB
		void clickedstate(bool);
		void clickedOpt(bool);
		void clickedFlashTimeAuto(bool);			// SLB
		void eyeIconClicked(void);					// SLB
		void brainIconClicked(void);				// SLB
		void eyeTrackerChecked(bool);				// SLB
		void clickedtobiifilter(bool);				// SLB
		void clickedUtilMode(bool);					// SLB
		void changeGameCheat1();					// SLB
		void changeGameCheat2();					// SLB
		void clickedGameMode(bool);					// SLB
		void clickedGameApplyButton();				// SLB
		void clickedGameExitButton();				// SLB
		void clickedGameStartButton();				// SLB
		void clickedGameNextButton();				// SLB
		void clickedGameSkipButton();				// SLB
		void clickedGameRockButton();				// SLB
		void clickedGamePaperButton();				// SLB
		void clickedGameScissorsButton();			// SLB
		void clickGameUseOptionsCheckBox(bool);		// SLB
		void getSkillSliderValue(int);				// SLB
		void getSignalPlotGainValue(int);			// SLB
		void startCount();		// SLB disable Worker (no idea why it was used at all)
		void countFinished();	// SLB disable Worker (no idea why it was used at all)
		void updateTimerTime();						// SLB
		void updateTimer();							// SLB
	private:
		string notificationSound;
		string logFilePath; // SLB
		//CFile logFile; // SLB
		QTimer *timerTime;	// SLB
		QTimer *timer;		// SLB
		QLabel *timerLabel;	// SLB
		QHBoxLayout *targetLayout;
		QLabel *labelUsername;
		QLabel *labelSkill;		// SLB
		QSlider *skillSlider;	// SLB
		QPushButton *iconEyeLabel;		// SLB
		QPushButton *iconBrainLabel;	// SLB
		QDialog *dialogControl; // SLB
		QDialog *dialogUser;	// SLB
		QDialog *gameSessionDialogControl;	// SLB
		QLineEdit *durationLineEdit;	// SLB
		QLabel *dialogLabelUser;		// SLB
		QLabel *dialogIconLabel;		// SLB
		QPushButton *closedEyesButton;	// SLB
		QPushButton *openedEyesButton;	// SLB
		QLabel *pbarLabel;		// SLB
		QProgressBar *pbar;		// SLB
		QLabel *pbarLabel1;		// SLB
		QProgressBar *pbar1;	// SLB
		QProgressBar *pbar2;	// SLB
		QStatusBar *statusbar;	// SLB
		QLabel *labelModalita;
		QLabel *labelInterface;
		QLabel *labelNumCalibTag;
		QLabel *labelTarget;
		QLabel *labelFlash;
		QLabel *labelFlashTime;	// SLB
		QLabel *labelChan;		// SLB
		QLabel *labelSensor;
		QLabel *labelUtilMode;	// SLB
		QLabel *labelRobot;
		QLabel *labelFlashAnalysis;
		QLabel *labelCalibrationAnalysis;
		QLineEdit *lineTagOnlineAnalysis;
		QLineEdit *lineUsername;
		QComboBox* cusername;
		QComboBox* ctextures;			// SLB
		QLineEdit *linePaletteString;	// SLB
		QLabel *paletteStringCounter;	// SLB
		QComboBox *comboModalita;
		QComboBox *comboInterfaccia;
		vector< QComboBox* > comboTarget;
		QLineEdit *lineNumCalibTag;
		QLineEdit *lineFlash;
		QLineEdit *lineFlashTime;		// SLB
		QLineEdit *lineChan;			// SLB
		QLineEdit *lineChan2;			// SLB
		QRadioButton *radioAlphabetic;
		QRadioButton *radioSymbolic;
		//QRadioButton *radioCustomPalette; // SLB
		QCheckBox *checkCustomPalette;	// SLB
		QRadioButton *radioBasic;		// SLB
		QRadioButton *radioHybrid;		// SLB
		QRadioButton *radioBioHybrid;	// SLB
		//QRadioButton *radioOn;		// SLB
		//QRadioButton *radioOff;		// SLB
		QCheckBox *checkFlashOpt;		// SLB
		QCheckBox *checkFlashTimeAuto;	// SLB
		QRadioButton *tobiiFilteredRadio;	// SLB
		QRadioButton *tobiiRawRadio;	// SLB
		QRadioButton *gameRadioFair;	// SLB
		QRadioButton *gameRadioCheatW;	// SLB
		QRadioButton *gameRadioCheatL;	// SLB
		QLineEdit *gameFairEdit1;		// SLB
		QLineEdit *gameCheatEdit1;		// SLB
		QLineEdit *gameCheatEdit2;		// SLB
		QLineEdit *gameFairEdit2;		// SLB
		QPushButton *gameRockButton;	// SLB
		QPushButton *gamePaperButton;	// SLB
		QPushButton *gameScissorsButton;	// SLB
		QPixmap iconRock;				// SLB
		QPixmap iconPaper;				// SLB
		QPixmap iconScissors;			// SLB
		QPixmap iconRockG;				// SLB
		QPixmap iconPaperG;				// SLB
		QPixmap iconScissorsG;			// SLB
		QLabel *gameCounterLabel;		// SLB
		QCheckBox *gameUseKinectCheckBox;	// SLB
		QCheckBox *gameUseBCICheckBox;		// SLB
		QCheckBox *gameUseRobotCheckBox;	// SLB
		QString q_username;
		QLabel *labelEyeTracker;			// SLB
		QCheckBox *eyeTrackerCheckBox;		// SLB
		QCheckBox *eyeTrackerGazeCheckBox;	// SLB
		QLineEdit *lineIpRobot;
		QLineEdit *linePortRobot;
		QLineEdit *lineCalibrationAnalysis;
		QPushButton *startCalibrationAnalysis;
		QPushButton *okButton;
		//QPushButton *resetButton; // SLB TODO
		QPushButton *closeButton;
		string username;
		int number_of_calibration_tag;
		int number_of_row_interface;
		int number_of_flash;
		bool time_flash_auto;		// SLB
		int number_channels;		// SLB
		int number_channels_plus;	// SLB
		//int tot_channels;			// SLB
		int number_of_calibrations;
		//bool flashingMode;
		int mode;
		//int optFlash; // SLB
		bool optFlash;
		int number_of_tags;
		int* target_of_calibration;
		bool symbolicMode;
		bool customPalette;
		int elements;
		int velocita;
		map<string,int> mappingVel;
		map<int, string> mappingIdTag;
		bool userobot;				// SLB
		string ipRobot;
		string portRobot;
		string ipnao;
		string portnao;
		bool tobiiInitialised;		// SLB
		bool kinectInitialised;		// SLB
		bool kinectGuiStarted;		// SLB
		int skillSliderValue;		// SLB
		bool tobiienabled;			// SLB
		bool tobiigazeenabled;		// SLB
		bool tobiifiltered;			// SLB
		bool bioHybridMode;			// SLB
		bool acqReady;				// SLB
		bool acqDone;				// SLB
		int eyesAcqStatus;			// SLB
		bool currentRunDone;		// SLB
		bool gameRunDone;			// SLB
		bool gameRunStarted;		// SLB
		bool gameUseBCI;			// SLB
		//int ocular_filter;		// SLB
		int gameMode;				// SLB
		int gameSessionCounter;		// SLB
		int gameSessionCounterTot;	// SLB
		int gameFair1CounterTot;	// SLB
		int gameCheatCounter;		// SLB
		int gameCheatCurrent;		// SLB
		int gameCheatCounterTot;	// SLB
		int gameFair2CounterTot;	// SLB
		int gamePlayerChoice;		// SLB
		int gameRobotChoice;		// SLB
		vector <int> gameWinCounts; // SLB
		int gameLastResult;			// SLB
		bool gameUseKinect;			// SLB
		float* receivedData;		// SLB
		float receivedDataN;		// SLB
		float receivedDataN1;		// SLB
		string impaired_theme;		// SLB
		string impaired_letterCol;	// SLB
		string impaired_bgCol;		// SLB
		string palette_string;		// SLB
		string language;			// SLB
		bool running;
		Worker *worker;				// SLB disable Worker (no idea why it was used at all)
		QPushButton *signalCheckButton;			// SLB
		QPushButton *baselineAcquisitionButton;	// SLB
		QPushButton *gameSessionButton;			// SLB
		QPushButton *calibration;
		QPushButton *speller;
		QPushButton *spellerFile;
		QPushButton *flashAnalysis;
		QPushButton *onlineAnalysis;
		QTextEdit *textarea;
		QSlider *index1Slider;		// SLB
		QSlider *index2Slider;		// SLB
		QSlider *index3Slider;		// SLB
		//QLabel *graphWidget;		// SLB
		//QLabel *graphWidget2;		// SLB
		QCustomPlot *customPlot;	// SLB
		QCustomPlot *customPlot2;	// SLB
		QSlider *signalPlotGainSlider;	// SLB
		int signalPlotGain;			// SLB
		QGroupBox *chanStatusBox;	// SLB
		QHBoxLayout *chanStatusLayout;		// SLB
		vector <QLabel*> chanStatusLabels;	// SLB
		QCPBars *rsquare;			// SLB
		QCPBars *epy;				// SLB
		QCPBars *egy;				// SLB
		QVector<double> ticks;		// SLB
		QVector<QString> labels;	// SLB
		QVector<double> rsquareData, epyData, egyData;	// SLB
		QVector<double> x;			// SLB
		vector <QVector <double>> y;	// SLB
		bool countRunning;			// SLB disable Worker (no idea why it was used at all)
		string messaggeFromFramework;
		vector <string> list_files(string);		// SLB
		vector <string> list_folders(string);	// SLB
		vector <string> files;					// SLB
		vector <string> textures;				// SLB
		CEvent _eventFinishInitialization;
		CEvent _eventSignalCheckStart;			// SLB
		CEvent _eventGameSessionStart;			// SLB
		CEvent _eventBaselineAcquisitionStart;	// SLB
		CEvent _eventCalibrationStart;
		CEvent _eventSpellerStart;
		CEvent _eventSpellerFileStart;
		CEvent _eventFlashOptimization;
		CEvent _eventFlashAnalysis;
		CEvent _eventOnlineFlashAnalysis;
		CEvent _eventCalibrationAnalysis;
		CEvent _eventKill;				// SLB
		CWinThread* _networkTagThread;	// SLB Kinect
		KinectListener *listener;		// SLB Kinect
};

// SLB -------------------------------------------------------------
// color setter
inline void SetColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// helper class
class color {
public:
	color(WORD val) : m_val(val) { }

	void set() const {
		SetColor(m_val);
	}

private:
	WORD m_val;
};

// instances of helper class to avoid need to remember 4 is red, etc
static const color RED(4);
static const color GREEN(2);
static const color BLUE(1);
static const color WHITE(7);
// etc

// overload operator<< to get manipulator to work
inline std::ostream& operator<<(std::ostream& os, const color& c) {
	c.set();
	return os;
}
// -----------------------------------------------------------------

#endif