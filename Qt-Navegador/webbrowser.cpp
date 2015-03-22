#include "webbrowser.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QAction>
#include <QToolBar>
#include <QMenu>
#include <QBoxLayout>
#include "favoritoaction.h"
#include <QTabBar>
#include <QWebFrame>
#include <QLabel>
#include <QAbstractScrollArea>


WebBrowser::WebBrowser(QWidget *parent) :
    QWidget(parent)
{
    address_ = new QLineEdit;
    refresh_ = new QToolButton;
    back_ = new QToolButton;
    forward_ = new QToolButton;
    home_ = new QToolButton;
    layout_ = new QGridLayout;
    add_marcadores_ = new QToolButton;
    settingsButton_ = new QToolButton;
    settingsMenu_ = new QToolButton;


    marcadores_ = new QList<QAction*>();
    barra_ = new QToolBar();

    homepage_ = Home(false);

    barraPestanas_ = new QTabBar(this);



    web_=new QList<QWebView*>;

    setWeb(0);
    barraPestanas_->setTabsClosable(true);
    barraPestanas_->setMovable(true);

    setHome_ = new QAction(tr("&Establecer como home"), this);
    getHistorial_ = new QAction(tr("&Historial"),this);

    current_=0;
    refresh_->setIcon(QIcon(QPixmap(":/icons/resources/refresh.png")));
    back_->setIcon(QIcon(QPixmap(":/icons/resources/go-previous.png")));
    forward_->setIcon(QIcon(QPixmap(":/icons/resources/go-next.png")));
    home_->setIcon(QIcon(QPixmap(":/icons/resources/go-home.png")));
    settingsMenu_ ->setIcon((QIcon(QPixmap(":/icons/menu-alt-512.png"))));
    layout_->addWidget(back_,0,0,1,1);
    layout_->addWidget(forward_,0,1,1,1);
    layout_->addWidget(home_,0,2,1,1);
    layout_->addWidget(refresh_,0,3,1,1);
    layout_->addWidget(address_,0,4,1,1);
    layout_->addWidget(add_marcadores_,0,5,1,1);
    layout_->addWidget(settingsMenu_,0,6,1,1);
    layout_->addWidget(barraPestanas_,1,0,1,7);
    layout_->addWidget(barra_,4,0,1,6);
    address_->setText(homepage_);
    setLayout(layout_);
    setupConnections();
    iniciarMarcadores();
    settingsMenu_->addAction(setHome_);
    settingsMenu_->addAction(getHistorial_);
}

void WebBrowser::setupConnections()
{
    connect(address_,SIGNAL(returnPressed()),this,SLOT(onLoad()));
    connect(refresh_,SIGNAL(pressed()),web_->at(current_),SLOT(reload()));
    connect(forward_,SIGNAL(pressed()),web_->at(current_),SLOT(forward()));
    connect(back_,SIGNAL(pressed()),web_->at(current_),SLOT(back()));
    connect(home_,SIGNAL(pressed()),this,SLOT(onHome()));
    connect(web_->at(current_),SIGNAL(urlChanged(QUrl)),this,SLOT(onUrlChange(QUrl)));
    connect(web_->at(current_),SIGNAL(loadFinished(bool)),this,SLOT(onLoadFinished(bool)));
    connect(add_marcadores_,SIGNAL(pressed()),this,SLOT(onAddMarcador()));
    connect(barraPestanas_,SIGNAL(tabCloseRequested(int)),this,SLOT(cerrarPestana(int)));
    connect(barraPestanas_,SIGNAL(tabBarClicked(int)),this,SLOT(setWeb(int)));
    connect(setHome_,SIGNAL(triggered()),this,SLOT(setHome()));
    connect(getHistorial_,SIGNAL(triggered()),this,SLOT(historial()));
    connect(barraPestanas_,SIGNAL(tabMoved(int,int)),this,SLOT(updateTab(int,int)));
}

