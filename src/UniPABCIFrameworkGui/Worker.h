#ifndef __COUNT_WORKER_H__
#define __COUNT_WORKER_H__

// SLB QT4
#include <QtCore/QObject>
//
/* SLB QT4
#include <Qt/qobject.h>
*/
/* SLB QT5
#include <QtCore\qobject.h>
*/
#include <string>

using std::string;


class Worker : public QObject {
    Q_OBJECT
    public:
        Worker();
    public slots:
        void doWork();						// metodo che invia il messaggio alla gui
	signals:
		void updateStamp(string);
		//void gameSetChoiceIcons(int);		// SLB
        void finished();
    private:
		string message;
		//int gameMove;
};
 
#endif // __COUNT_WORKER_H__