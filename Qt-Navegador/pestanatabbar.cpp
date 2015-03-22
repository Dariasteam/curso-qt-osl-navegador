include "pestanatabbar.h"
#include <QDebug>
#include <QThread>
#include "web.h"
#include <QWebPage>

PestanaTabBar::PestanaTabBar(QWidget *parent)
{
    listaPestanas_= new QList<QWebView*>;
    listaThreads_ = new QList<Web*>;
    this->addTab("");
    listaPestanas_->push_back(new QWebView);
}

PestanaTabBar::~PestanaTabBar()
{

}

void PestanaTabBar::cerrarPestana(int i){
    if(i>=0 && i<listaPestanas_->size()-1){
        removeTab(i);
        listaPestanas_->removeAt(i);
    }
}

void PestanaTabBar::nuevaPestana(){
    this->addTab("Nueva pestaña");
    listaPestanas_->push_back(new QWebView);
    listaPestanas_->last()->load(QString("https://duckduckgo.com"));
    //listaThreads_->push_back(new Web);
    //listaPestanas_->last()->moveToThread(listaThreads_->last());
    //QThread* a = new QThread;
    //listaThreads_->last()->moveToThread(a);
    //listaThreads_->last()->setWeb(listaPestanas_->last());
    //qDebug()<<listaPestanas_->size()<<" "<<listaThreads_->size();
}


QWebView* PestanaTabBar::getWeb(int i){
    if(i>=0 && i<listaPestanas_->size()-1){
        return listaPestanas_->at(i);
    }else{
        nuevaPestana();
        this->setTabEnabled(0,true);
        this->setCurrentIndex(0);
        return listaPestanas_->at(i);
    }
}

int PestanaTabBar::getSz(){
    return listaPestanas_->size();
}

void PestanaTabBar::setWeb(int i, QWebView* w){}