void WebBrowser::updateConnections()
{
    connect(address_,SIGNAL(returnPressed()),this,SLOT(onLoad()));
    connect(refresh_,SIGNAL(pressed()),web_->at(current_),SLOT(reload()));
    connect(forward_,SIGNAL(pressed()),web_->at(current_),SLOT(forward()));
    connect(back_,SIGNAL(pressed()),web_->at(current_),SLOT(back()));
    connect(home_,SIGNAL(pressed()),this,SLOT(onHome()));
    address_->setText((web_->at(current_)->url().toString()));
}

void WebBrowser::historial(){
    historial_ = new QAbstractScrollArea();
    historial_->setMaximumHeight(50);
    layout_->addWidget(historial_,2,0,1,7);
    QList<QToolButton*>* A = new QList<QToolButton*>;
    QFile* h = new QFile("history.txt");
    h->open(QIODevice::ReadOnly|QIODevice::Text);
    if(h->isOpen()){
        QTextStream a(h);
        a.seek(0);
        while(!a.atEnd()){
            A->push_back(new QToolButton(this));
            A->last()->setText(a.readLine());
            historial_->addScrollBarWidget(A->last(),Qt::AlignBottom);
        }
    }
    h->close();
    web_->at(current_)->setVisible(false);
}



void WebBrowser::onLoad()
{
    barraPestanas_->setTabText(barraPestanas_->currentIndex(),web_->at(current_)->page()->mainFrame()->title());
    if(!address_->text().startsWith("http://")
            && !address_->text().startsWith("https://")
            && address_->text().length()!=0){
        QString dir = address_->text();                     //Ajuste para la búsqueda de varias palabras
        for(int i=0;i<dir.size();i++){
            if(dir[i]==' '){
                dir[i]='+';
            }
        }
        QString aux;
        aux="https://duckduckgo.com/?q=";
        web_->at(current_)->load(aux+dir);
    }else{
        web_->at(current_)->load(address_->text());
    }
}

void WebBrowser::onHome()
{
    web_->at(current_)->load(Home(false));
}

void WebBrowser::onUrlChange(QUrl url)
{
    address_->setText(url.toString());
}

void WebBrowser::onLoadFinished(bool ok)
{
    barraPestanas_->setTabText(barraPestanas_->currentIndex(),web_->at(current_)->page()->mainFrame()->title());
    if(!ok)
        onLoad();
    else{
        comprobarMarcadores();
        QFile* h = new QFile("history.txt");
        h->open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
        if(h->isOpen()){
            QTextStream a(h);
            a<<address_->text()<<"\n";
        }
        h->close();
    }
}


void WebBrowser::iniciarMarcadores(){
    QFile* fav = new QFile("favoritos.txt");
    fav->open(QIODevice::ReadOnly |QIODevice::Text);
    if(fav->isOpen()){
        QTextStream a(fav);
        marcadores_->clear();
        a.seek(0);
        while(!a.atEnd()){
            marcadores_->push_back(new QAction(this));
            marcadores_->last()->setText(a.readLine());
        }
        layout_->removeWidget(barra_);
        delete barra_;
        barra_ = new QToolBar();
        for(int i=0;i<marcadores_->size();i++){
           barra_->addAction(marcadores_->at(i));
        }
        layout_->addWidget(barra_,3,0,1,6);
        layout_->update();
        connect(barra_,SIGNAL(actionTriggered(QAction*)),this,SLOT(enviarUrl(QAction*)));
    }
}



void WebBrowser::enviarUrl(QAction* A){
    web_->at(current_)->load(A->text());
    qDebug()<<"yes";
}


