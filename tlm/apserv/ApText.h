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
#ifndef _TLM_APTEXT_H
#define _TLM_APTEXT_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QReadWriteLock>

/*! \brief Shared data buffer
 * 
 * Used by the writer <ApServ>, and the reader <ApClient>.
 *
 * \author John Pritchard, jdp@ulsf.net
 */
class ApText : public QObject, public QByteArray
{

public:
    explicit ApText(QObject *parent = 0);
    ~ApText();
    /*
     * Reader
     */
    void enter();
    /*
     * Reader
     */
    void exit();
    /*
     * Reader updates its local copy of the last read version
     */
    int getVersion();
    /*
     * Version is not equal to the current version, and the current
     * version is not zero
     */
    bool isReady(int version);
    /*
     * Has been updated, version is not zero
     */
    bool isReady();
    /*
     * Writer
     */
    void update(const char *buffer, int length);

    void update(const QByteArray& buffer);

    void update(const QString& buffer);

private:
    QReadWriteLock *lock;
    int version;
};

#endif // APTEXT_H
