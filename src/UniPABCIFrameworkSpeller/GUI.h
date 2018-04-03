/*****************************************************************************
* 
* UniPA BCI Framework: an Augmented BCI Framework
* 
* Copyright (C) 2016-2017 Salvatore La Bua (slabua@gmail.com)
* RoboticsLab, Department of Industrial and Digital Innovation (DIID),
* Universita'� degli Studi di Palermo, V. delle Scienze, Palermo, Italy.
* 
* http://www.slblabs.com/projects/unipa-bci-framework
* https://github.com/slabua/UniPABCIFramework
* 
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
*****************************************************************************/

#include <string>
#include <map>
#include <vector>
#include "glut.h"
#include "P300Interface.h"
#include <stack>
#include "clientTCP.h"
//#include "serverTCP.h"

#ifdef _WIN32
#	include <windows.h>
#else
#	include <sys/time.h>
#endif

using std::string;
using std::map;
using std::vector;
using std::stack;

// ------------------------------------------------------------------------------------------
// ----------------------- Parametri per l'interfaccia --------------------------------------
// ------------------------------------------------------------------------------------------
const int CIBO						= 0;
const int TEMP						= 1; // SLB
const int BERE						= 2;
const int LUCE						= 3;
const int ASSISTENZA				= 4;
const int TV						= 5;
const int DORMIRE					= 6; //
const int FINESTRA					= 7;
const int RADIO						= 8; //
const int LETTO						= 9;
const int TELEFONO					= 10;
const int AMBULANZA					= 11;
const int PORTA						= 12;
const int EMOZIONI					= 13;
const int SPELLER					= 14;
const int CHIUDI					= 15;

const int BACK						= 6;
//const int CHIUDI_SOTTOINTERFACCIA	= 8;
const int CLOSE						= 8; // SLB renamed above const
const char* g_Title = "UniPA BCI Framework Visual Interface";		// Window Title // SLB
const int g_Width = 700;	// 1350;				// Initial window width // SLB
const int g_Height = 700;	// 760;					// Initial window height // SLB
int elements;										// Numero di elementi del sistema
int squaresNumber;									// Numero di quadrati
int rowsNumber;										// Numero di righe
string texturePathName = "";						// Path delle texture
string texturetype = "";
int size;											// Grandezza dei quadrati // SLB was GLfloat
int square2pad_ratio = 90;							// Proporzione tra quadrati e padding // SLB
bool square_ratio = true;							// Forza square ratio alla finestra dello speller // SLB
int text_height = 30;								// Altezza del testo // SLB
int timeFlash;										// Millisecondi di intervallo tra ogni flash
string textureFlash;								// Flashing Texture // SLB
int numberOfFlash;									// Numero di flash per sessione
int currentSession = 0;								// Numero di sessione attuale
int flashingMode;
float backGroundColor[] = { 0.024, 0.059, 0.259, 0.5 };		// Colore di sfondo
float textColor[] = { 1, 1, 0 };					// Colore del testo
bool symbolicTexture = true;						// TRUE = texture dai sensori, FALSE = texture alfabetiche
int idSymbolicTexture = 1;							// ID di mapping per scegliere quali texture mostrare dai sensori
int lastSensorialSelection = -1;					// Indice dell'ultima selezione nel modulo sensoriale effettuata, il default � -1
map<int, string> mapTexture;						// Mappa che associa ad ogni elemento della gui una texture
map<int, string> mapTextureName;					// Nome delle texture da mostrare
map<int, string> mapBere;
map<int, string> mapBereName;
map<int, string> mapCaldo;
map<int, string> mapCaldoName;
map<int, string> mapFreddo;
map<int, string> mapFreddoName;
map<int, string> mapLetto;
map<int, string> mapLettoName;
map<int, string> mapEmozioni;
map<int, string> mapEmozioniName;
map<int, string> mapTv;
map<int, string> mapTvName;
map<int, string> mapTelefono;
map<int, string> mapTelefonoName;
map<int, string> mapRadio;
map<int, string> mapRadioName;
map<int, string> mapCibo;
map<int, string> mapCiboName;
map<int, string> mapSpeller;
map<int, string>* mapVector;
map<int, string>* mapVectorName;
bool spellingOn = false;							// Booleano che indica che � in corso lo speller
bool spellingController = false;					// Booleano attivo quando si avvia lo speller dalla modalit� sensoriale
bool tobiienabled;									// SLB from readConfig, replaces tobii event
//bool featuresenabled;								// SLB TODO temp variable for epyegy/r^2 testing

