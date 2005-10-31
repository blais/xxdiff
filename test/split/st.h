#include <qapplication.h>
#include <qmainwindow.h>
#include <qsplitter.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qlayout.h>
#include <qobject.h>

class TApp : public QApplication {

   Q_OBJECT

public:

   TApp(int argc, char** argv );
   virtual ~TApp();

public slots:

   void clicked1();
   void clicked2();

public:

   QPushButton* b1;
   QPushButton* b2;

};
