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
#ifndef _TLM_APCLIENT_H
#define _TLM_APCLIENT_H

#include <QThread>
#include <QPointer>
#include <QTcpSocket>

class ApClient;

#include "ApServ.h"
#include "ApText.h"

/*! \brief Client socket handler writes updates
 * 
 * Used by <ApServ> to update clients from the shared buffer, <ApText>.
 *
 * \author John Pritchard, jdp@ulsf.net
 */
class ApClient : public QThread
{
    Q_OBJECT

public:
    ApClient(ApServ *service, QTcpSocket *socket);
    ~ApClient();

    bool isNotAlive();
    bool isAlive();
    void run();

protected:
    bool isConnected();
    bool isServiceAlive();
    bool isTextReady();
    int getTextVersion();
    ApText* getText();
    void textEnter();
    void textExit();

private:
    QTcpSocket* socket;
    QPointer<ApServ>* service;
    QPointer<ApText>* text;
    int version;
    unsigned long cycle;
    volatile bool alive;
};

#endif // APCLIENT_H
