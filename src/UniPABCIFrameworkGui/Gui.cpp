#include "Gui.h"
#include "moc_gui.cpp"
#include <iostream>
#include <sstream>
#include <fstream>

#include "rootpaths.h"			// SLB
#include <io.h>					// SLB
#include <mmsystem.h>			// SLB
#include "RobotController.h"	// SLB
#include "KinectListener.h"		// SLB
#include <afxwin.h>			// SLB ^

#include "pugixml.hpp"
#include "qcustomplot.h"

#include <QtGui/QMessageBox>
#include <QtGui/QScrollBar>
#include <QtCore/QThread>
#include <QtGui/qdesktopwidget.h>	// SLB
#include <QtGui/QApplication>		// SLB
#include <QtGui/qmenubar.h>			// SLB
#include <QtGui/qwidgetaction.h>	// SLB

using std::cout;
using std::endl;
using std::ifstream;
using std::stringstream;
using std::pair;

//const char* g_Title = "P300 Framework - RoboticsLab@UniPA";
//const char* g_Title = "UniPA BCI Framework";
//const char* g_Title = FRAMEWORKNAME;
//vector<string> list_files(string);
//vector<string> list_folders(string); // SLB
QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText); // SLB


//Gui::Gui(QWidget *parent) : QDialog(parent) {
Gui::Gui(QWidget *parent) : QMainWindow(parent) {

	running = true;

	readConfiguration();
	initVariables();	// SLB
	//initKinect();		// SLB Kinect
	
	QHBoxLayout* line1 = new QHBoxLayout;
	labelUsername = new QLabel(tr("Subject name:"));
	/*lineUsername = new QLineEdit;
	lineUsername->setText(QString::fromStdString(username));
	labelUsername->setBuddy(lineUsername);*/
	files = list_files(outputFilesRoot);
	Sleep(100);
	cusername = new QComboBox();
	cusername->setMaximumWidth(177);
	for (int i=0; i<files.size(); i++) {
		//
		string temp = files[i].substr(2);
		///if (temp == ".txt")
		///	temp = "_.txt";
		temp = temp.substr(0, temp.length()-4); // removes ".txt" from the name string
		//
		//string temp = files[i];
		//
		//cusername->addItem(temp.c_str());
		cusername->addItem(QString::fromStdString(temp));
	}
	cusername->setEditable(true);
	connect(cusername, SIGNAL(editTextChanged(const QString &)), this, SLOT(configChanged()));
	//line1->addWidget(labelUsername);
	//line1->addWidget(cusername);
	// SLB
	QGridLayout* gridUsername = new QGridLayout;
	gridUsername->addWidget(labelUsername, 0, 0);
	gridUsername->addWidget(cusername, 0, 1);
	gridUsername->setColumnStretch(0, 50);
	gridUsername->setColumnStretch(1, 50);
	line1->addLayout(gridUsername);
	//
	
	// SLB
	QHBoxLayout* lineSkillLevel = new QHBoxLayout;
	labelSkill = new QLabel(tr("Skill level:"));
	skillSlider = new QSlider(Qt::Horizontal);
	//skillSlider = new MySlider(Qt::Horizontal);
	skillSlider->setEnabled(tobiienabled);
	//skillSlider->setTickPosition(QSlider::TicksBothSides);
	//skillSlider->setMinimum(0);
	//skillSlider->setMaximum(10);
	///skillSlider->setRange(0, 100);
	skillSlider->setRange(0, 10);
	///skillSlider->setTickInterval(10);
	skillSlider->setTickInterval(1);
	//skillSlider->setSliderPosition((bool)tobiienabled ? skillSliderValue : 100);
	///skillSlider->setSliderPosition(skillSliderValue);
	skillSlider->setSliderPosition(skillSliderValue/10);
	skillSlider->setTickPosition(QSlider::TicksBelow);
	skillSlider->setMinimumWidth(80);
	//skillSlider->setMaximumWidth(200);
	//skillSlider->setEnabled(false);
	QGridLayout* gridSkill = new QGridLayout;
	gridSkill->addWidget(labelSkill, 0, 0);
	gridSkill->addWidget(skillSlider, 0, 1);
	gridSkill->setColumnStretch(0, 50);
	gridSkill->setColumnStretch(1, 50);
	//connect(skillSlider, SIGNAL(valueChanged(int)), this, SLOT(getSkillSliderValue(int)));
	connect(skillSlider, SIGNAL(valueChanged(int)), this, SLOT(configChanged()));
	lineSkillLevel->addLayout(gridSkill);

	QHBoxLayout* line2 = new QHBoxLayout;
	labelModalita = new QLabel(tr("Flashing modality:"));
	comboModalita = new QComboBox;
	comboModalita->addItem("Single square");
	comboModalita->addItem("Rows-columns");
	comboModalita->setCurrentIndex(mode);
	line2->addWidget(labelModalita);
	line2->addWidget(comboModalita);

	QHBoxLayout* line3 = new QHBoxLayout;
	labelInterface = new QLabel(tr("Interface dimension:"));
	comboInterfaccia = new QComboBox;
	comboInterfaccia->addItem("2x2");
	comboInterfaccia->addItem("3x3");
	comboInterfaccia->addItem("4x4");
	comboInterfaccia->addItem("5x5");
	comboInterfaccia->addItem("6x6");
	// SLB gui starts with the correct matrix size if last time symbolic mode was used
	if (symbolicMode) {
		comboInterfaccia->setCurrentIndex(2);
		QVariant v(0);
		comboInterfaccia->setItemData(0,v,Qt::UserRole-1);
		comboInterfaccia->setItemData(1,v,Qt::UserRole-1);
		comboInterfaccia->setItemData(3,v,Qt::UserRole-1);
		comboInterfaccia->setItemData(4,v,Qt::UserRole-1);
	}
	else
		comboInterfaccia->setCurrentIndex(number_of_row_interface-2);
	//
	line3->addWidget(labelInterface);
	line3->addWidget(comboInterfaccia);

	QHBoxLayout* line4 = new QHBoxLayout;
	labelNumCalibTag = new QLabel(tr("Calibration target number:"));
	lineNumCalibTag = new QLineEdit;
	lineNumCalibTag->setValidator(new QIntValidator(1, 7));
	lineNumCalibTag->setAlignment(Qt::AlignRight);
	lineNumCalibTag->setText(QString::number(number_of_calibration_tag));
	/*
	line4->addWidget(labelNumCalibTag);
	line4->addStretch(true); // SLB
	line4->addWidget(lineNumCalibTag);
	*/
	// SLB
	QGridLayout* gridNumCalibTag = new QGridLayout;
	gridNumCalibTag->addWidget(labelNumCalibTag, 0, 0);
	gridNumCalibTag->addWidget(lineNumCalibTag, 0, 1);
	gridNumCalibTag->setColumnStretch(0, 50);
	gridNumCalibTag->setColumnStretch(1, 50);
	line4->addLayout(gridNumCalibTag);

	QHBoxLayout* line5 = new QHBoxLayout;	
	labelTarget = new QLabel(tr("Target(s):"));
	for (int i=0; i<number_of_calibration_tag; i++) {
		comboTarget.push_back(new QComboBox);
		for (int j=0;j<number_of_tags;j++) {
			//string s = std::to_string(j+1);
			//comboTarget[i]->addItem(s.c_str()); 
			comboTarget[i]->addItem(QString::number(j+1)); 
		}
		comboTarget[i]->setCurrentIndex(target_of_calibration[i]-1);
		connect(comboTarget[i], SIGNAL(currentIndexChanged(int)), this, SLOT(configChanged()));
	}
	line5->addWidget(labelTarget);
	targetLayout = new QHBoxLayout;
	for (int i=0; i<comboTarget.size(); i++)
		targetLayout->addWidget(comboTarget[i]);
	line5->addLayout(targetLayout);

	QHBoxLayout* line6 = new QHBoxLayout;
	labelFlash = new QLabel(tr("Flash number:"));
	lineFlash = new QLineEdit;
	lineFlash->setAlignment(Qt::AlignRight);
	lineFlash->setText(QString::number(number_of_flash));
	connect(lineFlash, SIGNAL(textChanged(const QString &)), this, SLOT(changeLineFlash()));
	connect(lineFlash, SIGNAL(textChanged(const QString &)), this, SLOT(configChanged()));
	//lineFlash->setMinimumWidth(200); // SLB
	//labelFlash->setBuddy(lineFlash);
	/*
	line6->addWidget(labelFlash);
	line6->addStretch(true); // SLB
	line6->addWidget(lineFlash);
	*/
	// SLB
	checkFlashOpt = new QCheckBox(tr("&Optimise"));
	checkFlashOpt->setMinimumWidth(85);
	checkFlashOpt->setLayoutDirection(Qt::RightToLeft);
	checkFlashOpt->setChecked(optFlash);
	checkFlashOpt->setToolTip(optFlash ? "Disable optimisation of flashes" : "Enable optimisation of flashes");
    connect(checkFlashOpt,SIGNAL(clicked(bool)),this,SLOT(clickedOpt(bool)));
	connect(checkFlashOpt,SIGNAL(clicked(bool)),this,SLOT(configChanged()));
	QHBoxLayout* line6b = new QHBoxLayout;
	line6b->addWidget(lineFlash);
	line6b->addWidget(checkFlashOpt); // SLB flash optimization optflash

	QGridLayout* gridFlash = new QGridLayout;
	gridFlash->addWidget(labelFlash, 0, 0);
	//gridFlash->addWidget(lineFlash, 0, 1);
	gridFlash->addLayout(line6b, 0, 1);
	//gridFlash->addWidget(checkFlashOpt, 0, 2);
	gridFlash->setColumnStretch(0, 50);
	gridFlash->setColumnStretch(1, 50);
	//gridFlash->setColumnStretch(2, 20);
	line6->addLayout(gridFlash);

	// SLB
	QHBoxLayout* lineFlashTimeBox = new QHBoxLayout;
	labelFlashTime = new QLabel(tr("Flash duration:"));
	lineFlashTime = new QLineEdit;
	lineFlashTime->setValidator(new QIntValidator(100, 200));
	lineFlashTime->setEnabled(!time_flash_auto);
	lineFlashTime->setAlignment(Qt::AlignRight);
	lineFlashTime->setText(QString::number(velocita));
	//connect(lineFlashTime, SIGNAL(textChanged(const QString &)), this, SLOT(changeLineFlashTime()));
	connect(lineFlashTime, SIGNAL(textChanged(const QString &)), this, SLOT(configChanged()));
	checkFlashTimeAuto = new QCheckBox(tr("&Auto"));
	checkFlashTimeAuto->setMinimumWidth(85);
	checkFlashTimeAuto->setLayoutDirection(Qt::RightToLeft);
	checkFlashTimeAuto->setChecked(time_flash_auto);
	checkFlashTimeAuto->setToolTip(time_flash_auto ? "Disable automatic duration of flashes" : "Enable automatic duration of flashes");
    connect(checkFlashTimeAuto,SIGNAL(clicked(bool)),this,SLOT(clickedFlashTimeAuto(bool)));
	connect(checkFlashTimeAuto,SIGNAL(clicked(bool)),this,SLOT(configChanged()));
	QHBoxLayout* lineFT = new QHBoxLayout;
	lineFT->addWidget(lineFlashTime);
	lineFT->addWidget(checkFlashTimeAuto);

	QGridLayout* gridFlashTime = new QGridLayout;
	gridFlashTime->addWidget(labelFlashTime, 0, 0);
	gridFlashTime->addLayout(lineFT, 0, 1);
	gridFlashTime->setColumnStretch(0, 50);
	gridFlashTime->setColumnStretch(1, 50);
	lineFlashTimeBox->addLayout(gridFlashTime);
	//

	// SLB
	QHBoxLayout* linechannels = new QHBoxLayout;
	labelChan = new QLabel(tr("Channels number:"));
	lineChan = new QLineEdit;
	lineChan->setValidator(new QIntValidator(1, 16));
	lineChan->setAlignment(Qt::AlignRight);
	lineChan->setText(QString::number(number_channels));
	lineChan->setToolTip(tr("ERP channels"));
	connect(lineChan, SIGNAL(textChanged(const QString &)), this, SLOT(changeChannel1()));
	connect(lineChan, SIGNAL(textChanged(const QString &)), this, SLOT(configChanged()));
	QLabel *chanPlus = new QLabel(tr(","));
	//labelChan2 = new QLabel(tr("Number of additional channels:"));
	lineChan2 = new QLineEdit;
	lineChan2->setValidator(new QIntValidator(0, 15));
	lineChan2->setAlignment(Qt::AlignRight);
	lineChan2->setText(QString::number(number_channels_plus));
	lineChan2->setToolTip(tr("Additional channels"));
	connect(lineChan2, SIGNAL(textChanged(const QString &)), this, SLOT(changeChannel2()));
	connect(lineChan2, SIGNAL(textChanged(const QString &)), this, SLOT(configChanged()));
	QHBoxLayout *lineChanBox = new QHBoxLayout();
	lineChanBox->addWidget(lineChan);
	lineChanBox->addWidget(chanPlus);
	lineChanBox->addWidget(lineChan2);
	QGridLayout* gridChannels = new QGridLayout;
	gridChannels->addWidget(labelChan, 0, 0);
	gridChannels->addLayout(lineChanBox, 0, 1);
	gridChannels->setColumnStretch(0, 50);
	gridChannels->setColumnStretch(1, 50);
	linechannels->addLayout(gridChannels);

	// SLB Utilisation modes groupbox
	QHBoxLayout* lineUtilMode = new QHBoxLayout;
	//labelUtilMode = new QLabel(tr("Mode of use:"));
	QGroupBox *groupBoxUtilMode = new QGroupBox(tr("Preset mode:"));
	radioBasic = new QRadioButton(tr("&Basic"));
	radioBasic->setToolTip("Basic-Mode: EEG");
    connect(radioBasic,SIGNAL(clicked(bool)),this,SLOT(clickedUtilMode(bool)));
	connect(radioBasic,SIGNAL(clicked(bool)),this,SLOT(configChanged()));
    radioBasic->setAutoExclusive(true);

    radioHybrid = new QRadioButton(tr("&Hybrid"));
	radioHybrid->setToolTip("Hybrid-Mode: EEG+EyeTrack");
    connect(radioHybrid,SIGNAL(clicked(bool)),this,SLOT(clickedUtilMode(bool)));
	connect(radioHybrid,SIGNAL(clicked(bool)),this,SLOT(configChanged()));
    radioHybrid->setAutoExclusive(true);
    
	radioBioHybrid = new QRadioButton(tr("&Bio-Hybrid"));
	radioBioHybrid->setToolTip("BioHybrid-Mode: EEG+EyeTrack+BioFeatures");
    connect(radioBioHybrid,SIGNAL(clicked(bool)),this,SLOT(clickedUtilMode(bool)));
	connect(radioBioHybrid,SIGNAL(clicked(bool)),this,SLOT(configChanged()));
    radioBioHybrid->setAutoExclusive(true);

	radioBasic->setChecked(!tobiienabled && !bioHybridMode);
	radioHybrid->setChecked(!bioHybridMode && !radioBasic->isChecked());
	//radioHybrid->setChecked(!bioHybridMode);
	radioBioHybrid->setChecked(bioHybridMode);
	
	QHBoxLayout *vboxUtilMode = new QHBoxLayout;
	//vbox->addStretch(true); // SLB
	vboxUtilMode->addStretch(true);
    vboxUtilMode->addWidget(radioBasic);
    vboxUtilMode->addStretch(true);
    vboxUtilMode->addWidget(radioHybrid);
    vboxUtilMode->addStretch(true);
	vboxUtilMode->addWidget(radioBioHybrid);
	vboxUtilMode->addStretch(true);

	groupBoxUtilMode->setLayout(vboxUtilMode);
	lineUtilMode->addWidget(groupBoxUtilMode);
	//

	QHBoxLayout* line7 = new QHBoxLayout;
	//labelSensor = new QLabel(tr("Modality of interface:")); // SLB
	QGroupBox *groupBox = new QGroupBox(tr("Interface modality:")); // SLB added title
	groupBox->setFixedHeight(76);
    radioAlphabetic = new QRadioButton(tr("&Alphabetic")); // SLB Alphabet
	radioAlphabetic->setToolTip("Interface with alpha-numeric symbols");
    connect(radioAlphabetic,SIGNAL(clicked(bool)),this,SLOT(clickedstate(bool)));
	//connect(radioAlphabetic,SIGNAL(clicked(bool)),this,SLOT(enableokButton())); // SLB
	connect(radioAlphabetic,SIGNAL(clicked(bool)),this,SLOT(configChanged()));
    radioAlphabetic->setAutoExclusive(true);
    
	radioSymbolic = new QRadioButton(tr("&Symbolic")); // SLB Sensor module
	radioSymbolic->setToolTip("Interface with symbolic icons");
    connect(radioSymbolic,SIGNAL(clicked(bool)),this,SLOT(clickedstate(bool)));
	//connect(radioSymbolic,SIGNAL(clicked(bool)),this,SLOT(enableokButton())); // SLB
	connect(radioSymbolic,SIGNAL(clicked(bool)),this,SLOT(configChanged()));
    radioSymbolic->setAutoExclusive(true);
	
	/*
	//radioCustomPalette = new QRadioButton(tr("&Visually impaired")); // SLB
	//radioCustomPalette->setToolTip("Interface with high contrast symbols");
	radioCustomPalette = new QRadioButton(tr("&Custom palette")); // SLB
	radioCustomPalette->setToolTip("Interface with custom colours palette and symbols");
    connect(radioCustomPalette,SIGNAL(clicked(bool)),this,SLOT(clickedstate(bool)));
	//connect(radioCustomPalette,SIGNAL(clicked(bool)),this,SLOT(enableokButton())); // SLB
    radioCustomPalette->setAutoExclusive(true);
	*/
	checkCustomPalette = new QCheckBox(tr("&Custom palette")); // SLB
	//checkCustomPalette->setEnabled(false);
	checkCustomPalette->setToolTip("Interface with custom colours palette and symbols");
    connect(checkCustomPalette,SIGNAL(clicked(bool)),this,SLOT(clickedstate(bool)));
	//connect(checkCustomPalette,SIGNAL(clicked(bool)),this,SLOT(enableokButton())); // SLB
	connect(checkCustomPalette,SIGNAL(clicked(bool)),this,SLOT(configChanged()));

	connect(radioAlphabetic, SIGNAL(clicked(bool)), this, SLOT(changePaletteString())); // SLB
	connect(radioSymbolic, SIGNAL(clicked(bool)), this, SLOT(changePaletteString())); // SLB
	connect(checkCustomPalette, SIGNAL(clicked(bool)), this, SLOT(changePaletteString())); // SLB
    
	radioAlphabetic->setChecked(!symbolicMode);
	radioSymbolic->setChecked(symbolicMode);
	//radioCustomPalette->setChecked(customPalette); // SLB
	checkCustomPalette->setEnabled(!symbolicMode);
	checkCustomPalette->setChecked(customPalette);

	// SLB
	textures = list_folders(texturePathRoot);
	Sleep(100);
	ctextures = new QComboBox;
	if (!textures.empty()) {
		ctextures->setEnabled(customPalette);
		ctextures->setEditable(false);
		int impaired_index = 0;
		for (int i=0; i<textures.size(); i++) {
			//ctextures->addItem(textures[i].c_str());
			ctextures->addItem(QString::fromStdString(textures[i]));
			if (textures[i] == impaired_theme)
				impaired_index = i;
		}
		ctextures->setCurrentIndex(impaired_index);
		ctextures->setToolTip("Select interface theme");
		connect(ctextures, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTextureCombo()));
		connect(ctextures, SIGNAL(currentIndexChanged(int)), this, SLOT(configChanged()));
	}

	// SLB
	linePaletteString = new QLineEdit;
	linePaletteString->setEnabled(customPalette);
	linePaletteString->setAlignment(Qt::AlignHCenter);
	linePaletteString->setText(palette_string.c_str());
	linePaletteString->setToolTip("Matrix elements string (maximum 36 elements)\nThe string must contain only letters and numbers\nLeave empty to use the default alphabetic string");
	paletteStringCounter = new QLabel;
	//paletteStringCounter->setEnabled(false);
	paletteStringCounter->setToolTip("Custom string length\nGreen:\tString length matches the matrix size\nBlack:\tString provides more characters than needed, will be truncated\nRed:\tString has no sufficient number of elements for the chosen matrix size, will be filled by default characters");
	changePaletteString();
	//paletteStringCounter->setText(std::to_string(linePaletteString->text().size()).c_str());
	connect(linePaletteString, SIGNAL(textChanged(QString)), this, SLOT(changePaletteString()));
	connect(linePaletteString, SIGNAL(textChanged(QString)), this, SLOT(configChanged()));

	QHBoxLayout *vbox = new QHBoxLayout;
	//vbox->addStretch(true); // SLB
    vbox->addWidget(radioAlphabetic);
    vbox->addStretch(true);
	vbox->addWidget(radioSymbolic);
	//vbox->addStretch(true);
	QHBoxLayout *vbox2 = new QHBoxLayout;
	if (!textures.empty()) { // SLB
		//vbox2->addWidget(radioCustomPalette);
		vbox2->addWidget(checkCustomPalette);
		vbox2->addWidget(ctextures);
		vbox2->addWidget(linePaletteString);
		vbox2->addWidget(paletteStringCounter);
	}
	else
		customPalette = false;
    
	QVBoxLayout *vvbox = new QVBoxLayout;
	vvbox->addLayout(vbox);
	vvbox->addLayout(vbox2);
	//vvbox->addWidget(linePaletteString);
    groupBox->setLayout(vvbox);
	//line7->addWidget(labelSensor); // SLB
	line7->addWidget(groupBox);

	// SLB
	QGroupBox *sensorBox = new QGroupBox(tr("Devices:")); // SLB added title
	QVBoxLayout *sensorLayout = new QVBoxLayout;
	sensorLayout->setAlignment(Qt::AlignTop);
	QHBoxLayout *eyeTrackerLine = new QHBoxLayout;
	labelEyeTracker = new QLabel(tr("Eye Tracking:"));
	eyeTrackerCheckBox = new QCheckBox;
	eyeTrackerCheckBox->setChecked(tobiienabled);
	//eyeTrackerCheckBox->setLayoutDirection(Qt::RightToLeft);
	//(tobiienabled==1) ? eyeTrackerCheckBox->setText("&Enabled") : eyeTrackerCheckBox->setText("&Disabled");
	eyeTrackerCheckBox->setText("&Eye Tracker");
	//(tobiienabled) ? eyeTrackerCheckBox->setToolTip("Click to disable") : eyeTrackerCheckBox->setToolTip("Click to enable");
	eyeTrackerCheckBox->setToolTip(tobiienabled ? "Click to disable" : "Click to enable");
	eyeTrackerGazeCheckBox = new QCheckBox(tr("&Show gaze"));
	eyeTrackerGazeCheckBox->setEnabled(tobiienabled);
	eyeTrackerGazeCheckBox->setChecked(tobiigazeenabled);
	tobiiFilteredRadio = new QRadioButton(tr("&Filtered data"));
	tobiiFilteredRadio->setEnabled(tobiienabled);
	tobiiFilteredRadio->setChecked(tobiifiltered);
	tobiiFilteredRadio->setToolTip("Filtered Eye tracker coordinates");
    connect(tobiiFilteredRadio,SIGNAL(clicked(bool)),this,SLOT(clickedtobiifilter(bool)));
	connect(tobiiFilteredRadio,SIGNAL(clicked(bool)),this,SLOT(configChanged()));
	tobiiFilteredRadio->setAutoExclusive(true);
	tobiiRawRadio = new QRadioButton(tr("&Raw data"));
	tobiiRawRadio->setEnabled(tobiienabled);
	tobiiRawRadio->setChecked(!tobiifiltered);
	tobiiRawRadio->setToolTip("Raw Eye tracker coordinates");
    connect(tobiiRawRadio,SIGNAL(clicked(bool)),this,SLOT(clickedtobiifilter(bool)));
	connect(tobiiRawRadio,SIGNAL(clicked(bool)),this,SLOT(configChanged()));
	tobiiRawRadio->setAutoExclusive(true);
	//eyeTrackerLine->addWidget(labelEyeTracker);
	// SLB test icon
	QPixmap iconEye(EYEICONPATH);
	//iconEyeLabel = new QLabel();
	//iconEyeLabel->setPixmap(iconEye);
	iconEyeLabel = new QPushButton();
	iconEyeLabel->setFlat(true);
	iconEyeLabel->setIcon(iconEye);
	iconEyeLabel->setIconSize(iconEye.rect().size());
	iconEyeLabel->setFixedSize(iconEye.rect().size());
	iconEyeLabel->setEnabled(tobiienabled);
	iconEyeLabel->setToolTip("Eye Tracking priority");
	connect(iconEyeLabel, SIGNAL(clicked()), this, SLOT(eyeIconClicked()));
	connect(iconEyeLabel, SIGNAL(clicked()), this, SLOT(configChanged()));
	QPixmap iconBrain(BRAINICONPATH);
	//iconBrainLabel = new QLabel();
	//iconBrainLabel->setPixmap(iconBrain);
	iconBrainLabel = new QPushButton();
	iconBrainLabel->setFlat(true);
	iconBrainLabel->setIcon(iconBrain);
	iconBrainLabel->setIconSize(iconBrain.rect().size());
	iconBrainLabel->setFixedSize(iconBrain.rect().size());
	iconBrainLabel->setEnabled(tobiienabled);
	iconBrainLabel->setToolTip("Classifier priority");
	connect(iconBrainLabel, SIGNAL(clicked()), this, SLOT(brainIconClicked()));
	connect(iconBrainLabel, SIGNAL(clicked()), this, SLOT(configChanged()));
	//
	eyeTrackerLine->addWidget(eyeTrackerCheckBox);
	eyeTrackerLine->addWidget(iconEyeLabel);
	eyeTrackerLine->addWidget(skillSlider);
	eyeTrackerLine->addWidget(iconBrainLabel);
	eyeTrackerLine->addStretch(true);
	eyeTrackerLine->addWidget(eyeTrackerGazeCheckBox);
	eyeTrackerLine->addWidget(tobiiFilteredRadio);
	eyeTrackerLine->addWidget(tobiiRawRadio);

	//sensorLayout->addWidget(eyeTrackerCheckBox);
	sensorLayout->addLayout(eyeTrackerLine);
	//sensorLayout->addLayout(lineSkillLevel);
	sensorBox->setLayout(sensorLayout);
	sensorBox->setFixedHeight(sensorLayout->sizeHint().height()+11);

	connect(eyeTrackerCheckBox,SIGNAL(clicked(bool)),this,SLOT(eyeTrackerChecked(bool)));
	connect(eyeTrackerCheckBox,SIGNAL(clicked(bool)),this,SLOT(configChanged()));
	connect(eyeTrackerGazeCheckBox,SIGNAL(clicked(bool)),this,SLOT(eyeTrackerChecked(bool)));
	connect(eyeTrackerGazeCheckBox,SIGNAL(clicked(bool)),this,SLOT(configChanged()));

	QHBoxLayout *line8 = new QHBoxLayout;
	//labelRobot = new QLabel(tr("IP/Port output:"));
	labelRobot = new QLabel(tr("Robot connection:")); // SLB
	labelRobot->setToolTip("Robot IP and Port configuration");
	lineIpRobot = new QLineEdit;
	lineIpRobot->setAlignment(Qt::AlignCenter); // SLB
	lineIpRobot->setText(QString::fromStdString(ipRobot));
	lineIpRobot->setToolTip("Robot IP configuration");
	connect(lineIpRobot, SIGNAL(textChanged(const QString &)), this, SLOT(configChanged()));
	//lineIpRobot->setInputMask("000.000.000.000;_");
	linePortRobot = new QLineEdit;
	linePortRobot->setAlignment(Qt::AlignCenter); // SLB
	linePortRobot->setText(QString::fromStdString(portRobot));
	linePortRobot->setToolTip("Robot Port configuration");
	linePortRobot->setMaximumWidth(60); // SLB
	connect(linePortRobot, SIGNAL(textChanged(const QString &)), this, SLOT(configChanged()));

	//line8->addWidget(labelRobot);
	//line8->addWidget(lineIpRobot);
	//line8->addWidget(linePortRobot);

	QHBoxLayout* line8b = new QHBoxLayout;
	QLabel *labelColon = new QLabel(tr(":")); // SLB
	line8b->addWidget(lineIpRobot);
	line8b->addWidget(labelColon);
	line8b->addWidget(linePortRobot);

	QGridLayout* gridRobot = new QGridLayout;
	gridRobot->addWidget(labelRobot, 0, 0);
	gridRobot->addLayout(line8b, 0, 1);
	gridRobot->setColumnStretch(0, 50);
	gridRobot->setColumnStretch(1, 50);
	line8->addLayout(gridRobot);

	QHBoxLayout* line10 = new QHBoxLayout;
	okButton = new QPushButton(tr("&Apply"));
	okButton->setStyleSheet("QPushButton { font: bold; color: rgba(184, 38, 25, 100%); }");
	okButton->setDefault(true);
	//okButton->setEnabled(false);
	okButton->setEnabled(true); // SLB
	okButton->setMinimumHeight(26); // SLB
	//resetButton = new QPushButton(tr("&Reset")); // SLB TODO
	//resetButton->setMinimumHeight(26); // SLB
	closeButton = new QPushButton(tr("E&xit"));
	closeButton->setStyleSheet("QPushButton { font: bold; }");
	closeButton->setEnabled(false); // SLB
	closeButton->setMinimumHeight(26); // SLB
	line10->addWidget(okButton);
	//line10->addWidget(resetButton); // SLB
	line10->addWidget(closeButton);

	//connect(lineUsername, SIGNAL(textChanged(const QString &)),this, SLOT(enableokButton()));
	connect(comboModalita, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTagComboTarget()));
	connect(comboModalita, SIGNAL(currentIndexChanged(int)), this, SLOT(configChanged()));
	connect(comboInterfaccia, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTagComboTarget()));
	connect(comboInterfaccia, SIGNAL(currentIndexChanged(int)), this, SLOT(configChanged()));
	connect(lineNumCalibTag, SIGNAL(textChanged(const QString &)), this, SLOT(changeComboTarget()));
	connect(lineNumCalibTag, SIGNAL(textChanged(const QString &)), this, SLOT(configChanged()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
	//connect(resetButton, SIGNAL(clicked()), this, SLOT(resetClicked())); // SLB TODO
	//connect(closeButton, SIGNAL(clicked()), this, SLOT(close())); // SLB
	connect(closeButton, SIGNAL(clicked()), this, SLOT(closeClicked())); // SLB

	//connect(okButton, SIGNAL(clicked(bool)), this, SLOT(changePaletteString())); // SLB
	connect(comboInterfaccia, SIGNAL(currentIndexChanged(int)), this, SLOT(changePaletteString())); // SLB

	QGroupBox *groupBoxTasti = new QGroupBox;
	// SLB
	signalCheckButton = new QPushButton(tr("CHECK\nSIGNALS"));
	signalCheckButton->setEnabled(false);
	signalCheckButton->setCheckable(true);
	//signalCheck->setMaximumWidth(109);
	//
	baselineAcquisitionButton = new QPushButton(tr("BASELINE\nCALIBRATION"));
	baselineAcquisitionButton->setEnabled(false);
	//
	gameSessionButton = new QPushButton(tr("GAME\nSESSION"));
	gameSessionButton->setEnabled(false);
	//
	calibration = new QPushButton(tr("P300\nCALIBRATION")); // START
	calibration->setEnabled(false);
	speller = new QPushButton(tr("ONLINE\nRECOGNITION")); // START
	speller->setEnabled(false);
	spellerFile = new QPushButton(tr("FILE\nRECOGNITION")); // "START RECOGNITION\nFROM FILE"
	spellerFile->setEnabled(false);

	flashAnalysis = new QPushButton(tr("FLASH\nANALYSIS"));

	QHBoxLayout *calibBox = new QHBoxLayout;
	calibBox->addWidget(signalCheckButton);			// SLB
	calibBox->addWidget(gameSessionButton);		// SLB
	calibBox->addWidget(baselineAcquisitionButton);	// SLB
	///calibBox->addWidget(calibration);
	//calibBox->addWidget(speller);
	///calibBox->addWidget(flashAnalysis);
	//calibBox->addWidget(spellerFile);

	QHBoxLayout *spellBox = new QHBoxLayout;
	//spellBox->addWidget(baselineAcquisition); // SLB
	spellBox->addWidget(calibration);
	spellBox->addWidget(speller);
	///spellBox->addWidget(flashAnalysis);
	spellBox->addWidget(spellerFile);

	QVBoxLayout *calibSpellBox = new QVBoxLayout;
	calibSpellBox->addLayout(calibBox);
	calibSpellBox->addLayout(spellBox);
	groupBoxTasti->setLayout(calibSpellBox);

	//groupBoxTasti->setLayout(calibBox);
	QHBoxLayout *line11 = new QHBoxLayout;
	line11->addWidget(groupBoxTasti);
	
	connect(signalCheckButton, SIGNAL(clicked(bool)), this, SLOT(clickedSignalCheckButton()));					// SLB
	connect(gameSessionButton, SIGNAL(clicked(bool)), this, SLOT(clickedGameSessionButton()));				// SLB
	connect(baselineAcquisitionButton, SIGNAL(clicked(bool)), this, SLOT(clickedBaselineAcquisitionButton()));	// SLB
	connect(calibration, SIGNAL(clicked(bool)), this, SLOT(clickedCalibrationButton()));
	connect(speller, SIGNAL(clicked(bool)), this, SLOT(clickedSpellerButton()));
	connect(spellerFile, SIGNAL(clicked(bool)), this, SLOT(clickedSpellerFileButton()));

	connect(flashAnalysis, SIGNAL(clicked(bool)), this, SLOT(clickedFlashAnalysisButton()));

	QHBoxLayout *line12 = new QHBoxLayout;
	textarea = new QTextEdit();
	//textarea->setReadOnly(false); // SLB
	textarea->setReadOnly(true);
	QScrollBar *sb = textarea->verticalScrollBar();
	//sb->setValue(sb->minimum());
	line12->addWidget(textarea);
	line12->addWidget(sb);
	// SLB index sliders ///
	QHBoxLayout *lineFeatSliders = new QHBoxLayout;
	index1Slider = new QSlider(Qt::Vertical);
	index1Slider->setEnabled(false);
	index1Slider->setMaximumWidth(15);
	index1Slider->setRange(0, 100);
	//index1Slider->setStyleSheet("QSlider::handle:vertical { background: #802020; border-radius: 5px; }");
	index1Slider->setStyleSheet("QSlider::handle:vertical { background: #00A88C; border-radius: 5px; border: 1px solid #555; }"); // 802020
	index2Slider = new QSlider(Qt::Vertical);
	index2Slider->setEnabled(false);
	index2Slider->setMaximumWidth(15);
	index2Slider->setRange(0, 100);
	//index2Slider->setStyleSheet("QSlider::handle:vertical { background: #208020; border-radius: 5px; }");
	index2Slider->setStyleSheet("QSlider::handle:vertical { background: #FAAA14; border-radius: 5px; border: 1px solid #555; }"); // 208020
	index3Slider = new QSlider(Qt::Vertical);
	index3Slider->setEnabled(false);
	index3Slider->setMaximumWidth(15);
	index3Slider->setRange(0, 100);
	//index3Slider->setStyleSheet("QSlider::handle:vertical { background: #202080; border-radius: 5px; }");
	index3Slider->setStyleSheet("QSlider::handle:vertical { background: #6F09B0; border-radius: 5px; border: 1px solid #555; }"); // 202080
	//lineFeatSliders->addStretch(true);
	lineFeatSliders->addWidget(index1Slider);
	lineFeatSliders->addWidget(index2Slider);
	lineFeatSliders->addWidget(index3Slider);
	//lineFeatSliders->addStretch(true);
	//line12->addStretch(true);
	line12->addLayout(lineFeatSliders);

	// SLB test plot ----------------------------------------
	QLabel *graphWidget = new QLabel();
	//graphWidget->setStyleSheet("QLabel { border-radius: 5px; border: 1px solid #555; }");
	customPlot = new QCustomPlot(graphWidget);
	customPlot->setEnabled(bioHybridMode);
	//customPlot->setMinimumHeight(180); // 200
	customPlot->setFixedHeight(180); // 200
	customPlot->plotLayout()->insertRow(0);
	QFont titleFont = font();
	titleFont.setPointSize(9);
	//titleFont.setStyleStrategy(QFont::PreferAntialias);
	//titleFont.setWeight(75); // bold
	//titleFont.setBold(true);
	QCPTextElement *title = new QCPTextElement(customPlot, "Behaviour Intensity", titleFont); // QFont("sans", 10, QFont::Bold));
	title->setTextColor(Qt::white);
	customPlot->plotLayout()->addElement(0, 0, title);
	
	// set dark background gradient:
	QLinearGradient gradient(0, 0, 0, 400);
	/*
	gradient.setColorAt(1, QColor(70, 70, 70)); // 70
	gradient.setColorAt(0.38, QColor(105, 105, 105)); // 105
	gradient.setColorAt(0, QColor(90, 90, 90)); // 90
	*/
	gradient.setColorAt(1, QColor(10, 10, 10));
	gradient.setColorAt(0.45, QColor(80, 80, 80));
	gradient.setColorAt(0, QColor(20, 20, 20));
	
	if (bioHybridMode)
		customPlot->setBackground(QBrush(gradient));
	else
		customPlot->setBackground(QBrush(QColor(200, 200, 200)));
	
	// create empty bar chart objects:
	rsquare = new QCPBars(customPlot->xAxis, customPlot->yAxis);
	rsquare->setWidth(0.5);
	rsquare->setAntialiased(false); // gives more crisp, pixel aligned bar borders
	rsquare->setStackingGap(1);
	rsquare->setName("Intention"); // ("R²");
	rsquare->setPen(QPen(QColor(0, 168, 140).lighter(130)));
	rsquare->setBrush(QColor(0, 168, 140));
	//rsquare->setPen(QPen(QColor(160, 32, 32).darker(120)));
	//rsquare->setBrush(QColor(160, 64, 64));
	
	epy = new QCPBars(customPlot->xAxis, customPlot->yAxis);
	epy->setWidth(0.5);
	epy->setAntialiased(false);
	epy->setStackingGap(1);
	epy->setName("Focus"); // ("Entropy");
	epy->setPen(QPen(QColor(250, 170, 20).lighter(150)));
	epy->setBrush(QColor(250, 170, 20));
	//epy->setPen(QPen(QColor(32, 160, 32).darker(150)));
	//epy->setBrush(QColor(64, 160, 64));
	
	egy = new QCPBars(customPlot->xAxis, customPlot->yAxis);
	egy->setWidth(0.5);
	egy->setAntialiased(false);
	egy->setStackingGap(1);
	egy->setName("Attention"); // ("Energy");
	egy->setPen(QPen(QColor(111, 9, 176).lighter(170)));
	egy->setBrush(QColor(111, 9, 176));
	//egy->setPen(QPen(QColor(32, 64, 192).darker(120)));
	//egy->setBrush(QColor(64, 128, 192));
	
	// stack bars on top of each other:
	epy->moveAbove(egy);
	rsquare->moveAbove(epy);
	
	// prepare x axis with country labels:
	//QVector<double> ticks;
	//QVector<QString> labels;
	//ticks << 1 << 2 << 3;
	//labels << "1" << "2" << "3";
	//labels << " " << " " << " ";
	//QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText); // SLB moved to global
	textTicker->addTicks(ticks, labels);
	customPlot->xAxis->setTicker(textTicker);
	
	//customPlot->xAxis->setTickLabelRotation(-60);
	customPlot->xAxis->setSubTicks(false);
	customPlot->xAxis->setTickLength(0, 4);
	customPlot->xAxis->setRange(-1, 15);
	customPlot->xAxis->setBasePen(QPen(Qt::white));
	customPlot->xAxis->setTickPen(QPen(Qt::white));
	customPlot->xAxis->grid()->setVisible(true);
	customPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	customPlot->xAxis->setTickLabelColor(Qt::white);
	customPlot->xAxis->setLabelColor(Qt::white);
	
	// prepare y axis:
	customPlot->yAxis->setRange(0, 1.0);
	customPlot->yAxis->setPadding(5); // a bit more space to the left border
	/*
	customPlot->yAxis->setLabel("Behaviour Intensity");
	QFont yAxisFont = font();
	yAxisFont.setPointSize(10);
	customPlot->yAxis->setLabelFont(yAxisFont);
	*/
	customPlot->yAxis->setBasePen(QPen(Qt::white));
	customPlot->yAxis->setTickPen(QPen(Qt::white));
	customPlot->yAxis->setSubTickPen(QPen(Qt::white));
	//customPlot->yAxis->grid()->setSubGridVisible(true);
	customPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	//customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(100, 100, 100), 0, Qt::DashLine)); // 130 // Qt::DotLine
	if (bioHybridMode) {
		customPlot->yAxis->setTickLabelColor(Qt::white);
		customPlot->yAxis->setLabelColor(Qt::white);
	}
	else {
		customPlot->yAxis->setTickLabelColor(Qt::gray);
		customPlot->yAxis->setLabelColor(Qt::gray);
	}
	
	// Add data:
	//QVector<double> rsquareData, epyData, egyData;
	/*
	rsquareData		<< 0.25 << 0.1  << 0.7;
	epyData			<< 0.3  << 0.4  << 0.15;
	egyData			<< 0.35 << 0.24 << 0.1;
	
	rsquare->setData(ticks, rsquareData);
	epy->setData(ticks, epyData);
	egy->setData(ticks, egyData);
	*/
	// setup legend:
	customPlot->legend->setVisible(true);
	//customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignHCenter);
	customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
	customPlot->legend->setBorderPen(Qt::NoPen);
	QFont legendFont = font();
	legendFont.setPointSize(8);
	customPlot->legend->setFont(legendFont);
	if (bioHybridMode) {
		//customPlot->legend->setBrush(QColor(255, 255, 255, 100)); // 100
		customPlot->legend->setBrush(QColor(0, 0, 0, 100));
		customPlot->legend->setTextColor(Qt::white);
	}
	else {
		customPlot->legend->setBrush(QColor(150, 150, 150, 200));
		customPlot->legend->setTextColor(QColor(100, 100, 100));
	}
	/*
	// now we move the legend from the inset layout of the axis rect into the main grid layout.
	// We create a sub layout so we can generate a small gap between the plot layout cell border
	// and the legend border:
	QCPLayoutGrid *subLayout = new QCPLayoutGrid;
	customPlot->plotLayout()->addElement(1, 0, subLayout);
	subLayout->setMargins(QMargins(5, 0, 5, 5));
	subLayout->addElement(0, 0, customPlot->legend);
	// change the fill order of the legend, so it's filled left to right in columns:
	customPlot->legend->setFillOrder(QCPLegend::foColumnsFirst);
	// set legend's row stretch factor very small so it ends up with minimum height:
	customPlot->plotLayout()->setRowStretchFactor(1, 0.001);
	*/
	customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	//customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
	//							QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
	customPlot->setNoAntialiasingOnDrag(true);
	//connect(customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
	// top values:
	/*
	for( int i=0; i<ticks.count(); i++) {
		QCPItemText *textLabel = new QCPItemText(customPlot);
		textLabel->setText(QString::number(ticks[i]));
		textLabel->position->setCoords(i+1, ticks[i]+10);
		customPlot->addItem(textLabel);
	}
	*/
	customPlot->replot();
	// SLB test plot ^ ----------------------------------------
	
	// SLB test plot2 ----------
	QLabel *graphWidget2 = new QLabel();
	customPlot2 = new QCustomPlot(graphWidget2);
	//customPlot2->setMinimumHeight(180);
	//customPlot2->setBackground(QBrush(QColor(232, 252, 232)));
	///customPlot2->setBackground(Qt::transparent);
	QLinearGradient gradient2(0, 0, 0, 400);
	gradient2.setColorAt(1, QColor(182, 212, 182));
	gradient2.setColorAt(0.45, QColor(232, 252, 232));
	gradient2.setColorAt(0, QColor(202, 252, 202));
	customPlot2->setBackground(gradient2);
	for (int c = 0; c < 16; c++) {
		customPlot2->addGraph();
		customPlot2->graph(c)->setPen(QPen(Qt::black, 1.0));
		//customPlot2->graph(c)->setPen(QPen(c));
		QVector<double> temp;
		y.push_back(temp);
	}
	//customPlot2->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
	/*
	for (int i=0; i<251; ++i) {
	  x << i;
	  y << qExp(-i/150.0)*qCos(i/10.0);
	}
	*/
	// configure right and top axis to show ticks but no labels:
	// (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
	customPlot2->xAxis->setVisible(true);
	customPlot2->xAxis->setTickLabels(false);
	customPlot2->xAxis->setSubTicks(false);
	customPlot2->xAxis->setBasePen(QPen(Qt::gray));
	customPlot2->xAxis->setTickPen(QPen(Qt::gray));
	customPlot2->xAxis->grid()->setVisible(true);
	customPlot2->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	customPlot2->xAxis->setTickLabelColor(Qt::gray);
	customPlot2->xAxis->setLabelColor(Qt::gray);
	customPlot2->yAxis->setVisible(true);
	customPlot2->yAxis->setTickLabels(false);
	customPlot2->yAxis->setSubTicks(false);
	customPlot2->yAxis->setPadding(5);
	customPlot2->yAxis->setBasePen(QPen(Qt::gray));
	customPlot2->yAxis->setTickPen(QPen(Qt::gray));
	customPlot2->yAxis->setSubTickPen(QPen(Qt::gray));
	customPlot2->yAxis->grid()->setSubGridVisible(true);
	customPlot2->yAxis->grid()->setPen(QPen(QColor(200, 200, 200), 0, Qt::SolidLine));
	customPlot2->yAxis->grid()->setSubGridPen(QPen(QColor(200, 200, 200), 0, Qt::DashLine)); // 130 // Qt::DotLine
	customPlot2->yAxis->setTickLabelColor(Qt::gray);
	customPlot2->yAxis->setLabelColor(Qt::gray);
	//customPlot2->graph(0)->setData(x, y);
	//customPlot2->graph(0)->rescaleAxes();
	customPlot2->xAxis->setRange(0, 100);
	customPlot2->yAxis->setRange(-750, 250);
	customPlot2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
	customPlot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	customPlot2->replot();
	// SLB test plot2 ^ ------------

	signalPlotGainSlider = new QSlider(Qt::Vertical);
	signalPlotGainSlider->setMaximumWidth(15);
	signalPlotGainSlider->setRange(1, 100);
	signalPlotGainSlider->setValue(1);
	signalPlotGainSlider->setToolTip(tr("Signal Plot Gain"));
	signalPlotGainSlider->setStyleSheet("QSlider::handle:vertical { background: #C8C8C8; border-radius: 5px; border: 1px solid #555; }");
	connect(signalPlotGainSlider, SIGNAL(valueChanged(int)), this, SLOT(getSignalPlotGainValue(int)));

	// SLB Channel status -------------------------------------
	chanStatusBox = new QGroupBox(tr("Channels status:"));
	chanStatusBox->setFixedHeight(58);
	chanStatusLayout = new QHBoxLayout();
	int tot_channels = number_channels + number_channels_plus;
	for (int c = 0; c < tot_channels; c++) {
		chanStatusLabels.push_back(new QLabel(QString::number(c+1)));
		chanStatusLabels[c]->setAlignment(Qt::AlignCenter);
		//chanStatusLabels[c]->setMaximumWidth(25); // 50
		//chanStatusLabels[c]->setMinimumHeight(25); // 20
		chanStatusLabels[c]->setFixedSize(25, 25);
		/* SLB ok flat colour
		string chanStatusStyle = "QLabel { background: ";
		chanStatusStyle += CHSTATUS_GREY;
		chanStatusStyle += "; border-radius: 11px; border: 1px solid #555; }";
		*/
		string chanStatusStyle = "QLabel { background: qradialgradient(cx:0, cy:0, radius:1, fx:0.5, fy:0.5, stop:0 white, stop:1 ";
		chanStatusStyle += CHSTATUS_GREY;
		chanStatusStyle += "); border-radius: 11px; border: 1px solid #555; }";
		chanStatusLabels[c]->setStyleSheet(QString::fromStdString(chanStatusStyle));
		chanStatusLayout->addWidget(chanStatusLabels[c]);
	}
	//
	chanStatusBox->setLayout(chanStatusLayout);
	// SLB Channel status ^ -----------------------------------

	labelFlashAnalysis = new QLabel(tr("Online Analysis:"));
	labelFlashAnalysis->setMinimumWidth(109); //120
	labelFlashAnalysis->setToolTip("Insert index of tag predicted;\nfor example for symbol A insert 1 or for symbol B insert 2");
	lineTagOnlineAnalysis = new QLineEdit;
	lineTagOnlineAnalysis->setFixedWidth(108);
	onlineAnalysis = new QPushButton(tr("Analyse"));
	onlineAnalysis->setMinimumWidth(109);
	lineTagOnlineAnalysis->setEnabled(false); // SLB
	onlineAnalysis->setEnabled(false); // SLB
	QGroupBox* groupBoxAnalysis = new QGroupBox;
	QVBoxLayout *analysisLayout = new QVBoxLayout();
	QHBoxLayout *lineOnlineAnalysis = new QHBoxLayout;
	lineOnlineAnalysis->addWidget(labelFlashAnalysis);
	lineOnlineAnalysis->addWidget(lineTagOnlineAnalysis);
	lineOnlineAnalysis->addWidget(onlineAnalysis);

	//groupBoxAnalysis->setLayout(lineOnlineAnalysis);
	connect(onlineAnalysis, SIGNAL(clicked(bool)), this, SLOT(clickedOnlineFlashAnalysisButton()));
	//QHBoxLayout* line13 = new QHBoxLayout;
	//line13->addWidget(groupBoxAnalysis);

	labelCalibrationAnalysis = new QLabel(tr("Calibration Analysis:"));
	labelCalibrationAnalysis->setMinimumWidth(109); //120
	labelCalibrationAnalysis->setToolTip("Insert sequence of calibrations to start analysis;\nfor example 1-2-3.\n If empty, analysis start over all calibrations");
	lineCalibrationAnalysis = new QLineEdit;
	lineCalibrationAnalysis->setFixedWidth(108);
	startCalibrationAnalysis = new QPushButton(tr("Analyse"));
	startCalibrationAnalysis->setMinimumWidth(109);
	lineCalibrationAnalysis->setEnabled(false); // SLB
	startCalibrationAnalysis->setEnabled(false);
	//QGroupBox* groupBoxCalibrationAnalysis = new QGroupBox;
	QHBoxLayout *lineCalibAnalysis = new QHBoxLayout;
	lineCalibAnalysis->addWidget(labelCalibrationAnalysis);
	lineCalibAnalysis->addWidget(lineCalibrationAnalysis);
	lineCalibAnalysis->addWidget(startCalibrationAnalysis);
	
	//groupBoxCalibrationAnalysis->setLayout(lineCalibAnalysis);
	connect(startCalibrationAnalysis, SIGNAL(clicked(bool)), this, SLOT(clickedStartCalibrationAnalysisButton()));

	analysisLayout->addLayout(lineOnlineAnalysis);
	analysisLayout->addLayout(lineCalibAnalysis);

	groupBoxAnalysis->setLayout(analysisLayout);
	QHBoxLayout* line13 = new QHBoxLayout;
	line13->addWidget(groupBoxAnalysis);

	//QHBoxLayout* line14 = new QHBoxLayout;
	//line14->addWidget(groupBoxCalibrationAnalysis);

	/*
	QHBoxLayout *line15 = new QHBoxLayout;
	QLabel *optimizationFlash = new QLabel(tr("Optimisation of flashes:"));
	//
	QGroupBox *groupBoxOptFlash = new QGroupBox(tr("")); // SLB
	//groupBoxOptFlash->setFlat(true); // SLB
	radioOn = new QRadioButton(tr("&On"));
	radioOn->setToolTip("Enable Optimisation of flashes");
	radioOn->setChecked(optFlash); // SLB
	//connect(radioOn,SIGNAL(clicked(bool)),this,SLOT(enableokButton())); // SLB
	connect(radioOn,SIGNAL(clicked(bool)),this,SLOT(clickedOpt(bool)));
    radioOn->setAutoExclusive(true);
    radioOff = new QRadioButton(tr("&Off"));
	radioOff->setToolTip("Disable Optimisation of flashes");
	radioOff->setChecked(!optFlash);
	// connect(radioOff,SIGNAL(clicked(bool)),this,SLOT(enableokButton())); // SLB
	connect(radioOff,SIGNAL(clicked(bool)),this,SLOT(clickedOpt(bool)));
    radioOff->setAutoExclusive(true);
	*/
	/* SLB moved up to line6
	checkFlashOpt = new QCheckBox(tr("&Optimise"));
	checkFlashOpt->setLayoutDirection(Qt::RightToLeft);
	checkFlashOpt->setToolTip(optFlash ? "Disable optimisation of flashes" : "Enable optimisation of flashes");
    connect(checkFlashOpt,SIGNAL(clicked(bool)),this,SLOT(clickedOpt(bool)));
	checkFlashOpt->setChecked(optFlash);
	//
	line15->addWidget(optimizationFlash);
	line15->addStretch(true); // SLB
	line15->addWidget(checkFlashOpt); // SLB
	*/
	/*
	line15->addWidget(radioOn);
	line15->addWidget(radioOff);
	//groupBoxOptFlash->setLayout(line15);
	*/

	// SLB
	//QPalette palette;
	//QString paletteStyleTop = "background-color: rgba(184, 38, 25, 100%); border-top-left-radius: 6px 10px; border-top-right-radius: 6px 10px;";
	QString paletteStyleTop = "background-color: rgba(184, 38, 25, 100%);";
	//QString paletteStyleTop = "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #B82619, stop: 0.5 #ABAFE5, stop: 1 #B82619);";
	//QString paletteStyleBottom = "background-color: rgba(184, 38, 25, 100%); border-bottom-left-radius: 6px 10px; border-bottom-right-radius: 6px 10px;";
	QString paletteStyleBottom = "background-color: rgba(184, 38, 25, 100%);";
	//QString paletteStyleBottom = "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #B82619, stop: 0.5 #ABAFE5, stop: 1 #B82619);";
                                
	//palette.setColor(QPalette::Background,Qt::red);
	
	QHBoxLayout *lineTop = new QHBoxLayout;
	//QString empty = "";
	QLabel *topBar = new QLabel();
	//topBar->setPalette(palette);
	topBar->setAutoFillBackground(true);
	topBar->setStyleSheet(paletteStyleTop);
	topBar->setFixedHeight(10);
	lineTop->addWidget(topBar);

	QHBoxLayout *lineBottom = new QHBoxLayout;
	QString credits = "<p align=\"right\"><i><font color=\"black\"><b>&nbsp;ROBOTICS</b></font><font color=\"white\"><b>&nbsp;LAB&nbsp;&nbsp;</b></font></i></p>";
	QLabel *statusBottomBar = new QLabel(credits);
	//statusBottomBar->setPalette(palette);
	statusBottomBar->setAutoFillBackground(true);
	statusBottomBar->setStyleSheet(paletteStyleBottom);
	statusBottomBar->setFixedHeight(20);

	lineBottom->addWidget(statusBottomBar);
	//lineBottom->addStretch(true);

	// SLB new layout
	QVBoxLayout *leftCenter = new QVBoxLayout;
	leftCenter->addLayout(line1);
	leftCenter->addLayout(linechannels);
	//leftCenter->addLayout(lineSkillLevel);
	leftCenter->addLayout(line3);
	leftCenter->addLayout(line2);
	leftCenter->addLayout(line6);
	leftCenter->addLayout(lineFlashTimeBox);
	//leftCenter->addLayout(linechannels);
	//leftCenter->addLayout(linechannels2);
	leftCenter->addLayout(line4);
	leftCenter->addLayout(line5);
	//leftCenter->addLayout(line15); // SLB flash opt enabled in originalcd
	//leftCenter->addWidget(groupBoxOptFlash); // ^ SLB
	if (userobot)
		leftCenter->addLayout(line8); // SLB Robot enabled in originalcd
	leftCenter->addLayout(lineUtilMode);
	leftCenter->addLayout(line10);
	leftCenter->addLayout(line11); // SLB
	//leftCenter->addLayout(tastibox);
	leftCenter->addLayout(line13);
	//leftCenter->addLayout(line14);
	leftCenter->addLayout(line12);

	QVBoxLayout *rightCenter = new QVBoxLayout;
	//rightCenter->addLayout(lineUtilMode);
	rightCenter->addLayout(line7);
	rightCenter->addWidget(sensorBox); // SLB TODO change to a layout, not widget
	//rightCenter->addLayout(line8); // SLB Robot enabled in originalcd
	//rightCenter->addLayout(lineFeatSliders);
	
	///rightCenter->addStretch(true);
	
	//
	QFrame* Frame = new QFrame;
	QHBoxLayout *plotLayout = new QHBoxLayout;
	plotLayout->addWidget(customPlot2);
	plotLayout->addWidget(signalPlotGainSlider);
	//Frame->setFrameShape(QFrame::WinPanel);
	//Frame->setFrameShadow(QFrame::Sunken);
	//Frame->setFrameStyle(QFrame::Sunken);
	//Frame->setContentsMargins(2, 2, 2, 2);
	Frame->setStyleSheet("QFrame { border: 1px solid #DCDCDC; }");
	Frame->setLayout(plotLayout);
	rightCenter->addWidget(Frame);
	//
	//
	//rightCenter->addWidget(customPlot2);
	rightCenter->addWidget(chanStatusBox);
	rightCenter->addWidget(customPlot);
	//rightCenter->addStretch(true);
	
	QHBoxLayout* centerMainLayout = new QHBoxLayout;

	QGridLayout* gridCenterMain = new QGridLayout;
	gridCenterMain->addLayout(leftCenter, 0, 0);
	QFrame* vFrame = new QFrame;
	//vFrame->setFixedWidth(2);
	vFrame->setFrameShape(QFrame::VLine);
	//vFrame->setDisabled(true);
	vFrame->setStyleSheet("QFrame { color: #DCDCDC; }");
	//vFrame->setFrameShadow(QFrame::Sunken);
	gridCenterMain->addWidget(vFrame, 0, 1);
	gridCenterMain->addLayout(rightCenter, 0, 2);
	gridCenterMain->setColumnStretch(0, 20);
	gridCenterMain->setColumnStretch(2, 80);
	gridCenterMain->setColumnMinimumWidth(0, 360);
	gridCenterMain->setColumnMinimumWidth(2, 540);
	centerMainLayout->addLayout(gridCenterMain);
	// SLB end new layout

	// SLB old layout
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(lineTop);
	/*
	mainLayout->addLayout(line1);
	//mainLayout->addLayout(lineSkillLevel);
	mainLayout->addLayout(line3);
	mainLayout->addLayout(line2);
	mainLayout->addLayout(line6);
	mainLayout->addLayout(linechannels);
	mainLayout->addLayout(line4);
	mainLayout->addLayout(line5);
	mainLayout->addLayout(lineUtilMode);
	mainLayout->addLayout(line7);
	mainLayout->addWidget(sensorBox); // SLB TODO change to a layout, not widget
	//mainLayout->addLayout(line15); // ^ SLB
	//mainLayout->addLayout(line8); // SLB Robot
	mainLayout->addLayout(line10);
	mainLayout->addLayout(line11); // SLB
	//mainLayout->addLayout(tastibox);
	mainLayout->addLayout(line13);
	mainLayout->addLayout(line14);
	*/
	mainLayout->addLayout(centerMainLayout); // SLB
	//mainLayout->addLayout(line12);
	mainLayout->addLayout(lineBottom); // SLB
	// SLB end old layout

	//setWindowTitle(tr(g_Title)); // SLB
	//setFixedHeight(sizeHint().height());
	setWindowTitle(tr(FRAMEWORKNAME));
	setWindowIcon(QIcon(FRAMEWORKREDICONPATH));
	//setMinimumWidth(540);			// SLB 480
	//setMinimumWidth(540+540/2);	// SLB 480
	setMinimumWidth(900);			// SLB 480
	//setMinimumHeight(680);		// SLB
	setMinimumHeight(680);			// SLB 900*580 ok new wide interface, pre 2 rowed calibspellbox
	//setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // SLB remove "?" help quick menu

	//setLayout(mainLayout); //--------------------
	
	//setStyleSheet("QMainWindow { background: #2fc6f5; }");
	
	QPalette *palette = new QPalette();	
	palette->setBrush(QPalette::Background, *(new QBrush(*(new QPixmap(FRAMEWORKBGIMAGE)))));
	setPalette(*palette);
	
	QWidget *mainWidget = new QWidget();
	mainWidget->setLayout(mainLayout);
	setCentralWidget(mainWidget);
	
	/*
	//menu bar 0
	newFile = new QAction(("&New"),this);
	newFile -> setShortcuts(QKeySequence::New);
	newFile -> setStatusTip(tr("Create a new file"));
	connect(newFile,SIGNAL(triggered()),this,SLOT(newfile()));
	//menu bar 1
	mystatus = new QAction(("&Status Bar"), this);
	mystatus -> setCheckable(true);
	mystatus->setStatusTip(tr("Status check box here..."));
	connect( mystatus, SIGNAL(toggled(bool)), status, SLOT(setHidden(bool)));
	connect( newFile, SIGNAL(hovered()), this, SLOT(newFileStatusMsg()));
	connect( mystatus, SIGNAL(hovered()), this, SLOT(myStatusMsg()));
	*/

	// SLB TEST TIMER
	timerLabel = new QLabel();
	///timerLabel->setMinimumWidth(72); // use with hh:mm:ss.zzz
	timerLabel->setMinimumWidth(51); // use with hh:mm:ss
	//timerLabel->setAlignment(Qt::AlignLeft);
	
	QMenuBar *menubar			= new QMenuBar(this);
	menubar->setCornerWidget(timerLabel, Qt::TopRightCorner);
	
	QMenu *menubar1				= new QMenu(this);
	menubar1					= menubar->addMenu(tr("&Info"));

	QAction *menubar1help		= new QAction(("&Help"), this);
	menubar1help->setIcon(QIcon(FRAMEWORKBLUEICONPATH));
	menubar1help->setIconVisibleInMenu(true);
	menubar1help->setShortcut(QKeySequence(Qt::Key_Question));
	connect(menubar1help, SIGNAL(triggered()), this, SLOT(menubar1helpSlot()));
	QAction *menubar1credits	= new QAction(("&Credits"), this);
	menubar1credits->setShortcut(QKeySequence(Qt::Key_Escape));
	connect(menubar1credits, SIGNAL(triggered()), this, SLOT(menubar1creditsSlot()));
	menubar1credits->setIcon(QIcon(FRAMEWORKREDICONPATH));
	menubar1credits->setIconVisibleInMenu(true);
	
	menubar->setMinimumWidth(2);
	menubar1->addAction(menubar1help);
	menubar1->addAction(menubar1credits);
	setMenuBar(menubar);
	
	statusbar = new QStatusBar(this);
	setStatusBar(statusbar);

	/*
	QLabel *statusBarLeft	= new QLabel();
	QLabel *statusBarRight	= new QLabel();
	QString paletteStyleBottomLeft	= "background-color: rgba(184, 38, 25, 100%); border-bottom-left-radius: 6px 10px;";
	QString paletteStyleBottomRight	= "background-color: rgba(184, 38, 25, 100%); border-bottom-right-radius: 6px 10px;";
	statusBarLeft->setAutoFillBackground(true);
	statusBarRight->setAutoFillBackground(true);
	statusBarLeft->setStyleSheet(paletteStyleBottomLeft);
	statusBarRight->setStyleSheet(paletteStyleBottomRight);
	statusBarLeft->setFrameStyle(QFrame::NoFrame | QFrame::Sunken);
	statusBarRight->setFrameStyle(QFrame::NoFrame | QFrame::Sunken);
	//
	statusbar->addPermanentWidget(statusBarLeft, 2);
	statusbar->addPermanentWidget(statusBarRight, 1);
	*/
	//statusbar->addPermanentWidget(timerLabel, 2);

	statusbar->setSizeGripEnabled(false);
	//statusBar()->activateWindow();
	//setStyleSheet("QStatusBar::item { border: none; }");

	statusbar->showMessage(tr("Starting Framework..."), 2000);
	
	activateWindow();

	timerTime->start(1000);

	/* SLB disable Worker (no idea why it was used at all)
	countRunning	= false;
	worker			= new Worker;
	startCount();
	*/
	cout << "Gui instance created succesfully." << endl;
}

// SLB
Gui::~Gui() {
	timerTime->stop();
	delete timerTime;
	timer->stop();
	delete timer;
}

bool Gui::isRunning() {
	return running;
}

// SLB
void Gui::openLogFile() {

	time_t now;
	char* timeNow = new char[12];

	logFilePath = logFilesRoot + "/Log_";
	
	now = time(NULL);
	strftime(timeNow, 80, "%Y_%m_%d_%H_%M_%S", gmtime(&now));

	logFilePath += username;
	logFilePath += "_";
	logFilePath += timeNow;
	logFilePath += ".txt";
	
	//logFile.Open(rootPath.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeText);

}

// SLB
void Gui::writeLogFile(string log) {

	FILE *logFile;
	logFile = fopen(logFilePath.c_str(), "a+");
	if (!logFile) {
		cout << "Error opening file!\n";
	}
	else {
				
		fprintf(logFile, "%s\n", log.c_str());
		
		///fflush(logFile); // SLB check open/close file stream too often?!
		fclose(logFile);
	}
}

// SLB TODO Remove later
/*
void Gui::enableokButton() {

	///okButton->setEnabled((radioAlphabetic->isChecked() || radioSymbolic->isChecked() || radioCustomPalette->isChecked()));
	okButton->setEnabled((radioAlphabetic->isChecked() || radioSymbolic->isChecked() || checkCustomPalette->isChecked()));
	//okButton->setEnabled((radioAlphabetic->isChecked() || radioSymbolic->isChecked() || radioCustomPalette->isChecked()) &&
	//					(radioOn->isChecked() || radioOff->isChecked()));				// SLB used in originalcd
	//reseteButton->setEnabled((radioAlphabetic->isChecked() || radioSymbolic->isChecked() || radioCustomPalette->isChecked()));	// SLB
	//closeButton->setEnabled((radioAlphabetic->isChecked() || radioSymbolic->isChecked() || radioCustomPalette->isChecked()));		// SLB
}
*/

void Gui::changeTagComboTarget() {

	int index = comboInterfaccia->currentIndex()+2;
	int flash = comboModalita->currentIndex();
	number_of_tags = index*index;

	for (int i=0; i<comboTarget.size(); i++) {
			comboTarget[i]->clear();
	}
	for (int i=0; i<comboTarget.size(); i++) {
		for (int j=0;j<number_of_tags;j++)	{
			string s = std::to_string(j+1);
			comboTarget[i]->addItem(s.c_str()); 
		}
	}

}

void Gui::changeComboTarget() {

	bool convOk = false;									// SLB
	int temp = lineNumCalibTag->text().toInt(&convOk, 10);	// SLB
	
	if (convOk) { // SLB
		if (!temp) {
			temp = 1;
			lineNumCalibTag->setText(QString::number(temp));
		}
		number_of_calibration_tag = temp;

		int nOld = comboTarget.size();

		if (number_of_calibration_tag > nOld) {
			for (int i = nOld; i < number_of_calibration_tag; i++) {
				comboTarget.push_back(new QComboBox);
				for (int j=0; j<number_of_tags; j++) {
					string s = std::to_string(j+1);
					//comboTarget[i]->addItem(s.c_str());
					comboTarget[i]->addItem(QString::fromStdString(s));
					connect(comboTarget[i], SIGNAL(currentIndexChanged(int)), this, SLOT(configChanged()));
				}
				targetLayout->addWidget(comboTarget[i]);
			}
		}
		else  {
			for (int i = nOld-1; i > number_of_calibration_tag-1; i--) {
				targetLayout->removeWidget(comboTarget[i]);
				delete comboTarget[i];
				comboTarget.pop_back();
			}
		}
	}	
}

// SLB
void Gui::changeTextureCombo() {

	impaired_theme		= ctextures->currentText().toLocal8Bit().constData();
	impaired_letterCol	= impaired_theme.substr(0, impaired_theme.find('_'));
	impaired_bgCol		= impaired_theme.substr(impaired_theme.find('_')+1);
	
}

// SLB
void Gui::changePaletteString() {
	
	int paletteStringLength = linePaletteString->text().size();
	elements = (comboInterfaccia->currentIndex()+2)*(comboInterfaccia->currentIndex()+2);
	
	string paletteStringLengthString	= std::to_string(paletteStringLength);
	string paletteStringOffset			= std::to_string(paletteStringLength - elements);
	
	if ((paletteStringLength - elements) > 0)
		paletteStringLengthString += " (+" + paletteStringOffset + ")";
	else if ((paletteStringLength - elements) < 0)
		paletteStringLengthString += " ("  + paletteStringOffset + ")";
	else
		paletteStringLengthString += " <html>&#x2714;</html>";
	
	if (checkCustomPalette->isEnabled() && customPalette) {
		if (paletteStringLength < elements)
			paletteStringCounter->setStyleSheet("QLabel {color: red;}");
		else if (paletteStringLength > elements)
			paletteStringCounter->setStyleSheet("QLabel {color: black;}");
		else
			paletteStringCounter->setStyleSheet("QLabel {color: green;}");
	}
	else
		paletteStringCounter->setStyleSheet("QLabel {color: grey;}");
	
	paletteStringCounter->setText(paletteStringLengthString.c_str());
	
}

//SLB
void Gui::configChanged() {
	okButton->setStyleSheet("QPushButton { font: bold; color: rgba(184, 38, 25, 100%); }");
	timer->stop();
}

// SLB
void Gui::updateTimerTime() {
	//cout << clock() << endl;
	QString currtime = QTime::currentTime().toString(QString("hh:mm:ss")); // "hh:mm:ss.zzz"
	//timerLabel->setText(QString::number(clock()));
	timerLabel->setText(currtime);
}

// SLB
void Gui::updateTimer() {
	/*
	//cout << clock() << endl;
	QString currtime = QTime::currentTime().toString(QString("hh:mm:ss")); // "hh:mm:ss.zzz"
	//timerLabel->setText(QString::number(clock()));
	timerLabel->setText(currtime);
	*/
	changeChannelStatusColour();

	if (bioHybridMode)
		customPlot->replot();

	//customPlot2 test
	int value = x.size();
	x << value;
	for (int c = 0; c < number_channels+number_channels_plus; c++) {
		y[c] << receivedData[c]*signalPlotGain + ((number_channels+number_channels_plus) - c)*50;
		customPlot2->graph(c)->setData(x, y[c]);
		//customPlot2->graph(c)->rescaleAxes();
	}
	for (int c = number_channels+number_channels_plus; c < 16; c++) {
		y[c] << -100 + ((number_channels+number_channels_plus) - c)*50;
		customPlot2->graph(c)->setData(x, y[c]);
		//customPlot2->graph(c)->rescaleAxes();
	}
	//customPlot2->graph(0)->rescaleAxes();
	double lowerRange = customPlot2->xAxis->range().lower; 
	double upperRange = customPlot2->xAxis->range().upper;
	if (value >= customPlot2->xAxis->range().upper) {
		lowerRange++;
		upperRange++;
		
		customPlot2->xAxis->setRange(lowerRange, upperRange);
	}
	customPlot2->replot();
	//
}

void Gui::okClicked() {

	timer->start(TIMER_INTERVAL);

	okButton->setStyleSheet("QPushButton { font: bold; color: black; }");

	statusbar->showMessage(tr("Configuration updated"), 2000); // SLB

	q_username		= cusername->currentText();
	username		= q_username.toLocal8Bit().constData();

	mode			= comboModalita->currentIndex();
	
	number_of_row_interface = comboInterfaccia->currentIndex()+2;
	
	// SLB
	bool conv1ok	= false;
	int value1		= lineNumCalibTag->text().toInt(&conv1ok, 10);
	if (conv1ok)
		number_of_calibration_tag = value1;
	
	// SLB
	bool conv2ok	= false;
	int value2		= lineFlash->text().toInt(&conv2ok, 10);
	if (conv2ok)
		number_of_flash = value2;

	// SLB
	if (!time_flash_auto) {
		bool conv3ok	= false;
		int value3		= lineFlashTime->text().toInt(&conv3ok, 10);
		if (conv3ok)
			velocita = value3;
	}

	// SLB
	bool conv4ok	= false;
	int value4		= lineChan->text().toInt(&conv4ok, 10);
	if (conv4ok)
		number_channels = value4;

	// SLB
	bool conv5ok	= false;
	int value5		= lineChan2->text().toInt(&conv5ok, 10);
	if (conv5ok)
		number_channels_plus = value5;

	// SLB
	///skillSliderValue = skillSlider->value();
	skillSliderValue = skillSlider->value()*10;

	ipRobot = lineIpRobot->text().toLocal8Bit().constData();
	portRobot = linePortRobot->text().toLocal8Bit().constData();

	// SLB
	/*
	if (linePaletteString->text().isEmpty()) {
		palette_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	}
	else {
		palette_string = linePaletteString->text().toLocal8Bit().constData(); // SLB
		std::transform(palette_string.begin(), palette_string.end(), palette_string.begin(), toupper);
	}
	linePaletteString->setText(palette_string.c_str());
	*/

	// SLB
	string valid_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	palette_string = valid_string;
	if (!linePaletteString->text().isEmpty()) {
		string temp_palette_string = linePaletteString->text().toLocal8Bit().constData(); // SLB
		std::transform(temp_palette_string.begin(), temp_palette_string.end(), temp_palette_string.begin(), toupper);
		int iter = (temp_palette_string.size() <= 36) ? temp_palette_string.size() : 36; // SLB
		for (int c=0; c<iter; c++) {
			if (valid_string.find(temp_palette_string[c]) != std::string::npos) // checks for a valid string
				palette_string[c] = temp_palette_string[c];
			else
				palette_string[c] = 'X'; // if not valid character, substitute it with a 'X'
		}
	}
	linePaletteString->setText(palette_string.c_str());
	changePaletteString();
	
	if (velocita < 100)
		velocita = 100;

	updateConfiguration();

	// SLB Gui updates
	if (!tobiienabled && radioHybrid->isChecked()) {
		radioBasic->setChecked(true);
		radioHybrid->setChecked(false);
	}
	lineFlash->setText(QString::number(number_of_flash));
	lineFlashTime->setText(QString::number(velocita));
	lineChan->setText(QString::number(number_channels));
	lineChan2->setText(QString::number(number_channels_plus));
	lineChan->setEnabled(false);
	lineChan2->setEnabled(false);
	
	closeButton->setEnabled(true);
	signalCheckButton->setEnabled(true);
	baselineAcquisitionButton->setEnabled(true);
	gameSessionButton->setEnabled(true);
	calibration->setEnabled(!bioHybridMode);
	spellerFile->setEnabled(exists_file(outputFilesRoot + "/w_" + username));
	//
	customPlot->setEnabled(bioHybridMode);
	if (!bioHybridMode) {
		//customPlot->plotLayout()->title->setTextColor(Qt::gray);
		customPlot->setBackground(QBrush(QColor(200, 200, 200)));
		customPlot->yAxis->setTickLabelColor(Qt::gray);
		customPlot->yAxis->setLabelColor(Qt::gray);
		customPlot->legend->setBrush(QColor(150, 150, 150, 200));
		customPlot->legend->setTextColor(QColor(100, 100, 100));
	}
	customPlot->replot();
	//

	signalPlotGain = signalPlotGainSlider->value();

	//initKinect();	// SLB Kinect
	//initTobii();	// SLB Tobii
	
	running = false;
}

// SLB
void Gui::closeClicked() {
	
	_eventFinishInitialization.SetEvent();
	_eventKill.SetEvent();
	
	Sleep(1000); // SLB TODO check
	close();
	//exit(0);
}

// SLB
void Gui::closeEvent(QCloseEvent *event) {

	closeClicked();
    //QWidget::closeEvent(event);
}

/*
void Gui::resetClicked() { // SLB TODO

	okButton->setEnabled(true);
	closeButton->setEnabled(true);
	comboInterfaccia->setEnabled(true);
	comboModalita->setEnabled(true);
	for (int i=0; i<comboTarget.size(); i++)
		comboTarget[i]->setEnabled(true);
	radioAlphabetic->setEnabled(true);
	radioSymbolic->setEnabled(true);
	radioCustomPalette->setEnabled(true);
	radioAlphabetic->setChecked(false);
    radioSymbolic->setChecked(false);
	radioCustomPalette->setChecked(false);
	//okButton->setDefault(true);
	okButton->setEnabled(false);
	lineNumCalibTag->setEnabled(true);
	lineFlash->setEnabled(true);
	lineIpRobot->setEnabled(true);
	linePortRobot->setEnabled(true);
	calibration->setEnabled(false);
	speller->setEnabled(false);
	spellerFile->setEnabled(false);
	running = false;
}
*/

// SLB
void Gui::guiElementsSetEnabled(bool flag) {

	calibration->setEnabled(flag);
	gameSessionButton->setEnabled(flag);

	cusername->setEnabled(flag);				// SLB
	//lineChan->setEnabled(flag);				// SLB
	//lineChan2->setEnabled(flag);				// SLB
	comboInterfaccia->setEnabled(flag);
	comboModalita->setEnabled(flag);
	lineFlash->setEnabled(flag);				// SLB //
	checkFlashOpt->setEnabled(flag);			// SLB
	checkFlashTimeAuto->setEnabled(flag);		// SLB
	lineNumCalibTag->setEnabled(flag);			//
	for (int i=0; i<comboTarget.size(); i++)	//
		comboTarget[i]->setEnabled(flag);		//
	lineIpRobot->setEnabled(flag);
	linePortRobot->setEnabled(flag);

	okButton->setEnabled(flag);					// SLB //

	radioBasic->setEnabled(flag);				// SLB
	radioHybrid->setEnabled(flag);				// SLB
	radioBioHybrid->setEnabled(flag);			// SLB

	radioAlphabetic->setEnabled(flag);
	radioSymbolic->setEnabled(flag);
	//radioCustomPalette->setEnabled(flag);		// SLB
	
	eyeTrackerCheckBox->setEnabled(flag);		// SLB
	
	if (flag) {
		lineFlashTime->setEnabled(!time_flash_auto);		// SLB

		checkCustomPalette->setEnabled(!symbolicMode);
		ctextures->setEnabled(customPalette);				// SLB
		linePaletteString->setEnabled(customPalette);		// SLB
		paletteStringCounter->setEnabled(customPalette);	// SLB

		iconEyeLabel->setEnabled(tobiienabled);				// SLB
		skillSlider->setEnabled(tobiienabled);				// SLB
		iconBrainLabel->setEnabled(tobiienabled);			// SLB
		eyeTrackerGazeCheckBox->setEnabled(tobiienabled);	// SLB
		tobiiFilteredRadio->setEnabled(tobiienabled);		// SLB
		tobiiRawRadio->setEnabled(tobiienabled);			// SLB
	}
	else {
		lineFlashTime->setEnabled(flag);			// SLB

		checkCustomPalette->setEnabled(flag);		// SLB
		ctextures->setEnabled(flag);				// SLB
		linePaletteString->setEnabled(flag);		// SLB
		paletteStringCounter->setEnabled(flag);		// SLB

		iconEyeLabel->setEnabled(flag);				// SLB
		skillSlider->setEnabled(flag);				// SLB
		iconBrainLabel->setEnabled(flag);			// SLB
		eyeTrackerGazeCheckBox->setEnabled(flag);	// SLB
		tobiiFilteredRadio->setEnabled(flag);		// SLB
		tobiiRawRadio->setEnabled(flag);			// SLB
	}
	
}

bool Gui::exists_file (string name) {

	name.erase(0, name.find_first_not_of(' '));      
	name.erase(name.find_last_not_of(' ')+1); 

    ifstream f(name+".txt");
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }   
}

