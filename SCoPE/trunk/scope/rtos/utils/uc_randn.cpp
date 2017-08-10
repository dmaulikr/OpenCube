//==========================================================================
//	uc_randn.cpp
//	Author: David Quijano, Juan Castillo, Héctor Posadas
//	Date: mié abr 2 2008
//	Description:
//==========================================================================
//  The following code is derived, directly or indirectly, from SCoPE,
//  released June 30, 2008. Copyright (C) 2006 Design of Systems on Silicon (DS2)
//  The Initial Developer of the Original Code is the University of Cantabria
//  for Design of Systems on Silicon (DS2)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License (GPL) or GNU Lesser General Public License(LGPL) as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License (GPL) or GNU Lesser General Public License(LGPL) for more details.
//
//  You should have received a copy of the GNU General Public License (GPL) or GNU Lesser General Public License(LGPL)
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  For more information about SCOPE you can visit
//  http://www.teisa.unican.es/scope or write an e-mail to
//  scope@teisa.unican.es or a letter to SCoPE, GIM - TEISA, University
//  of Cantabria, AV. Los Castros s/n, ETSIIT, 39005, Santander, Spain
//==========================================================================

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159

#include "rtos/utils/uc_randn.h"

/*!
 * \fn randn
 * \brief Mathematic function. Return random values in a gaussian distribution
 * \param m mean value
 * \param var variance value
 * \param neg indicate if the distribution return values lower than the mean or not
 */
float randn(float m, float var, char neg){
	float x1, x2, y1, y2;
	x1 = (unsigned)rand();
	x1 /= RAND_MAX;
	x2 = (unsigned)rand();
	x2 /= RAND_MAX;

//	y1 = sqrt( - 2 * log(x1) ) * cos( PI * x2 );

	do{
		if(neg)
			y2 = sqrt( - 2 * log(x1) ) * sin( 2 * PI * x2 );
		else
			y2 = sqrt( - 2 * log(x1) ) * sin( PI * x2 );
		x1 = m + y2 * var;
	}while( x1<0 || x1> (m+4*var));

	return( x1 );
}
