/*
*	Exemple de moteur de particules avec gestion de la physique
*
*  dake/calodox
*  http://www.calodox.org
*
*/

#include "stdafx.h"
#include "MoteurParticules.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////

// fonction qui retourne un nombre al�atoire entre 0.0 et 1.0
inline float frnd()
{
	return (float(rand()) / RAND_MAX);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// fonction qui permet de tester l'�tat d'une touche sans passer par des messages Windows
inline bool EtatTouche(int k)
{
	return (GetKeyState(k)&0x8000) != 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// constructeur de la classe MoteurParticules
MoteurParticules::MoteurParticules()
{
	time = old_time = dt = 0.0f;

	// on initialise le timer le plus pr�cis de Windows
	QueryPerformanceCounter(&TimeStart);
	QueryPerformanceFrequency(&TimerFreq);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// cette fonction place les particules de mani�re al�atoire � la surface d'une sph�re. 
// plusieurs m�thodes sont envisageables, nous allons cr�er des vecteurs al�atoires dans toutes les
// directions  et les normaliser, ensuite les multiplier par le rayon. Une vitesse initiale est 
// donn�e � chaque particule qui s'�loigne ainsi du centre.

void MoteurParticules::Genere_Explosion_Particules(V3D centre, float rayon, int nb_particules)
{
	Particule particule;

	for (int i = 0; i < nb_particules; i++) {
		// vecteur al�atoire dans n'importe quelle direction, cette distribution se fait dans un cube
		V3D direction(-0.5 + frnd(), -0.5 + frnd(), -0.5 + frnd());

		direction.Unitaire();
		particule.position = centre + direction * rayon;

		// une vitesse variable donne une meilleure impression
		particule.vitesse = direction * frnd()*2.0f;

		// pour plus de r�alisme, on attribue une masse variable � chaque particule, 
		// une masse de 0 est � proscrire (division par z�ro lors du calcul de l'acc�l�ration)

		particule.masse = 0.5 + frnd()*0.5;	

		// ainsi qu'une couleur variant entre l'orange et le blanc (x,y,z correspond � r,g,b)
		particule.couleur.x = 0.7 + frnd()*0.3;
		particule.couleur.y = 0.6 + frnd()*0.1;
		particule.couleur.z = frnd()*0.4;

		pList.push_back(particule);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// cette fonction ajoute une force de type gravitationnelle � la r�sultante d'une particule

void MoteurParticules::Ajoute_Force_Trou_Noir(Particule* p, V3D position, float masse_trou_noir)
{
	V3D gravitation_force = position - p->position;

	float r2 = gravitation_force.CarreLongueur();
	float r = sqrtf(r2);

	// la constante 1.0f permet d'�viter les divisions par des nombres proches de 0
	float intensite = (p->masse * masse_trou_noir) / (1.0f + r2);

	// on rend le vecteur unitaire sans passer par la fonction de la classe V3D, cela permet
	// d'�viter des redondances dans le calcul.
	gravitation_force.x /= r;
	gravitation_force.y /= r;
	gravitation_force.z /= r;

	gravitation_force *= intensite;

	// on ajoute la force � la r�sultante
	p->force += gravitation_force;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// Dans cette fonction, on ajoute diverses forces pour chaque particule

void MoteurParticules::Calcul_Forces()
{
	// la masse de ce trou noir devient parfois n�gative et varie avec le temps, il repousse
	// les particules durant une courte p�riode.
	float masse_trou_noir = (-0.1 + sin(time * 0.5))*40;

	// pour avoir une explosion puissante, la "masse", si elle est n�gative, est fix�e
	// � un constante assez grande
	if (masse_trou_noir < 0.0)
		masse_trou_noir = -20;

	for (int i = 0; i < pList.size(); i++) {
		// remise � 0 de la r�sultante 
		pList[i].force.Zero();

		// exemple de force constante : la gravit� dirig�e vers le bas
		V3D grav(0.0,-1.0,0);
		pList[i].force += grav;

		// exemple de force variable : une force de frottement oppos�e � la vitesse sous la forme de : 
		// ff = -k*vitesse, le coefficient 'k' d�termine la viscosit� du fluide. 

		V3D ff = pList[i].vitesse * -0.9f;
		pList[i].force += ff;

		// on ajoute 2 trous noirs de m�me masse en orbite autour de l'origine
		V3D t1(4*sin(time * 0.5),4*cos(time * 0.5),0);
		V3D t2(2.5*cos(time * 0.05),2.5*sin(time * 0.05),0);

		Ajoute_Force_Trou_Noir(&pList[i],t1, masse_trou_noir);
		Ajoute_Force_Trou_Noir(&pList[i],t2, masse_trou_noir);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// Cette fonction se charge de mettre � jour la position de chaque particule 

void MoteurParticules::Mise_A_Jour()
{
	// on met � jour le timer et on prend la diff�rence de temps
	QueryPerformanceCounter(&TimeCur);

	old_time = time;
	time = double(TimeCur.QuadPart - TimeStart.QuadPart) / double(TimerFreq.QuadPart);
	dt = time - old_time;

	// on traite chaque particule, si une particule est trop �loign�e de son centre, on la remet
	// en (0,0,0). Ensuite la nouvelle position est recherch�e via Euler.

	for (int n = 0; n < pList.size(); n++) {
		if (pList[n].position.CarreLongueur() > 70) {
			pList[n].position.Zero();
			pList[n].position.x = sin(time)*2;
			pList[n].position.y = sin(time * 0.5)*2;
			pList[n].position.z = cos(time * 0.25)*2;
		}

		pList[n].Euler(dt);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// cette m�thode g�re la position de la cam�ra. Un d�placement de la souris de gauche � droite
// provoque une rotation dans le plan XZ. Un d�placement de la souris de haut en bas fait tourner
// la cam�ra dans le plan YZ. Pour zoomer, il faut appuyer sur la touche fl�ch�e "haut" et pour
// reculer, la touche "bas". On peut s'affranchir des messages Windows de la souris gr�ce GetCursorPos

float distance_camera = 12.0f;

void MoteurParticules::Calcul_Camera()
{
	if (EtatTouche(VK_UP))
		distance_camera -= 0.5f;
	if (EtatTouche(VK_DOWN))
		distance_camera += 0.5f;

	// on �vite d'aller trop pr�s ou trop loin
	if (distance_camera < 1.0)
		distance_camera = 1.0f;
	if (distance_camera > 20.0)
		distance_camera = 20.0f;

	// on r�cup�re la position de la souris � l'�cran
	POINT souris;
	GetCursorPos(&souris);

	// on diminue la sensibilit� de la souris
	float angle_y = souris.y * 0.01;
	float angle_x = souris.x * 0.01;

	// on effectue des rotations, la cam�ra pointe vers 0,0,0
	float plan_xy_rayon = cos(angle_y);

	V3D camera(sin(angle_x)*plan_xy_rayon - cos(angle_x)*plan_xy_rayon,
	sin(angle_y),
	sin(angle_x)*plan_xy_rayon + cos(angle_x)*plan_xy_rayon);

	// on place le vecteur � la bonne distance
	camera *= distance_camera;

	// et on informe openGL de cette nouvelle position
	gluLookAt(camera.x, camera.y, camera.z, 0,0,0,0,cos(angle_y),0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// proc�dure d'affichage

void MoteurParticules::Afficher()
{
	Calcul_Forces();
	Mise_A_Jour();

	// on met en place une transparence additive satur�e			
	glEnable(GL_BLEND);									
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);				

	// on efface l'�cran
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	


	// la matrice ModelView = matrice identit�
	glLoadIdentity();	


	Calcul_Camera();

	// technique du billboarding pour obtenir des vecteurs parall�les au plan form� par la cam�ra
	GLfloat mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*) &mat);

	V3D v_gauche(-mat[0], -mat[4], -mat[8]);
	V3D v_droite(mat[0], mat[4], mat[8]);
	V3D v_haut(mat[1], mat[5], mat[9]);
	V3D v_bas(-mat[1], -mat[5], -mat[9]);


	// d�but de l'affichage, nous allons afficher chaque particule dans un carr�
	glBegin(GL_QUADS);

	for (int i = 0; i < pList.size(); i++) {
		float x = pList[i].position.x;
		float y = pList[i].position.y;
		float z = pList[i].position.z;

		// la taille des particules varie selon leur vitesse et leur profondeur, une limite 
		// est fix�e exp�rimentalement pour �viter de trop grosses particules qui ralentissent 
		// rapidement le syst�me. On utilise le carr� de la longueur pour �viter la racine.

		float taille = pList[i].vitesse.CarreLongueur();
		taille *= 0.010f;
		taille += 0.05f;
		if (taille > 0.15f)
			taille = 0.15f;

		// on calcule les coins de notre quadrilat�re
		V3D a,b,c,d;

		a = pList[i].position + ((v_gauche + v_bas)*taille);
		b = pList[i].position + ((v_droite + v_bas)*taille);
		c = pList[i].position + ((v_droite + v_haut)*taille);
		d = pList[i].position + ((v_gauche + v_haut)*taille);

		// on affiche la particule
		glColor4f(pList[i].couleur.x, pList[i].couleur.y, pList[i].couleur.z, 1.0f);
		glTexCoord2f(0,0);
		glVertex3f(a.x, a.y, a.z);
		glTexCoord2f(1,0);
		glVertex3f(b.x, b.y, b.z);
		glTexCoord2f(1,1);
		glVertex3f(c.x, c.y, c.z);
		glTexCoord2f(0,1);
		glVertex3f(d.x, d.y, d.z);
	}

	// fin d'affichage.
	glEnd();
}
