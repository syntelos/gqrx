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

#include "ApServ.h"

ApServ::ApServ() :
    QRunnable(),
    text(),
    pool(),
    portnum(6170),
    clientWait(500),
    failedOpen(false),
    alive(false)
{
    this->server = 0;
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
void ApServ::update(char *buffer, int length){

    this->text->update(buffer,length);
}
void ApServ::update(QByteArray& buffer){

    this->text->update(buffer);
}
void ApServ::update(QString& buffer){

    this->text->update(buffer);
}
ApText& ApServ::getText(){

    return *(this->text);
}
quint16 ApServ::getPortnum(){

    return this->portnum;
}
quint16 ApServ::setPortnum(quint16 p){

    if (0 == this->server){

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

    if (0 == this->server){

        if (0 < ms){

            this->clientWait = ms;
        }
    }
    return this->clientWait;
}
bool ApServ::autoDelete(){

    return true;
}
void ApServ::run(){
    if (0 == this->server){

        this->server = new QTcpServer();

        if (this->server->listen(QHostAddress::Any,this->portnum)){

            this->failedOpen = false;

            while (this->server->waitForNewConnection()){

                this->alive = true;

                QTcpSocket* sock = this->server->nextPendingConnection();

                if (0 != sock){

                    ApClient *cli = new ApClient(*this,*sock);

                    this->pool->start(cli);
                }
            }
            this->alive = false;
            this->server->close();
        }
        else {
            this->failedOpen = true;
            this->alive = false;
        }
    }
}
