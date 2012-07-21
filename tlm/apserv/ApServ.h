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
#include <QObject>

class ApServ;

#include "ApClient.h"
#include "ApText.h"

/*! \brief Data dump service, default port 6170.
 * 
 *
 * \section Startup
 * 
 * Run this in the global thread pool, for example...
 * 
 * \code
 *     this->apserv = new ApServ();
 *
 *     // optionally set portnum or client wait
 * 
 *     QThreadPool::globalInstance()->start(this->apserv);
 * \endcode
 * 
 * After construction and before starting, optionally define port
 * number and client polling wait time.
 * 
 *
 * \section Shutdown
 * 
 * Simply call the "shutdown" method, the threads will handle
 * tear-down and cleanup of all (memory, network and thread)
 * resources.  
 * 
 * After calling shutdown, the caller would disconnect any signals and
 * void its reference to the <ApServ> class -- for example...
 * 
 * \code
 *     this->apserv = 0;
 * \endcode
 * 
 *
 * \section Sending data
 * 
 * This class accepts input on its "update" methods, which are
 * programmable directly or via the Qt slots framework.
 * 
 * The data received via "update" is repeated verbatum to each TCP
 * network client.  The caller and client are responsible for the
 * content of this data.  See "Operation", next.
 * 
 *
 * \section Operation
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
 * desireable, and presents no solution otherwise -- beyond increasing
 * the performance of the underlying hardware and network (or possibly
 * the application architecture).
 * 
 * A design for a guaranteed delivery requirement would employ a data
 * structure different from <ApText>, either a multi-user queue or one
 * client on a conventional queue.  
 * 
 * 
 * \section Service period
 * 
 * The service period parameter defines the frequency with which the
 * principal service thread checks for shutdown.  
 * 
 * The default value of 500ms is intended to be short enough that the
 * actual shutdown activity is relevant to user interaction,
 * experience and expectation -- while this polling should not
 * interfere with the principal operation of GQRX.
 * 
 * The shutdown process is handled in a highly independent way (via
 * the Qt thread pool), so the only obligation here is to user
 * experience.
 * 
 * \section Client wait
 * 
 * Each client sleeps for a period (defined by this parameter) between
 * locking and polling the shared data buffer.  The value of this
 * parameter is a constraint on the throughput to each client.
 * 
 * \subsection Few clients
 * 
 * The default value of 500ms is intended for timely updates in the
 * neighborhood of once per second -- for fewer clients than CPUs.
 * 
 * \subsection Many clients
 * 
 * With a "client wait" value less than the update frequency, multiple
 * client threads may converge in time (to perform their I/O
 * operations concurrently).
 * 
 * This would not be practical for more clients than CPUs, as in this
 * moment GQRX could be starved of processing cycles.  In this case,
 * and update frequency plus ten percent (for example) might preserve
 * the operation of GQRX while occasionally starving a client of an
 * update.
 * 
 * 
 * \section Example Application
 * 
 * See <Afsk1200Win>.  This application employs the default values for
 * port number (6170), and client polling delay (500ms).
 * 
 * 
 * \author John Pritchard, jdp@ulsf.net
 */
class ApServ : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit ApServ(QObject *parent = 0);
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
     * Default 500ms
     */
    unsigned long getClientWait();
    /*
     * Define client wait between polling shared service buffer. Must
     * be greater than zero.  Default 500ms for timely once per second
     * updates.  No change after starting.
     */
    unsigned long setClientWait(unsigned long millis);
    /*
     * Default 500ms
     */
    unsigned long getServicePeriod();
    /*
     * Define service wait between polling on shutdown. Must be
     * greater than zero.  Default 500ms.  No change after starting.
     */
    unsigned long setServicePeriod(unsigned long millis);
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
    /*
     * Accept no new clients
     */
    void shutdown();
    /*
     * Current number of clients
     */
    int getNumClients();

public slots:
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

private:
    ApText* text;
    QThreadPool* pool;
    QTcpServer *server;
    quint16 portnum;
    unsigned long clientWait;
    unsigned long servicePeriod;
    bool failedOpen;
    bool alive;
    bool closing;
};

#endif
