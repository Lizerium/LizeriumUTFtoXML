/******************************************************************************
 * ale.cpp
 *
 *
 *		Freelancer UTF to XML Conversion Utility.
 *
 *		Converts ALEffectLib & AlchemyNodeLibrary.
 *
 ******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UTFXML.h"
#include "UTFXMLDlg.h"
#include <float.h>

MapCRCName ale_type;

void Init_Ale_Types()
{
	static const struct
	{
		int    type;
		LPCSTR name;
	}
	TypeNames[] =
	{
		{ 0x001, "bool"  },
		{ 0x002, "int"   },
		{ 0x003, "float" },
		{ 0x103, "name"  },
		{ 0x104, "ipair" },
		{ 0x105, "xform" },
		{ 0x200, "fanim" }, // float "animation"
		{ 0x201, "canim" }, // color "animation"
		{ 0x202, "vanim" }, // vector? "animation"
	};

	for (int i = sizeof(TypeNames)/sizeof(*TypeNames); --i >= 0;)
		ale_type[TypeNames[i].type] = TypeNames[i].name;
}


void UTFXMLDlg::WriteALEffectLib(BYTE *leaf)
{
	Ptrs p;
	p.b = leaf;

	float version = *p.f++;
	DWORD i = *p.d++;
	OpenNode("ALEffectLib", "version=\"%g\" count=\"%d\"", version, i);
	m_Depth++;
	while (i--)
	{
		DWORD len = *p.w++;
		Indent();
		fprintf(m_XmlFile, "<effect name=\"%s\">", p.c);
		p.c += len + (len & 1);
		m_Depth++;
		if (version == 1.1f)
		{
			Indent();
			fprintf(m_XmlFile, "<unused>%s; %s</unused>", ftoa(3, p.f, 0), ftoa(p.f[3], 0));
			p.f += 4;
		}
		len = *p.d++;
		Indent();
		fprintf(m_XmlFile, "<fx count=\"%d\">", len);
		m_Depth++;
		while (len--)
		{
			Indent();
			fprintf(m_XmlFile, "%2d, %5d, %u, %s", p.ael->index, p.ael->parent, p.ael->flag, crc_name(node_crc, p.ael->CRC));
			p.ael++;
		}
		Indent(-1);
		fprintf(m_XmlFile, "</fx>");
		len = *p.d++;
		Indent();
		fprintf(m_XmlFile, "<pairs count=\"%d\">", len);
		m_Depth++;
		while (len--)
		{
			Indent();
			fprintf(m_XmlFile, "%2d, %2d", p.i[0], p.i[1]);
			p.i += 2;
		}
		Indent(-1);
		fprintf(m_XmlFile, "</pairs>");
		Indent(-1);
		fprintf(m_XmlFile, "</effect>");
	}
	Indent(-1);
	CloseNode();
}


void UTFXMLDlg::WriteAlchemyNodeLibrary(BYTE *leaf)
{
	Ptrs p;
	p.b = leaf;

	float version = *p.f++;
	DWORD i = *p.d++;
	OpenNode("AlchemyNodeLibrary", "version=\"%g\" count=\"%d\"", version, i);
	m_Depth++;
	while (i--)
	{
		DWORD len = *p.w++;
		Indent();
		fprintf(m_XmlFile, "<node name=\"%s\">", p.c);
		p.c += len + (len & 1);
		m_Depth++;
		DWORD id, crc;
		CIterCRCName iter;
		while (true)
		{
			id = *p.w++;
			if (!id)
			{
				break;
			}
			Indent();
			iter = ale_type.find(id & 0x7FFF);
			if (iter == ale_type.end())
			{
				fprintf(m_XmlFile, "<effect type=\"0x%03X\" ", id & 0x7FFF);
			}
			else
			{
				fprintf(m_XmlFile, "<effect type=\"%s\" ", iter->second);
				// Keep the names aligned
				for (int i = strlen(iter->second); i < 5; ++i)
					putc(' ', m_XmlFile);
			}
			crc = *p.d++;
			iter = fx_crc.find(crc);
			if (iter == fx_crc.end())
			{
				fprintf(m_XmlFile, "crc=\"0x%08X\">", crc);
			}
			else
			{
				fprintf(m_XmlFile, "name=\"%s\">", iter->second);
			}
			switch (id & 0x7FFF)
			{
			case 0x001 : 
				fprintf(m_XmlFile, "%s", (id & 0x8000) ? "true" : "false");
				break;

			case 0x002 :
				fprintf(m_XmlFile, "%d", *p.i++);
				break;

			case 0x003 :
				fprintf(m_XmlFile, "%s", (*p.f >= FLT_MAX) ? "Infinite" : ftoa(*p.f, 0));
				p.f++;
				break;

			case 0x103 :
				if (*p.w)
				{
					fprintf(m_XmlFile, "%s", p.c+2);
					p.c += *p.w + (*p.w & 1);
				}
				p.w++;
				break;

			case 0x104 :
				fprintf(m_XmlFile, "%d, %d", p.i[0], p.i[1]);
				p.i += 2;
				break;

			case 0x105 :
			{
				DWORD xform = *p.b++ << 8;
				xform |= *p.b++ << 4;
				xform |= *p.b++;
				fprintf(m_XmlFile, "0x%03X", xform);
				if (*p.b++)
				{
					for (int j = 0; j < 9; ++j)
					{
						p.b = do_Single(p.b);
					}
					Indent();
				}
				break;
			}

			case 0x200 :
			{
				BYTE type  = *p.b++;
				BYTE count = *p.b++;
				Indent(1);
				fprintf(m_XmlFile, "<float_header type=\"%u\" count=\"%u\">", type, count);
				m_Depth++;
				int left = (count > 1) ? 2 : 0;
				while (count--)
				{
					Indent();
					fprintf(m_XmlFile, "%s:", ftoa(*p.f++, left));
					type = *p.b++;
					DWORD fcount = *p.b++;
					Indent(1);
					fprintf(m_XmlFile, "<float type=\"%u\" count=\"%u\">", type, fcount);
					m_Depth++;
					int fleft = (fcount > 1) ? 2 : 0;
					while (fcount--)
					{
						Indent();
						fprintf(m_XmlFile, "%s: %s", ftoa(p.f[0], fleft), ftoa(p.f[1], 0));
						p.f += 2;
					}
					Indent(-1);
					fprintf(m_XmlFile, "</float>");
					m_Depth--;
				}
				Indent(-1);
				fprintf(m_XmlFile, "</float_header>");
				Indent(-1);
				break;
			}

			case 0x201 :
			{
				BYTE type  = *p.b++;
				BYTE count = *p.b++;
				Indent(1);
				fprintf(m_XmlFile, "<rgb_header type=\"%u\" count=\"%u\">", type, count);
				m_Depth++;
				int left = (count > 1) ? 2 : 0;
				while (count--)
				{
					Indent();
					fprintf(m_XmlFile, "%s:", ftoa(*p.f++, left));
					type = *p.b++;
					DWORD rcount = *p.b++;
					Indent(1);
					fprintf(m_XmlFile, "<rgb type=\"%u\" count=\"%u\">", type, rcount);
					m_Depth++;
					int rleft = (rcount > 1) ? 2 : 0;
					while (rcount--)
					{
						Indent();
						fprintf(m_XmlFile, "%s: %s", ftoa(p.f[0], rleft), ftoc(p.f+1, m_OptionRGB));
						p.f += 4;
					}
					Indent(-1);
					fprintf(m_XmlFile, "</rgb>");
					m_Depth--;
				}
				Indent(-1);
				fprintf(m_XmlFile, "</rgb_header>");
				Indent(-1);
				break;
			}

			case 0x202 :
				p.b = do_Single(p.b);
				Indent();
				break;

			/*
			case 0x100 :
			{
				BYTE type, count;
				while (count--)
				{
					float f; // ?
					DWORD d; // ?
				}
				break;
			}

			case 0x101 :
				do_Single();
				break;

			case 0x102 :
			{
			    BYTE type, count;
				while (count--)
				{
					float f1, f2[3]; // ?
				}
				break;
			}
			*/

			}
			fprintf(m_XmlFile, "</effect>");
		}
		Indent(-1);
		fprintf(m_XmlFile, "</node>");
	}
	Indent(-1);
	CloseNode();
}


