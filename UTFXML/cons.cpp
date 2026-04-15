/******************************************************************************
 * cons.cpp
 *
 *
 *		Freelancer UTF to XML Conversion Utility.
 *
 *		Converts the various Cons nodes.
 *
 ******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UTFXML.h"
#include "UTFXMLDlg.h"


void UTFXMLDlg::WriteFix(BYTE *leaf, DWORD size)
{
	size /= sizeof(FIX);
	OpenNode("Fix", "count=\"%d\"", size);
	m_Depth++;
	for (FIX *fix = (FIX *) leaf; size--; ++fix)
	{
		WriteConsOpen(fix, false);
		WriteConsOrient(&fix->orientation, false);
		// Ugh, back up one to remove the comma
		fseek(m_XmlFile, -1, SEEK_CUR);
		WriteConsClose();
	}
	Indent(-1);
	CloseNode();
}


void UTFXMLDlg::WriteRev(BYTE *leaf, DWORD size)
{
	size /= sizeof(REV);
	OpenNode("Rev", "count=\"%d\"", size);
	m_Depth++;
	for (REV *rev = (REV *) leaf; size--; ++rev)
	{
		WriteConsOpen(rev, true);
		WriteConsOrient(&rev->orientation, true);
		WriteConsMinMax(rev->minmax, 1);
		WriteConsClose();
	}
	Indent(-1);
	CloseNode();
}


void UTFXMLDlg::WriteCyl(BYTE *leaf, DWORD size)
{
	size /= sizeof(CYL);
	OpenNode("Cyl", "count=\"%d\"", size);
	m_Depth++;
	for (CYL *cyl = (CYL *) leaf; size--; ++cyl)
	{
		WriteConsOpen(cyl, true);
		WriteConsOrient(&cyl->orientation, true);
		WriteConsMinMax(cyl->minmax, 2);
		WriteConsClose();
	}
	Indent(-1);
	CloseNode();
}


void UTFXMLDlg::WriteSphere(BYTE *leaf, DWORD size)
{
	size /= sizeof(SPHERE);
	OpenNode("Sphere", "count=\"%d\"", size);
	m_Depth++;
	for (SPHERE *sphere = (SPHERE *) leaf; size--; ++sphere)
	{
		WriteConsOpen(sphere, true);
		WriteConsOrient(&sphere->orientation, false);
		WriteConsMinMax(sphere->minmax, 3);
		WriteConsClose();
	}
	Indent(-1);
	CloseNode();
}


void UTFXMLDlg::WriteConsOpen(const void *cons, bool has_ofs)
{
	REV *rev = (REV *) cons;
	Indent();
	fprintf(m_XmlFile, "<part>");
	Indent(1);
	fprintf(m_XmlFile, "<!-- parent      --> \"%s\",", rev->parent);
	Indent();
	fprintf(m_XmlFile, "<!-- child       --> \"%s\",", rev->child);
	Indent();
	fprintf(m_XmlFile, "<!-- position    --> %s,", vtoal(rev->position, 0));
	if (has_ofs)
	{
	Indent();
	fprintf(m_XmlFile, "<!-- offset      --> %s,", vtoal(rev->offset, 0));
	}
}


void UTFXMLDlg::WriteConsOrient(const void *orient, bool has_axis)
{
	Matrix *ornt = (Matrix *) orient;
	Vector *axis;
	if (has_axis)
	{
		axis = (Vector *) (ornt+1);
	}
	Indent();
	if (m_OptionRotation)
	{
		Vector rot;
		Ornt_to_Rot(rot, *ornt);
		fprintf(m_XmlFile, "<!-- orientation --> %s,", rtoa(rot));
		if (has_axis)
		{
		Indent();
		fprintf(m_XmlFile, "<!-- axis        --> %s,", vtoal(*axis, 0));
		}
	}
	else
	{
		fprintf(m_XmlFile, "<!-- orientation --> %s,", vtoa(ornt->i));
		Indent();
		fprintf(m_XmlFile, "                     %s,", vtoa(ornt->j));
		Indent();
		fprintf(m_XmlFile, "                     %s,", vtoa(ornt->k));
		if (has_axis)
		{
		Indent();
		fprintf(m_XmlFile, "<!-- axis        --> %s,", vtoa(*axis));
		}
	}
}


void UTFXMLDlg::WriteConsMinMax(const float *minmax, int count)
{
	Indent();
	fprintf(m_XmlFile, "<!-- min, max    --> ");
	while (count--)
	{
		if (m_OptionDegrees)
		{
			fprintf(m_XmlFile, "%s, %s", rtoa(minmax[0]), rtoa(minmax[1]));
		}
		else
		{
			fprintf(m_XmlFile, "%s, %s", ftoa(minmax[0], 0), ftoa(minmax[1], 0));
		}
		if (count)
		{
			fprintf(m_XmlFile, ",");
			Indent();
			fprintf(m_XmlFile, "                     ");
			minmax += 2;
		}
	}
}


void UTFXMLDlg::WriteConsClose()
{
	Indent(-1);
	fprintf(m_XmlFile, "</part>");
}