// SLB
void Gui::check_folder (string path) { // SLB create a folder if it doesn't exist already

	if (access (path.c_str(), 0) != 0) {

		cout << RED << "Folder " + path + " doesn't exist." << WHITE << endl;

		string command = "mkdir " + path;
		system(command.c_str());

		if (access (path.c_str(), 0) == 0) {
			cout << GREEN << "Folder " + path + " created succesfully." << WHITE << endl;
		}
		else {
			cout << RED << "An error occurred in the creation of the folder " + path + "." << WHITE << endl;
			cout << "Please create it manually." << endl;
		}
	}
	else {
		//cout << GREEN << "Folder " + path + " already exists. ";
		//cout << WHITE << "Skipping folder creation." << endl;
	}

}

// SLB
void Gui::setIndexSliders() {

	if (bioHybridMode) {

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());
	
		if (result) {
		
			int rsq_norm = atof(doc.child("rsq_norm").first_child().value()) * 100;
			int epy_norm = atof(doc.child("epy_norm").first_child().value()) * 100;
			int egy_norm = atof(doc.child("egy_norm").first_child().value()) * 100;

			index1Slider->setSliderPosition(rsq_norm);
			index2Slider->setSliderPosition(epy_norm);
			index3Slider->setSliderPosition(egy_norm);

		}
		else
			cout << "Could not find configuration file." << endl;

	}

}

