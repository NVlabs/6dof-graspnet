/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
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
#include <QtCore>

void slowOperation1()
{
    static char buf[256];
    for (int i = 0; i < (1<<20); ++i)
        buf[i % sizeof buf] = i;
}

void slowOperation2(int) { slowOperation1(); }

void startExample()
{
//![0]
    timer = QElapsedTimer()
    timer.start()

    slowOperation1()

    sys.stderr.write("The slow operation took" + timer.elapsed() + "milliseconds")
//![0]
}

//![1]
def executeSlowOperations(timeout):
    timer = QElapsedTimer()
    timer.start()
    slowOperation1()

    remainingTime = timeout - timer.elapsed()
    if remainingTime > 0:
        slowOperation2(remainingTime)
//![1]

//![2]
def executeOperationsForTime(ms):
    timer = QElapsedTimer()
    timer.start()

    while not timer.hasExpired(ms):
        slowOperation1()
//![2]

int restartExample()
{
//![3]
    timer = QElapsedTimer()

    count = 1
    timer.start()

    while True:
        count *= 2
        slowOperation2(count)
        if timer.restart() < 250:
            break

    return count
//![3]
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    startExample();
    restartExample();
    executeSlowOperations(5);
    executeOperationsForTime(5);
}
