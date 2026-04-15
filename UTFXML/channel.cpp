/******************************************************************************
 * channel.cpp
 *
 *
 *		Freelancer UTF to XML Conversion Utility.
 *
 *		Converts the two Channel nodes.
 *
 ******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UTFXML.h"
#include "UTFXMLDlg.h"


void UTFXMLDlg::WriteHeader(BYTE *leaf)
{
	m_AnimHeader = (Header *) leaf;

	OpenNode("Header");
	Indent(1);
	fprintf(m_XmlFile, "<!-- count    --> %d,", m_AnimHeader->count);
	Indent();
	fprintf(m_XmlFile, "<!-- interval --> %s,", (m_AnimHeader->interval < 0) ? "Explicit" : ftoa(m_AnimHeader->interval, 0));
	Indent();
	fprintf(m_XmlFile, "<!-- flags    --> 0x%X", m_AnimHeader->flags);
	Indent(-1);
	CloseNode();
}


// This is specific to what actually occurs in the files.
void UTFXMLDlg::WriteFrames(BYTE *leaf, DWORD size)
{	
	DWORD fsize, count;
	fsize = 0;
	if (m_AnimHeader->interval < 0) fsize =   4;
	if (m_AnimHeader->flags & 0x01) fsize +=  4;
	if (m_AnimHeader->flags & 0x02) fsize += 12;
	if (m_AnimHeader->flags & 0x04) fsize += 16;
	if (m_AnimHeader->flags & 0xc0) fsize +=  6;
	// This is what it does, rather than using m_AnimHeader->count.
	count = size / fsize;
	OpenNode("Frames", "count=\"%d\"", count);
	m_Depth++;
	Ptrs p;
	p.b = leaf;
	while (count--)
	{
		Indent();
		if (m_AnimHeader->interval < 0)
		{
			fprintf(m_XmlFile, "%s: ", ftoa(*p.f++, 2));
		}
		if (m_AnimHeader->flags & 0x01)
		{
			LPCSTR val = (!m_is_rev) ? ftoa(*p.f, -4) :
						 (m_OptionDegrees) ? rtoa(*p.f, true, true) : 
											 ftoa(*p.f);
			fprintf(m_XmlFile, "%s", val);
			p.f++;
		}
		if (m_AnimHeader->flags & 0x02)
		{
			/*if (m_AnimHeader->flags & 0x01)
			{
				fprintf(m_XmlFile, "; ");
			}*/
			fprintf(m_XmlFile, "%s", ftoa(3, p.f));
			p.f += 3;
		}
		if (m_AnimHeader->flags & 0x04)
		{
			if (m_AnimHeader->flags & 0x03)
			{
				fprintf(m_XmlFile, "; ");
			}
			fprintf(m_XmlFile, "%s", (m_OptionRotation) ? qtoa(p.f, true) : ftoa(4, p.f));
			p.f += 4;
		}
		if (m_AnimHeader->flags & 0xc0)
		{
			if (m_AnimHeader->flags & 0x07)
			{
				fprintf(m_XmlFile, "; ");
			}
			fprintf(m_XmlFile, "%s, %s, %s", ftoa(p.s[0] / 32767.0f), ftoa(p.s[1] / 32767.0f), ftoa(p.s[2] / 32767.0f));
			//fprintf(m_XmlFile, "% 6d, % 6d, % 6d", p.s[0], p.s[1], p.s[2]);
			p.s += 3;
		}
	}
	Indent(-1);
	CloseNode();
}
