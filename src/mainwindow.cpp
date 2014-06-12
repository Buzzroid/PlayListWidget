#include "mainwindow.h"

mainwindow::mainwindow():
	prevbutton(new QPushButton("Prev",this)),
	nextbutton(new QPushButton("Next",this)),
	layout(new QGridLayout(this)),
	playlist(new playlistwidgetnew(this))
{
	layout->addWidget(playlist,0,0,10,2);
	layout->addWidget(prevbutton,10,0,1,1);
	layout->addWidget(nextbutton,10,1,1,1);
	this->setLayout(layout);
	prevbutton->setEnabled(false);
	nextbutton->setEnabled(false);
	connect(prevbutton,SIGNAL(clicked()),this,SLOT(prevslot()));
	connect(nextbutton,SIGNAL(clicked()),this,SLOT(nextslot()));
	connect(playlist,SIGNAL(removeactionsignal(const QList<QTreeWidgetItem *>)),this,SLOT(removeactionslot(const QList<QTreeWidgetItem *>)));
	connect(playlist,SIGNAL(clearactionsignal()),this,SLOT(clearslot()));
	connect(playlist,SIGNAL(trackcountchange(int)),this,SLOT(trackcountslot(int)));
}

void mainwindow::nextslot()
{
	playlist->skiptonext();
}

void mainwindow::prevslot()
{
	playlist->skiptoprev();
}

void mainwindow::clearslot()
{
	playlist->clear();
}

void mainwindow::removeactionslot(const QList<QTreeWidgetItem *> list)
{
	bool activechanged=false;
	const playlisttrack *actualtrack=NULL;

	for (int i=0; i<list.count(); i++)
	{
		if (static_cast<playlisttrack *>(list.at(i))->isactive())
		{
			activechanged=true;
			actualtrack=playlist->skiptonext();
		}

		delete list.at(i);
	}

	playlist->recalcall();
}

void mainwindow::trackcountslot(int count)
{
	if (count>1)
	{
		prevbutton->setEnabled(true);
		nextbutton->setEnabled(true);
	}
	else
	{
		prevbutton->setEnabled(false);
		nextbutton->setEnabled(false);
	}

}