void WebBrowser::onAddMarcador(){
    QFile* fav = new QFile("favoritos.txt");
    fav->open(QIODevice::ReadWrite |QIODevice::Text | QIODevice::Append);
    if(fav->isOpen()){
        int existe = -1;
        for(int i=0;i<marcadores_->size();i++){
            if(marcadores_->at(i)->text()==address_->text()){
                existe=i;
            }
        }
        QTextStream a(fav);
        a.seek(0);
        if(existe>-1){
            fav->resize(0);
            for(int i=0;i<marcadores_->size();i++){
                if(i!=existe){
                    a<<marcadores_->at(i)->text()<<"\n";
                }else{
                    marcadores_->removeAt(i);
                }
            }
            add_marcadores_->setIcon(QIcon(QPixmap(":/icons/05-star_37717_lg.gif")));
        }else{
            marcadores_->push_back(new QAction(address_->text(),this));
            a<<address_->text()<<"\n";
            add_marcadores_->setIcon(QIcon(QPixmap(":/icons/star_PNG1592.png")));
        }

        layout_->removeWidget(barra_);
        barra_->clear();
        for(int i=0;i<marcadores_->size();i++){
           barra_->addAction(marcadores_->at(i));
        }
        layout_->addWidget(barra_,3,0,1,6);
        fav->close();
    }
}

bool WebBrowser::comprobarMarcadores(){
    for(int i=0;i<marcadores_->size();i++){
        if(address_->text()==marcadores_->at(i)->text()){
            add_marcadores_->setIcon(QIcon(QPixmap(":/icons/star_PNG1592.png")));
            return true;
        }
    }
    add_marcadores_->setIcon(QIcon(QPixmap(":/icons/05-star_37717_lg.gif")));
    return false;
}


void WebBrowser::setHome(){
    Home(true);
}

QString WebBrowser::Home(bool write){
    QFile* fav = new QFile("defaulthome.txt");
    fav->open(QIODevice::ReadWrite |QIODevice::Text);
    if(fav->isOpen()){
        QTextStream a(fav);
        if(write){
            fav->resize(0);
            a<<address_->text();
            fav->close();
        }else{
            QString Q(a.readAll());
            if(!Q.isEmpty()){
                return Q;
            }else{
                QString F("www.duckduckgo.com");
                return F;
            }
        }
    }
}

void WebBrowser::setWeb(int i){
    if(i!=barraPestanas_->currentIndex() && i>=0){
        if(barraPestanas_->count()==0){                        //Inicializacion
            web_->push_back(new QWebView);
            web_->last()->load(Home(false));
            layout_->addWidget(web_->at(0),2,0,1,7);
            barraPestanas_->addTab("");
            barraPestanas_->addTab("Nueva Pestaña");
            barraPestanas_->setCurrentIndex(0);
            current_=0;
        }else if(i==barraPestanas_->count()-1){               //Añadir nueva pestaña
            web_->at(current_)->setVisible(false);
            web_->push_back(new QWebView);
            web_->last()->load(Home(false));
            layout_->addWidget(web_->at(i),2,0,1,7);
            current_=i;
            barraPestanas_->addTab("Nueva Pestaña");;
            connect(web_->at(current_),SIGNAL(urlChanged(QUrl)),this,SLOT(onUrlChange(QUrl)));
            connect(web_->at(current_),SIGNAL(loadFinished(bool)),this,SLOT(onLoadFinished(bool)));
        }else if(i!=web_->size()){                            //Seleccioanr cualquier pestaña menos la última
            web_->at(current_)->setVisible(false);
            current_=i;
            web_->at(current_)->setVisible(true);
        }
        updateConnections();
        comprobarMarcadores();
    }
}

void WebBrowser::updateTab(int to, int from){
    if(from==web_->size()){
        //barraPestanas_->moveTab(to,from);
    }else if(to==web_->size()){
        //web_->move(from,web_->size()-1);
        //barraPestanas_->moveTab(to,1);
        current_=0;
    }else{
        web_->move(from,to);
        current_=to;
    }
}

void WebBrowser::cerrarPestana(int i){
    if(i>=0 && i<barraPestanas_->count()-1){
        web_->removeAt(i);
        barraPestanas_->removeTab(i);
        if(barraPestanas_->count()!=1){
            if(i==0){
                web_->at(1)->setVisible(true);
                barraPestanas_->setCurrentIndex(0);
                current_=0;
                address_->setText(web_->at(1)->url().toString());
            }else{
                web_->at(i-1)->setVisible(true);
                barraPestanas_->setCurrentIndex(i-1);
                current_=i-1;
                address_->setText(web_->at(i-1)->url().toString());
            }
        }else{
            this->close();
        }
    }
}
