#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimeEdit>
#include <QTime>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <windows.h>
#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <signal.h>

using namespace std;

pthread_t th_time,th_clock;
QTime timp_oprire;
QLabel *lb_ceas;
QLabel *lb_dummy;
int active=0;

class Widget : public QWidget
{
private:
    QTime timp_curent=QTime::currentTime();

    QTimeEdit *time_box=new QTimeEdit(timp_curent);

    QHBoxLayout *up_lay=new QHBoxLayout;
    QHBoxLayout *mid_lay=new QHBoxLayout;
    QHBoxLayout *down_lay=new QHBoxLayout;
    QVBoxLayout *main_lay=new QVBoxLayout;
    QLabel *lb_timp=new QLabel("Ora ");
    QLabel *lb_oprire=new QLabel("NESETAT");
    QLabel *lb_et_oprire=new QLabel("Ora la care se va opri: ");


    QPushButton *pb_set=new QPushButton("Set shutdown");
    QPushButton *pb_cancel=new QPushButton("Cancel current shutdown");

    void conectare()
    {
        QObject::connect(pb_set,&QPushButton::clicked,[&]()
        {
            timp_oprire=time_box->time();
            timp_oprire.setHMS(timp_oprire.hour(),timp_oprire.minute(),0);
            lb_oprire->setText(timp_oprire.toString());
            if(active==0)
            {
                active=1;

                pthread_create(&th_time,NULL,[](void* p)->void*
                {
                    QTime tmp;
                    QTime tmp2;
                    tmp=QTime::currentTime();
                    int h,m,s;


                    while((active)&&((tmp<timp_oprire)||(tmp>tmp2)))
                    {
                        h=timp_oprire.hour();
                        m=timp_oprire.minute();
                        s=timp_oprire.second();

                        m+=1;
                        if(m>=60)
                            h+=1;
                        if(h>=24)
                            h=0;
                        tmp2.setHMS(h,m,s);
                        Sleep(1000);
                        tmp=QTime::currentTime();
                    }
                    if(active)
                    {
                        Widget* w;
                        w=(Widget*)p;
                        system("shutdown /s /t 0");
                        w->close();
                      // system("cls");
                        return NULL;
                    }
                    cout<<"DEZACTIVAT";
                    return NULL;
                }
                ,this);
            }
            cout<<"CLICK";
        });

        QObject::connect(pb_cancel,&QPushButton::clicked,[&]()
        {
            if(active)
            {
                active=0;
                lb_oprire->setText("NESETAT!");
            }

        });
    }
public:
    Widget()
    {
        conectare();
        lb_ceas=new QLabel("Ceas: ");
        lb_dummy=new QLabel("");
        lb_ceas->setText(QTime::currentTime().toString());

        setLayout(main_lay);
        main_lay->addLayout(up_lay);
        main_lay->addLayout(mid_lay);
        main_lay->addLayout(down_lay);
        up_lay->addWidget(lb_timp);
        up_lay->addWidget(time_box);
        up_lay->addWidget(pb_set);
        mid_lay->addWidget(lb_ceas);
        mid_lay->addWidget(pb_cancel);
        down_lay->addWidget(lb_et_oprire);
        down_lay->addWidget(lb_oprire);
        pthread_create(&th_clock,NULL,[](void* p)->void*
        {
            while(true)
            {
                Sleep(500);
                lb_ceas->setText(QTime::currentTime().toString());
            }
            return NULL;}
        ,NULL);
    }

    ~Widget()
    {
        pthread_cancel(th_clock);
        pthread_join(th_time,NULL);

    }
};

#endif // WIDGET_H
