#ifndef WEBBROWSER_H
#define WEBBROWSER_H

#include <QGridLayout>
#include <QWebView>
#include <QLineEdit>
#include <QToolButton>
#include <QWidget>
#include <QList>
#include <QToolBar>
#include <QAction>
#include <QBoxLayout>
#include "favoritoaction.h"
#include <QTabBar>
#include <QStaticText>
#include <QAbstractScrollArea>

class WebBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit WebBrowser(QWidget *parent = 0);

private:
    QList<QWebView*>*  web_;
    QAbstractScrollArea*    historial_;
    QLineEdit *address_;
    QToolButton *refresh_;
    QToolButton *back_;
    QToolButton *forward_;
    QToolButton *home_;
    QGridLayout *layout_;
    QString         homepage_;
    QToolButton* add_marcadores_;
    QList<QAction*>*    marcadores_;
    QToolBar*     barra_;
    QToolButton*  settingsButton_;
    QToolButton*        settingsMenu_;
    QBoxLayout*   pestana_;
    QTabBar*     barraPestanas_;
    QAction*      setHome_;
    QAction*      getHistorial_;
    int           current_;



private:
    void setupConnections();
    void updateConnections();
    bool comprobarMarcadores();
    void iniciarMarcadores();
    QString Home(bool);
signals:

public slots:
    void historial();
    void enviarUrl(QAction*);
    void onLoad();
    void onHome();
    void onUrlChange(QUrl url);
    void onAddMarcador();
    void onLoadFinished(bool ok);
    void setWeb(int);
    void setHome();
    void cerrarPestana(int);
    void updateTab(int, int);
public:
};

#endif // WEBBROWSER_H