// Why "single" and "loop"?  There's a warning:
//		"Failed to set non-zero loop type for animated single on load."
BYTE *UTFXMLDlg::do_Single(BYTE *leaf)
{						
	Ptrs p;
	p.b = leaf;

	BYTE type  = *p.b++;
	BYTE count = *p.b++;
	Indent(1);
	fprintf(m_XmlFile, "<single type=\"%u\" count=\"%u\">", type, count);
	m_Depth++;
	int left = (count > 1) ? 2 : 0;
	while (count--)
	{
		Indent();
		fprintf(m_XmlFile, "%s: %s", ftoa(p.f[0], left), ftoa(p.f[1], 0));
		p.f += 2;
		DWORD loop = *p.w++;
		DWORD lcnt = *p.w++;
		// loop should always be zero, but just in case
		if (loop || lcnt)
		{
			Indent(1);
			fprintf(m_XmlFile, "<loop ");
			if (loop)
			{
				fprintf(m_XmlFile, "type=\"%u\" ", loop);
			}
			fprintf(m_XmlFile, "count=\"%u\">", lcnt);
			m_Depth++;
			int lleft = (lcnt > 1) ? -2 : 0;
			while (lcnt--)
			{
				Indent();
				fprintf(m_XmlFile, "%s: %s", ftoa(p.f[0], lleft), ftoa(3, p.f+1, 0));
				p.f += 4;
			}
			Indent(-1);
			fprintf(m_XmlFile, "</loop>");
			m_Depth--;
		}
	}
	Indent(-1);
	fprintf(m_XmlFile, "</single>");
	m_Depth--;

	return p.b;
}


