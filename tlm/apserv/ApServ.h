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
#ifndef _TLM_APSERV_H
#define _TLM_APSERV_H


#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadPool>
#include <QRunnable>

class ApServ;

#include "ApClient.h"
#include "ApText.h"

/*! \brief Data dump service, default port 6170.
 * 
 * Run this in the global thread pool, for example...
 * 
 * \code
 *     ApServ *apserv = new ApServ();
 *
 *     // optionally set portnum or client wait
 * 
 *     QThreadPool::globalInstance()->start(apserv);
 * \endcode
 * 
 * After construction and before starting, optionally define port
 * number and client polling wait time.
 * 
 * Each client periodically checks on a shared buffer, <ApText>.  When
 * the shared buffer has changes, each client writes the apparent
 * change.  If the shared buffer update frequency is greater than the
 * client polling and writing frequency, the (network) service will
 * simply skip updates.
 * 
 * This design covers most application cases, but not all.  Depending
 * on the application and use of this facility, the "most recent
 * update" behavior may be desireable or undesireable.  This facility
 * is designed for application and use cases where this behavior is
 * desireable, and presents no solution beyond increasing the
 * performance of the underlying hardware and network (or possibly the
 * application architecture).
 * 
 * A design for a guaranteed delivery requirement would employ a data
 * structure different from <ApText>, either a multi-user queue or one
 * client on a conventional queue.
 * 
 * \author John Pritchard, jdp@ulsf.net
 */
class ApServ : public QRunnable
{

public:
    ApServ();
    /*
     * This destructor will block until all clients have terminated
     */
    ~ApServ();
    /*
     * Not set after starting.
     */
    quint16 getPortnum();
    /*
     * Define TCP service port number.  Default 6170.  No change after
     * starting.
     */
    quint16 setPortnum(quint16 p);
    /*
     * Default 300ms
     */
    unsigned long getClientWait();
    /*
     * Define client wait between polling shared service buffer. Must
     * be greater than zero.  Default 500ms for timely once per second
     * updates.  No change after starting.
     */
    unsigned long setClientWait(unsigned long millis);
    /*
     * Modify the shared buffer with content to write to all clients
     */
    void update(char *buffer, int length);
    /*
     * Modify the shared buffer with content to write to all clients
     */
    void update(QByteArray& buffer);
    /*
     * Modify the shared buffer with content to write to all clients
     */
    void update(QString& buffer);
    /*
     * Client reference to the shared data buffer
     */
    ApText& getText();
    /*
     * Return true when server listen call returns false, indicating
     * an occupied port number.
     */
    bool errorOpen();
    /*
     * Return true when the server socket accept loop has entered.
     */
    bool isAlive();
    /*
     * QRunnable
     */
    bool autoDelete();
    /*
     * QRunnable
     */
    void run();

private:
    ApText* text;
    QThreadPool* pool;
    QTcpServer *server;
    quint16 portnum;
    unsigned long clientWait;
    bool failedOpen;
    bool alive;
};

#endif
