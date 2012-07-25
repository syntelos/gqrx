/* -*- c++ -*- */
/*
 * Gqrx
 * Copyright 2011 Alexandru Csete OZ9AEC.
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

#include "ApClient.h"


ApClient::ApClient(ApServ *service, QTcpSocket *socket) :
    QThread(),
    socket(socket),
    service(new QPointer<ApServ>(service)),
    text(new QPointer<ApText>(service->getText())),
    version(0),
    cycle(service->getClientWait()),
    alive(false)
{
    this->setObjectName(socket->peerAddress().toString());
}
ApClient::~ApClient()
{
    delete this->socket;

    delete this->service;

    delete this->text;
}
bool ApClient::isAlive(){

    return (this->alive);
}
bool ApClient::isConnected(){

    return (QAbstractSocket::ConnectedState == this->socket->state());
}
/*
 * Intended for external (thread) management
 */
bool ApClient::isNotAlive(){

    return (!this->alive);
}
bool ApClient::isServiceAlive(){
    ApServ *service = this->service->data();
    if (NULL != service)
        return service->isAlive();
    else
        return false;
}
bool ApClient::isTextReady(){
    ApText *text = this->text->data();
    if (NULL != text)
        return text->isReady(this->version);
    else
        return false;
}
int ApClient::getTextVersion(){
    ApText *text = this->text->data();
    if (NULL != text)
        return text->getVersion();
    else
        return 0;
}
ApText* ApClient::getText(){

    return this->text->data();
}
void ApClient::textEnter(){
    ApText *text = this->text->data();

    if (NULL != text){

        text->enter();
    }
}
void ApClient::textExit(){
    ApText *text = this->text->data();

    if (NULL != text){

        text->exit();
    }
}
void ApClient::run(){

    this->alive = true;

    this->setPriority(QThread::LowPriority);

    bool error = false;

    while (this->isServiceAlive() && this->isConnected()){

        this->textEnter();
        /*
         * Polling via read lock on shared data buffer
         */
        if (this->isTextReady()){

            qDebug() << "ApClient found update";

            this->version = this->getTextVersion();

            ApText *text = this->getText();

            if (NULL != text){

                const ApText& tr = *text;

                error = (-1 == this->socket->write(tr));

                if (!error)
                    qDebug() << "ApClient wrote " << tr;
                else
                    qDebug() << "ApClient error writing " << tr;
            }
            else
                error = true;
        }

        this->textExit();

        if (error ||
            (!this->isServiceAlive()) ||
            (!this->isAlive()))
        {
            break;
        }
        else {

            QThread::msleep(this->cycle);
        }
    }

    if (this->isServiceAlive()){

        this->socket->close();
    }

    this->alive = false;
}
