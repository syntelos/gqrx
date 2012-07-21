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

#include <QThread>
#include <QDebug>

#include "ApClient.h"

ApClient::ApClient(ApServ& service, QTcpSocket& socket) :
    QRunnable(),
    service(service),
    text(service.getText()),
    socket(socket),
    version(0),
    cycle(service.getClientWait())
{
}
ApClient::~ApClient()
{
    delete &(this->socket);
}
bool ApClient::autoDelete(){

    return true;
}
void ApClient::run(){

    QThread *thread = QThread::currentThread();

    bool error = false;

    while (this->service.isAlive() && this->socket.isValid()){

        this->text.enter();
        /*
         * Polling via read lock on shared data buffer
         */
        if (this->text.isReady(this->version)){

            this->version = this->text.getVersion();

            error = (-1 == this->socket.write(this->text));
        }

        this->text.exit();

        if (error || (!this->service.isAlive())){

            break;
        }
        else {

            //thread->msleep(this->cycle);
        }
    }

    qDebug() << ("client shutdown\n");

    this->socket.close();

    qDebug() << ("client end\n");
}