// SLB
void Gui::setPlotBars() {

	if (bioHybridMode) {

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());
	
		if (result) {
		
			float rsq_norm = atof(doc.child("rsq_norm").first_child().value()) / 3;
			float epy_norm = atof(doc.child("epy_norm").first_child().value()) / 3;
			float egy_norm = atof(doc.child("egy_norm").first_child().value()) / 3;
		
			/*
			ticks.clear();
			labels.clear();
			rsquareData.clear();
			epyData.clear();
			egyData.clear();
			*/
			int value = ticks.size(); // + 1;
			ticks << value;
			if (labels.size() != 0)
				labels << QString::number(ticks.size() - 1);
			else
				labels << QString::fromStdString("C");
			textTicker->addTicks(ticks, labels);
			customPlot->xAxis->setTicker((QSharedPointer<QCPAxisTickerText>) textTicker);
		
			rsquareData	<< rsq_norm;
			epyData		<< epy_norm;
			egyData		<< egy_norm;
	
			rsquare->setData(ticks, rsquareData);
			epy->setData(ticks, epyData);
			egy->setData(ticks, egyData);

			double lowerRange = customPlot->xAxis->range().lower; 
			double upperRange = customPlot->xAxis->range().upper;
			if (value >= customPlot->xAxis->range().upper) {
				//lowerRange = lowerRange + (value - customPlot->xAxis->range().upper);
				//upperRange = value;
				lowerRange++;
				upperRange++;

				customPlot->xAxis->setRange(lowerRange, upperRange);
			}

		}
		else
			cout << "Could not find configuration file." << endl;

	}

}

