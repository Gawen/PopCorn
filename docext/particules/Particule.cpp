/*
*	Exemple de moteur de particules avec gestion de la physique
*
*  dake/calodox
*  http://www.calodox.org
*
*/

#include "stdafx.h"
#include "V3D.h"
#include "Particule.h"

// int�grations successives en partant de l'acc�l�ration jusqu'� la trajectoire
// la m�thode est celle d'Euler avec une petite modification, on utilise la vitesse au temps t+dt
// au lieu de la vitesse au temps t.  cela am�liore la stabilit� d'un facteur d'environ 10.

void Particule::Euler(double dt)
{
	V3D a, dv, dr;

	a = force / masse;

	// dv/dt = a  ==> dv = a*dt;
	dv = a * dt;

	// mise � jour de la vitesse (on obtient la vitesse au temps t+dt)
	vitesse += dv;

	// dr/dt = v  ==> dr = v*dt;
	dr = vitesse * dt;

	// mise � jour de la position
	position += dr;
}