bool firstFlash = true;
bool startFramework = false;
map<int, bool> memoryInformation;	// SLB

string testoPre;					// Stringa di testo da visualizzare
string testoProcessed;				// Stringa di testo processata
vector <string> targets;			// SLB vector di string targets
string testoSpelling = "";			// Stringa di testo da visualizzare durante lo spelling
int idFromClient[2] = { -1, -1 };	// Valore ricevuto tramite socket
int maxSessions;					// Sessioni
bool readyToFlash = true;			// Variabile che indica se l'interfaccia sia pronta a flashare
bool customPalette;

#define SLBDEBUG FALSE // SLB

// ------------------------------------------------------------------------------------------
// -------------------------------- Parametri per il modulo sensoriale-----------------------
// ------------------------------------------------------------------------------------------
const string API_WEATHER_KEY="ccb404257f4bcca45076537726e2deff";

string city;		//citta' di digitazione
float temperature;	//temperatura
string description;	//descrizione tempo
float wind;			//velocita' vento

// ------------------------------------------------------------------------------------------
// --------------------------- Parametri per il canvas --------------------------------------
// ------------------------------------------------------------------------------------------
#define SPELLER_OUT 5
#define SPELLER_ON 6
typedef int BOOL;

#define TRUE 1
#define FALSE 0
#define VIEWING_DISTANCE_MIN 1.0

static BOOL g_bButton1Down = FALSE;
static BOOL g_bButton2Down = FALSE;
static GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
static GLfloat g_nearPlane = 1;
static GLfloat g_farPlane = 1000 / 13.66;
static int g_yClick = 0;
static int g_xClick = 0;

// ------------------------------------------------------------------------------------------
// -------------------------------- Codici per l'interfaccia --------------------------------
// ------------------------------------------------------------------------------------------
#define START_CALIBRATION 100	// Codice che indica l'inizio della calibrazione
#define END_CALIBRATION -100	// Codice che indica la fine della calibrazione
#define START_STIMULUS 200		// Codice che indica l'inizio di uno stimolo
#define END_STIMULUS -200		// Codice che indica la fine di uno stimolo
#define FLASH_RC 300			// Codice che indica che la modalit� di flash sar� righe e colonne
#define FLASH_S 400				// Codice che indica che la modalit� di flash sar� singolo quadrato
#define RESTART_RC 500			// Codice che indica che ricomincer� la modalit� righe e colonne
//#define	BASELINE_ACQ 550	// SLB
#define RESTART_S 600			// Codice che indica che ricomincer� la modalit� singolo quadrato
#define START_SPELLER 700		// Codice che indica all'interfaccia l'inizio dello speller
#define END_SPELLER -700		// Codice che indica all'interfaccia la fine dello speller
#define START_FRAMEWORK 650		// Codice che indica l'avvio del framework
#define KILL_FRAMEWORK 000		// SLB Codice che indica l'arresto del framework

// -----------------------------------------------------------------------------------------
// --------------------------------- Codici per la socket ----------------------------------
// -----------------------------------------------------------------------------------------
#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

// ------------------------------------------------------------------------------------------
// ---------------------- Inizio della sezione riguardante i quadrati -----------------------
// ------------------------------------------------------------------------------------------
P300Square** squaresVector;			// Vettore di quadrati
P300Row** rowsVector;				// Vettore delle righe
P300Interface* interface;			// Interfaccia grafica

// Vettori con le coordinate u, v
GLfloat *u_coor;
GLfloat *v_coor;
string *tpnVector;		// Vettore dei path delle texture
string *tpnfVector;		// Vettore dei path delle texture flashate

// Funzioni relative ai quadrati nello schermo
void drawRectangle();		 // Funzione che disegna un rettangolo (da eliminare)
void initSquare();           // Funzione che inizializza i quadrati
void initRows();             // Funzione che inizializza le righe
void initInterface();        // Funzione che inizializza l'interfaccia
void loadSquare();           // Funzione che carica le texture
void initParameterVectors(); // Funzione che inizializza il vettore di parametri
void initPaths();            // Funzione che inizializza i vettori con i path
void initMapTexture();  	 // Funzione che inizializza la mappa delle texture
void rebuildInterface(int);
void reloadParameters(int);
void manageSpeller(int);
float colorBronzeDiff[4] = { 1, 1, 1, 1.0 };
float colorBronzeSpec[4] = { 1.0, 1.0, 0.4, 1.0 };
float colorBlue[4] = { 1, 0, 0, 0.0 };
bool pausa = false;
float posx = 0;

