/* -*- c++ -*- */
/*
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
#include <iostream>
#include "receivers/wfmrx.h"

#define PREF_QUAD_RATE  240000.0  // Nominal channel spacing is 200 kHz
#define PREF_AUDIO_RATE 48000.0

wfmrx_sptr make_wfmrx(float quad_rate, float audio_rate)
{
    return gnuradio::get_initial_sptr(new wfmrx(quad_rate, audio_rate));
}

wfmrx::wfmrx(float quad_rate, float audio_rate)
    : receiver_base_cf("WFMRX"),
      d_running(false),
      d_quad_rate(quad_rate),
      d_audio_rate(audio_rate)
{
    iq_resamp = make_resampler_cc(PREF_QUAD_RATE/d_quad_rate);

    filter = make_rx_filter(PREF_QUAD_RATE, -80000.0, 80000.0, 20000.0);
    sql = gr_make_simple_squelch_cc(-150.0, 0.001);
    meter = make_rx_meter_c(DETECTOR_TYPE_RMS);
    demod_fm = make_rx_demod_fm(PREF_QUAD_RATE, PREF_AUDIO_RATE, 75000.0, 50.0e-6);

    std::cout << __FUNCTION__ << ": FIXME: audio filter" << std::endl;
    audio_rr = make_resampler_ff(d_audio_rate/PREF_QUAD_RATE); // cause we have no audio filter...

    connect(self(), 0, iq_resamp, 0);
    connect(iq_resamp, 0, filter, 0);
    connect(filter, 0, meter, 0);
    connect(filter, 0, sql, 0);
    connect(sql, 0, demod_fm, 0);
    connect(demod_fm, 0, audio_rr, 0);
    connect(audio_rr, 0, self(), 0);
}

wfmrx::~wfmrx()
{

}

bool wfmrx::start()
{
    d_running = true;

    return true;
}

bool wfmrx::stop()
{
    d_running = false;

    return true;
}

void wfmrx::set_quad_rate(float quad_rate)
{
    if (abs(d_quad_rate-quad_rate) > 0.5)
    {
#ifndef QT_NO_DEBUG_OUTPUT
        std::cout << "Changing NB_RX quad rate: "  << d_quad_rate << " -> " << quad_rate << std::endl;
#endif
        d_quad_rate = quad_rate;
        lock();
        iq_resamp->set_rate(PREF_QUAD_RATE/d_quad_rate);
        unlock();
    }
}

void wfmrx::set_audio_rate(float audio_rate)
{
    (void) audio_rate;
}

void wfmrx::set_filter(double low, double high, double tw)
{
    filter->set_param(low, high, tw);
}

float wfmrx::get_signal_level(bool dbfs)
{
    if (dbfs)
        return meter->get_level_db();
    else
        return meter->get_level();

}

/*
void nbrx::set_nb_on(int nbid, bool on)
{
    if (nbid == 1)
        nb->set_nb1_on(on);
    else if (nbid == 2)
        nb->set_nb2_on(on);
}

void nbrx::set_nb_threshold(int nbid, float threshold)
{
    if (nbid == 1)
        nb->set_threshold1(threshold);
    else if (nbid == 2)
        nb->set_threshold2(threshold);
}
*/

void wfmrx::set_sql_level(double level_db)
{
    sql->set_threshold(level_db);
}

void wfmrx::set_sql_alpha(double alpha)
{
    sql->set_alpha(alpha);
}

/*
void nbrx::set_agc_on(bool agc_on)
{
    agc->set_agc_on(agc_on);
}

void nbrx::set_agc_hang(bool use_hang)
{
    agc->set_use_hang(use_hang);
}

void nbrx::set_agc_threshold(int threshold)
{
    agc->set_threshold(threshold);
}

void nbrx::set_agc_slope(int slope)
{
    agc->set_slope(slope);
}

void nbrx::set_agc_decay(int decay_ms)
{
    agc->set_decay(decay_ms);
}

void nbrx::set_agc_manual_gain(int gain)
{
    agc->set_manual_gain(gain);
}
*/

void wfmrx::set_demod(int rx_demod)
{
    (void) rx_demod;
}

void wfmrx::set_fm_maxdev(float maxdev_hz)
{
    demod_fm->set_max_dev(maxdev_hz);
}

void wfmrx::set_fm_deemph(double tau)
{
    demod_fm->set_tau(tau);
}
