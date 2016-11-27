/*
    Copyright (C) 2003 The SilverCoders
    
    This file is part of PopCorn Library.

    Foobar is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "popcorn.hpp"
namespace PopCorn
{
//Constructeur
CLst::CLst()
{
	m_pDebut = NULL;
	m_nNbElem = 0;
}

//Destructeur
CLst::~CLst()
{
	if (m_pDebut != NULL)
		delete[] m_pDebut;
}

//Ajouter un �l�ment � la suite de la liste
int CLst::AddElem(void* val)
{
	LISTE* pT = new LISTE;

	pT->val = val;

	if(m_pDebut == NULL)
	{//Si c'est le premier �l�ment qu'on ins�re
		m_pDebut = pT;
//		m_pDebut->pSuivant = pT;
	}
	else
		m_pFin->pSuivant = pT;

	m_pFin = pT;
	m_pFin->pSuivant = NULL;

	m_nNbElem++;
	
	return m_nNbElem - 1;
}

//Retourne la valeur se trouvant � une position donn�e
void* CLst::operator [] (int nPos)
{
	LISTE* pT = (LISTE*)GetPtrAt(nPos);
	return (pT == NULL) ? NULL : pT->val;
}

//Ins�rer un �l�ment suppl�mentaire � la position nPos
//Tous les autres �l�ments seront d�cal�
//retourne false en cas de position invalide
bool CLst::InsertElem(int nPos,void* val)
{
	LISTE* pV = new LISTE;
	LISTE* pT = NULL;

	pV->val = val;

	if(nPos == 0)
	{
		pV->pSuivant = m_pDebut;
		m_pDebut = pV;
	}
	else
	{
		pT = (LISTE*)GetPtrAt(nPos-1);
		if(pT == NULL)
		{
			delete pV;
			return false;
		}

		pV->pSuivant = pT->pSuivant;
		pT->pSuivant = pV;
	}

	m_nNbElem++;

	return true;
}

//Remplacer la valeur de l'�l�ment se trouvant � la position nPos par val
//retourne false en cas de position invalide
bool CLst::SetAt(int nPos, void* val)
{
	LISTE* pT = (LISTE*)GetPtrAt(nPos);

	if(pT == NULL)
		return false;

	pT->val = val;

	return true;
}

//D�truire l'item se trouvant � la position nPos
//retourne false en cas de position invalide
bool CLst::DeleteElem(int nPos)
{
	LISTE* pD;
	LISTE* pT;

	if(nPos == 0)
	{
		pT = m_pDebut;
		if(pT == NULL)
			return false;

		pD = pT;
		pT = pT->pSuivant;
		m_pDebut = pT;
	}
	else
	{
		pT = (LISTE*)GetPtrAt(nPos -1);

/*
		if(pT == NULL || pT->pSuivant == NULL)
			return false;

		pD = pT;
		pD = pD->pSuivant;
		pT->pSuivant = pD->pSuivant;
*/
		if(pT == NULL) return false;
        pD = pT;
		if(pT->pSuivant == NULL)
		{// Dernier
		    pT->pSuivant = NULL;
		    m_pFin = pT;
		} else {
		    pD = pD->pSuivant;
	    	pT->pSuivant = pD->pSuivant;
	    	if(nPos == m_nNbElem-1)
	    	    m_pFin = pT;
		}
	}

	delete pD;
	m_nNbElem--;

	return true;
}

//Retourne l'adresse de la position nPos
void* CLst::GetPtrAt(int nPos)
{
	LISTE* pT = m_pDebut;
	int i = 0;

	if(nPos < 0 || nPos >= m_nNbElem)
		return NULL;

	while(i < nPos && pT != NULL)
	{
		pT = pT->pSuivant;
		i++;
	}

	return pT;
}

//D�truire la liste
void CLst::Delete()
{
	if (m_pDebut != NULL)
	{
		delete[] m_pDebut;
		m_pDebut = NULL;
		m_nNbElem = 0;
	}
}

int CLst::GetNbElem() { return m_nNbElem; }

int CLst::Find(void* val)
{
	LISTE* pT = this->m_pDebut;
	int i = 0;

	while(pT != NULL)
	{
		if(pT->val == val)
		    return i;
        pT = pT->pSuivant;
		i++;
	}

	return -1;
}
bool CLst::DeleteElem(void* val)
{
   int i = this->Find(val);
   if(i == -1)
       return false;
   else
       return this->DeleteElem(i);
}

};

