#include <qvaluelist.h>

#include "st.h"

#include <iostream>

   
TApp::TApp( int argc, char** argv ) : 
   QApplication( argc, argv ), b1(0), b2(0)
{
}

TApp::~TApp()
{}

void TApp::clicked1()
{
   if ( b2->isVisible() ) {
      b2->hide();
   }
   else {
      b2->show();
   }
}

void TApp::clicked2()
{
   if ( b1->isVisible() ) {
      b1->hide();
   }
   else {
      b1->show();
   }
}

//------------------------------------------------------------------------------
//
int main( int argc, char** argv )
{
   TApp app( argc, argv );
   
   QSizePolicy maxsp( QSizePolicy::Maximum, QSizePolicy::Maximum );

   QMainWindow* mw = new QMainWindow;
   QWidget* cw = new QWidget( mw );
   {
      QHBoxLayout* lcw = new QHBoxLayout( cw );

      QSplitter* spl = new QSplitter( cw );
      {
         spl->setOrientation( Qt::Vertical );
         {
            app.b1 = new QPushButton( spl );
            app.b1->setText( "button 1" );
            app.b2 = new QPushButton( spl);
            app.b2->setText( "button 2" );
            
            QObject::connect( app.b1, SIGNAL(clicked()), 
                              &app, SLOT(clicked1()) );
            QObject::connect( app.b2, SIGNAL(clicked()),
                              &app, SLOT(clicked2()) );

         }

         QValueList<int> vl;
         vl.append( 10 );
         vl.append( 20 );
         spl->setSizes( vl );
      }
      lcw->addWidget( spl );

      QPushButton* but = new QPushButton( cw );
      but->setText( "overview" );
      but->setSizePolicy( maxsp );
      lcw->addWidget( but );
   }

   mw->statusBar();
   
   mw->setCentralWidget( cw );
   app.setMainWidget( mw );

   mw->show();

   int r = app.exec();
   return r;
}

