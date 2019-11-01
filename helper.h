/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef HELPER_H
#define HELPER_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include<vector>
#include<QMutex>

using namespace std;

//! [0]
class Helper
{
public:
    Helper();

    double Dr_calculate(float disTance,float lValue,float theta);


public:
    void paint(QPainter *painter, QPaintEvent *event, int elapsed);

    int Window_wid;
    int Window_height;

    int radiusMeter;               //半径显示多少米

    float maxDistance_;

    int showFrameNum;
    int showAngle;
    int showTOFmax;       //显示最大多少米
    int maxDistance;      //显示最大多少米时 对应的Tof值



    float offsetArray[256];   //每个点对应一个offset

//![0]


private:
    QBrush background;
    QBrush circleBrush;
    QFont textFont;
    QPen circlePen;
    QPen textPen;
    QPen axiPen;
};
//! [0]
//!
//!
static double thetaArray[] = {
    -8.726646e-01,
    -8.687848e-01,
    -8.648690e-01,
    -8.609170e-01,
    -8.569282e-01,
    -8.529024e-01,
    -8.488391e-01,
    -8.447380e-01,
    -8.405986e-01,
    -8.364206e-01,
    -8.322036e-01,
    -8.279471e-01,
    -8.236508e-01,
    -8.193143e-01,
    -8.149371e-01,
    -8.105189e-01,
    -8.060593e-01,
    -8.015578e-01,
    -7.970141e-01,
    -7.924278e-01,
    -7.877983e-01,
    -7.831254e-01,
    -7.784086e-01,
    -7.736475e-01,
    -7.688417e-01,
    -7.639908e-01,
    -7.590944e-01,
    -7.541521e-01,
    -7.491635e-01,
    -7.441281e-01,
    -7.390456e-01,
    -7.339156e-01,
    -7.287377e-01,
    -7.235114e-01,
    -7.182365e-01,
    -7.129125e-01,
    -7.075390e-01,
    -7.021156e-01,
    -6.966420e-01,
    -6.911179e-01,
    -6.855428e-01,
    -6.799163e-01,
    -6.742383e-01,
    -6.685082e-01,
    -6.627258e-01,
    -6.568907e-01,
    -6.510026e-01,
    -6.450612e-01,
    -6.390662e-01,
    -6.330173e-01,
    -6.269143e-01,
    -6.207568e-01,
    -6.145446e-01,
    -6.082775e-01,
    -6.019551e-01,
    -5.955774e-01,
    -5.891441e-01,
    -5.826549e-01,
    -5.761098e-01,
    -5.695086e-01,
    -5.628511e-01,
    -5.561371e-01,
    -5.493667e-01,
    -5.425397e-01,
    -5.356560e-01,
    -5.287156e-01,
    -5.217184e-01,
    -5.146645e-01,
    -5.075539e-01,
    -5.003866e-01,
    -4.931627e-01,
    -4.858822e-01,
    -4.785453e-01,
    -4.711522e-01,
    -4.637029e-01,
    -4.561978e-01,
    -4.486369e-01,
    -4.410206e-01,
    -4.333491e-01,
    -4.256228e-01,
    -4.178420e-01,
    -4.100070e-01,
    -4.021183e-01,
    -3.941764e-01,
    -3.861816e-01,
    -3.781345e-01,
    -3.700356e-01,
    -3.618855e-01,
    -3.536848e-01,
    -3.454342e-01,
    -3.371342e-01,
    -3.287857e-01,
    -3.203894e-01,
    -3.119460e-01,
    -3.034564e-01,
    -2.949214e-01,
    -2.863419e-01,
    -2.777188e-01,
    -2.690532e-01,
    -2.603459e-01,
    -2.515981e-01,
    -2.428107e-01,
    -2.339849e-01,
    -2.251219e-01,
    -2.162227e-01,
    -2.072886e-01,
    -1.983207e-01,
    -1.893205e-01,
    -1.802891e-01,
    -1.712278e-01,
    -1.621381e-01,
    -1.530213e-01,
    -1.438787e-01,
    -1.347119e-01,
    -1.255223e-01,
    -1.163112e-01,
    -1.070803e-01,
    -9.783110e-02,
    -8.856504e-02,
    -7.928370e-02,
    -6.998865e-02,
    -6.068147e-02,
    -5.136376e-02,
    -4.203711e-02,
    -3.270314e-02,
    -2.336347e-02,
    -1.401971e-02,
    -4.673509e-03,
    4.673509e-03,
    1.401971e-02,
    2.336347e-02,
    3.270314e-02,
    4.203711e-02,
    5.136376e-02,
    6.068147e-02,
    6.998865e-02,
    7.928370e-02,
    8.856504e-02,
    9.783110e-02,
    1.070803e-01,
    1.163112e-01,
    1.255223e-01,
    1.347119e-01,
    1.438787e-01,
    1.530213e-01,
    1.621381e-01,
    1.712278e-01,
    1.802891e-01,
    1.893205e-01,
    1.983207e-01,
    2.072886e-01,
    2.162227e-01,
    2.251219e-01,
    2.339849e-01,
    2.428107e-01,
    2.515981e-01,
    2.603459e-01,
    2.690532e-01,
    2.777188e-01,
    2.863419e-01,
    2.949214e-01,
    3.034564e-01,
    3.119460e-01,
    3.203894e-01,
    3.287857e-01,
    3.371342e-01,
    3.454342e-01,
    3.536848e-01,
    3.618855e-01,
    3.700356e-01,
    3.781345e-01,
    3.861816e-01,
    3.941764e-01,
    4.021183e-01,
    4.100070e-01,
    4.178420e-01,
    4.256228e-01,
    4.333491e-01,
    4.410206e-01,
    4.486369e-01,
    4.561978e-01,
    4.637029e-01,
    4.711522e-01,
    4.785453e-01,
    4.858822e-01,
    4.931627e-01,
    5.003866e-01,
    5.075539e-01,
    5.146645e-01,
    5.217184e-01,
    5.287156e-01,
    5.356560e-01,
    5.425397e-01,
    5.493667e-01,
    5.561371e-01,
    5.628511e-01,
    5.695086e-01,
    5.761098e-01,
    5.826549e-01,
    5.891441e-01,
    5.955774e-01,
    6.019551e-01,
    6.082775e-01,
    6.145446e-01,
    6.207568e-01,
    6.269143e-01,
    6.330173e-01,
    6.390662e-01,
    6.450612e-01,
    6.510026e-01,
    6.568907e-01,
    6.627258e-01,
    6.685082e-01,
    6.742383e-01,
    6.799163e-01,
    6.855428e-01,
    6.911179e-01,
    6.966420e-01,
    7.021156e-01,
    7.075390e-01,
    7.129125e-01,
    7.182365e-01,
    7.235114e-01,
    7.287377e-01,
    7.339156e-01,
    7.390456e-01,
    7.441281e-01,
    7.491635e-01,
    7.541521e-01,
    7.590944e-01,
    7.639908e-01,
    7.688417e-01,
    7.736475e-01,
    7.784086e-01,
    7.831254e-01,
    7.877983e-01,
    7.924278e-01,
    7.970141e-01,
    8.015578e-01,
    8.060593e-01,
    8.105189e-01,
    8.149371e-01,
    8.193143e-01,
    8.236508e-01,
    8.279471e-01,
    8.322036e-01,
    8.364206e-01,
    8.405986e-01,
    8.447380e-01,
    8.488391e-01,
    8.529024e-01,
    8.569282e-01,
    8.609170e-01,
    8.648690e-01,
    8.687848e-01,
    8.726646e-01,

};
#endif