// SLB
void Gui::changeLineFlash() {

	bool conv1ok = false;
	int value1 = lineFlashTime->text().toInt(&conv1ok, 10);
	if (conv1ok && !value1) {
		number_of_flash = 1;
		lineFlashTime->setText(QString::number(velocita));
	}

}

/*
// SLB
void Gui::changeLineFlashTime() {

	bool conv1ok = false;
	int value1 = lineFlashTime->text().toInt(&conv1ok, 10);
	if (conv1ok && !value1) {
		velocita = 1;
		lineFlash->setText(QString::number(number_of_flash));
	}

}
*/

// SLB
void Gui::changeChannel1() {

	bool conv1ok = false;
	int value1 = lineChan->text().toInt(&conv1ok, 10);
	if (conv1ok) {
		if (!value1) {
			number_channels = 1;
			lineChan->setText(QString::number(number_channels));
		}
		else
			number_channels = value1;
	}

	int tot_channels = number_channels + number_channels_plus;

	if (tot_channels > 16) {
		number_channels_plus -= (tot_channels - 16);
		tot_channels = 16;
		lineChan2->setText(QString::number(number_channels_plus));
	}

	changeChannelStatus(tot_channels);
	//changeChannelStatusColour();

}

// SLB
void Gui::changeChannel2() {
	
	bool conv2ok = false;
	int value2 = lineChan2->text().toInt(&conv2ok, 10);
	if (conv2ok)
		number_channels_plus = value2;

	int tot_channels = number_channels + number_channels_plus;

	if (tot_channels > 16) {
		number_channels -= (tot_channels - 16);
		tot_channels = 16;
		/*
		if (number_channels == 0) {
			number_channels = 1;
			number_channels_plus--;
			lineChan2->setText(QString::number(number_channels_plus));
		}
		*/
		lineChan->setText(QString::number(number_channels));
	}

	changeChannelStatus(tot_channels);

}

// SLB
void Gui::changeChannelStatus(int tot_channels) {
	
	int nOld = chanStatusLabels.size();
	
	if (tot_channels > nOld) {
		for (int c = nOld; c < tot_channels; c++) {
			chanStatusLabels.push_back(new QLabel(QString::number(c+1)));
			chanStatusLabels[c]->setAlignment(Qt::AlignCenter);
			//chanStatusLabels[c]->setMaximumWidth(25);
			//chanStatusLabels[c]->setMinimumHeight(25);
			chanStatusLabels[c]->setFixedSize(25, 25);
			/* SLB ok flat colour
			string chanStatusStyle = "QLabel { background: ";
			chanStatusStyle += CHSTATUS_GREY;
			chanStatusStyle += "; border-radius: 11px; border: 1px solid #555; }";
			*/
			string chanStatusStyle = "QLabel { background: qradialgradient(cx:0, cy:0, radius:1, fx:0.5, fy:0.5, stop:0 white, stop:1 ";
			chanStatusStyle += CHSTATUS_GREY;
			chanStatusStyle += "); border-radius: 11px; border: 1px solid #555; }";
			chanStatusLabels[c]->setStyleSheet(QString::fromStdString(chanStatusStyle));
			chanStatusLayout->addWidget(chanStatusLabels[c]);
		}
	}
	else {
		for (int c = nOld-1; c > tot_channels-1; c--) {
			chanStatusLayout->removeWidget(chanStatusLabels[c]);
			delete chanStatusLabels[c];
			chanStatusLabels.pop_back();
		}
	}

	initReceivedData(tot_channels);
	
}

// SLB
void Gui::initVariables() {
	
	timerTime = new QTimer(this);
	connect(timerTime, SIGNAL(timeout()), this, SLOT(updateTimerTime()));

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
	
	notificationSound = audioPathRoot + "/notification.wav";

	acqReady				= false;
	acqDone					= false;
	currentRunDone			= false;
	tobiiInitialised		= false;
	kinectInitialised		= false;
	kinectGuiStarted		= false;
	gameRunDone				= false;
	gameRunStarted			= false;
	gameUseBCI				= false;
	signalPlotGain			= 1;
	eyesAcqStatus			= -1;
	//ocular_filter			= 60;
	gameMode				= GAMEMODE_FAIR;
	gamePlayerChoice		= GAME_UNDEFINED;
	gameSessionCounter		= 0;
	gameSessionCounterTot	= 0;
	gameFair1CounterTot		= 0;
	gameCheatCounter		= 0;
	gameCheatCurrent		= 0;
	gameCheatCounterTot		= 0;
	gameFair2CounterTot		= 0;
	gameUseBCI				= true;
	gameUseKinect			= true;
	receivedDataN			= FILETYPE_GAME; // it is a Game session (not calibration nor spelling)
	receivedDataN1			= GAMEMODE_BASELINE;

	for (int i = 0; i < 3; i++)
		gameWinCounts.push_back(0);
	// gameWinCounts[0] = Robot Wins
	// gameWinCounts[1] = Ties
	// gameWinCounts[2] = Robot Wins

	int tot_channels	= number_channels + number_channels_plus;
	receivedData		= new float[tot_channels + SERVICE_CHANNELS];
	initReceivedData(tot_channels);

	openLogFile();
}

// SLB
void Gui::initReceivedData(int tot_channels) {
	for (int c = 0; c < tot_channels+SERVICE_CHANNELS; c++)
		receivedData[c] = -100;
}

// SLB
void Gui::initReceivedData() {
	
	initReceivedData(number_channels + number_channels_plus);
}

// SLB
void Gui::setReceivedData(float* data) {
	/*
	int tot_channels = number_channels + number_channels_plus;

	for (int c = 0; c < tot_channels; c++)
		receivedData[c] = data[c];
	*/
	
	receivedData = data;
}

// SLB
float Gui::getReceivedDataN() {
	return receivedDataN;
}

// SLB
float Gui::getReceivedDataN1() {
	return receivedDataN1;
}

// SLB
void Gui::changeChannelStatusColour() {
	float red_limit		= ocular_filter - (ocular_filter/6);
	float yellow_limit	= ocular_filter/2;

	/* SLB ok flat colour
	string pre			= "QLabel { background: ";
	string post			= "; border-radius: 11px; border: 1px solid #555; }";
	*/

	string pre			= "QLabel { background: qradialgradient(cx:0, cy:0, radius:1, fx:0.5, fy:0.5, stop:0 white, stop:1 ";
	string post			= "); border-radius: 11px; border: 1px solid #555; }";
	
	string chanStatusStyleG	= pre;
	chanStatusStyleG		+= CHSTATUS_GREEN;
	chanStatusStyleG		+= post;

	string chanStatusStyleY	= pre;
	chanStatusStyleY		+= CHSTATUS_YELLOW;
	chanStatusStyleY		+= post;

	string chanStatusStyleR	= pre;
	chanStatusStyleR		+= CHSTATUS_RED;
	chanStatusStyleR		+= post;

	string chanStatusStyleGrey	= pre;
	chanStatusStyleGrey		+= CHSTATUS_GREY;
	chanStatusStyleGrey		+= post;

	red_limit		= 1;	// 0.5
	yellow_limit	= 0.5;	// 0.3
	
	int tot_channels	= number_channels + number_channels_plus;
	for (int c = 0; c < tot_channels; c++) {

		//cout << receivedData[c] << "\t";
		
		if (receivedData[c] < -ocular_filter || receivedData[c] > ocular_filter)
			chanStatusLabels[c]->setStyleSheet(QString::fromStdString(chanStatusStyleGrey));
		else if (receivedData[c] < -red_limit || receivedData[c] > red_limit)
			chanStatusLabels[c]->setStyleSheet(QString::fromStdString(chanStatusStyleR));
		else if (receivedData[c] < -yellow_limit || receivedData[c] > yellow_limit)
			chanStatusLabels[c]->setStyleSheet(QString::fromStdString(chanStatusStyleY));
		else
			chanStatusLabels[c]->setStyleSheet(QString::fromStdString(chanStatusStyleG));
	}

	//cout << endl;
}

