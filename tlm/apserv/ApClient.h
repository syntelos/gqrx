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

class ApClient;

#include "ApServ.h"
#include "ApText.h"

/*! \brief Client socket handler writes updates
 * 
 * Used by <ApServ> to update clients from the shared buffer, <ApText>.
 *
 * \author John Pritchard, jdp@ulsf.net
 */
class ApClient : public QRunnable
{

public:
    ApClient(ApServ& service, QTcpSocket& socket);
    ~ApClient();

    bool autoDelete();

    void run();

private:
    ApServ& service;
    ApText& text;
    QTcpSocket& socket;
    int version;
    unsigned long cycle;
};

#endif // APCLIENT_H
