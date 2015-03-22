#ifndef WEB_H
#define WEB_H

#include<QThread>
#include <QWebView>
#include <QDebug>
#include <QWebView>

class Web : public QThread
{
    Q_OBJECT
private:
    QWebView*   web_;
public:
    void setWeb(QWebView* a){
        web_=a;
    }
    QWebView* getWeb(){
        return web_;
    }

    void run(){

    }
    void start(){

    }
};


#endif // WEB_H
