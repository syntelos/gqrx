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

#include "ApText.h"


ApText::ApText() :
    QByteArray(),
    lock(new QReadWriteLock()),
    version(0)
{
}
ApText::~ApText()
{
    delete this->lock;
}

void ApText::enter(){

    this->lock->lockForRead();
}
void ApText::exit(){

    this->lock->unlock();
}
bool ApText::isReady(int version){

    return ((0 != this->version)&&(version != this->version));
}
bool ApText::isReady(){

    return (0 != this->version);
}
int ApText::getVersion(){

    return this->version;
}
void ApText::update(char *buffer, int length){
    if (NULL != buffer && 0 < length){
        this->lock->lockForWrite();
        {
            this->version++;
            if (0 == this->version)
                this->version = 1;

            this->clear();
            this->append(buffer,length);
        }
        this->lock->unlock();
    }
}
void ApText::update(QByteArray& buffer){
    if (0 < buffer.size()){
        this->lock->lockForWrite();
        {
            this->version++;
            if (0 == this->version)
                this->version = 1;

            this->clear();
            this->append(buffer);
        }
        this->lock->unlock();
    }
}
void ApText::update(QString& buffer){
    if (0 < buffer.size()){
        this->lock->lockForWrite();
        {
            this->version++;
            if (0 == this->version)
                this->version = 1;

            this->clear();
            this->append(buffer);
        }
        this->lock->unlock();
    }
}
