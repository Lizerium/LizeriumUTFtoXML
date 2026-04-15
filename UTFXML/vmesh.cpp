/******************************************************************************
 * vmesh.cpp
 *
 *
 *		Freelancer UTF to XML Conversion Utility.
 *
 *		Converts the various mesh nodes.
 *
 ******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UTFXML.h"
#include "UTFXMLDlg.h"


void UTFXMLDlg::WriteVMeshData(BYTE *leaf, DWORD size)
{
	Ptrs p;
	int  i, idx;

    p.b = leaf;
	VMeshData *vmd = p.vmd++;
	OpenNode("VMeshData", "format=\"%u\" surface=\"%u\"", vmd->mesh_type, vmd->surf_type);
	Indent(1);
	fprintf(m_XmlFile, "<mesh count=\"%u\">", vmd->mesh_count);
	Indent(1);
	fprintf(m_XmlFile, "<!-- Mesh Triangle  First   Last Count   Pad  Material -->");
	idx = 0;
	for (i = 0; i < vmd->mesh_count; ++i)
	{
		Indent();
		fprintf(m_XmlFile, "<!-- %3u  %5u --> %5u, %5u, %4u, 0x%02x, %s",
						   i, idx, p.w[2], p.w[3], p.w[4], p.w[5], crc_name(mat_crc, *p.d));
		idx += p.w[4] / 3;
		p.b += 12;
	}
	Indent(-1);
	fprintf(m_XmlFile, "</mesh>");
	Indent();
	fprintf(m_XmlFile, "<index count=\"%u\">", vmd->index_count);
	Indent(1);
	fprintf(m_XmlFile, "<!-- Triangle  Vtx1, Vtx2, Vtx3 -->");
	idx = 0;
	for (i = vmd->index_count; i > 0; i -= 3)
	{
		Indent();
		fprintf(m_XmlFile, "<!-- %5u --> %4u, %4u, %4u", idx++, p.w[0], p.w[1], p.w[2]);
		p.w += 3;
	}
	Indent(-1);
	fprintf(m_XmlFile, "</index>");
	Indent();
	fprintf(m_XmlFile, "<vertex FVF=\"0x%03X\" count=\"%u\">", vmd->FVF, vmd->vertex_count);
	Indent(1);
	fprintf(m_XmlFile, "<!-- Vertex           X                  Y                  Z         ");
	if (vmd->FVF & 0x10)
	{
		fprintf(m_XmlFile, "      X Normal        Y Normal        Z Normal  ");
	}
	if (vmd->FVF & 0x40)
	{
		fprintf(m_XmlFile, "    Diffuse ");
	}
	if (vmd->FVF & 0x100)
	{
		fprintf(m_XmlFile, "        U                 V     ");
	}
	if (vmd->FVF & 0x200)
	{
		fprintf(m_XmlFile, "        U1                V1                  U2                  V2    ");
	}
	fprintf(m_XmlFile, " -->");
	for (i = 0; i < vmd->vertex_count; ++i)
	{
		Indent();
		fprintf(m_XmlFile, "<!-- %5u --> %s", i, ftoa(3, p.f, -5));
		p.f += 3;
		if (vmd->FVF & 0x10)
		{
			fprintf(m_XmlFile, "; %s", ftoa(3, p.f));
			p.f += 3;
		}
		if (vmd->FVF & 0x40)
		{
			fprintf(m_XmlFile, "; 0x%08X", *p.d++);
		}
		if (vmd->FVF & 0x300)
		{
			fprintf(m_XmlFile, "; %s", ftoa(2, p.f, -4));
			p.f += 2;
			if (vmd->FVF & 0x200)
			{
				fprintf(m_XmlFile, "; %s", ftoa(2, p.f, -6));
				p.f += 2;
			}
		}
	}
	Indent(-1);
	fprintf(m_XmlFile, "</vertex>");
	Indent(-1);
	CloseNode();
}


void UTFXMLDlg::WriteVWireData(BYTE *leaf, DWORD size)
{
	Ptrs p;
    p.b = leaf;
	OpenNode("VWireData");
	Indent(1);
	fprintf(m_XmlFile, "<!-- size  --> %u,", p.vwd->size);
	Indent();
	fprintf(m_XmlFile, "<!-- mesh  --> %s,", crc_name(mesh_crc, p.vwd->vmesh_hash));
	Indent();
	fprintf(m_XmlFile, "<!-- base  --> %u,", p.vwd->base);
	Indent();
	fprintf(m_XmlFile, "<!-- used  --> %u,", p.vwd->used);
	Indent();
	fprintf(m_XmlFile, "<!-- count --> %u,", p.vwd->count);
	Indent();
	fprintf(m_XmlFile, "<!-- span  --> %u\n", p.vwd->span);
	Indent();
	fprintf(m_XmlFile, "<!--  Line     Vtx1, Vtx2 -->");
	int idx = 0;
	int i = p.vwd->count / 2;
	p.vwd++;
	while (i--)
	{
		Indent();
		fprintf(m_XmlFile, "<!-- %5u --> %4u, %4u", idx++, p.w[0], p.w[1]);
		p.w += 2;
	}
	Indent(-1);
	CloseNode();
}


void UTFXMLDlg::WriteVMeshRef(BYTE *leaf)
{
    VMeshRef *vmesh_ref = (VMeshRef *) leaf;
	OpenNode("VMeshRef");
	Indent(1);
	fprintf(m_XmlFile, "<!-- header size  --> %d,", vmesh_ref->header_size);
    Indent();
	fprintf(m_XmlFile, "<!-- vmesh lib    --> %s,", crc_name(mesh_crc, vmesh_ref->vmesh_hash));
    Indent();
	fprintf(m_XmlFile, "<!-- start vert   --> %d,", vmesh_ref->start_vert);
    Indent();
	fprintf(m_XmlFile, "<!-- num verts    --> %d,", vmesh_ref->end_vert);
    Indent();
	fprintf(m_XmlFile, "<!-- start index  --> %d,", vmesh_ref->start_vert_ref);
    Indent();
	fprintf(m_XmlFile, "<!-- num indices  --> %d,", vmesh_ref->end_vert_ref);
    Indent();
	fprintf(m_XmlFile, "<!-- start mesh   --> %d,", vmesh_ref->mesh_no);
    Indent();
	fprintf(m_XmlFile, "<!-- num meshes   --> %d,", vmesh_ref->no_of_meshes);
    Indent();
	// Take the trouble to line these up according to their values
	char *box[9];
	int point[9], spc[9], max;
	int i;
	box[0] = ftoa(vmesh_ref->bounding_box[0], 0);
	box[1] = ftoa(vmesh_ref->bounding_box[1], 0);
	box[2] = ftoa(vmesh_ref->center[0], 0);
	box[3] = ftoa(vmesh_ref->bounding_box[2], 0);
	box[4] = ftoa(vmesh_ref->bounding_box[3], 0);
	box[5] = ftoa(vmesh_ref->center[1], 0);
	box[6] = ftoa(vmesh_ref->bounding_box[4], 0);
	box[7] = ftoa(vmesh_ref->bounding_box[5], 0);
	box[8] = ftoa(vmesh_ref->center[2], 0);
	for (i = 0; i < 9; ++i)
	{
		point[i] = strcspn(box[i], ".");
	}
	for (i = 0; i < 9; i += 3)
	{
		max = max(max(point[i], point[i+1]), point[i+2]);
		spc[i+0] = 1 + max - point[i+0];
		spc[i+1] = 1 + max - point[i+1];
		spc[i+2] = 1 + max - point[i+2];
	}
	for (i = 3; i < 9; i += 3)
	{
		int len1 = strlen(box[i-3]+point[i-3]);
		int len2 = strlen(box[i-2]+point[i-2]);
		int len3 = strlen(box[i-1]+point[i-1]);
		max = max(max(len1, len2), len3);
		spc[i+0] += max - len1;
		spc[i+1] += max - len2;
		spc[i+2] += max - len3;
	}
	fprintf(m_XmlFile, "<!-- bounding box -->%*c%s,%*c%s,%*c%s,", spc[0], ' ', box[0], spc[3], ' ', box[3], spc[6], ' ', box[6]);
    Indent();
	fprintf(m_XmlFile, "                     %*c%s,%*c%s,%*c%s,", spc[1], ' ', box[1], spc[4], ' ', box[4], spc[7], ' ', box[7]);
    Indent();
	fprintf(m_XmlFile, "<!-- center       -->%*c%s,%*c%s,%*c%s,", spc[2], ' ', box[2], spc[5], ' ', box[5], spc[8], ' ', box[8]);
    Indent();
	fprintf(m_XmlFile, "<!-- radius       --> %s", ftoa(vmesh_ref->radius, 0));
	Indent(-1);
	CloseNode();
}