void UTFXMLDlg::ExtractAlchemyNodes(BYTE *leaf)
{
	Ptrs p;
	p.b = leaf;

	p.f++;
	DWORD i = *p.d++;
	while (i--)
	{
		DWORD len = *p.w++;
		p.c += len + (len & 1);
		DWORD id, crc;
		BYTE  count, icount;
		while (true)
		{
			id = *p.w++;
			if (!id)
			{
				break;
			}
			crc = *p.d++;
			switch (id & 0x7FFF)
			{
			case 0x001 : 
				break;

			case 0x002 :
				p.i++;
				break;

			case 0x003 :
				p.f++;
				break;

			case 0x103 :
				if (crc == 0xEFA8CE01 && *p.w)	// Node_Name
				{
					node_crc[fl_crc32_b(p.b+2, *p.w-1)] = strdup(p.c+2);
				}
				p.c += 2 + *p.w + (*p.w & 1);
				break;

			case 0x104 :
				p.i += 2;
				break;

			case 0x105 :
				p.b += 3;
				if (*p.b++)
				{
					for (int j = 0; j < 9; ++j)
					{
						p.b = Skip_Single(p.b);
					}
				}
				break;

			case 0x200 :
				p.b++;
				count = *p.b++;
				while (count--)
				{
					p.f++;
					p.b++;
					icount = *p.b++;
					while (icount--)
					{
						p.f += 2;
					}
				}
				break;

			case 0x201 :
				p.b++;
				count = *p.b++;
				while (count--)
				{
					p.f++;
					p.b++;
					icount = *p.b++;
					while (icount--)
					{
						p.f += 4;
					}
				}
				break;

			case 0x202 :
				p.b = Skip_Single(p.b);
				break;
			}
		}
	}
}


BYTE *UTFXMLDlg::Skip_Single(BYTE *leaf)
{						
	Ptrs p;
	p.b = leaf;

	p.b++;
	BYTE count = *p.b++;
	while (count--)
	{
		p.f += 2;
		p.w++;
		DWORD lcnt = *p.w++;
		while (lcnt--)
		{
			p.f += 4;
		}
	}

	return p.b;
}
