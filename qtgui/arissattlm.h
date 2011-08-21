/* -*- c++ -*- */
/*
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
#ifndef ARISSATTLM_H
#define ARISSATTLM_H

#include <QWidget>
#include <QByteArray>
#include "tlm/arissat/ss_types_common.h"


namespace Ui {
    class ArissatTlm;
}


/*! \brief Disaply widget for ARISSat-1 telemetry. */
class ArissatTlm : public QWidget
{
    Q_OBJECT

public:
    explicit ArissatTlm(QWidget *parent = 0);
    ~ArissatTlm();

    void processData(QByteArray &data);

private:
    void showMissionData(ss_telem_t &tlm);
    void showIhuTemps(ss_telem_t &tlm);
    void showBattery(ss_telem_t &tlm);
    void showPower(ss_telem_t &tlm);

private:
    Ui::ArissatTlm *ui;

    u32 frameCounter;
};

#endif // ARISSATTLM_H