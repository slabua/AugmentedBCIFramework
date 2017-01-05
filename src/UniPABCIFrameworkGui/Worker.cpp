// SLB QT4
#include <QtGui/QApplication>
//
/* SLB QT4
#include <Qt/qapplication.h>
*/
/* SLB QT5
#include <QtWidgets\qapplication.h>
*/
#include "worker.h"
#include "moc_worker.cpp"
 
Worker::Worker() {

	message = "";
	//gameMove = -1;
}

void Worker::doWork() {

	emit updateStamp(message);
	//emit gameSetChoiceIcons(gameMove); // SLB
	qApp->processEvents();
	emit finished();
}