// -----------------------------------------------------------------------------
// -------------------- Inizio sezione flash -----------------------------------
// -----------------------------------------------------------------------------
int *counterSquares;		// Contatore di flash dei quadrati
int *counterRows;			// Contatore di flash delle righe
int *counterColumns;		// Contatore di flash delle colonne

int stackIndex = 0;			// Indice dello stack delle righe e delle colonne

bool *showSquare;			// vettore indicante quali quadrati sono flashati
bool *showRow;				// vettore indicante quali righe sono flashate
bool *showColumn;			// vettore indicante quali colonne sono flashate

void flashSquare(int);		// Funzione che flasha un quadrato
void flashRC(int);			// Funzione che flasha righe e colonne

bool firstTime = true;		// booleano che indica quando l'interfaccia viene avviata
int selectedRow = 0;		// codice della riga selezionata
int selectedColumn = 0;		// codice della colonna selezionata

// Per fare flashare randomicamente righe e colonne usiamo uno stack
// riempito randomicamente e che usiamo per selezionare quali indici flashare
stack <int> flashHelperRC;	// Stack per righe e colonne
stack <int> flashHelperS;	// Stack per i quadrati

void fillStackS();			// Funzione che riempie lo stack dei flash per i quadrati
void fillStackRC();			// Funzione che riempie lo stack dei flash per righe e colonne

void initCounterSquare();	// Funzione che setta a zero il contatore di flash dei quadrati
void initCounterRow();		// Funzione che setta a zero il contatore di flash delle righe
void initCounterColumn();	// Funzione che setta a zero il contatore di flash delle colonne

// ------------------------------------------------------------------------------------------
// ------------------------------ Funzioni per il rendering----------------------------------
// ------------------------------------------------------------------------------------------
//string water	= "0_1.f_1.f"; // SLB
string water	= "0_0.949f_1.f"; // SLB edited to average the mismatching background colour in the provided set of icons.
string green	= "0_1_0.1294.f";
string orange	= "1_0.4588_0.098";
string purple	= "1_0_0.647";
string red		= "1_0_0";
string white	= "1_1_1";
string yellow	= "1_1_0";
string black	= "0_0_0";
string blue		= "0_0_1";
string cyan		= "0_0.5412_0.90196";

string WATER	= "water";
string GREEN	= "green";
string ORANGE	= "orange";
string PURPLE	= "purple";
string RED		= "red";
string WHITE	= "white";
string YELLOW	= "yellow";
string BLACK	= "black";
string BLUE		= "blue";
string CYAN		= "cyan";
map<string,string> colorMap;

void Keyboard(unsigned char, int, int);		// Funzione che intercetta gli eventi da tastiera
//void MouseButton(int, int, int, int);		// Funzione che intercetta il click del mouse
//void MouseMotion(int, int);				// Funzione che intercetta il trascinamento del mouse
void display(void);							// Callback principale per il rendering
void reshape(GLint, GLint);					// Callback usata all'avvio della window

// ------------------------------------------------------------------------------------------
// --------------------------- Funzioni per la scritta a video ------------------------------
// ------------------------------------------------------------------------------------------
void printStringOnVideo(void*, char*);		// Funzione che stampa una stringa
void drawOnScreen();						// Funzione che mostra la stringa a video
void processText();							// Funzione che processa la stringa da visualizzare
int selectTextureToShow();					// Funzione che ritorna quale texture mostrare
void updateTestoSpelling();					// Funzione che aggiorna la stringa da mostrare a testo
void rebuildMainWindow(int);
void setTexturesFromMemory();				// SLB

// ------------------------------------------------------------------------------------------
// ------------------------------ Funzioni ausiliarie varie ---------------------------------
// ------------------------------------------------------------------------------------------
void setColorMap();
void sendToServer(int);		// Funzione che invia un comando al server
void initializeAll();		// Funzione che inizializza path, array e puntatori
void deleteAll();			// Funzione che cancella tutti i puntatori
void readConfiguration();	//leggo il file xml di configurazione

// ------------------------------------------------------------------------------------------
// ------------------------------ Funzioni per la socket -- ---------------------------------
// ------------------------------------------------------------------------------------------
DWORD WINAPI createSocket(LPVOID);		// Funzione che crea la socket
size_t write_to_string(void*, size_t, size_t, void*);
int getOnlineParameters();	//Leggo i dati online per conoscere i parametri
void getMemoryInformation(); // SLB

void updateXML(int); //metodo per aggiornare il file xml della memoria
const char* BoolToString(bool b);
void sendCommandToRobot(int);
bool spellerHandler=false;

