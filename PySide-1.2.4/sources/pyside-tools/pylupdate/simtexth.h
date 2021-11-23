/*
 * This file is part of the PySide Tools project.
 *
 * Copyright (C) 1992-2006 Trolltech AS. All rights reserved.
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: PySide team <pyside@openbossa.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef SIMTEXTH_H
#define SIMTEXTH_H

const int textSimilarityThreshold = 190;

#include <QString>
#include <QList>

class MetaTranslator;

struct Candidate {
    QString source;
    QString target;

    Candidate() { }
    Candidate( const QString& source0, const QString& target0 )
    : source( source0 ), target( target0 ) { }
};

inline bool operator==( const Candidate& c, const Candidate& d ) {
    return c.target == d.target && c.source == d.source;
}
inline bool operator!=( const Candidate& c, const Candidate& d ) {
    return !operator==( c, d );
}

typedef QList<Candidate> CandidateList;

struct CoMatrix;
/**
 * This class is more efficient for searching through a large array of candidate strings, since we only
 * have to construct the CoMatrix for the \a stringToMatch once, 
 * after that we just call getSimilarityScore(strCandidate).
 * \sa getSimilarityScore
 */
class StringSimilarityMatcher {
public:
    StringSimilarityMatcher(const QString &stringToMatch);
    ~StringSimilarityMatcher();
    int getSimilarityScore(const QString &strCandidate);

private:
    CoMatrix *m_cm;
    int m_length;
};

int getSimilarityScore(const QString &str1, const char* str2);

CandidateList similarTextHeuristicCandidates( const MetaTranslator *tor,
                          const char *text,
                          int maxCandidates );

#endif
