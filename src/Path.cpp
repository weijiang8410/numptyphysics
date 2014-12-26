/*
 * This file is part of NumptyPhysics
 * Copyright (C) 2008 Tim Edmonds
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#include <cstring>
#include <iostream>

#include "Path.h"


static float32 calcDistanceToLine( const Vec2& pt,
				 const Vec2& l1, const Vec2& l2,
				 bool* withinLine=NULL )
{
  b2Vec2 l = l2 - l1; 
  b2Vec2 w = pt - l1;
  float32 mag = l.Normalize();
  float32 dist = b2Cross( w, l );
  if ( withinLine ) {
    float32 dot = b2Dot( l, w );
    *withinLine = ( dot >= 0.0f && dot <= mag );
  }  
  return b2Abs( dist );
}


static float32 calcDistance( const Vec2& l1, const Vec2& l2 ) 
{
  return b2Vec2(l1-l2).Length();
}


float32 Segment::distanceTo( const Vec2& p )
{
  bool withinLine;
  float32 d = calcDistanceToLine( p, m_p1, m_p2, &withinLine );
  if ( !(m_p1 == m_p2) && withinLine ) {
    return d;
  } else {
    return b2Min( calcDistance( p, m_p2 ), calcDistance( p, m_p1 ) );
  }
}


Path::Path()
    : std::vector<Vec2>()
{
}

Path::Path(const Vec2 &p)
    : std::vector<Vec2>()
{
    push_back(p);
}

Path::Path(int n, Vec2* p)
    : std::vector<Vec2>()
{
    for (int i=0; i<n; i++) {
        push_back(*p++);
    }
}

Path::Path( const char *s )
{
  float32 x,y;      
  while ( sscanf( s, "%f,%f", &x, &y )==2) {
    push_back( Vec2((int)x,(int)y) );
    while ( *s && *s!=' ' && *s!='\t' ) s++;
    while ( *s==' ' || *s=='\t' ) s++;
  }
}

Path
Path::fromSVG(const std::string &svgpath)
{
    Path path;

    int start = 1;
    while (start < svgpath.length()) {
        int end = svgpath.find("L", start);
        if (end == std::string::npos) {
            end = svgpath.length();
        }
        std::string part = svgpath.substr(start, end-1);

        int x, y;
        if (sscanf(part.c_str(), "%d %d", &x, &y) == 2) {
            path.push_back(Vec2(x, y));
        }

        start = end + 1;
    }

    return path;
}

void Path::makeRelative() 
{
  for (int i=size()-1; i>=0; i--) 
    at(i)-=at(0); 
}


Path& Path::translate(const Vec2& xlate) 
{
  for (int i=0;i<size();i++)
    at(i) += xlate; 
  return *this;
}

Path& Path::rotate(const b2Mat22& rot) 
{
  float32 j1 = rot.col1.x;
  float32 k1 = rot.col1.y;
  float32 j2 = rot.col2.x;
  float32 k2 = rot.col2.y;
  Vec2 v;

  for (int i=0;i<size();i++) {
    //at(i) = b2Mul( rot, at(i) );
    at(i) = Vec2( j1 * at(i).x + j2 * at(i).y,
		  k1 * at(i).x + k2 * at(i).y );
  }
  return *this;
}

Path& Path::scale(float32 factor)
{
  for (int i=0;i<size();i++) {
    at(i).x = at(i).x * factor;
    at(i).y = at(i).y * factor;
  }
  return *this;
}

void Path::simplify( float32 threshold )
{
  bool keepflags[size()];
  memset( &keepflags[0], 0, sizeof(keepflags) );

  keepflags[0] = keepflags[size()-1] = true;
  simplifySub( 0, size()-1, threshold, &keepflags[0] );

  int k=0;
  for ( int i=0; i<size(); i++ ) {
    if ( keepflags[i] ) {
      at(k++) = at(i);
    }
  }
  //printf("simplify %f %dpts to %dpts\n",threshold,size(),k);
  resize(k);

  // remove duplicate points (shouldn't be any)
  std::vector<Vec2> result;
  Vec2 old = at(0);
  result.push_back(old);
  for (int i=1; i<size(); i++) {
      Vec2 cur = at(i);
      if (old != cur) {
          result.push_back(cur);
      }
      old = cur;
  }
  std::swap(*this, result);
}

void Path::simplifySub( int first, int last, float32 threshold, bool* keepflags )
{
  float32 furthestDist = threshold;
  int furthestIndex = 0;
  if ( last - first > 1 ) {
    Segment s( at(first), at(last) );
    for ( int i=first+1; i<last; i++ ) {
      float32 d = s.distanceTo( at(i) );
      if ( d > furthestDist ) {
	furthestDist = d;
	furthestIndex = i;
      }
    }
    if ( furthestIndex != 0 ) {
      keepflags[furthestIndex] = true;
      simplifySub( first, furthestIndex, threshold, keepflags );
      simplifySub( furthestIndex, last, threshold, keepflags );
    }
  }
}

Rect Path::bbox() const
{
    if (size() == 0) {
        return Rect(Vec2(), Vec2());
    }

    Rect r(at(0), at(0));
    for (auto &p: *this) {
        r.expand(p);
    }
    return r;
}
