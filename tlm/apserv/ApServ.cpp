/* -*- c++ -*- */
/*
 * Gqrx
 * Copyright 2012 Alexandru Csete OZ9AEC.
 *
 * Gqrx is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Gqrx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gqrx; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <QHostAddress>
#include <QDebug>

#include "ApServ.h"

ApServ::ApServ(QObject *p) :
    QObject(p),
    QRunnable(),
    text(new ApText(this)),
    pool(new ApClient*[10]),
    server(),
    portnum(6170),
    clientWait(500),
    servicePeriod(500),
    numClients(10),
    failedOpen(false),
    alive(false),
    closing(false)
{
    this->poolset();
}
ApServ::~ApServ()
{
    delete this->text;
    delete this->pool;
    delete this->server;
}
bool ApServ::errorOpen(){

    return this->failedOpen;
}
bool ApServ::isAlive(){

    return this->alive;
}
void ApServ::update(const char *buffer, int length){

    this->text->update(buffer,length);
}
void ApServ::update(const QByteArray& buffer){

    this->text->update(buffer);
}
void ApServ::update(const QString& buffer){

    this->text->update(buffer);
}
ApText* ApServ::getText(){

    return this->text;
}
quint16 ApServ::getPortnum(){

    return this->portnum;
}
quint16 ApServ::setPortnum(quint16 p){

    if (NULL == this->server){

        if (0 < p){

            this->portnum = p;
        }
    }
    return this->portnum;
}
unsigned long ApServ::getClientWait(){

    return this->clientWait;
}
unsigned long ApServ::setClientWait(unsigned long ms){

    if (NULL == this->server){

        if (0 < ms){

            this->clientWait = ms;
        }
    }
    return this->clientWait;
}
unsigned long ApServ::getServicePeriod(){

    return this->servicePeriod;
}
unsigned long ApServ::setServicePeriod(unsigned long ms){

    if (NULL == this->server){

        if (0 < ms){

            this->servicePeriod = ms;
        }
    }
    return this->servicePeriod;
}
int ApServ::getNumClients(){

    return this->numClients;
}
int ApServ::setNumClients(int num){

    if (NULL == this->server){

        if (0 < num){

            this->numClients = num;

            delete this->pool;

            this->pool = new ApClient*[num];

            this->poolset();
        }
    }
    return this->numClients;
}
/*
 * Portable memset
 */
void ApServ::poolset(){

    int num = this->numClients;

    for (int cc = 0; cc < num; cc++){

        this->pool[cc] = NULL;
    }
}
bool ApServ::autoDelete(){

    return true;
}
void ApServ::shutdown(){

    this->closing = true;
}
bool ApServ::cleanup(){

    bool more = false;

    int count = this->numClients;

    for (int cc = 0; cc < count; cc++){

        ApClient* client = this->pool[cc];

        if (NULL != client){

            if (client->isNotAlive()){

                this->pool[cc] = NULL;

                delete client;
            }
            else {
                more = true;
            }
        }
    }
    return more;
}
void ApServ::waitfor(){

    int count = this->numClients;

    for (int cc = 0; cc < count; cc++){

        ApClient* client = this->pool[cc];

        if (NULL != client && client->isAlive()){

            client->wait();

            return;
        }
    }
}
int ApServ::next(){

    int count = this->numClients;

    for (int cc = 0; cc < count; cc++){

        if (NULL == this->pool[cc])
            return cc;
    }
    return -1;
}
void ApServ::run(){

    if (NULL == this->server){

        this->server = new QTcpServer();

        if (this->server->listen(QHostAddress::Any,this->portnum)){

            qDebug() << "Server accepting connections on port " << this->portnum;

            this->failedOpen = false;

            while (!this->closing){

                this->alive = true;
                /*
                 * Polling on shutdown
                 */
                if (this->server->waitForNewConnection(this->servicePeriod)){
                    /*
                     * Accept client
                     */
                    QTcpSocket* sock = this->server->nextPendingConnection();

                    if (NULL != sock){

                        int next = this->next();

                        if (-1 < next){

                            ApClient *cli = new ApClient(this,sock);

                            this->pool[next] = cli;

                            cli->start();
                        }
                        else
                            sock->close();
                    }
                }
                else {

                    this->cleanup();
                }
            }

            this->alive = false;

            this->server->close();

            while (this->cleanup()){

                this->waitfor();
            }
        }
        else {
            qDebug() << "Server failed open of port " << this->portnum;

            this->failedOpen = true;
            this->alive = false;
        }
    }
}