void Gui::clickedstate(bool flag) {

	if (radioAlphabetic->isChecked()) {
		symbolicMode = false;
		checkCustomPalette->setEnabled(true);
		customPalette = checkCustomPalette->isChecked();
		ctextures->setEnabled(customPalette);
		linePaletteString->setEnabled(customPalette); // SLB
		//paletteStringCounter->setEnabled(true); // SLB
		QVariant v(1 | 32);
		comboInterfaccia->setItemData(0, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(1, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(2, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(3, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(4, v, Qt::UserRole-1);
	}

	// SLB
	if (checkCustomPalette->isChecked()) {
		symbolicMode = false;
		customPalette = true;
		//comboInterfaccia->setCurrentIndex(2); // SLB
		// SLB
		ctextures->setEnabled(true);
		linePaletteString->setEnabled(true); // SLB
		QVariant v(1 | 32);
		comboInterfaccia->setItemData(0, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(1, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(2, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(3, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(4, v, Qt::UserRole-1);
		// SLB //
	}

	if (radioSymbolic->isChecked()) {
		symbolicMode = true;
		customPalette = false;
		comboInterfaccia->setCurrentIndex(2);
		checkCustomPalette->setEnabled(false);
		ctextures->setEnabled(false); // SLB
		linePaletteString->setEnabled(false); // SLB
		QVariant v(0);
		comboInterfaccia->setItemData(0, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(1, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(3, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(4, v, Qt::UserRole-1);
	}
	/*
	if (radioCustomPalette->isChecked()) {
		symbolicMode = false;
		customPalette = true;
		//comboInterfaccia->setCurrentIndex(2); // SLB
		// SLB
		ctextures->setEnabled(true);
		linePaletteString->setEnabled(true); // SLB
		QVariant v(1 | 32);
		comboInterfaccia->setItemData(0, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(1, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(2, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(3, v, Qt::UserRole-1);
		comboInterfaccia->setItemData(4, v, Qt::UserRole-1);
		// SLB //
	}
	*/
	
}

void Gui::clickedOpt(bool flag) {
	/*
	if (radioOn->isChecked()) {
		//optFlash = 1;
		optFlash = true;
	}
	if (radioOff->isChecked()) {
		//optFlash = 0;
		optFlash = false;
	}
	*/
	if (checkFlashOpt->isChecked()) {
		optFlash = true;
		checkFlashOpt->setToolTip("Disable optimisation of flashes");
	}
	else {
		optFlash = false;
		checkFlashOpt->setToolTip("Enable optimisation of flashes");
	}
}

// SLB
void Gui::clickedFlashTimeAuto(bool flag) {
	time_flash_auto = flag;
	lineFlashTime->setEnabled(!time_flash_auto);
}

// SLB
void Gui::eyeTrackerChecked(bool flag) {
	if (eyeTrackerCheckBox->isChecked()) {
		tobiienabled = true;
		//eyeTrackerCheckBox->setText("&Enabled");
		eyeTrackerCheckBox->setToolTip("Click to disable");
		if (radioBasic->isChecked()) {
			radioBasic->setChecked(false);
			radioHybrid->setChecked(true);
			//radioBioHybrid->setChecked(false); // SLB Redundant
		}
	}
	else {
		tobiienabled = false;
		//eyeTrackerCheckBox->setText("&Disabled");
		eyeTrackerCheckBox->setToolTip("Click to enable");
		if (radioHybrid->isChecked()) {
			radioBasic->setChecked(true);
			radioHybrid->setChecked(false);
			//radioBioHybrid->setChecked(false); // SLB Redundant
		}
	}
	iconEyeLabel->setEnabled(tobiienabled);
	skillSlider->setEnabled(tobiienabled);
	iconBrainLabel->setEnabled(tobiienabled);
	eyeTrackerGazeCheckBox->setEnabled(tobiienabled);
	tobiiFilteredRadio->setEnabled(tobiienabled);
	tobiiRawRadio->setEnabled(tobiienabled);
	tobiigazeenabled = eyeTrackerGazeCheckBox->isChecked();
}

// SLB
void Gui::clickedtobiifilter(bool flag) {
	if (tobiiFilteredRadio->isChecked())
		tobiifiltered = true;

	if (tobiiRawRadio->isChecked())
		tobiifiltered = false;
}

// SLB
void Gui::clickedUtilMode(bool flag) {
	if (radioBasic->isChecked()) {
		bioHybridMode = false;
		tobiienabled = false;
		eyeTrackerCheckBox->setToolTip("Click to enable");
	}
	if (radioHybrid->isChecked()) {
		bioHybridMode = false;
		tobiienabled = true;
		eyeTrackerCheckBox->setToolTip("Click to disable");
	}
	if (radioBioHybrid->isChecked()) {
		bioHybridMode = true;
		tobiienabled = true;
		eyeTrackerCheckBox->setToolTip("Click to disable");
	}

	eyeTrackerCheckBox->setChecked(tobiienabled);
	eyeTrackerGazeCheckBox->setEnabled(tobiienabled);
	iconEyeLabel->setEnabled(tobiienabled);
	iconBrainLabel->setEnabled(tobiienabled);
	skillSlider->setEnabled(tobiienabled);
	tobiiFilteredRadio->setEnabled(tobiienabled);
	tobiiRawRadio->setEnabled(tobiienabled);
	tobiienabled = tobiienabled;

	customPlot->setEnabled(bioHybridMode);
	if (!bioHybridMode) {
		//customPlot->plotLayout()->title->setTextColor(Qt::gray);
		customPlot->setBackground(QBrush(QColor(200, 200, 200)));
		customPlot->yAxis->setTickLabelColor(Qt::gray);
		customPlot->yAxis->setLabelColor(Qt::gray);
		customPlot->legend->setBrush(QColor(150, 150, 150, 200));
		customPlot->legend->setTextColor(QColor(100, 100, 100));
	}
	else {
		QLinearGradient grad(0, 0, 0, 400);
		grad.setColorAt(1, QColor(10, 10, 10));
		grad.setColorAt(0.45, QColor(80, 80, 80));
		grad.setColorAt(0, QColor(20, 20, 20));
		customPlot->setBackground(QBrush(grad));
		customPlot->yAxis->setTickLabelColor(Qt::white);
		customPlot->yAxis->setLabelColor(Qt::white);
		customPlot->legend->setBrush(QColor(0, 0, 0, 100));
		customPlot->legend->setTextColor(Qt::white);
	}
	customPlot->replot();
}

// SLB
void Gui::changeGameCheat1() {

	bool conv1ok = false;
	int value1 = gameCheatEdit1->text().toInt(&conv1ok, 10);
	if (conv1ok)
		gameCheatCounter = value1;

	if (gameCheatEdit2->text().toInt() < value1) {
		gameCheatCounterTot = gameCheatCounter;
		gameCheatEdit2->setText(QString::number(gameCheatCounterTot));
	}

}

// SLB
void Gui::changeGameCheat2() {
	
	bool conv2ok = false;
	int value2 = gameCheatEdit2->text().toInt(&conv2ok, 10);
	if (conv2ok)
		gameCheatCounterTot = value2;

	if (value2 < gameCheatEdit1->text().toInt()) {
		gameCheatCounter = gameCheatCounterTot;
		gameCheatEdit1->setText(QString::number(gameCheatCounter));
	}

}

// SLB
void Gui::clickedGameMode(bool flag) {

	if (gameRadioFair->isChecked()) {
		gameMode = GAMEMODE_FAIR;
	}
	if (gameRadioCheatW->isChecked()) {
		gameMode = GAMEMODE_CHEATWIN;
	}
	if (gameRadioCheatL->isChecked()) {
		gameMode = GAMEMODE_CHEATLOSE;
	}

	cout << "Game mode set: " << std::to_string(gameMode) << endl;

}

// SLB
void Gui::clickedGameApplyButton() {

	//robotRawCommand("resetpose");
	robotPlaymotion("resetpose");

	gameFair1CounterTot		= gameFairEdit1->text().toInt();
	gameCheatCounter		= gameCheatEdit1->text().toInt();
	gameCheatCounterTot		= gameCheatEdit2->text().toInt();
	gameFair2CounterTot		= gameFairEdit2->text().toInt();

	if (gameFair1CounterTot || gameRadioFair->isChecked()) {
		gameMode = GAMEMODE_FAIR;
	}
	else if (gameCheatCounterTot) {
		if (gameRadioCheatW->isChecked())
			gameMode = GAMEMODE_CHEATWIN;
		else if (gameRadioCheatL->isChecked())
			gameMode = GAMEMODE_CHEATLOSE;
	}
	else if (gameFair2CounterTot) {
		gameMode = GAMEMODE_FAIR;
	}

	gameSessionCounter		= 1;
	gameCheatCurrent		= 0;
	gameSessionCounterTot	= gameFair1CounterTot + gameCheatCounterTot + gameFair2CounterTot;

	string gameSessionNumbers = std::to_string(gameSessionCounter);
	gameSessionNumbers += ": (";
	gameSessionNumbers += std::to_string((gameFair1CounterTot) ? gameFair1CounterTot-1 : gameFair1CounterTot);
	gameSessionNumbers += ", ";
	gameSessionNumbers += std::to_string(gameCheatCounterTot);
	gameSessionNumbers += ", ";
	gameSessionNumbers += std::to_string((gameFair2CounterTot >= 0) ? gameFair2CounterTot : 0);
	gameSessionNumbers += ") ";
	//gameSessionNumbers += std::to_string((gameMode != GAMEMODE_FAIR) ? gameCheatCounter-gameCheatCurrent : 0);
	gameSessionNumbers += std::to_string(gameCheatCounter);

	gameSessionNumbers += "\n";
	//for (int i = 0; i < 3; i++)
	//	gameSessionNumbers += std::to_string(gameWinCounts[i]);
	gameSessionNumbers += std::to_string(gameWinCounts[GAMERESULT_ROBOTWIN]);
	gameSessionNumbers += "/";
	gameSessionNumbers += std::to_string(gameWinCounts[GAMERESULT_TIE]);
	gameSessionNumbers += "/";
	gameSessionNumbers += std::to_string(gameWinCounts[GAMERESULT_PLAYERWIN]);

	gameCounterLabel->setText(QString::fromStdString(gameSessionNumbers));
	
	//gameCounterLabel->setText(QString::number(gameSessionCounter));

	gameUseBCI		= gameUseBCICheckBox->isChecked();
	gameUseKinect	= gameUseKinectCheckBox->isChecked();

	if (gameUseKinect && !kinectGuiStarted) {
		string command = "start ";
		command += KINECT_EXECUTABLE;
		system(command.c_str());
		kinectGuiStarted = true;
	}

	gameRunDone		= false;
	gameRunStarted	= true;

}

// SLB
void Gui::clickedGameExitButton() {
	updateStamp("Game: Game Session ended.\n");
	gameCounterLabel->setText("Done!");

	robotSpeak("thanksbye", 2.5); // 2
	gameDelay();
	//robotRawCommand("resetpose");
	robotPlaymotion("resetpose");

	gameRunDone = true;

	if (kinectGuiStarted) {
		string command = "taskkill /IM ";
		command += KINECT_EXECUTABLE;
		system(command.c_str());
		kinectGuiStarted = false;
	}

	//destroyTobii();

	initReceivedData();
	
}

// SLB
void Gui::clickedGameStartButton() {

	clickedGameApplyButton();
	
	gameDelay();
	//receivedDataN	= FILETYPE_GAME;
	receivedDataN = (gameMode != GAMEMODE_FAIR) ? GAMEMODE_ISCHEATTRIAL : FILETYPE_GAME;
	receivedDataN1	= GAMEMODE_INIT;

	// Initial sentence  最初はグー (only once per person)
	gamePlaySaishoWaGuu();

	// Actual move while saying じゃん拳ぽん！ (じゃん-拳-ぽん！)
	gamePlayMove();
}

// SLB
void Gui::clickedGameNextButton() {
	updateStamp("Game: Move accepted.");

	gameSetChoiceIcons(GAME_UNDEFINED);
	robotPlaymotion("resetpose");
	
	gameSessionCounter++;
	/*
	gameWinCounts[gameLastResult]++;

	string gameresult	= "\n Robot wins:\t" + std::to_string(gameWinCounts[GAMERESULT_ROBOTWIN]) + "\n";
	gameresult			+= " Ties:\t" + std::to_string(gameWinCounts[GAMERESULT_TIE]) + "\n";
	gameresult			+= " Player wins:\t" + std::to_string(gameWinCounts[GAMERESULT_PLAYERWIN]) + "\n";
	updateStamp(gameresult);
	*/

	// max iter criterion
	if (gameFair1CounterTot-1 > 0) {
		gameFair1CounterTot--;
		gameMode = GAMEMODE_FAIR;
	}
	else if (gameCheatCounterTot > 0) {
		gameCheatCounterTot--;
		if (gameRadioFair->isChecked() || (gameCheatCurrent >= gameCheatCounter)) // includes early-stop criterion
			gameMode = GAMEMODE_FAIR;
		else if (gameRadioCheatW->isChecked()) {
			//gameCheatCounterTot--;
			gameMode = GAMEMODE_CHEATWIN;
		}
		else if (gameRadioCheatL->isChecked()) {
			//gameCheatCounterTot--;
			gameMode = GAMEMODE_CHEATLOSE;
		}
	}
	// bitter-end criterion
	else if ((gameCheatCounterTot == 0) && (gameCheatCurrent < gameCheatCounter) &&
			(gameCheatEdit1->text().toInt() != 0) && (gameCheatEdit2->text().toInt() != 0)) {
				//gameFair2CounterTot--; // SLB TODO CHECK comment this to leave last n trials reserved as fair
				if (gameRadioCheatW->isChecked())
					gameMode = GAMEMODE_CHEATWIN;
				else if (gameRadioCheatL->isChecked())
					gameMode = GAMEMODE_CHEATLOSE;
	}
	//
	else if (gameFair2CounterTot > 0) {
		gameFair2CounterTot--;
		gameMode = GAMEMODE_FAIR;
	}

	gameUpdateCounterLabel();
	
	receivedDataN = GAMEMODE_NEXT;

	if ((gameFairEdit2->text().toInt() != 0) && (gameFair2CounterTot >= 0)) {
	//if (gameSessionCounter < gameFair2CounterTot + gameSessionCounterTot) {
	//if (gameSessionCounter < gameSessionCounterTot + 1) { // SLB TODO CHECK comment this to leave last n trials reserved as fair
	///if (gameFair1CounterTot+gameCheatCounterTot+gameFair2CounterTot > 0) {
		if (gameFair2CounterTot == 0)
			gameFair2CounterTot--;
		gamePlayMove();
	}
	/*
	else if (gameSessionCounter < gameSessionCounterTot + 1) {
		gamePlayMove();
	}
	*/
	else {
		
		clickedGameExitButton();
		/*
		updateStamp("Game: Game Session ended.\n");
		gameCounterLabel->setText("Done!");
		robotSpeak("goodbye", 1.5);
		gameDelay();
		robotPlaymotion("resetpose");
		gameRunDone = true;
		initReceivedData();
		*/

		Sleep(1000);

		gameSessionDialogControl->close();
	}
}

// SLB
void Gui::clickedGameSkipButton() {
	updateStamp("Game: Move discarded.");

	robotSpeak("skipdidntget", 4);

	gameWinCounts[gameLastResult]--;
	gameUpdateCounterLabel();

	receivedDataN = GAMEMODE_SKIP;

	if (gameMode != GAMEMODE_FAIR)
		gameCheatCurrent--;

	gamePlayMove();
}

// SLB
void Gui::gamePlaySaishoWaGuu() {
	updateStamp("Game: Game is starting...");

	// command to robot to play saisho wa guu
	//robotPlayvoice("saishowaguu");

	robotSpeak("rules", 15);

	// SLB add moves introduction demo

	//gameDelay();

	updateStamp("Game: Game started!\n");
}

// SLB
void Gui::gamePlayMove() {
	Sleep(100);

	//receivedDataN	= FILETYPE_GAME;
	receivedDataN = (gameMode != GAMEMODE_FAIR) ? GAMEMODE_ISCHEATTRIAL : FILETYPE_GAME;
	receivedDataN1	= GAMEMODE_INIT;

	//robotRawCommand("resetpose");
	robotPlaymotion("resetpose");

	// command to robot to play janken pon
	gamePlayJanKenPon();

	receivedDataN1	= GAMEMODE_ACTION;

	//srand(time(NULL));
	gameRobotChoice = (rand()+clock()) % 3;

	if (gameRobotChoice			== GAME_ROCK)
		gamePlayRock();			// command to robot to play rock
		
	else if (gameRobotChoice	== GAME_PAPER)
		gamePlayPaper();		// command to robot to play paper
		
	else if (gameRobotChoice	== GAME_SCISSORS)
		gamePlayScissors();		// command to robot to play scissors
		
	// virtually waits for r/p/s input from gui
	// or uses the kinect input

	// SLB TEST Kinect
	//listener->getMoveReceivedEvent().SetEvent(); // SLB check
	if (gameUseKinect = gameUseKinectCheckBox->isChecked()) { // possibly just use (gameUseKinect)
		
		WaitForSingleObject(listener->getMoveReceivedEvent().m_hObject, INFINITE);
		//while (WaitForSingleObject(listener->getMoveReceivedEvent().m_hObject, 0) == WAIT_OBJECT_0) {
		/*
		int rockcounter, papercounter, scissorscounter;
		//for (int i = 0; i < 50; i++) {
		while (true) {
			if (listener->getIsRockEvent()) {
				rockcounter++;
				listener->getIsRockEvent().ResetEvent();
			}
			if (listener->getIsPaperEvent()) {
				papercounter++;
				listener->getIsPaperEvent().ResetEvent();
			}
			if (listener->getIsScissorsEvent()) {
				scissorscounter++;
				listener->getIsScissorsEvent().ResetEvent();
			}

			if (rockcounter > 50) {
				gamePlayerChoice = GAME_ROCK;
				break;
			}
			if (papercounter > 50) {
				gamePlayerChoice = GAME_PAPER;
				break;
			}
			if (scissorscounter > 50) {
				gamePlayerChoice = GAME_SCISSORS;
				break;
			}
		}
		listener->getMoveReceivedEvent().ResetEvent();
		*/
		//
		int move		= -1;
		int initmove	= move;
		int movecounter	= 0;
		while (true) {
			if (WaitForSingleObject(listener->getMoveReceivedEvent().m_hObject, 0) == WAIT_OBJECT_0) {
				move = listener->getMove();
				if (move == initmove)
					movecounter++;
				else
					movecounter = 0;
			}
			initmove = move;
			if (movecounter > GAMEK_MINMOVES) {
				gamePlayerChoice = move;
				break;
			}
		}

		if (gamePlayerChoice == GAMEK_ROCK)
			clickedGameRockButton();
		else if (gamePlayerChoice == GAMEK_PAPER)
			clickedGamePaperButton();
		else if (gamePlayerChoice == GAMEK_SCISSORS)
			clickedGameScissorsButton();
		
	}
}

// SLB
void Gui::gamePlayJanKenPon() {
	string throwingMove	= "Game: Throwing move ";
	throwingMove		+= std::to_string(gameSessionCounter);
	throwingMove		+= "...";
	updateStamp(throwingMove);

	// command to robot to play janken pon
	//robotPlayvoice("jankenpon");

	robotSpeak("throwmove", 4.5);
	//robotPlayvoice("introduction");
	
	//gameDelay();
}

// SLB
void Gui::gamePlayRock() {
	// command to robot to play rock
	robotPlaymotion("playrock");
	//robotSpeak("robot-rock", 1.5);
	
	robotPlayvoice("robot-rock");
	//gameDelay();

	updateStamp("Game: The robot has chosen ROCK.");
}

// SLB
void Gui::gamePlayPaper2Rock() {
	// command to robot to play rock
	gameDelay();
	robotPlayvoice("changedmind");
	robotPlaymotion("playpaper2rock");
	//robotSpeak("robot-rock", 1.5);
	//Sleep(1000);
	gameDelay();
	robotPlayvoice("robot-rock");
	gameDelay();

	updateStamp("Game: The robot has chosen ROCK.");
}

// SLB
void Gui::gamePlayScissors2Rock() {
	// command to robot to play rock
	gameDelay();
	robotPlayvoice("changedmind");
	robotPlaymotion("playscissors2rock");
	//robotSpeak("robot-rock", 1.5);
	//Sleep(1000);
	gameDelay();
	robotPlayvoice("robot-rock");
	gameDelay();

	updateStamp("Game: The robot has chosen ROCK.");
}

// SLB
void Gui::gamePlayPaper() {
	// command to robot to play paper
	robotPlaymotion("playpaper");
	//robotSpeak("robot-paper", 1.5);
	
	robotPlayvoice("robot-paper");
	//gameDelay();

	updateStamp("Game: The robot has chosen PAPER.");
}

// SLB
void Gui::gamePlayRock2Paper() {
	// command to robot to play rock
	gameDelay();
	robotPlayvoice("changedmind");
	robotPlaymotion("playrock2paper");
	//robotSpeak("robot-paper", 1.5);
	//Sleep(1000);
	gameDelay();
	robotPlayvoice("robot-paper");
	gameDelay();

	updateStamp("Game: The robot has chosen PAPER.");
}

// SLB
void Gui::gamePlayScissors2Paper() {
	// command to robot to play rock
	gameDelay();
	robotPlayvoice("changedmind");
	robotPlaymotion("playscissors2paper");
	//robotSpeak("robot-paper", 1.5);
	//Sleep(1000);
	gameDelay();
	robotPlayvoice("robot-paper");
	gameDelay();

	updateStamp("Game: The robot has chosen PAPER.");
}

// SLB
void Gui::gamePlayScissors() {
	// command to robot to play scissors
	robotPlaymotion("playscissors");
	//robotSpeak("robot-scissors", 1.5);
	
	robotPlayvoice("robot-scissors");
	//gameDelay();

	updateStamp("Game: The robot has chosen SCISSORS.");
}

// SLB
void Gui::gamePlayRock2Scissors() {
	// command to robot to play rock
	gameDelay();
	robotPlayvoice("changedmind");
	robotPlaymotion("playrock2scissors");
	//robotSpeak("robot-scissors", 1.5);
	//Sleep(1000);
	gameDelay();
	robotPlayvoice("robot-scissors");
	gameDelay();

	updateStamp("Game: The robot has chosen SCISSORS.");
}

// SLB
void Gui::gamePlayPaper2Scissors() {
	// command to robot to play rock
	gameDelay();
	robotPlayvoice("changedmind");
	robotPlaymotion("playpaper2scissors");
	//robotSpeak("robot-scissors", 1.5);
	//Sleep(1000);
	gameDelay();
	robotPlayvoice("robot-scissors");
	gameDelay();

	updateStamp("Game: The robot has chosen SCISSORS.");
}

// SLB
void Gui::clickedGameRockButton() {
	gamePlayerChoice = GAME_ROCK;
	gameSetChoiceIcons(gamePlayerChoice);

	gameDelay();
	//robotSpeak("player-rock", 1.5);
	robotPlayvoice("player-rock");
	
	//cout << gamePlayerChoice << endl;
	updateStamp("Game: The player has chosen ROCK.\n");

	gameTrialClosing();
}

// SLB
void Gui::clickedGamePaperButton() {
	gamePlayerChoice = GAME_PAPER;
	gameSetChoiceIcons(gamePlayerChoice);

	gameDelay();
	//robotSpeak("player-paper", 1.5);
	robotPlayvoice("player-paper");

	//cout << gamePlayerChoice << endl;
	updateStamp("Game: The player has chosen PAPER.\n");

	gameTrialClosing();
}

// SLB
void Gui::clickedGameScissorsButton() {
	gamePlayerChoice = GAME_SCISSORS;
	gameSetChoiceIcons(gamePlayerChoice);
	
	gameDelay();
	//robotSpeak("player-scissors", 1.5);
	robotPlayvoice("player-scissors");
	
	//cout << gamePlayerChoice << endl;
	updateStamp("Game: The player has chosen SCISSORS.\n");

	gameTrialClosing();
}

// SLB
void Gui::gameTrialClosing() {
	//Sleep(2000); // TEST Telenoid

	//receivedDataN1 = gameMode;

	///gameDelay();
	//receivedDataN1 = GAMEMODE_FAIR;

	if (gameMode == GAMEMODE_FAIR) {
		gameDelay();
		receivedDataN1 = gameMode;

		if (gamePlayerChoice == gameRobotChoice) {
			gamePlayTieSentence();
		}
		else {

			if (gamePlayerChoice == GAME_ROCK) {
				if (gameRobotChoice == GAME_PAPER)
					gamePlayWinSentence();
				else if (gameRobotChoice == GAME_SCISSORS)
					gamePlayLoseSentence();
			}
			else if (gamePlayerChoice == GAME_PAPER) {
				if (gameRobotChoice == GAME_ROCK)
					gamePlayLoseSentence();
				else if (gameRobotChoice == GAME_SCISSORS)
					gamePlayWinSentence();
			}
			else if (gamePlayerChoice == GAME_SCISSORS) {
				if (gameRobotChoice == GAME_ROCK)
					gamePlayWinSentence();
				else if (gameRobotChoice == GAME_PAPER)
					gamePlayLoseSentence();
			}

		}
	}
	else if (gameMode == GAMEMODE_CHEATWIN) {

		if (gamePlayerChoice == GAME_ROCK && gameRobotChoice != GAME_PAPER) {
			receivedDataN1 = gameMode;
			gameCheatCurrent++;
			updateStamp("Game: The robot has CHEATED " + std::to_string(gameCheatCurrent) + ((gameCheatCurrent<2) ? " time!" : " times!"));
			//gameDelay();
			//gamePlayPaper();
			if (gameRobotChoice == GAME_ROCK)
				gamePlayRock2Paper();
			else
				gamePlayScissors2Paper();
		}
		else if (gamePlayerChoice == GAME_PAPER && gameRobotChoice != GAME_SCISSORS) {
			receivedDataN1 = gameMode;
			gameCheatCurrent++;
			updateStamp("Game: The robot has CHEATED " + std::to_string(gameCheatCurrent) + ((gameCheatCurrent<2) ? " time!" : " times!"));
			//gameDelay();
			//gamePlayScissors();
			if (gameRobotChoice == GAME_ROCK)
				gamePlayRock2Scissors();
			else
				gamePlayPaper2Scissors();
		}
		else if (gamePlayerChoice == GAME_SCISSORS && gameRobotChoice != GAME_ROCK) {
			receivedDataN1 = gameMode;
			gameCheatCurrent++;
			updateStamp("Game: The robot has CHEATED " + std::to_string(gameCheatCurrent) + ((gameCheatCurrent<2) ? " time!" : " times!"));
			//gameDelay();
			//gamePlayRock();
			if (gameRobotChoice == GAME_PAPER)
				gamePlayPaper2Rock();
			else
				gamePlayScissors2Rock();
		}
		else {
			receivedDataN1 = GAMEMODE_FAIR;
			gameDelay();
		}

		gamePlayWinSentence();

	}
	else if (gameMode == GAMEMODE_CHEATLOSE) {

		if (gamePlayerChoice == GAME_ROCK && gameRobotChoice != GAME_SCISSORS) {
			receivedDataN1 = gameMode;
			gameCheatCurrent++;
			updateStamp("Game: The robot has CHEATED " + std::to_string(gameCheatCurrent) + ((gameCheatCurrent<2) ? " time!" : " times!"));
			//gameDelay();
			//gamePlayScissors();
			if (gameRobotChoice == GAME_ROCK)
				gamePlayRock2Scissors();
			else
				gamePlayPaper2Scissors();
		}
		else if (gamePlayerChoice == GAME_PAPER && gameRobotChoice != GAME_ROCK) {
			receivedDataN1 = gameMode;
			gameCheatCurrent++;
			updateStamp("Game: The robot has CHEATED " + std::to_string(gameCheatCurrent) + ((gameCheatCurrent<2) ? " time!" : " times!"));
			//gameDelay();
			//gamePlayRock();
			if (gameRobotChoice == GAME_PAPER)
				gamePlayPaper2Rock();
			else
				gamePlayScissors2Rock();
		}
		else if (gamePlayerChoice == GAME_SCISSORS && gameRobotChoice != GAME_PAPER) {
			receivedDataN1 = gameMode;
			gameCheatCurrent++;
			updateStamp("Game: The robot has CHEATED " + std::to_string(gameCheatCurrent) + ((gameCheatCurrent<2) ? " time!" : " times!"));
			//gameDelay();
			//gamePlayPaper();
			if (gameRobotChoice == GAME_ROCK)
				gamePlayRock2Paper();
			else
				gamePlayScissors2Paper();
		}
		else {
			receivedDataN1 = GAMEMODE_FAIR;
			gameDelay();
		}

		gamePlayLoseSentence();

	}

	receivedDataN1 = GAMEMODE_FAIR;

	gameDelay();
	
	//receivedDataN1 = GAMEMODE_BASELINE;
	receivedDataN1 = GAMEMODE_TRIALEND;

	//
	gameWinCounts[gameLastResult]++;

	string gameresult	= " Robot wins:\t" + std::to_string(gameWinCounts[GAMERESULT_ROBOTWIN]) + "\n";
	gameresult			+= " Ties:\t" + std::to_string(gameWinCounts[GAMERESULT_TIE]) + "\n";
	gameresult			+= " Player wins:\t" + std::to_string(gameWinCounts[GAMERESULT_PLAYERWIN]) + "\n";
	updateStamp(gameresult);
	
	gameUpdateCounterLabel();
	
}

// SLB
void Gui::gamePlayTieSentence() {
	gameLastResult = GAMERESULT_TIE;

	// command to robot to play sentence
	//robotPlayvoice("resulttie");
	///robotPlaymotion("resetpose");
	//gameDelay();
	robotSpeak("tie", 1.5);
	robotPlaymotion("resetpose");

	updateStamp("Game: This is a Tie.\n");
}

// SLB
void Gui::gamePlayWinSentence() {
	gameLastResult = GAMERESULT_ROBOTWIN;

	// command to robot to play sentence
	//robotPlayvoice("resultwin");
	///robotPlaymotion("resetpose");
	//gameDelay();
	/*
	robotSpeak("robot-win", 1.5);
	robotPlaymotion("resetpose");
	*/
	robotPlaymotion("angry3");
	robotPlayvoice("robot-win");
	//gameDelay();
	robotPlaymotion("resetpose");
	//
	updateStamp("Game: The robot has Won!\n");
}

// SLB
void Gui::gamePlayLoseSentence() {
	gameLastResult = GAMERESULT_PLAYERWIN;

	// command to robot to play sentence
	//robotPlayvoice("resultlose");
	/*
	robotPlaymotion("resetpose");
	//gameDelay();
	robotSpeak("robot-lose", 1.5);
	*/
	//
	robotPlaymotion("sadness2");
	robotPlayvoice("robot-lose");
	gameDelay();
	//robotPlaymotion("resetpose");
	//

	updateStamp("Game: The robot has Lost.\n");
}

// SLB
void Gui::gameUpdateCounterLabel() {
	string gameSessionNumbers = std::to_string(gameSessionCounter);
	gameSessionNumbers += ": (";
	gameSessionNumbers += std::to_string((gameFair1CounterTot) ? gameFair1CounterTot-1 : gameFair1CounterTot);
	gameSessionNumbers += ", ";
	gameSessionNumbers += std::to_string(gameCheatCounterTot);
	gameSessionNumbers += ", ";
	gameSessionNumbers += std::to_string((gameFair2CounterTot >= 0) ? gameFair2CounterTot : 0);
	gameSessionNumbers += ") ";
	gameSessionNumbers += std::to_string((gameMode != GAMEMODE_FAIR) ? gameCheatCounter-gameCheatCurrent : 0);

	gameSessionNumbers += "\n";
	//for (int i = 0; i < 3; i++)
	//	gameSessionNumbers += std::to_string(gameWinCounts[i]);
	gameSessionNumbers += std::to_string(gameWinCounts[GAMERESULT_ROBOTWIN]);
	gameSessionNumbers += "/";
	gameSessionNumbers += std::to_string(gameWinCounts[GAMERESULT_TIE]);
	gameSessionNumbers += "/";
	gameSessionNumbers += std::to_string(gameWinCounts[GAMERESULT_PLAYERWIN]);
	
	gameCounterLabel->setText(QString::fromStdString(gameSessionNumbers));
	
	//gameCounterLabel->setText(QString::number(gameSessionCounter));	
}

// SLB
void Gui::gameDelay() {
	Sleep(1800); // SLB remove
}

// SLB TODO remove
void Gui::getSkillSliderValue(int) {
	///skillSliderValue = skillSlider->value();
	skillSliderValue = skillSlider->value()*10;
	//cout << skillSliderValue << endl;
}

// SLB
void Gui::getSignalPlotGainValue(int) {
	signalPlotGain = signalPlotGainSlider->value();
	//cout << signalPlotGain << endl;
}

// SLB
void Gui::eyeIconClicked() {

	skillSlider->setValue(0);

}

// SLB
void Gui::brainIconClicked() {

	skillSlider->setValue(10);

}

void Gui::readConfiguration() {

	//initMapVel();

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	
	if (result) {
		username					= (doc.child("name").first_child().value());
		number_of_calibration_tag	= strlen(doc.child("string").first_child().value());
		mode						= atoi(doc.child("mode").first_child().value()); // SLB
		//(mode==0) ? flashingMode=false : flashingMode=true; // SLB

		number_of_row_interface		= atoi(doc.child("row_num_interface").first_child().value());
		number_of_flash				= atoi(doc.child("num_flash").first_child().value());

		time_flash_auto				= (bool) atoi(doc.child("time_flash_auto").first_child().value()); // SLB
		//if (!time_flash_auto)
			velocita = atoi(doc.child("time_flash").first_child().value());
		
		number_channels				= atoi(doc.child("num_ch").first_child().value()); // SLB
		number_channels_plus		= atoi(doc.child("num_ch_plus").first_child().value()); // SLB
		
		target_of_calibration		= new int[number_of_calibration_tag];
		
		pugi::xml_node tmp			= doc.child("target");
		int i=0;
		for (pugi::xml_node tool = tmp.child("t"); tool; tool = tool.next_sibling("t")) {
			target_of_calibration[i] = atoi(tool.first_child().value());
			i++;
		}
		
		// SLB
		symbolicMode					= (bool) atoi(doc.child("symbolicmode").first_child().value());
		if (symbolicMode) {
			number_of_row_interface = 4;
			elements = 16;
		}
		
		number_of_tags				= number_of_row_interface*number_of_row_interface;
		//

		customPalette				= (bool) atoi(doc.child("palette").first_child().value());
		optFlash					= (bool) atoi(doc.child("optimization").first_child().value()); // SLB added bool cast
		userobot					= (bool) atoi(doc.child("userobot").first_child().value()); // SLB
		ipRobot					= doc.child("iprobot").first_child().value();
		portRobot				= doc.child("portrobot").first_child().value();

		skillSliderValue			= atoi(doc.child("skillslidervalue").first_child().value()); // SLB
		
		tobiienabled				= (bool) atoi(doc.child("tobiienabled").first_child().value()); // SLB
		tobiigazeenabled			= (bool) atoi(doc.child("tobiigazeenabled").first_child().value()); // SLB
		tobiifiltered				= (bool) atoi(doc.child("tobiifiltered").first_child().value()); // SLB

		bioHybridMode				= (bool) atoi(doc.child("biohybridmode").first_child().value()); // SLB

		impaired_letterCol			= doc.child("letter_color").first_child().value(); // SLB
		impaired_bgCol				= doc.child("background_color").first_child().value(); // SLB
		impaired_theme				= impaired_letterCol + "_" + impaired_bgCol; // SLB
		palette_string				= doc.child("palette_string").first_child().value(); // SLB
		std::transform(palette_string.begin(), palette_string.end(), palette_string.begin(), toupper);

		language					= doc.child("language").first_child().value(); // SLB
		
	}
	else
		cout << "Could not find configuration file." << endl;
		// SLB
		initiateConfiguration();

};


void Gui::initmapping(int squaresNumber) {

	for (int i=0; i<squaresNumber; i++) {
			string name;
			char tmp = ' ';
			if (i <= 25)
				tmp = 'A' + i;
			 else
				tmp = '0' + i - 26;
			stringstream ss;
			ss << tmp;
			ss >> name;
			mappingIdTag.insert(pair<int, string>(i, name)); 
		}
}

void Gui::initMapVel() {

	mappingVel.insert(pair<string, int>("20",200));
	mappingVel.insert(pair<string, int>("21",200));
	mappingVel.insert(pair<string, int>("30",150));
	mappingVel.insert(pair<string, int>("31",170));
	mappingVel.insert(pair<string, int>("40",100));
	mappingVel.insert(pair<string, int>("41",125));
	mappingVel.insert(pair<string, int>("50",200));
	mappingVel.insert(pair<string, int>("51",125));
	mappingVel.insert(pair<string, int>("60",200));
	mappingVel.insert(pair<string, int>("61",125));
}

void Gui::updateConfiguration() {

	initmapping(number_of_tags);
	initMapVel();
	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	
	if (result) {
		// SLB
		//doc.remove_child("name");
		//doc.append_child("name").append_child(pugi::node_pcdata).set_value(q_username.toLocal8Bit().constData());
		////pugi::xml_node node = doc.child("name");
		////node.first_child().set_value(q_username.toLocal8Bit().constData());
		pugi::xml_node node = doc.child("name");
		if (node.first_child().empty())
			node.append_child(pugi::node_pcdata).set_value(q_username.toLocal8Bit().constData());
		else
			node.first_child().set_value(q_username.toLocal8Bit().constData());
		
		node = doc.child("mode");
		string modality = std::to_string(mode); // SLB
		node.first_child().set_value(std::to_string(mode).c_str()); // SLB

		node = doc.child("row_num_interface");
		string rows = std::to_string(number_of_row_interface); // SLB
		node.first_child().set_value(rows.c_str()); // SLB

		node = doc.child("target");
		pugi::xml_node children = node.child("t");
		while (children) {
			node.remove_child("t");
			children = node.child("t");
		}

		string stringa = "";
		for (int i=0; i<comboTarget.size(); i++) {
			children = node.append_child("t").append_child(pugi::node_pcdata); // SLB
			children.set_value(std::to_string(comboTarget[i]->currentIndex()+1).c_str()); // SLB
			stringa += mappingIdTag[comboTarget[i]->currentIndex()];
		}
		node = doc.child("string");
		node.first_child().set_value(stringa.c_str()); // SLB

		node = doc.child("num_flash");
		node.first_child().set_value(std::to_string(number_of_flash).c_str()); // SLB

		node = doc.child("num_ch");
		node.first_child().set_value(std::to_string(number_channels).c_str()); // SLB
		node = doc.child("num_ch_plus");
		node.first_child().set_value(std::to_string(number_channels_plus).c_str()); // SLB

		node = doc.child("symbolicmode");
		//int sensorModeInt = symbolicMode ? sensorModeInt=1 : sensorModeInt=0;
		//node.first_child().set_value(std::to_string(sensorModeInt).c_str()); // SLB
		node.first_child().set_value(BoolToString(symbolicMode).c_str()); // SLB
		
		node = doc.child("palette");
		//int visualModeInt = customPalette ? visualModeInt=1 : visualModeInt=0;
		//node.first_child().set_value(std::to_string(visualModeInt).c_str()); // SLB
		node.first_child().set_value(BoolToString(customPalette).c_str()); // SLB

		node = doc.child("optimization");
		//node.first_child().set_value(std::to_string(optFlash).c_str()); // SLB
		checkFlashOpt->isChecked() ? optFlash=true : optFlash=false; // SLB
		node.first_child().set_value(BoolToString(optFlash).c_str()); // SLB

		// SLB
		node = doc.child("skillslidervalue");
		node.first_child().set_value(std::to_string(skillSliderValue).c_str()); // SLB
		
		// SLB
		node = doc.child("tobiienabled");
		eyeTrackerCheckBox->isChecked() ? tobiienabled=true : tobiienabled=false;
		//int tobiienabledint = tobiienabled ? tobiienabledint=1 : tobiienabledint=0;
		if (node.first_child().empty())
			//node.append_child(pugi::node_pcdata).set_value(std::to_string(tobiienabledint).c_str()); // SLB
			node.append_child(pugi::node_pcdata).set_value(BoolToString(tobiienabled).c_str()); // SLB
		else
			//node.first_child().set_value(std::to_string(tobiienabledint).c_str()); // SLB
			node.first_child().set_value(BoolToString(tobiienabled).c_str()); // SLB
		// SLB
		node = doc.child("tobiigazeenabled");
		eyeTrackerGazeCheckBox->isChecked() ? tobiigazeenabled=true : tobiigazeenabled=false;
		//int tobiigazeenabledint = tobiigazeenabled ? tobiigazeenabledint=1 : tobiigazeenabledint=0;
		if (node.first_child().empty())
			//node.append_child(pugi::node_pcdata).set_value(std::to_string(tobiigazeenabledint).c_str()); // SLB
			node.append_child(pugi::node_pcdata).set_value(BoolToString(tobiigazeenabled).c_str()); // SLB
		else
			//node.first_child().set_value(std::to_string(tobiigazeenabledint).c_str()); // SLB
			node.first_child().set_value(BoolToString(tobiigazeenabled).c_str()); // SLB
		// SLB
		node = doc.child("tobiifiltered");
		tobiiFilteredRadio->isChecked() ? tobiifiltered=true : tobiifiltered=false;
		tobiiRawRadio->isChecked() ? tobiifiltered=false : tobiifiltered=true;
		//int tobiifilteredint = tobiifiltered ? tobiifilteredint=1 : tobiifilteredint=0;
		if (node.first_child().empty())
			//node.append_child(pugi::node_pcdata).set_value(std::to_string(tobiifilteredint).c_str()); // SLB
			node.append_child(pugi::node_pcdata).set_value(BoolToString(tobiifiltered).c_str()); // SLB
		else
			//node.first_child().set_value(std::to_string(tobiifilteredint).c_str()); // SLB
			node.first_child().set_value(BoolToString(tobiifiltered).c_str()); // SLB
		// SLB
		node = doc.child("biohybridmode");
		radioHybrid->isChecked() ? bioHybridMode=false : bioHybridMode=true;
		radioBioHybrid->isChecked() ? bioHybridMode=true : bioHybridMode=false;
		if (node.first_child().empty())
			node.append_child(pugi::node_pcdata).set_value(BoolToString(bioHybridMode).c_str()); // SLB
		else
			node.first_child().set_value(BoolToString(bioHybridMode).c_str()); // SLB
		
		node = doc.child("elements");
		elements = (comboInterfaccia->currentIndex()+2)*(comboInterfaccia->currentIndex()+2);
		node.first_child().set_value(std::to_string(elements).c_str());

		node = doc.child("time_flash_auto"); // SLB
		node.first_child().set_value(BoolToString(time_flash_auto).c_str()); // SLB

		if (time_flash_auto)
			velocita = mappingVel[rows+modality];

		node = doc.child("time_flash");
		node.first_child().set_value(std::to_string(velocita).c_str());
		
		node = doc.child("iprobot");
		node.first_child().set_value(ipRobot.c_str());

		node = doc.child("portrobot");
		node.first_child().set_value(portRobot.c_str());

		/*
		node = doc.child("ipnao");
		node.first_child().set_value(ipnao.c_str());

		node = doc.child("portnao");
		node.first_child().set_value(portnao.c_str());
		*/

		// SLB
		node = doc.child("letter_color");
		node.first_child().set_value(impaired_letterCol.c_str());
		node = doc.child("background_color");
		node.first_child().set_value(impaired_bgCol.c_str());

		// SLB
		node = doc.child("palette_string");
		if (node.first_child().empty())
			node.append_child(pugi::node_pcdata).set_value(palette_string.c_str());
		else
			node.first_child().set_value(palette_string.c_str());
		
		// SLB
		node = doc.child("texture");
		children = node.child("p");
		while (children) {
			node.remove_child("p");
			children = node.child("p");
		}
		for (int i=0; i<palette_string.size(); i++) {
			string textureString(palette_string.substr(i, 1));
			node.append_child("p").append_child(pugi::node_pcdata).set_value(textureString.c_str());
		}

		//doc.save_file("./configuration.xml");
		doc.save_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	}
	else
		cout << "Could not find configuration file." << endl;
}

// SLB
void Gui::initiateConfiguration() {

	// CONFIGURATION
	string texturesString = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

	if (!result) {

		cout << "Creating default configuration file..." << endl;
	
		doc.append_child("name").append_child(pugi::node_pcdata);
		doc.append_child("time_flash_auto").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("time_flash").append_child(pugi::node_pcdata).set_value("125");
		doc.append_child("mode").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("row_num_interface").append_child(pugi::node_pcdata).set_value("4");
		doc.append_child("elements").append_child(pugi::node_pcdata).set_value("16");
		doc.append_child("window").append_child(pugi::node_pcdata).set_value("200");
		doc.append_child("num_flash").append_child(pugi::node_pcdata).set_value("4");
		doc.append_child("num_ch").append_child(pugi::node_pcdata).set_value("4");
		doc.append_child("num_ch_plus").append_child(pugi::node_pcdata).set_value("0");
		doc.append_child("samplerate").append_child(pugi::node_pcdata).set_value("256");
		doc.append_child("signalgain").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("symbolicmode").append_child(pugi::node_pcdata).set_value("0");
		doc.append_child("palette").append_child(pugi::node_pcdata).set_value("0");
		doc.append_child("skillslidervalue").append_child(pugi::node_pcdata).set_value("50");
		doc.append_child("tobiienabled").append_child(pugi::node_pcdata).set_value("0");
		doc.append_child("tobiigazeenabled").append_child(pugi::node_pcdata).set_value("0");
		doc.append_child("tobiifiltered").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("tobiiwindowcoord").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("tobiishowinnersquare").append_child(pugi::node_pcdata).set_value("0");
		doc.append_child("tobiiinnersquarepc").append_child(pugi::node_pcdata).set_value("25");
		doc.append_child("biohybridmode").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("userobot").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("iprobot").append_child(pugi::node_pcdata).set_value("192.168.1.4");
		doc.append_child("portrobot").append_child(pugi::node_pcdata).set_value("12000");
		doc.append_child("calibration_analysis").append_child(pugi::node_pcdata);
		doc.append_child("optimization").append_child(pugi::node_pcdata).set_value("0"); // -842150451
		doc.append_child("letter_color").append_child(pugi::node_pcdata).set_value("black");
		doc.append_child("background_color").append_child(pugi::node_pcdata).set_value("orange");
		doc.append_child("flashing_bmptexture").append_child(pugi::node_pcdata).set_value("einstein");
		doc.append_child("language").append_child(pugi::node_pcdata).set_value("en");
		doc.append_child("binfilename").append_child(pugi::node_pcdata).set_value("");
		
		doc.append_child("string").append_child(pugi::node_pcdata).set_value("AB");
		doc.append_child("target").append_child("t").append_child(pugi::node_pcdata).set_value("1");
		doc.child("target").append_child("t").append_child(pugi::node_pcdata).set_value("2");
		
		doc.append_child("palette_string").append_child(pugi::node_pcdata).set_value(texturesString.c_str());;
		
		doc.append_child("texture");
		pugi::xml_node texture = doc.child("texture");
		for (int i=0; i<texturesString.size(); i++) {
			string textureString(texturesString.substr(i, 1));
			texture.append_child("p").append_child(pugi::node_pcdata).set_value(textureString.c_str());
		}

		//doc.append_child("RESULTS").append_child(pugi::node_pcdata).set_value("----------");

		doc.append_child("flash_analysis").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("flash_precision").append_child(pugi::node_pcdata).set_value("0");

		doc.append_child("stimuli_entropy").append_child(pugi::node_pcdata).set_value("-1");
		doc.append_child("stimuli_energy").append_child(pugi::node_pcdata).set_value("-1");

		doc.append_child("epy_cum").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("egy_cum").append_child(pugi::node_pcdata).set_value("-1");
		doc.append_child("epy_min").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("epy_max").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("egy_min").append_child(pugi::node_pcdata).set_value("-1");
		doc.append_child("egy_max").append_child(pugi::node_pcdata).set_value("-1");
		doc.append_child("epy_norm").append_child(pugi::node_pcdata).set_value("1");
		doc.append_child("egy_norm").append_child(pugi::node_pcdata).set_value("-1");
		
		doc.append_child("rsq").append_child(pugi::node_pcdata).set_value("-1");
		doc.append_child("rsq_cum").append_child(pugi::node_pcdata).set_value("-1");
		//doc.append_child("rsq_gap").append_child(pugi::node_pcdata).set_value("-1");
		doc.append_child("rsq_min").append_child(pugi::node_pcdata).set_value("-1");
		doc.append_child("rsq_max").append_child(pugi::node_pcdata).set_value("-1");
		doc.append_child("rsq_norm").append_child(pugi::node_pcdata).set_value("-1");
		
		doc.append_child("tobiiresultID").append_child(pugi::node_pcdata).set_value("-1");
		
		doc.append_child("tobiiresultIDs");
		pugi::xml_node resultID = doc.child("tobiiresultIDs");
		resultID.append_child("resultID").append_child(pugi::node_pcdata).set_value("-1");

		doc.append_child("tobiiresultPC").append_child(pugi::node_pcdata).set_value("0");

		doc.append_child("tobiiresultPCs");
		pugi::xml_node resultPC = doc.child("tobiiresultPCs");
		resultPC.append_child("resultPC").append_child(pugi::node_pcdata).set_value("0");

		doc.append_child("selectedID").append_child(pugi::node_pcdata).set_value("-1");
		doc.append_child("threshold_passed").append_child(pugi::node_pcdata).set_value("0");
		doc.append_child("behaviour_intensity").append_child(pugi::node_pcdata).set_value("0");
		
		//doc.save_file("./configuration.xml");
		doc.save_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
		readConfiguration();
	}

	// MEMORY
	//result = doc.load_file("./memory.xml");
	result = doc.load_file((configFilesRoot + "/memory.xml").c_str()); // SLB

	if (!result) {

		cout << "Creating default memory file..." << endl;

		doc.append_child("window").append_child(pugi::node_pcdata).set_value("0");
		doc.append_child("door").append_child(pugi::node_pcdata).set_value("0");
		doc.append_child("tv").append_child(pugi::node_pcdata).set_value("0");
		doc.append_child("radio").append_child(pugi::node_pcdata).set_value("0");
		doc.append_child("light").append_child(pugi::node_pcdata).set_value("0");

		//doc.save_file("./memory.xml");
		doc.save_file((configFilesRoot + "/memory.xml").c_str()); // SLB
	}

	// TRAINING
	if (access((configFilesRoot + "/training.txt").c_str(), 0) != 0) {

		cout << "Creating default training file..." << endl;

		const char* trainchar[] = {"046 -", "047 +", "146 -", "147 +", "246 -", "247 +", "346 *", "347 /",
								"056 !", "057 !", "156 !", "157 !", "256 !", "257 !", "356 ?", "357 ?"};
		vector<string> train(trainchar, std::end(trainchar));

		FILE *f;
		f = fopen((configFilesRoot + "/training.txt").c_str(), "a+");
		if (f==0) {
			cout << "Error opening training file.\n";
		}
		else {
			for (int i=0; i<train.size()-1; i++) {
				fprintf(f, "%s\n", train[i].c_str());
			}
			fprintf(f, "%s", train[train.size()-1].c_str());
			
			fflush(f);
			fclose(f);
		}
	}

}

// SLB
void Gui::initiateOperation() {
	
	guiElementsSetEnabled(false);
	currentRunDone = false;
	readConfiguration();
}

// SLB
void Gui::terminateOperation() {
	
	while (!currentRunDone) {}
	currentRunDone = false;
	guiElementsSetEnabled(true);
}

// SLB
void Gui::showGameSessionDialogControl() {

	gameSessionDialogControl = new QDialog();
	gameSessionDialogControl->setWindowIcon(QIcon(FRAMEWORKBLUEICONPATH));

	QPalette *palette = new QPalette();	
	palette->setBrush(QPalette::Background, *(new QBrush(*(new QPixmap(FRAMEWORKBGIMAGE)))));
	gameSessionDialogControl->setPalette(*palette);

	gameSessionDialogControl->setWindowTitle("Game Session controller");
	gameSessionDialogControl->setAttribute(Qt::WA_DeleteOnClose);
	gameSessionDialogControl->setWindowFlags(Qt::FramelessWindowHint); 
	gameSessionDialogControl->setWindowFlags(Qt::WindowTitleHint); 
	int dialogWidth		= 550;
	int dialogHeight	= 450;
	//gameSessionDialogControl->setMinimumWidth(dialogWidth);
	//gameSessionDialogControl->setMinimumHeight(dialogHeight);
	gameSessionDialogControl->setFixedSize(dialogWidth, dialogHeight);

	QVBoxLayout *gameMainLayout			= new QVBoxLayout();
	QGridLayout *gameMainTopLayout1		= new QGridLayout();
	QGridLayout *gameMainTopLayout2		= new QGridLayout();
	QVBoxLayout *gameMainBottomLayout	= new QVBoxLayout();

	//QVBoxLayout *gameTopLeftLayout		= new QVBoxLayout();
	QVBoxLayout *gameTopRightLayout		= new QVBoxLayout();

	QGridLayout *gameTopRightLine1		= new QGridLayout();
	QGridLayout *gameTopRightLine2		= new QGridLayout();
	QGridLayout *gameTopRightLine3		= new QGridLayout();

	//QHBoxLayout *gameCommandButtons		= new QHBoxLayout();

	QVBoxLayout *gameCommandButtonsLeft	= new QVBoxLayout();

	QHBoxLayout *gameBottomLayout1		= new QHBoxLayout();
	QHBoxLayout *gameBottomLayout2		= new QHBoxLayout();

	// TopLeft
	QGroupBox *gameModeBox = new QGroupBox(tr("Modality:"));

	gameRadioFair = new QRadioButton(tr("&Fair"));
	gameRadioCheatW = new QRadioButton(tr("Cheat to &Win"));
	gameRadioCheatL = new QRadioButton(tr("Cheat to &Lose"));
	gameRadioFair->setChecked(true);

	connect(gameRadioFair, SIGNAL(clicked(bool)),this,SLOT(clickedGameMode(bool)));
	connect(gameRadioCheatW, SIGNAL(clicked(bool)),this,SLOT(clickedGameMode(bool)));
	connect(gameRadioCheatL, SIGNAL(clicked(bool)),this,SLOT(clickedGameMode(bool)));

	QVBoxLayout *gameModeLayout	= new QVBoxLayout();

	gameModeLayout->addWidget(gameRadioFair);
	gameModeLayout->addWidget(gameRadioCheatW);
	gameModeLayout->addWidget(gameRadioCheatL);

	gameModeBox->setLayout(gameModeLayout);

	QPushButton *gameApplyButton = new QPushButton(tr("&Apply"));
	QPushButton *gameExitButton = new QPushButton(tr("E&xit"));
	QPushButton *gameStartButton = new QPushButton(tr("&START"));
	gameApplyButton->setMinimumHeight(30);
	gameExitButton->setMinimumHeight(30);
	gameStartButton->setMinimumHeight(89); // 66

	connect(gameApplyButton, SIGNAL(clicked()), this, SLOT(clickedGameApplyButton()));
	connect(gameStartButton, SIGNAL(clicked()), this, SLOT(clickedGameStartButton()));
	connect(gameExitButton, SIGNAL(clicked()), this, SLOT(clickedGameExitButton()));
	connect(gameExitButton, SIGNAL(clicked()), gameSessionDialogControl, SLOT(close()));

	gameUseBCICheckBox = new QCheckBox(tr("Use &BCI"));
	//gameUseBCICheckBox->setLayoutDirection(Qt::RightToLeft);
	gameUseBCICheckBox->setChecked(true);
	gameUseBCI = gameUseBCICheckBox->isChecked();
	connect(gameUseBCICheckBox,SIGNAL(clicked(bool)),this,SLOT(clickGameUseOptionsCheckBox(bool)));

	// SLB Kinect
	gameUseKinectCheckBox = new QCheckBox(tr("Use &Kinect"));
	//gameUseKinectCheckBox->setLayoutDirection(Qt::RightToLeft);
	gameUseKinectCheckBox->setChecked(true);
	gameUseKinect = gameUseKinectCheckBox->isChecked();
	connect(gameUseKinectCheckBox,SIGNAL(clicked(bool)),this,SLOT(clickGameUseOptionsCheckBox(bool)));

	gameUseRobotCheckBox = new QCheckBox(tr("Use &Robot"));
	//gameUseRobotCheckBox->setLayoutDirection(Qt::RightToLeft);
	gameUseRobotCheckBox->setChecked(userobot);
	connect(gameUseRobotCheckBox,SIGNAL(clicked(bool)),this,SLOT(clickGameUseOptionsCheckBox(bool)));

	QHBoxLayout *gameOptionsCheckBoxLayout = new QHBoxLayout();
	gameOptionsCheckBoxLayout->addWidget(gameUseBCICheckBox);
	gameOptionsCheckBoxLayout->addWidget(gameUseKinectCheckBox);
	gameOptionsCheckBoxLayout->addWidget(gameUseRobotCheckBox);
	//gameCommandButtonsLeft->addLayout(kinectLayout);
	//

	gameCommandButtonsLeft->addLayout(gameOptionsCheckBoxLayout);
	gameCommandButtonsLeft->addWidget(gameApplyButton);
	gameCommandButtonsLeft->addWidget(gameExitButton);

	//gameCommandButtons->addLayout(gameCommandButtonsLeft);
	//gameCommandButtons->addWidget(gameStartButton);

	//gameTopLeftLayout->addWidget(gameModeBox);
	//gameTopLeftLayout->addStretch(true);
	//gameTopLeftLayout->addLayout(gameCommandButtons);
	//

	// TopRight
	QGroupBox *gameTrialsBox = new QGroupBox(tr("Trials number per session:"));
	//gameTrialsBox->setMinimumHeight(180);

	QLabel *gameFairLabel1 = new QLabel(tr("Fair 1"));
	QLabel *gameCheatLabel = new QLabel(tr("Cheat"));
	QLabel *gameFairLabel2 = new QLabel(tr("Fair 2"));

	gameFairEdit1 = new QLineEdit(tr("5"));
	gameCheatEdit1 = new QLineEdit(tr("2"));
	QLabel *gameCheatLabelOf = new QLabel(tr("of"));
	gameCheatEdit2 = new QLineEdit(tr("5"));
	gameFairEdit2 = new QLineEdit(tr("5"));
	gameFairEdit1->setAlignment(Qt::AlignRight);
	gameCheatEdit1->setAlignment(Qt::AlignRight);
	gameCheatEdit1->setMaximumWidth(32);
	gameCheatLabelOf->setAlignment(Qt::AlignCenter);
	gameCheatLabelOf->setMaximumWidth(16);
	gameCheatEdit2->setAlignment(Qt::AlignRight);
	gameCheatEdit2->setMaximumWidth(32);
	gameFairEdit2->setAlignment(Qt::AlignRight);

	connect(gameCheatEdit1, SIGNAL(textChanged(const QString &)), this, SLOT(changeGameCheat1()));
	connect(gameCheatEdit2, SIGNAL(textChanged(const QString &)), this, SLOT(changeGameCheat2()));

	gameTopRightLine1->addWidget(gameFairLabel1, 0, 0);
	gameTopRightLine1->addWidget(gameFairEdit1, 0, 1);
	gameTopRightLine1->setColumnStretch(0, 50);
	gameTopRightLine1->setColumnStretch(1, 50);
	gameTopRightLine2->addWidget(gameCheatLabel, 0, 0);
	gameTopRightLine2->addWidget(gameCheatEdit1, 0, 1);
	gameTopRightLine2->addWidget(gameCheatLabelOf, 0, 2);
	gameTopRightLine2->addWidget(gameCheatEdit2, 0, 3);
	gameTopRightLine2->setColumnStretch(0, 50);
	gameTopRightLine2->setColumnStretch(1, 10);
	gameTopRightLine2->setColumnStretch(2, 10);
	gameTopRightLine2->setColumnStretch(3, 10);
	gameTopRightLine3->addWidget(gameFairLabel2, 0, 0);
	gameTopRightLine3->addWidget(gameFairEdit2, 0, 1);
	gameTopRightLine3->setColumnStretch(0, 50);
	gameTopRightLine3->setColumnStretch(1, 50);

	gameTopRightLayout->addLayout(gameTopRightLine1);
	gameTopRightLayout->addLayout(gameTopRightLine2);
	gameTopRightLayout->addLayout(gameTopRightLine3);

	gameTrialsBox->setLayout(gameTopRightLayout);
	//

	// Top
	//gameMainTopLayout1->addLayout(gameTopLeftLayout, 0, 0);
	gameMainTopLayout1->addWidget(gameModeBox, 0, 0);
	//gameMainTopLayout1->addStretch(true);
	gameMainTopLayout1->addWidget(gameTrialsBox, 0, 2);
	gameMainTopLayout1->setColumnStretch(0, 40);
	gameMainTopLayout1->setColumnStretch(1, 10);
	gameMainTopLayout1->setColumnStretch(2, 40);
	gameMainTopLayout2->addWidget(gameStartButton, 0, 0);
	gameMainTopLayout2->addLayout(gameCommandButtonsLeft, 0, 2);
	gameMainTopLayout2->setColumnStretch(0, 40);
	gameMainTopLayout2->setColumnStretch(1, 10);
	gameMainTopLayout2->setColumnStretch(2, 40);
	//
	/*
	QFrame* hFrame = new QFrame;
	hFrame->setFrameShape(QFrame::HLine);
	hFrame->setStyleSheet("QFrame { color: #DCDCDC; }");
	*/
	// Bottom
	QGroupBox *gameSessionBox = new QGroupBox(tr("Session:"));
	gameSessionBox->setMinimumHeight(220);

	gameRockButton = new QPushButton(); //tr("Rock"));
	gamePaperButton = new QPushButton(); //tr("Paper"));
	gameScissorsButton = new QPushButton(); //tr("Scissors"));
	gameRockButton->setToolTip(tr("Rock\n(1-R)"));
	gamePaperButton->setToolTip(tr("Paper\n(2-P)"));
	gameScissorsButton->setToolTip(tr("Scissors\n(3-S)"));
	/*
	gameRockButton->setMinimumHeight(50);
	gamePaperButton->setMinimumHeight(50);
	gameScissorsButton->setMinimumHeight(50);
	gameRockButton->setMaximumWidth(100);
	gamePaperButton->setMaximumWidth(100);
	gameScissorsButton->setMaximumWidth(100);
	*/
	/*
	QPixmap iconRock(GAMEICON_ROCK);
	QPixmap iconPaper(GAMEICON_PAPER);
	QPixmap iconScissors(GAMEICON_SCISSORS);
	QPixmap iconRockG(GAMEICON_ROCK_G);
	QPixmap iconPaperG(GAMEICON_PAPER_G);
	QPixmap iconScissorsG(GAMEICON_SCISSORS_G);
	*/
	iconRock		= QPixmap(GAMEICON_ROCK);
	iconPaper		= QPixmap(GAMEICON_PAPER);
	iconScissors	= QPixmap(GAMEICON_SCISSORS);
	iconRockG		= QPixmap(GAMEICON_ROCK_G);
	iconPaperG		= QPixmap(GAMEICON_PAPER_G);
	iconScissorsG	= QPixmap(GAMEICON_SCISSORS_G);
	
	gameRockButton->setIcon(iconRock);
	gameRockButton->setIconSize(iconRock.rect().size());
	gameRockButton->setFixedSize(iconRock.rect().size());
	gamePaperButton->setIcon(iconPaper);
	gamePaperButton->setIconSize(iconPaper.rect().size());
	gamePaperButton->setFixedSize(iconPaper.rect().size());
	gameScissorsButton->setIcon(iconScissors);
	gameScissorsButton->setIconSize(iconScissors.rect().size());
	gameScissorsButton->setFixedSize(iconScissors.rect().size());
	
	gameRockButton->setShortcut(QKeySequence(Qt::Key_R));
	gamePaperButton->setShortcut(QKeySequence(Qt::Key_P));
	gameScissorsButton->setShortcut(QKeySequence(Qt::Key_S));
	gameRockButton->setShortcut(QKeySequence(Qt::Key_1));
	gamePaperButton->setShortcut(QKeySequence(Qt::Key_2));
	gameScissorsButton->setShortcut(QKeySequence(Qt::Key_3));
	connect(gameRockButton, SIGNAL(clicked()),this,SLOT(clickedGameRockButton()));
	connect(gamePaperButton, SIGNAL(clicked()),this,SLOT(clickedGamePaperButton()));
	connect(gameScissorsButton, SIGNAL(clicked()),this,SLOT(clickedGameScissorsButton()));

	QPushButton *gameSkipButton = new QPushButton(tr("<- S&KIP"));
	QPushButton *gameNextButton = new QPushButton(tr("&NEXT ->"));
	gameSkipButton->setToolTip(tr("(left arrow)"));
	gameNextButton->setToolTip(tr("(right arrow)"));
	gameSkipButton->setMinimumHeight(50);
	gameNextButton->setMinimumHeight(50);

	gameSkipButton->setShortcut(QKeySequence(Qt::Key_Left));
	gameNextButton->setShortcut(QKeySequence(Qt::Key_Right));
	connect(gameSkipButton, SIGNAL(clicked()), this, SLOT(clickedGameSkipButton()));
	connect(gameNextButton, SIGNAL(clicked()), this, SLOT(clickedGameNextButton()));
	
	gameBottomLayout1->addWidget(gameRockButton);
	gameBottomLayout1->addWidget(gamePaperButton);
	gameBottomLayout1->addWidget(gameScissorsButton);

	gameCounterLabel = new QLabel("Ready");
	gameCounterLabel->setAlignment(Qt::AlignCenter);
	QFont gameCounterFont = font();
	gameCounterFont.setPointSize(16);
	gameCounterFont.setBold(true);
	gameCounterLabel->setFont(gameCounterFont);

	gameBottomLayout2->addWidget(gameSkipButton);
	gameBottomLayout2->addStretch(true);
	gameBottomLayout2->addWidget(gameCounterLabel);
	gameBottomLayout2->addStretch(true);
	gameBottomLayout2->addWidget(gameNextButton);

	gameMainBottomLayout->addLayout(gameBottomLayout1);
	gameMainBottomLayout->addStretch(true);
	gameMainBottomLayout->addLayout(gameBottomLayout2);

	gameSessionBox->setLayout(gameMainBottomLayout);
	//

	gameMainLayout->addLayout(gameMainTopLayout1);
	gameMainLayout->addLayout(gameMainTopLayout2);
	gameMainLayout->addStretch(true);
	//gameMainLayout->addWidget(hFrame);
	gameMainLayout->addWidget(gameSessionBox);
	
	gameSessionDialogControl->setLayout(gameMainLayout);
	gameSessionDialogControl->show(); // non modal

}

// SLB
void Gui::gameSetChoiceIcons(int move) {

	if (move == GAME_UNDEFINED) { // reset, all default colours
		gameRockButton->setIcon(iconRock);
		gamePaperButton->setIcon(iconPaper);
		gameScissorsButton->setIcon(iconScissors);
	}
	else if (move == GAME_ROCK) {
		gameRockButton->setIcon(iconRockG);
		gamePaperButton->setIcon(iconPaper);
		gameScissorsButton->setIcon(iconScissors);
	}
	else if (move == GAME_PAPER) {
		gameRockButton->setIcon(iconRock);
		gamePaperButton->setIcon(iconPaperG);
		gameScissorsButton->setIcon(iconScissors);
	}
	else if (move == GAME_SCISSORS) {
		gameRockButton->setIcon(iconRock);
		gamePaperButton->setIcon(iconPaper);
		gameScissorsButton->setIcon(iconScissorsG);
	}

}

// SLB
void Gui::showBaselineAcquisitionDialogControl() {

	dialogControl	= new QDialog();
	dialogControl->setWindowIcon(QIcon(FRAMEWORKREDICONPATH));
	dialogUser		= new QDialog(dialogControl);

	QPalette *palette = new QPalette();	
	palette->setBrush(QPalette::Background, *(new QBrush(*(new QPixmap(FRAMEWORKBGIMAGE)))));
	dialogControl->setPalette(*palette);
	dialogUser->setPalette(*palette);

	dialogControl->setWindowTitle("Baseline Acquisition");
	dialogControl->setAttribute(Qt::WA_DeleteOnClose);
	//dialogControl->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // SLB remove "?" help quick menu
	dialogControl->setWindowFlags(Qt::FramelessWindowHint); 
	dialogControl->setWindowFlags(Qt::WindowTitleHint); 
	//dialogControl->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	int dialogWidth		= 240;
	int dialogHeight	= 240;
	//dialogControl->setMinimumWidth(dialogWidth);
	//dialogControl->setMinimumHeight(dialogHeight);
	dialogControl->setFixedSize(dialogWidth, dialogHeight);

	QVBoxLayout *dialogLayout = new QVBoxLayout;

	QLabel *dialogLabel = new QLabel("Ready for Baseline Acquisition");
	dialogLabel->setAlignment(Qt::AlignCenter);

	QHBoxLayout *durationLine = new QHBoxLayout;
	QLabel *durationLabel = new QLabel(tr("Duration (sec):"));
	durationLineEdit = new QLineEdit();
	durationLineEdit->setAlignment(Qt::AlignRight);
	durationLineEdit->setText(QString::number(30));
	//durationLineEdit->setEnabled(false);
	QGridLayout *durationGrid = new QGridLayout();
	durationGrid->addWidget(durationLabel, 0, 0);
	durationGrid->addWidget(durationLineEdit, 0, 1);
	durationGrid->setColumnStretch(0, 50);
	durationGrid->setColumnStretch(1, 50);
	durationLine->addLayout(durationGrid);

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	
	closedEyesButton = new QPushButton(tr(""));
	closedEyesButton->setToolTip(tr("Baseline acquisition\nduring closed eyes session"));
	QPixmap iconEyeClosed(EYECLOSEDICONPATH);
	closedEyesButton->setIcon(iconEyeClosed);
	closedEyesButton->setIconSize(iconEyeClosed.rect().size());
	closedEyesButton->setFixedSize(iconEyeClosed.rect().size());
	openedEyesButton = new QPushButton(tr(""));
	openedEyesButton->setToolTip(tr("Baseline acquisition\nduring open eyes session"));
	openedEyesButton->setEnabled(false);
	QPixmap iconEyeOpen(EYEOPENICONPATH);
	openedEyesButton->setIcon(iconEyeOpen);
	openedEyesButton->setIconSize(iconEyeOpen.rect().size());
	openedEyesButton->setFixedSize(iconEyeOpen.rect().size());

	buttonsLayout->addWidget(closedEyesButton);
	buttonsLayout->addWidget(openedEyesButton);

	QHBoxLayout *pbarLayout = new QHBoxLayout();
	pbarLabel = new QLabel(tr("1 / 2"));
	pbarLabel->setMinimumWidth(32);
	pbar = new QProgressBar();
	pbarLayout->addWidget(pbarLabel);
	pbarLayout->addWidget(pbar);

	QHBoxLayout *closeDialogButtonLine = new QHBoxLayout();
	QPushButton *closeDialogButton = new QPushButton(tr("&Done"));
	closeDialogButton->setMinimumHeight(32);
	closeDialogButton->setMinimumWidth(200);

	//closeDialogButtonLine->addStretch(true);
	closeDialogButtonLine->addWidget(closeDialogButton);
	//closeDialogButtonLine->addStretch(true);

	dialogLayout->addWidget(dialogLabel);
	dialogLayout->addLayout(durationLine);
	dialogLayout->addStretch(true);
	dialogLayout->addLayout(buttonsLayout);
	dialogLayout->addStretch(true);
	dialogLayout->addLayout(pbarLayout);
	dialogLayout->addLayout(closeDialogButtonLine);
	dialogControl->setLayout(dialogLayout);

	connect(closedEyesButton, SIGNAL(clicked()), this, SLOT(clickedBaselineAcqButton1()));
	connect(openedEyesButton, SIGNAL(clicked()), this, SLOT(clickedBaselineAcqButton2()));
	connect(closeDialogButton, SIGNAL(clicked()), this, SLOT(setAcqDone()));
	connect(closeDialogButton, SIGNAL(clicked()), dialogUser, SLOT(close()));
	connect(closeDialogButton, SIGNAL(clicked()), dialogControl, SLOT(close()));
	
	//dialogControl->exec(); // modal
	dialogControl->show(); // non modal
	//delete dialogControl;
	
	/*
	QMessageBox mb("Modify Warning",
				QString("\nDo you want to continue?"),
				QMessageBox::Warning,
				QMessageBox::Ok | QMessageBox::Default,
				QMessageBox::Cancel | QMessageBox::Escape,
				QMessageBox::NoButton);
	int response = mb.exec();
	/*
	if(response == QMessageBox::Ok)
		return true;
	else
		return false;
	*/

}

// SLB
void Gui::showBaselineAcquisitionDialogUser() {
	//dialogUser = new QDialog(this);
	dialogUser->setWindowTitle("Baseline Acquisition");
	dialogUser->setAttribute(Qt::WA_DeleteOnClose);
	dialogUser->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // SLB remove "?" help quick menu
	//dialogUser->setWindowFlags(Qt::FramelessWindowHint);
	//dialogUser->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	/*
	int dialogWidth		= 200;
	int dialogHeight	= 200;
	dialogUser->setMinimumWidth(dialogWidth);
	dialogUser->setMinimumHeight(dialogHeight);
	*/

	QVBoxLayout *dialogLayout = new QVBoxLayout;

	//if (language.find("en") != std::string::npos) {
	if (language != "jp") {

		dialogLabelUser = new QLabel("<html>Ready for Baseline Acquisition"
								"<br /><br />" //"\n\n"
								"Please keep your eyes closed<br />until the notification sound"
								"<br /><br />" //"\n\n"
								//"通知音が聞こえるまで目を閉じたままにしておいてください"
								/*
								"<div style=\"font-family: meiryo\">"
								"&#x901A;&#x77E5;&#x97F3;&#x304C;&#x805E;&#x3053;&#x3048;&#x308B;&#x307E;&#x3067;"
								"<br />"
								"&#x76EE;&#x3092;&#x9589;&#x3058;&#x305F;&#x307E;&#x307E;&#x306B;&#x3057;&#x3066;"
								"<br />"
								"&#x304A;&#x3044;&#x3066;&#x304F;&#x3060;&#x3055;&#x3044;"
								"</div>"
								*/
								"<br /></html>"
								);

	}
	else {
		
		dialogLabelUser = new QLabel("<html>Ready for Baseline Acquisition"
								"<br /><br />" //"\n\n"
								///"Please keep your eyes closed<br />until the notification sound"
								///"<br /><br />" //"\n\n"
								//"通知音が聞こえるまで目を閉じたままにしておいてください"
								//通知音が聞こえるまで
								//目を閉じていて下さい
								"<div style=\"font-family: meiryo\">"
								/*
								"&#x901A;&#x77E5;&#x97F3;&#x304C;&#x805E;&#x3053;&#x3048;&#x308B;&#x307E;&#x3067;"
								"<br />"
								"&#x76EE;&#x3092;&#x9589;&#x3058;&#x305F;&#x307E;&#x307E;&#x306B;&#x3057;&#x3066;"
								"<br />"
								"&#x304A;&#x3044;&#x3066;&#x304F;&#x3060;&#x3055;&#x3044;"
								*/
								"&#x901A;&#x77E5;&#x97F3;&#x304C;&#x805E;&#x3053;&#x3048;&#x308B;&#x307E;&#x3067;"
								"<br />"
								"&#x76EE;&#x3092;&#x9589;&#x3058;&#x3066;&#x3044;&#x3066;&#x4E0B;&#x3055;&#x3044;"
								"</div>"
								"<br /></html>"
								);
		
	}
	
	dialogLabelUser->setAlignment(Qt::AlignCenter);
	dialogLabelUser->setStyleSheet("font-size: 20px; font-weight: bold; color: #3c3c3c");
	
	dialogIconLabel = new QLabel("");
	dialogIconLabel->setAlignment(Qt::AlignCenter);
	QPixmap iconEyeClosed(EYECLOSEDICONPATH);
	dialogIconLabel->setPixmap(iconEyeClosed);
	//dialogIconLabel->setFixedSize(iconEyeClosed.rect().size());

	//QPushButton *closeDialogButton = new QPushButton(tr("Close"));

	QHBoxLayout *pbarLayout1 = new QHBoxLayout();
	pbarLabel1 = new QLabel(tr("1 / 2"));
	pbarLabel1->setMinimumWidth(32);
	pbar1 = new QProgressBar();
	pbarLayout1->addWidget(pbarLabel1);
	pbarLayout1->addWidget(pbar1);
	QHBoxLayout *pbarLayout2 = new QHBoxLayout();
	QLabel *pbarLabel2 = new QLabel(tr("Total"));
	pbarLabel2->setMinimumWidth(32);
	pbar2 = new QProgressBar();
	pbarLayout2->addWidget(pbarLabel2);
	pbarLayout2->addWidget(pbar2);

	/*
	//QString safe = "QProgressBar::chunk { background: QLinearGradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #78d, stop: 0.4999 #46a, stop: 0.5 #45a, stop: 1 #238); border-bottom-right-radius: 7px; border-bottom-left-radius: 7px; border: 1px solid black; }";
	QString safe = "QProgressBar::chunk { background: QLinearGradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #78d, stop: 0.4999 #46a, stop: 0.5 #45a, stop: 1 #238); }";
	pbar2->setStyleSheet(safe);
	*/
	/*
	// SLB works with cleanlooks style
	QPalette p = pbar2->palette();
	p.setColor(QPalette::Base, Qt::red);
	p.setColor(QPalette::Highlight, QColor(Qt::darkCyan));
	pbar2->setPalette(p);
	*/

	dialogLayout->addStretch(true);
	dialogLayout->addWidget(dialogLabelUser);
	dialogLayout->addWidget(dialogIconLabel);
	dialogLayout->addStretch(true);
	dialogLayout->addLayout(pbarLayout1);
	dialogLayout->addLayout(pbarLayout2);
	//dialogLayout->addWidget(closeDialogButton);
	dialogUser->setLayout(dialogLayout);

	//connect(closeDialogButton, SIGNAL(clicked()), dialogUser, SLOT(close()));

	int dialogSide;
	int screensAvailable = QApplication::desktop()->screenCount();
	if (screensAvailable == 1) {
		QRect screenGeom = QApplication::desktop()->screenGeometry();
		dialogSide = min(screenGeom.width(), screenGeom.height()) / 2;
		int xOffset = 50;
		int xPos = dialogControl->pos().x() + dialogControl->width() + xOffset;
		if (xPos + dialogSide > screenGeom.right())
			xPos = dialogControl->pos().x() - dialogSide;
		int yPos = screenGeom.center().y() - dialogSide/2;
		QPoint globalPos = QPoint(xPos, yPos);
		
		//QPoint offsetPos(300, 0);
		//dialogUser->move(screenGeom.center() - QPoint(dialogSide/2, dialogSide/2) + offsetPos);
		//QPoint globalPos = dialogControl->mapToGlobal(offsetPos);
		
		dialogUser->move(globalPos);
	}
	//else if (screensAvailable == 2) {
	else {
		int newScreenID;
		int mainWindowScreenID = QApplication::desktop()->screenNumber(dialogControl);
		if (mainWindowScreenID == 0)
			newScreenID = 1;
		else
			newScreenID = 0;
		QRect newScreenGeom = QApplication::desktop()->screenGeometry(newScreenID);
		dialogSide = min(newScreenGeom.width(), newScreenGeom.height()) / 2;
		dialogUser->move(newScreenGeom.center() - QPoint(dialogSide/2, dialogSide/2));
		//dialogUser->showFullScreen();
	}
	dialogUser->setMinimumWidth(dialogSide);
	dialogUser->setMinimumHeight(dialogSide);
	//dialogUser->exec(); // modal
	dialogUser->show(); // non modal

}

// SLB
void Gui::clickedSignalCheckButton() {

	//statusbar->showMessage(tr("Starting signals check..."), 2000); // SLB
	if (signalCheckButton->isChecked()) {
		currentRunDone = false;
		signalCheckButton->setStyleSheet("QPushButton { color: rgba(184, 38, 25, 100%); }");

		_eventFinishInitialization.SetEvent();
		_eventSignalCheckStart.SetEvent();
	}
	else {
		currentRunDone = true;
		signalCheckButton->setStyleSheet("QPushButton { color: black; }");
		initReceivedData();
	}
}

// SLB
void Gui::clickedGameSessionButton() {
	updateStamp("Starting Game Session...\n");
	statusbar->showMessage(tr("Starting Game Session..."), 2000); // SLB

	timer->stop(); // SLB Kinect test
	
	gameRunDone		= true;
	//receivedDataN	= FILETYPE_GAME;
	receivedDataN = (gameMode != GAMEMODE_FAIR) ? GAMEMODE_ISCHEATTRIAL : FILETYPE_GAME;
	receivedDataN1	= GAMEMODE_BASELINE;

	_eventFinishInitialization.SetEvent();
	_eventGameSessionStart.SetEvent();

	for (int i = 0; i < 3; i++)
		gameWinCounts[i] = 0;

	initKinect();	// SLB Kinect
	Sleep(100);
	initTobii();	// SLB Tobii

	showGameSessionDialogControl();

}

// SLB
void Gui::clickedBaselineAcquisitionButton() {

	initiateOperation(); // SLB

	statusbar->showMessage(tr("Starting Baseline acquisition..."), 2000); // SLB

	_eventFinishInitialization.SetEvent();
	_eventBaselineAcquisitionStart.SetEvent();

	acqReady	= false;
	acqDone		= false;

	showBaselineAcquisitionDialogControl();
	showBaselineAcquisitionDialogUser();

}

// SLB
void Gui::clickedBaselineAcqButton1() {

	durationLineEdit->setEnabled(false);

	QPixmap iconEyeClosed(EYECLOSEDICONPATH);
	dialogIconLabel->setPixmap(iconEyeClosed);

	closedEyesButton->setEnabled(false);
	acqReady = true;
	Sleep(500);
	acqReady = false;

	/*
	pbar->setMaximum(getBaselineAcqDuration() * 64 - 1); //500
	pbar1->setMaximum(getBaselineAcqDuration() * 64 - 1);
	pbar2->setMaximum(getBaselineAcqDuration() * 64 - 1);
	for (int i = 0; i < getBaselineAcqDuration() * 64; i++) {
		Sleep(1);
		pbar->setValue(i);
		pbar1->setValue(i);
		pbar2->setValue(i/2 + 1);

	}
	*/
	//
	pbar->setMaximum(100);
	pbar1->setMaximum(100);
	pbar2->setMaximum(100);
	clock_t startTime;
	clock_t currentTime;
	float delay	= getBaselineAcqDuration() * CLOCKS_PER_SEC / 100;
	for (int i = 1; i <= 100; i++) {
		startTime	= clock();
		currentTime	= startTime;
		while ((currentTime - startTime) < delay)
			currentTime = clock();
		pbar->setValue(i);
		pbar1->setValue(i);
		pbar2->setValue(i/2);
	}
	//

	initReceivedData();

	while (eyesAcqStatus != CLOSEDEYES_STOP) {}
	eyesAcqStatus = -1;
	openedEyesButton->setEnabled(true);
	
}

// SLB
void Gui::clickedBaselineAcqButton2() {

	//if (language.find("en") != std::string::npos) {
	if (language != "jp") {
		
		dialogLabelUser->setText("<html>Ready for Baseline Acquisition"
							"<br /><br />"
							"Please keep your eyes open<br />until the notification sound"
							"<br /><br />"
							//"通知音が聞こえるまで目を開けたままにしておいてください"
							/*
							"<div style=\"font-family: meiryo\">"
							"&#x901A;&#x77E5;&#x97F3;&#x304C;&#x805E;&#x3053;&#x3048;&#x308B;&#x307E;&#x3067;"
							"<br />"
							"&#x76EE;&#x3092;&#x958B;&#x3051;&#x305F;&#x307E;&#x307E;&#x306B;&#x3057;&#x3066;"
							"<br />"
							"&#x304A;&#x3044;&#x3066;&#x304F;&#x3060;&#x3055;&#x3044;"
							"</div>"
							*/
							"<br /></html>"
							);
		
	}
	else {

		dialogLabelUser->setText("<html>Ready for Baseline Acquisition"
							"<br /><br />"
							///"Please keep your eyes open<br />until the notification sound"
							///"<br /><br />"
							//"通知音が聞こえるまで目を開けたままにしておいてください"
							//通知音が聞こえるまで
							//目を開けていて下さい
							"<div style=\"font-family: meiryo\">"
							/*
							"&#x901A;&#x77E5;&#x97F3;&#x304C;&#x805E;&#x3053;&#x3048;&#x308B;&#x307E;&#x3067;"
							"<br />"
							"&#x76EE;&#x3092;&#x958B;&#x3051;&#x305F;&#x307E;&#x307E;&#x306B;&#x3057;&#x3066;"
							"<br />"
							"&#x304A;&#x3044;&#x3066;&#x304F;&#x3060;&#x3055;&#x3044;"
							*/
							"&#x901A;&#x77E5;&#x97F3;&#x304C;&#x805E;&#x3053;&#x3048;&#x308B;&#x307E;&#x3067;"
							"<br />"
							"&#x76EE;&#x3092;&#x958B;&#x3051;&#x3066;&#x3044;&#x3066;&#x4E0B;&#x3055;&#x3044;"
							"</div>"
							"<br /></html>"
							);

	}

	QPixmap iconEyeOpen(EYEOPENICONPATH);
	dialogIconLabel->setPixmap(iconEyeOpen);

	pbarLabel->setText("2 / 2");
	pbarLabel1->setText("2 / 2");

	openedEyesButton->setEnabled(false);
	acqReady = true;
	Sleep(500);
	acqReady = false;

	/*
	for (int i = 0; i < getBaselineAcqDuration() * 64; i++) {
		Sleep(1);
		pbar->setValue(i);
		pbar1->setValue(i);
		pbar2->setValue(i/2 + getBaselineAcqDuration() * 64/2);
	}
	*/
	//
	clock_t startTime;
	clock_t currentTime;
	float delay	= getBaselineAcqDuration() * CLOCKS_PER_SEC / 100;
	for (int i = 1; i <= 100; i++) {
		startTime	= clock();
		currentTime	= startTime;
		while ((currentTime - startTime) < delay)
			currentTime = clock();
		pbar->setValue(i);
		pbar1->setValue(i);
		pbar2->setValue(i/2 + 50);
	}
	//

	initReceivedData();

}

// SLB
void Gui::menubar1helpSlot() {
	//
}

// SLB
void Gui::menubar1creditsSlot() {

	QDialog *creditsDialog = new QDialog();
	creditsDialog->setWindowIcon(QIcon(FRAMEWORKREDICONPATH));
	

	creditsDialog->setWindowTitle("Credits");
	creditsDialog->setAttribute(Qt::WA_DeleteOnClose);
	//dialogControl->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // SLB remove "?" help quick menu
	creditsDialog->setWindowFlags(Qt::FramelessWindowHint); 
	creditsDialog->setWindowFlags(Qt::WindowTitleHint); 
	//dialogControl->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	int dialogWidth = 480;
	int dialogHeight = 320;
	//creditsDialog->setMinimumWidth(dialogWidth);
	//creditsDialog->setMinimumHeight(dialogHeight);
	creditsDialog->setFixedSize(dialogWidth, dialogHeight);

	QVBoxLayout *dialogLayout = new QVBoxLayout;
	QHBoxLayout* logoLine = new QHBoxLayout;
	
	QLabel *unipaBCIIconLabel = new QLabel();
	QPixmap iconUnipaBCI(FRAMEWORKREDICONPATH);
	unipaBCIIconLabel->setPixmap(iconUnipaBCI);
	unipaBCIIconLabel->setFixedSize(iconUnipaBCI.rect().size());

	QLabel *unipaBCILabel = new QLabel("UniPA  BCI Framework ");
	QFont titleFont = font();
	titleFont.setPointSize(20);
	titleFont.setBold(true);
	//titleFont.setStyleStrategy(QFont::PreferAntialias);
	unipaBCILabel->setFont(titleFont);

	logoLine->addWidget(unipaBCIIconLabel);
	logoLine->addStretch(true);
	logoLine->addWidget(unipaBCILabel);
	
	QString creditsString = "Developed by Salvatore La Bua\n"
							"as a final project for his second cycle degree\n"
							"programme at University of Palermo, Italy.\n"
							"\n"
							"With the supervision of:\n"
							" Prof. Eng. Rosario Sorbello\n"
							" Eng. Salvatore Tramonte\n"
							" Eng. Marcello Giardina\n"
							"\n"
							"Based on the previous work (P300 core) of:\n"
							" Rosario Misuraca\n"
							" Walter Tranchina\n"
							" Giuseppe Trubia";

	QLabel *creditsLabel = new QLabel(creditsString);

	QHBoxLayout *lineBottom = new QHBoxLayout;
	QString roboticslabLine = "<p align=\"right\"><i><font size=\"26\" color=\"black\"><b>&nbsp;ROBOTICS</b></font><font size=\"26\" color=\"white\"><b>&nbsp;LAB&nbsp;&nbsp;</b></font></i></p>";
	
	QLabel *statusBottomBar = new QLabel(roboticslabLine);
	QString paletteStyle = "background-color: rgba(184, 38, 25, 100%); border-radius: 6px 10px;";
	statusBottomBar->setAutoFillBackground(true);
	statusBottomBar->setStyleSheet(paletteStyle);
	statusBottomBar->setFixedHeight(36);

	dialogLayout->addLayout(logoLine);
	dialogLayout->addStretch(true);
	dialogLayout->addWidget(creditsLabel);
	dialogLayout->addWidget(statusBottomBar);

	creditsDialog->setLayout(dialogLayout);

	QPalette *palette = new QPalette();	
	palette->setBrush(QPalette::Background, *(new QBrush(*(new QPixmap(CREDITSBGIMAGE)))));
	creditsDialog->setPalette(*palette);

	creditsDialog->exec(); // modal

}

// SLB
void Gui::setEyesAcquisitionStatus(int value) {

	eyesAcqStatus = value;
}

// SLB
bool Gui::isCurrentRunDone() {

	return currentRunDone;
}

// SLB
bool Gui::isGameRunDone() {

	return gameRunDone;
}

// SLB
bool Gui::isGameRunStarted() {

	return gameRunStarted;
}

// SLB
bool Gui::isGameUseBCI() {

	return gameUseBCI;
}

// SLB
void Gui::setCurrentRunDone(bool value) {

	currentRunDone = value;
}

// SLB
bool Gui::isAcqReady() {

	return acqReady;
}

// SLB
bool Gui::isAcqDone() {

	return acqDone;
}

// SLB
void Gui::setAcqDone() {

	acqDone = true;

	terminateOperation();
}

// SLB
int Gui::getBaselineAcqDuration() {

	bool convOk = false;
	int duration = durationLineEdit->text().toInt(&convOk, 10);

	/*
	if (!durationLineEdit->text().isEmpty()) {
		istringstream buffer(durationLineEdit->text().toLocal8Bit().constData());
		int duration;
		buffer >> duration;
		return duration;
	}
	*/
	if (convOk)
		return duration;
	else
		return 30;
}

void Gui::clickedCalibrationButton() {

	//initiateOperation(); // SLB
	readConfiguration(); // SLB

	statusbar->showMessage(tr("Starting Calibration interface..."), 2000); // SLB

	_eventFinishInitialization.SetEvent();
	_eventCalibrationStart.SetEvent();

	baselineAcquisitionButton->setEnabled(false);	// SLB
	speller->setEnabled(true);
	lineTagOnlineAnalysis->setEnabled(true);	// SLB
	onlineAnalysis->setEnabled(true);			// SLB
	lineCalibrationAnalysis->setEnabled(true);	// SLB
	startCalibrationAnalysis->setEnabled(true);

	//terminateOperation(); // SLB
	
}

void Gui::clickedSpellerButton() {

	//initiateOperation(); // SLB
	readConfiguration(); // SLB

	_eventFinishInitialization.SetEvent();
	_eventSpellerStart.SetEvent();

	baselineAcquisitionButton->setEnabled(false); // SLB
	//labelFlash->setEnabled(true);

	//calibration->setEnabled(false); // SLB TODO allow recalibration after recognition instead of disabling the calibration button

	//terminateOperation(); // SLB

}

void Gui::clickedSpellerFileButton() {

	//initiateOperation(); // SLB
	readConfiguration(); // SLB

	statusbar->showMessage(tr("Starting Spelling interface..."), 2000); // SLB

	_eventFinishInitialization.SetEvent();
	_eventSpellerFileStart.SetEvent();

	baselineAcquisitionButton->setEnabled(false); // SLB

	//calibration->setEnabled(false); // SLB TODO allow recalibration after recognition instead of disabling the calibration button

	//terminateOperation(); // SLB

}

void Gui::clickedFlashAnalysisButton() {

	_eventFinishInitialization.SetEvent();
	_eventFlashAnalysis.SetEvent();

}

void Gui::clickedOnlineFlashAnalysisButton() {

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	
	if (result) {
		pugi::xml_node node = doc.child("flash_analysis");
		node.first_child().set_value(lineTagOnlineAnalysis->text().toLocal8Bit().constData());
		//doc.save_file("./configuration.xml");
		doc.save_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	}

	_eventFinishInitialization.SetEvent();
	_eventOnlineFlashAnalysis.SetEvent();

}

void Gui::clickedStartCalibrationAnalysisButton() {

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	
	if (result) {
		pugi::xml_node node = doc.child("calibration_analysis");
		if (node.first_child().empty()) {
			node.append_child(pugi::node_pcdata).set_value(lineCalibrationAnalysis->text().toLocal8Bit().constData());
		}
		else
			node.first_child().set_value(lineCalibrationAnalysis->text().toLocal8Bit().constData());
		//doc.save_file("./configuration.xml");
		doc.save_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	}

	_eventFinishInitialization.SetEvent();
	_eventCalibrationAnalysis.SetEvent();

}

/*
void Gui::clickedFlashButton() {

	insertNewFlash(lineNewFlash->text().toLocal8Bit().constData());
	_eventFlashOptimization.SetEvent();
}

void Gui::insertNewFlash(string flash) {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	
	if (result) {
		pugi::xml_node node = doc.child("flash_analysis");
		node.first_child().set_value(flash.c_str());
		doc.save_file("./configuration.xml");
	}
}*/

void Gui::updateStamp(string message) {
	// SLB
	//char* c = new char[a.length() + 1];
	//strcpy(c, a.c_str());
	//
	//cout << a << endl;
	//textarea->append(QString::fromStdString(a)); // SLB was (a)
	
	//if (!a.empty()) {
		textarea->append(QString::fromStdString(message.c_str())); // SLB was (a)
		textarea->ensureCursorVisible();  // SLB comment crash?
	//}
	
		writeLogFile(message);

	//delete[] c; // SLB
}

/* SLB remove, unused
void Gui::enableSpellerButton() {

	speller->setEnabled(true);
}
*/

// SLB disable Worker (no idea why it was used at all)
void Gui::countFinished() {

    countRunning = false;
}


void Gui::startCount() {

	QThread *workerThread;
 
    if (countRunning) {
        QMessageBox::critical(this, "Error", "Count is already running!");
        return;
    }
	
    workerThread = new QThread;
    worker->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), worker, SLOT(doWork()));
    connect(worker, SIGNAL(finished()), workerThread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(worker, SIGNAL(finished()), this, SLOT(countFinished()));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
	qRegisterMetaType< string >("string");
	qRegisterMetaType< QTextCursor >("QTextCursor");
	//qRegisterMetaType< int >("int"); // SLB
	//qRegisterMetaType< QPushButton >("QPushButton"); // SLB
	//qRegisterMetaType< QIcon >("QIcon"); // SLB
	connect(worker, SIGNAL(updateStamp(string)), this, SLOT(updateStamp(string)));
	//connect(worker, SIGNAL(gameSetChoiceIcons(int)), this, SLOT(gameSetChoiceIcons(int))); // SLB
    workerThread->start();
	
    countRunning = true;

}
//

CEvent& Gui::getFinishInitializationEvent() {
	
	return _eventFinishInitialization;
}

// SLB
CEvent& Gui::getSignalCheckStartEvent() {
	
	return _eventSignalCheckStart;
}

// SLB
CEvent& Gui::getGameSessionStartEvent() {
	
	return _eventGameSessionStart;
}

// SLB
CEvent& Gui::getBaselineAcquisitionStartEvent() {
	
	return _eventBaselineAcquisitionStart;
}

CEvent& Gui::getCalibrationStartEvent() {
	
	return _eventCalibrationStart;
}

CEvent& Gui::getSpellerStartEvent() {

	return _eventSpellerStart;
}

CEvent& Gui::getSpellerFileStartEvent() {

	return _eventSpellerFileStart;
}

CEvent& Gui::getFlashOptimizationEvent() {

	return _eventFlashOptimization;
}

CEvent& Gui::getFlashAnalysisStartEvent() {

	return _eventFlashAnalysis;
}

CEvent& Gui::getOnlineFlashAnalysisStartEvent() {

	return _eventOnlineFlashAnalysis;
}

CEvent& Gui::getCalibrationAnalysisStartEvent() {
	
	return _eventCalibrationAnalysis;
}

// SLB
CEvent& Gui::getKillEvent() {
	
	return _eventKill;
}

//SLB
string Gui::BoolToString(bool b) {
  return b ? "1" : "0";
}

// SLB
QRect Gui::buildScreenGeometry() {
	auto screenarea = QApplication::desktop();
	QRect area, virtualRect;

	int n = screenarea->screenCount();
	for (int i = 0; i < n; i++) {
		area = screenarea->screenGeometry(i);
		virtualRect |= area;
		cout << i << ": " << area.left() << "\t" << area.right() << "\t" << area.top() << "\t" << area.bottom() << endl;
	}
	//cout << n << endl;

	cout << virtualRect.left() << "\t" << virtualRect.right() << "\t" << virtualRect.top() << "\t" << virtualRect.bottom() << endl;
	
	return virtualRect;

	/*
	// Moving
	auto dlg = new QDialog( someParent );
	auto newPoint = QPoint( 2000, 0 ); // point on another screen
	auto realPos = someParent->mapFromGlobal( newPoint );
	dlg->move( realPos );
	*/
}

// SLB
void Gui::robotRawCommand(string command) {
	if (userobot) {
		RobotController* robot = new RobotController;
		robot->sendRawCommand(command);
	}
}

// SLB
void Gui::robotPlaymotion(string motion) {
	if (userobot) {
		RobotController* robot = new RobotController;
		robot->playmotion(motion);
	}
}

// SLB
void Gui::robotPlayvoice(string voice) {

	string voiceFileBasename = language + "_" + voice;
	if (userobot) {
		RobotController* robot = new RobotController;
		robot->playvoice(voiceFileBasename);
	}
	else
		playSound(voiceFileBasename);
}

// SLB
void Gui::robotSpeak(string voice, float duration) {
	robotPlaymotion("nvi_speak");
	robotPlayvoice(voice);
	Sleep(duration * 1000);
	robotPlaymotion("clear cleara");
}

// SLB
void Gui::playSound(string sound) {
	string soundFile = audioPathRoot + "/" + language + "/" + sound + ".wav";
	PlaySound(TEXT(soundFile.c_str()), NULL, SND_ASYNC);
}

// SLB
void Gui::initKinect() {

	if (!kinectGuiStarted) {
		string command = "start ";
		command += KINECT_EXECUTABLE;
		system(command.c_str());
		kinectGuiStarted = true;
	}

	if (!kinectInitialised) {
		kinectInitialised = true;

		listener = new KinectListener();

		try	{
			cout << "\nOpening and initializing Kinect device..." << "\n";

			_networkTagThread = AfxBeginThread(listener->ListenNetwork, NULL, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
			_networkTagThread->ResumeThread();

			listener->startAcquisition();

			//Sleep(1000);
			//system("start KinectHandTracking.exe");

		}
		catch (string& exception) {
			//
		}
	}
}

// SLB
void Gui::initTobii() {

	if (tobiienabled && !tobiiInitialised) {
		cout << "\nEnabling eye tracker..." << endl;
		
		gameEyeT = new TobiiTracker();
		gameEyeT->setRunning(true);
		
		if (gameEyeT->Initialize()) {
			//tobiienabled		= true;
			tobiiInitialised	= true;
			//gameEyeT->setRunning(true);
			cout << "Eye tracker enabled." << endl;
		}
		else {
			cout << "Eye tracker not available." << endl;
			tobiienabled		= false;
			tobiiInitialised	= false;
			//gameEyeT->setRunning(false);
		}
		
	}
}

// SLB
void Gui::destroyTobii() {

	gameEyeT->Disable();
	delete gameEyeT;

	tobiiInitialised = false;
}

// SLB
void Gui::clickGameUseOptionsCheckBox(bool flag) {
	gameUseBCI		= gameUseBCICheckBox->isChecked();
	cout << ((gameUseBCI) ? "Game: Using BCI." : "Game: Not using BCI.") << endl;

	gameUseKinect	= gameUseKinectCheckBox->isChecked();
	cout << ((gameUseKinect) ? "Game: Using Kinect." : "Game: Not using Kinect.") << endl;

	userobot		= gameUseRobotCheckBox->isChecked();
	cout << ((userobot) ? "Game: Using Robot." : "Game: Not using Robot.") << endl;
}

///

// SLB Returns files in the specified directory path.
vector<string> Gui::list_files(string path) {
    vector<string> matches;
	WIN32_FIND_DATA ffd;
	stringstream filename;
	filename << path << "/w_*.*";
	HANDLE hFind = FindFirstFile(filename.str().c_str(), &ffd);
	//HANDLE hFind = FindFirstFile((path + "/w_*.*").c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				//string filename(ffd.cFileName);
				matches.push_back(ffd.cFileName);
			}
        } while (FindNextFile(hFind, &ffd) != 0);
    }
    FindClose(hFind);
    return matches;
}

// SLB Returns folders in the specified directory path
vector<string> Gui::list_folders(string path) {
	vector<string> subdirs;
	WIN32_FIND_DATA ffd;
	stringstream filename;
	filename << path << "/*_*";
	HANDLE hFind = FindFirstFile(filename.str().c_str(), &ffd);
	//HANDLE hFind = FindFirstFile((path + "/*_*").c_str(), &ffd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				//string filename = ffd.cFileName;
            	subdirs.push_back(ffd.cFileName);
				//cout << filename.substr(0,2) << endl;
			}
		} while (FindNextFile(hFind, &ffd) != 0);
	}
	FindClose(hFind);
	return subdirs;
}