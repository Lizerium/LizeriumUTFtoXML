/******************************************************************************
 * UTFXMLDlg.cpp
 *
 *
 *		Freelancer UTF to XML Conversion Utility
 *
 *		Written by Sir Lancelot (a.k.a. Sirlancelot, Elviis, Yud, vEct0r)
 *		email: elvviis@hotmail.com (or elvviis@yahoo.com)
 *		Placed in the Public Domain on 03-Nov-2004.
 *
 *		Updated by Jason Hood (a.k.a adoxa)
 *		email: jadoxa@yahoo.com.au
 *		http://freelancer.adoxa.cjb.net/
 *
 *
 * ----------------------------------------------------------------------------
 * PURPOSE:
 *
 *		This utility converts the Microsoft Freelancer UTF (Universal Tree
 *		Format) data files into XML format to allow you to examine (and
 *		modify) the contents of the UTF file.  There is a good UTF editor
 *		available (on Lancers Reactor) which allows direct editing of
 *		individual UTF files, but it does not support batch operations.
 *
 *		This utility allows you to convert a single UTF file or numerous UTF
 *		files in nested folders in a single pass.
 *
 *		The XMLUTF companion utility can be used to convert edited XML files
 *		back to UTF format.
 *
 *
 * ----------------------------------------------------------------------------
 * NOTES:
 *
 *		This is the fifth release of the UTF to XML Conversion Utility.
 *		This application was built using Microsoft Visual C++ 6.0.
 *
 *		If you make changes to this program, please send a copy to the
 *		original author at the email address listed above.	If it enhances
 *		the utility, I may include the changes in a future release.
 *
 *		Valid UTF file extensions are: 3db, ale, anm, cmp, dfm,
 *									   mat, sph, txm, utf, vms.
 *
 *
 * ----------------------------------------------------------------------------
 * TODO:
 *
 *		Enable the HELP button and provide some help!
 *		Warn if type="text" data contains illegal characters '>' or '<'
 *		Write Milkshape 3D Import/Export plug-in for FL XML format.
 *
 *
 * ----------------------------------------------------------------------------
 * CREDITS:
 *
 *		For researching/publishing of UTF and VMeshRef structs, fl_crc32
 *		  Mario "HCl" Brito (mbrito@student.dei.uc.pt)
 *		  Matthew "dizzy" Ruggiero
 *		For FLModelTool, FLAddRadius, CMP Exporter, SUR file structs
 *		  Colin Sandby, Harrier, Anton (hitchhiker54@yahoo.com), Twex,
 *		  Phantom Fox, Free Spirit, Dr Del, CCCP, shsan, Skyshooter, Brutus
 *		For improvement suggestions and bug reports:
 *		  Eirik "esflightsim" Sletteberg (bug report)
 *
 *		Sorry I don't exactly know who did what, but they all did something
 *		which paved the way for this utility.  If I left anyone out, please
 *		email me at the above email address and I will update the list.
 *
 *
 * ----------------------------------------------------------------------------
 * REVISION HISTORY:
 *
 *		01-Nov-2004  V1.0a Sir Lancelot
 *							Original design and implementation
 *
 *		04-Nov-2004  V1.0b Sir Lancelot
 *							Numerous changes to support the XML to UTF utility.
 *							Added UTFXML version number comment to XML file.
 *							Wait up to 5 seconds for the conversion to abort.
 *							Assume game is located in default install location
 *							   if AppPath is not found in the registry.
 *							Added "unk" attributes for unknown node data.
 *
 *		06-Nov-2004  V1.1  Sir Lancelot
 *							Fixed bug reported by Eirik Sletteberg.
 *							   Remove commas following unk attributes.
 *							Fixed bug causing heads and hands to disappear.
 *							   Remove code that replaces negative floating point
 *							   numbers in "Orientation" with zero.
 *							Added <UTF_ROOT>...</UTF_ROOT> to retain unk values.
 *							Added a number of attributes to UTFXML tag to make
 *							   the output file created by XMLUTF match the
 *							   characteristics of the original UTF file and
 *							   for verifying that the result was the same size.
 *							   These attributes include:
 *								   * dupstrings
 *								   * prepaddata
 *								   * string_padding
 *								   * nodes
 *								   * ssize
 *								   * salloc
 *								   * doffset
 *								   * original_size
 *								   * padding="4"
 *							   After testing thoroughly, it appears that the
 *							   resulting UTF file works fine with Freelancer
 *							   without these attributes, so they are commented
 *							   out.
 *							When all three unks are the same, create only one
 *							   attrib (unk234) to represent all three.
 *							Only add "unk" values if non-zero.
 *
 *		16-Jan-2010  V2.0  Jason Hood
 *							Don't exit automatically.
 *							Fixed retrieving destination path from the registry.
 *							Create the destination path (final directory only).
 *							Ignore case more often.
 *							Added ability to select a specific UTF extension.
 *							Unk4 & 5 in the header form a single FILETIME.
 *							Unk234 are DOS-style file date & time; only include
 *							   them if they differ from their parent and the
 *							   timestamp option is in effect.
 *							Convert almost all node types.
 *							Files are extracted to a subdirectory, removing the
 *							   need to suffix the name with CRC.
 *							Removed VMeshData & VWireData extraction options,
 *							   combining them into an "Include Files" option.
 *							Scan the audio ini files for msg nicknames, using
 *							   those instead of the hash; scan particular UTF
 *							   files for material names, using those instead of
 *							   the CRC.
 *							Display improvements.
 *
 *		16-Mar-2010  V2.1  Jason Hood
 *							Removed compatibility code.
 *							Relaxed int and float detection (allow size of 8).
 *							Added a log (UTFXML.log in the destination).
 *							Removed timestamp from file names.
 *							Checking RGB does integer; checking again does hex.
 *
 *		09-Aug-2010  V2.2  Jason Hood
 *							Continue if Freelancer not apparently installed.
 *							Use a name for Alchemy type values.
 *							Renamed the VMeshRef comments.
 *							Do not use degrees for Frames values of Pris parts.
 *							Convert quaternion to angle/axis if using Rotation.
 *							Command line options.
 *							Add the summary to the log.
 *							Create the entire destination path.
 *							Write the log to the temporary directory.
 *							Remove "DATA\" from the path attribute.
 *
 ******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UTFXML.h"
#include "UTFXMLDlg.h"
#include "ProgressDialog.h"
#include <io.h>


//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

#define UTFXML_VERSION	"XML generated by UTFXML Version 2.2 built 19-Aug-2010"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NODE(ofs) (UTFNode *) (m_Tree + ofs)
#define NAME(ofs) (m_String + ofs)
#define DATA(ofs) (m_Data + ofs)


static char fname[MAX_PATH];


// This is a list of the node types with special handling
enum
{
	NODE_TYPE_UNDETERMINED,
	NODE_TYPE_TEXT,
	NODE_TYPE_FILE,
	NODE_TYPE_DWORD,
	NODE_TYPE_DWORD_ARRAY,
	NODE_TYPE_BYTE,
	NODE_TYPE_WORD_ARRAY,
	NODE_TYPE_FLOAT,
	NODE_TYPE_FLOAT_ARRAY,
	NODE_TYPE_VECTOR,
	NODE_TYPE_RGB,
	NODE_TYPE_MATRIX,
	NODE_TYPE_TRANSFORM,
	NODE_TYPE_VMESHDATA,
	NODE_TYPE_VWIREDATA,
	NODE_TYPE_VMESHREF,
	NODE_TYPE_HEADER,
	NODE_TYPE_FRAMES,
	NODE_TYPE_FIX,	// and Trans, Loose
	NODE_TYPE_PRIS,
	NODE_TYPE_REV,
	NODE_TYPE_CYL,
	NODE_TYPE_SPHERE,
	NODE_TYPE_FRAME_RECTS,
	NODE_TYPE_ALEFFECTLIB,
	NODE_TYPE_ALCHEMYNODELIBRARY
};


struct SNodeTypeFormat
{
	int  wrap;		// array items per line
	int  left;		// digits to the left of the decimal
	bool hex;		// display as hex
	bool deg;		// possibly convert radians to degrees
}
NodeTypeFormat[] =
{
	{  0, -2, false, false },
	{  0,  1, false, false },
	{  0,  1, false, true  },
	{  0,  1, true,  false },
	{ 10,  4, false, false },
	{  1,  0, false, false },
	{  2, -2, false, false },
	{  3, -2, false, false },
	{  4, -3, false, false },
	{  5, -2, false, false },
	{ 10, -2, false, false },
};

enum
{
	NODE_FORMAT_DEFAULT,
	NODE_FORMAT_NORMAL,
	NODE_FORMAT_DEG,
	NODE_FORMAT_HEX,
	NODE_FORMAT_INT10,
	NODE_FORMAT_FLT1,
	NODE_FORMAT_FLT2,
	NODE_FORMAT_FLT3,
	NODE_FORMAT_FLT4,
	NODE_FORMAT_FLT5,
	NODE_FORMAT_FLT10,
};


struct SNodeType
{
	LPCSTR name;
	int    type;
	int    format;
}
NodeType[] =
{
	{ "alchemynodelibrary",   NODE_TYPE_ALCHEMYNODELIBRARY, NODE_FORMAT_DEFAULT },
	{ "aleffectlib",          NODE_TYPE_ALEFFECTLIB, NODE_FORMAT_DEFAULT },

	{ "count",                NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "edge count",           NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "face count",           NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "flip u",               NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "flip v",               NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "frame count",          NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "image x size",         NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "image y size",         NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "index",                NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "macount",              NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "material count",       NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "material identifier",  NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "material",             NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "normal count",         NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "object vertex count",  NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "sides",                NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "surface normal count", NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "texture count",        NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "texture vertex count", NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "uv_bone_id",           NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "uv_vertex_count",      NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "vertex batch count",   NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },
	{ "vertex count",         NODE_TYPE_DWORD,       NODE_FORMAT_DEFAULT },

	{ "bt_flags",             NODE_TYPE_DWORD,       NODE_FORMAT_HEX     },
	{ "dm0_flags",            NODE_TYPE_DWORD,       NODE_FORMAT_HEX     },
	{ "dm1_flags",            NODE_TYPE_DWORD,       NODE_FORMAT_HEX     },
	{ "dm_flags",             NODE_TYPE_DWORD,       NODE_FORMAT_HEX     },
	{ "dt_flags",             NODE_TYPE_DWORD,       NODE_FORMAT_HEX     },
	{ "et_flags",             NODE_TYPE_DWORD,       NODE_FORMAT_HEX     },
	{ "flags",                NODE_TYPE_DWORD,       NODE_FORMAT_HEX     },
	{ "maflags",              NODE_TYPE_DWORD,       NODE_FORMAT_HEX     },

	{ "bone_id_chain",        NODE_TYPE_DWORD_ARRAY, NODE_FORMAT_INT10   },
	{ "point_bone_count",     NODE_TYPE_DWORD_ARRAY, NODE_FORMAT_INT10   },
	{ "point_indices",        NODE_TYPE_DWORD_ARRAY, NODE_FORMAT_INT10   },
	{ "uv0_indices",          NODE_TYPE_DWORD_ARRAY, NODE_FORMAT_INT10   },
	{ "uv1_indices",          NODE_TYPE_DWORD_ARRAY, NODE_FORMAT_INT10   },

	{ "fix",                  NODE_TYPE_FIX,         NODE_FORMAT_DEFAULT },
	{ "loose",                NODE_TYPE_FIX,         NODE_FORMAT_DEFAULT },
	{ "cyl",                  NODE_TYPE_CYL,         NODE_FORMAT_DEFAULT },
	{ "pris",                 NODE_TYPE_PRIS,        NODE_FORMAT_DEFAULT },
	{ "rev",                  NODE_TYPE_REV,         NODE_FORMAT_DEFAULT },
	{ "sphere",               NODE_TYPE_SPHERE,      NODE_FORMAT_DEFAULT },

	{ "alpha",                NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "blend",                NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "bone_x_to_u_scale",    NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "bone_y_to_v_scale",    NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	//{ "constant",           NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "fade",                 NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "fovx",                 NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "fovy",                 NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "fps",                  NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "half x",               NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "half y",               NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "half z",               NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "length",               NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "mass",                 NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "max_du",               NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "max_dv",               NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "min_du",               NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "min_dv",               NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "oc",                   NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "radius",               NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "root height",          NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "scale",                NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "sp",					  NODE_TYPE_FLOAT,		 NODE_FORMAT_DEFAULT },
	{ "tilerate",             NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "tilerate0",            NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "tilerate1",            NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "uv_plane_distance",    NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "zfar",                 NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },
	{ "znear",                NODE_TYPE_FLOAT,       NODE_FORMAT_DEFAULT },

	{ "max",                  NODE_TYPE_FLOAT,       NODE_FORMAT_DEG     },
	{ "min",                  NODE_TYPE_FLOAT,       NODE_FORMAT_DEG     },

	{ "fractions",            NODE_TYPE_FLOAT_ARRAY, NODE_FORMAT_FLT1    },
	{ "switch2",              NODE_TYPE_FLOAT_ARRAY, NODE_FORMAT_FLT1    },

	{ "bone_weight_chain",    NODE_TYPE_FLOAT_ARRAY, NODE_FORMAT_FLT10   },

	{ "uv0",                  NODE_TYPE_FLOAT_ARRAY, NODE_FORMAT_FLT2    },
	{ "uv1",                  NODE_TYPE_FLOAT_ARRAY, NODE_FORMAT_FLT2    },

	{ "points",               NODE_TYPE_FLOAT_ARRAY, NODE_FORMAT_FLT3    },
	{ "vertex_normals",       NODE_TYPE_FLOAT_ARRAY, NODE_FORMAT_FLT3    },

	{ "makeys",               NODE_TYPE_FLOAT_ARRAY, NODE_FORMAT_FLT4    },

	{ "edge_angles",          NODE_TYPE_FLOAT_ARRAY, NODE_FORMAT_FLT5    },
	{ "madeltas",             NODE_TYPE_FLOAT_ARRAY, NODE_FORMAT_FLT5    },

	{ "header",               NODE_TYPE_HEADER,      NODE_FORMAT_DEFAULT },
	{ "frames",               NODE_TYPE_FRAMES,      NODE_FORMAT_DEFAULT },

	{ "frame rects",          NODE_TYPE_FRAME_RECTS, NODE_FORMAT_DEFAULT },

	{ "inertia tensor",       NODE_TYPE_MATRIX,      NODE_FORMAT_FLT3    },
	{ "orientation",          NODE_TYPE_MATRIX,      NODE_FORMAT_FLT3    },

	{ "ac",                   NODE_TYPE_RGB,         NODE_FORMAT_NORMAL  },
	//{ "constant",           NODE_TYPE_RGB,         NODE_FORMAT_NORMAL  },
	{ "dc",                   NODE_TYPE_RGB,         NODE_FORMAT_NORMAL  },
	{ "ec",                   NODE_TYPE_RGB,         NODE_FORMAT_NORMAL  },
	{ "sc",					  NODE_TYPE_RGB,		 NODE_FORMAT_NORMAL  },

	{ "bt_name",              NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "child name",           NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "dm0_name",             NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "dm1_name",             NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "dm_name",              NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "dt_name",              NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "et_name",              NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "exporter version",     NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "file name",            NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "m0",                   NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "m1",                   NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "m2",                   NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "m3",                   NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "m4",                   NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "m5",                   NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "m6",                   NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "material_name",        NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "name",                 NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "object name",          NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "parent name",          NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },
	{ "type",                 NODE_TYPE_TEXT,        NODE_FORMAT_DEFAULT },

	{ "bone to root",         NODE_TYPE_TRANSFORM,   NODE_FORMAT_FLT3    },
	{ "transform",            NODE_TYPE_TRANSFORM,   NODE_FORMAT_FLT3    },

	{ "axis",                 NODE_TYPE_VECTOR,      NODE_FORMAT_NORMAL  },
	{ "center of mass",       NODE_TYPE_VECTOR,      NODE_FORMAT_NORMAL  },
	{ "center",               NODE_TYPE_VECTOR,      NODE_FORMAT_NORMAL  },
	{ "centroid",             NODE_TYPE_VECTOR,      NODE_FORMAT_NORMAL  },
	{ "position",             NODE_TYPE_VECTOR,      NODE_FORMAT_NORMAL  },

	{ "vmeshdata",            NODE_TYPE_VMESHDATA,   NODE_FORMAT_DEFAULT },
	{ "vmeshref",             NODE_TYPE_VMESHREF,    NODE_FORMAT_DEFAULT },
	{ "vwiredata",            NODE_TYPE_VWIREDATA,   NODE_FORMAT_DEFAULT },

	{ "edge_indices",         NODE_TYPE_WORD_ARRAY,  NODE_FORMAT_INT10   },
	{ "tristrip_indices",     NODE_TYPE_WORD_ARRAY,  NODE_FORMAT_INT10   },

	// These nodes aren't used
	{ "lod bits",             NODE_TYPE_BYTE,        NODE_FORMAT_HEX     },
	{ "point_bone_first",     NODE_TYPE_DWORD_ARRAY, NODE_FORMAT_INT10   },
	{ "uv_vertex_id",         NODE_TYPE_DWORD_ARRAY, NODE_FORMAT_INT10   },
	{ "uv_default_list",      NODE_TYPE_FLOAT_ARRAY, NODE_FORMAT_FLT5    },
};


typedef std::map<DWORD, int> MapNameNode;
typedef MapNameNode::const_iterator NameNodeIter;
MapNameNode node_map;


//////////////////////////////////////////////////////////////////////////////////////////////////
// Forward References
//////////////////////////////////////////////////////////////////////

static UINT AFX_CDECL BackgroundConversionThread(void *args);


//////////////////////////////////////////////////////////////////////
// Import functions from common.dll.
//////////////////////////////////////////////////////////////////////

// Generate the hash value.
DWORD CreateID(LPCSTR);

// Use a stripped-down version of Freelancer's own ini-reading code.
class INI_Reader
{
public:
		   INI_Reader();
		  ~INI_Reader();
	void   close();
	LPCSTR get_value_string();
	bool   is_value(LPCSTR);
	bool   open(LPCSTR, bool = false);
	bool   read_header();
	bool   read_value();

private:
	BYTE data[0x1568];
};


// Manually import the functions, to provide path independence.
struct
{
	FARPROC addr;
	LPCSTR	name;
}
imports[] =
{
	NULL, "?CreateID@@YAIPBD@Z",
	NULL, "??0INI_Reader@@QAE@XZ",
	NULL, "??1INI_Reader@@QAE@XZ",
	NULL, "?close@INI_Reader@@QAEXXZ",
	NULL, "?get_value_string@INI_Reader@@QAEPBDXZ",
	NULL, "?is_value@INI_Reader@@QAE_NPBD@Z",
	NULL, "?open@INI_Reader@@QAE_NPBD_N@Z",
	NULL, "?read_header@INI_Reader@@QAE_NXZ",
	NULL, "?read_value@INI_Reader@@QAE_NXZ",
};

enum
{
	impCreateID,
	INI_Reader_ctor,
	INI_Reader_dtor,
	INI_Reader_close,
	INI_Reader_get_value_string,
	INI_Reader_is_value,
	INI_Reader_open,
	INI_Reader_read_header,
	INI_Reader_read_value,
	IMPORTS
};


#define IMP(func, idx) \
	__declspec(naked) func { __asm jmp imports[idx*8].addr }

IMP(DWORD CreateID(LPCSTR),				   impCreateID)
IMP(INI_Reader::INI_Reader(),			   INI_Reader_ctor)
IMP(INI_Reader::~INI_Reader(), 			   INI_Reader_dtor)
IMP(void INI_Reader::close(),			   INI_Reader_close)
IMP(LPCSTR INI_Reader::get_value_string(), INI_Reader_get_value_string)
IMP(bool INI_Reader::is_value(LPCSTR),	   INI_Reader_is_value)
IMP(bool INI_Reader::open(LPCSTR, bool),   INI_Reader_open)
IMP(bool INI_Reader::read_header(),		   INI_Reader_read_header)
IMP(bool INI_Reader::read_value(), 		   INI_Reader_read_value)


static HMODULE common;

void cleanup()
{
	FreeLibrary(common);
}

// Change to Freelancer's EXE directory and read the imports from Common.dll.
void import(LPCSTR fl)
{
	char fl_exe[MAX_PATH];

	// Need to do this in the EXE directory to find other DLLs.
	sprintf(fl_exe, "%s\\EXE", fl);
	SetCurrentDirectory(fl_exe);
	common = LoadLibrary("common.dll");
	if (common)
	{
		atexit(cleanup);

		for (int i = 0; i < IMPORTS; ++i)
			imports[i].addr = GetProcAddress(common, imports[i].name);
	}
}


class UTFXMLCommandLineInfo //: public CCommandLineInfo
{
public:
	UTFXMLCommandLineInfo()
	: audio(FALSE), textures(FALSE), includes(FALSE),
	  timestamps(FALSE), degrees(FALSE), rotation(FALSE), rgb(FALSE),
	  arg(NULL) { }

	/*virtual*/ void ParseParam(LPCSTR param, BOOL bFlag, BOOL bLast);

	CString dest;			// -o path
	CString src;			// file
	CString lst;			// -s string_list
	BOOL	audio;			// -a
	BOOL	textures;		// -g
	BOOL	includes;		// -i
	BOOL	timestamps; 	// -t
	BOOL	degrees;		// -d
	BOOL	rotation;		// -r
	UINT	rgb;			// -c, -C

	CString *arg;
};


void UTFXMLCommandLineInfo::ParseParam(LPCSTR param, BOOL bFlag, BOOL bLast)
{
	if (!bFlag)
	{
		if (arg)
		{
			*arg = param;
			arg = NULL;
		}
		else
		{
			src = param;
		}
		return;
	}

	for (; *param; ++param)
	{
		switch (*param)
		{
		case 'a': audio =      TRUE; break;
		case 'g': textures =   TRUE; break;
		case 'i': includes =   TRUE; break;
		case 't': timestamps = TRUE; break;
		case 'd': degrees =	   TRUE; break;
		case 'r': rotation =   TRUE; break;
		case 'c': rgb =	BST_CHECKED; break;
		case 'C': rgb = BST_INDETERMINATE; break;
		case 'o':
		case 's':
			arg = (*param == 'o') ? &dest : &lst;
			if (param[1])
			{
				*arg = param + 1;
				arg = NULL;
				return;
			}
			break;
		}
	}
}


//////////////////////////////////////////////////////////////////////
// BackgroundConversionTask entry point
//////////////////////////////////////////////////////////////////////

static UINT AFX_CDECL BackgroundConversionThread(void *args)
{
	return ((UTFXMLDlg *) args)->RunBackgroundConversionThread();
}


//////////////////////////////////////////////////////////////////////
// CAboutDlg class definition
//////////////////////////////////////////////////////////////////////

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CFont m_FontProgramName;
};


//////////////////////////////////////////////////////////////////////
// CAboutDlg class implementation
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LOGFONT logfont;
	GetDlgItem(IDC_PROGRAM_NAME)->GetFont()->GetLogFont(&logfont);
	logfont.lfWeight = FW_HEAVY;
	logfont.lfHeight = logfont.lfHeight * 3 / 2;
	m_FontProgramName.CreateFontIndirect(&logfont);
	GetDlgItem(IDC_PROGRAM_NAME)->SetFont(&m_FontProgramName);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


//////////////////////////////////////////////////////////////////////
// UTFXMLDlg class implementation
//////////////////////////////////////////////////////////////////////

UTFXMLDlg::UTFXMLDlg(CWnd* pParent /* = NULL */)
	: CDialog(UTFXMLDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(UTFXMLDlg)
	m_SourcePath = _T("");
	m_UtfFilenames = _T("");
	m_DestinationPath = _T("");
	m_StringsList = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ConversionInProgress = false;
    m_LogFile = NULL;
	m_XmlFile = NULL;
	m_FindingMaterials = false;
}


void UTFXMLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UTFXMLDlg)
	DDX_Control(pDX, IDC_UTF_FILENAMES, m_UtfFilenamesComboBox);
	DDX_Text(pDX, IDC_SOURCE_PATH, m_SourcePath);
	DDX_CBString(pDX, IDC_UTF_FILENAMES, m_UtfFilenames);
	DDX_Text(pDX, IDC_DESTINATION_PATH, m_DestinationPath);
	DDX_Text(pDX, IDC_STRINGS_LIST, m_StringsList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UTFXMLDlg, CDialog)
	//{{AFX_MSG_MAP(UTFXMLDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONVERT, OnConvert)
	ON_BN_CLICKED(IDC_BROWSE_UTF_FILENAME, OnBrowseUtfFilename)
	ON_BN_CLICKED(IDC_GENERATE_BUTTON, OnGenerate)
	ON_BN_CLICKED(IDC_HELP_BUTTON, OnHelpButton)
	ON_CBN_SELCHANGE(IDC_UTF_FILENAMES, OnSelchangeUtfFilenames)
	ON_BN_CLICKED(IDC_OPTION_RGB, OnOptionRgb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL UTFXMLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	// Add "About..." menu item to system menu.
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE); 		// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Initialize the Critical Section for the background conversion thread
	InitializeCriticalSection(&m_Mutex);

	char app_path[MAX_PATH];
	char dest_path[MAX_PATH];

	REGSAM sam = KEY_READ + KEY_WRITE;
	DWORD Size;

	HKEY hKey;
	/*if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, FREELANCER_KEYPATH, 0, NULL, 0, sam, NULL, &hKey, NULL) != ERROR_SUCCESS)
	{
		CString msg = "Unable to create/open Freelancer's registry key.";
		msg += "\n\nHKLM\\";
		msg += FREELANCER_KEYPATH;
		MessageBox(msg, "Error");
		EndDialog(0);
		return FALSE;
	}*/

	Size = sizeof(app_path);
	strcpy(app_path, "F:\\LIZERIUM\\6_COMPARE\\COMPARE_GAMES\\LizeriumFreelancerModeChange");
	// if (RegQueryValueEx(hKey, "AppPath", 0, NULL, (BYTE *) app_path, &Size) != ERROR_SUCCESS)
	// {
	// 	BROWSEINFO bi;
	// 	bi.hwndOwner = NULL;
	// 	bi.pidlRoot = NULL;
	// 	bi.pszDisplayName = app_path;
	// 	bi.lpszTitle = "Select path to Freelancer...";
	// 	bi.ulFlags = 0;
	// 	bi.lpfn = NULL;
	// 	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	// 	if (pidl == NULL)
	// 	{
	// 		EndDialog(0);
	// 		return FALSE;
	// 	}
	// 	SHGetPathFromIDList(pidl, app_path);
	// 	CoTaskMemFree(pidl);
	// 	RegSetValueEx(hKey, "AppPath", 0, REG_SZ, (BYTE *) app_path, strlen(app_path));
	// }

	Size = sizeof(dest_path);
	if (RegQueryValueEx(hKey, "UTFXML DestPath", 0, NULL, (BYTE *) dest_path, &Size) != ERROR_SUCCESS)
	{
		strcpy(dest_path, app_path);
		strcat(dest_path, "\\XML");
	}

	RegCloseKey(hKey);

	UTFXMLCommandLineInfo options;
	//ParseCommandLine(options);
	for (int a = 1; a < __argc; ++a)
	{
		LPCSTR arg = __argv[a];
		BOOL flag = FALSE;
		if (*arg == '-' || *arg == '/')
		{
			++arg;
			flag = TRUE;
		}
		options.ParseParam(arg, flag, (a == __argc - 1));
	}
	if (!options.dest.IsEmpty())
	{
		GetFullPathName(options.dest, MAX_PATH, dest_path, NULL);
	}
	else if (!options.src.IsEmpty())
	{
		GetCurrentDirectory(MAX_PATH, dest_path);
	}

	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	import(app_path);
	SetCurrentDirectory(path);

	if (options.src.IsEmpty())
	{
		m_Quiet = false;

		strcat(app_path, "\\DATA");

		m_UtfFilenamesComboBox.AddString("All UTF files (3db,ale,anm,cmp,dfm,mat,sph,txm,utf,vms)");
		m_UtfFilenamesComboBox.AddString("All Model files (*.3db)");
		m_UtfFilenamesComboBox.AddString("All Alchemy files (*.ale)");
		m_UtfFilenamesComboBox.AddString("All Animation files (*.anm)");
		m_UtfFilenamesComboBox.AddString("All Compound files (*.cmp)");
		m_UtfFilenamesComboBox.AddString("All Deformable files (*.dfm)");
		m_UtfFilenamesComboBox.AddString("All Material files (*.mat)");
		m_UtfFilenamesComboBox.AddString("All Sphere files (*.sph)");
		m_UtfFilenamesComboBox.AddString("All Texture files (*.txm)");
		m_UtfFilenamesComboBox.AddString("All Audio files (*.utf)");
		m_UtfFilenamesComboBox.AddString("All VMesh files (*.vms)");
		m_UtfFilenamesComboBox.SetCurSel(0);
	}
	else
	{
		m_Quiet = true;

		LPSTR name;
		GetFullPathName(options.src, MAX_PATH, app_path, &name);
		m_UtfFilenamesComboBox.SetWindowText(app_path);
		if (name)
			name[-1] = '\0';
	}

	m_SourcePath = app_path;
	m_DestinationPath = dest_path;

	SetDlgItemText(IDC_SOURCE_PATH, m_SourcePath);
	SetDlgItemText(IDC_DESTINATION_PATH, m_DestinationPath);

	CheckDlgButton(IDC_RECURSIVE,			  TRUE);
	CheckDlgButton(IDC_GENERATE_XML_FILES,	  TRUE);
	CheckDlgButton(IDC_CREATE_SUBFOLDERS,	  TRUE);
	CheckDlgButton(IDC_SKIP_AUDIO,			  TRUE);
	CheckDlgButton(IDC_SKIP_BASES,			  TRUE);
	CheckDlgButton(IDC_SKIP_CHARACTERS,		  TRUE);
	CheckDlgButton(IDC_SKIP_FX,				  TRUE);
	CheckDlgButton(IDC_EXTRACT_AUDIO_FILES,   options.audio);
	CheckDlgButton(IDC_EXTRACT_TEXTURE_FILES, options.textures);
	CheckDlgButton(IDC_EXTRACT_INCLUDE_FILES, options.includes);
	CheckDlgButton(IDC_OPTION_TIMESTAMPS,	  options.timestamps);
	CheckDlgButton(IDC_OPTION_DEGREES,		  options.degrees);
	CheckDlgButton(IDC_OPTION_ROTATION,		  options.rotation);
	CheckDlgButton(IDC_OPTION_RGB,			  options.rgb);
	OnOptionRgb();

	if (options.lst.IsEmpty())
	{
		m_StringsList.Format("%s\\strings.lst", (LPCSTR) m_DestinationPath);
		if (GetFileAttributes(m_StringsList) == -1)
		{
			if (GetFileAttributes("strings.lst") != -1)
			{
				m_StringsList.Format("%s\\strings.lst", path);
				SetDlgItemText(IDC_STRINGS_LIST, m_StringsList);
			}
			else
			{
				m_StringsList.Empty();
			}
		}
	}
	else
	{
		GetFullPathName(options.lst, MAX_PATH, path, NULL);
		m_StringsList = path;
	}
	SetDlgItemText(IDC_STRINGS_LIST, m_StringsList);

	SetCurrentDirectory(app_path);

	init_fx_crc();
	Init_Ale_Types();

	for (int i = 0; i < sizeof(NodeType)/sizeof(*NodeType); ++i)
	{
		node_map[CreateID(NodeType[i].name)] = i;
	}

	if (m_Quiet)
	{
		OnConvert();
		EndDialog(0);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}


BOOL UTFXMLDlg::DestroyWindow()
{
	// Make sure there is no conversion in progress
	if (m_ConversionInProgress)
	{
		// Tell the conversion to abort
		m_ConversionAborted = true;

		// Wait up to 5 seconds for the conversion to abort
		int count = 50; // five seconds
		while (m_ConversionInProgress && (count > 0))
		{
			// Wait 100 ms
			Sleep(100);
			count--;
		}
	}

	// Close the XML file, if necessary
	CloseXmlFile();

	// We are done with the critical section mutex
	DeleteCriticalSection(&m_Mutex);

	// Tell MFC to destroy this dialog
	return CDialog::DestroyWindow();
}


void UTFXMLDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void UTFXMLDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


HCURSOR UTFXMLDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void UTFXMLDlg::OnBrowseUtfFilename()
{
	UpdateData();

	CString path = m_UtfFilenames;
	if (m_UtfFilenames.Left(4) == "All ")
	{
		if (m_UtfFilenames[4] == 'U')
			path = m_SourcePath + "\\*.3db;*.ale;*.anm;*.cmp;*.dfm;*.mat;*.sph;*.txm;*.utf;*.vms";
		else
			path = m_SourcePath + "\\*" + m_UtfFilenames.Mid(m_UtfFilenames.GetLength()-5,4);
	}

	CFileDialog dlg(true, "All UTF Files", path,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		"All UTF Files|*.3db;*.ale;*.anm;*.cmp;*.dfm;*.mat;*.sph;*.txm;*.utf;*.vms|"
		"All Model Files (*.3db)|*.3db|"
		"All Alchemy Files (*.ale)|*.ale|"
		"All Animation Files (*.anm)|*.anm|"
		"All Compound Files (*.cmp)|*.cmp|"
		"All Deformable Files (*.dfm)|*.dfm|"
		"All Material Files (*.mat)|*.mat|"
		"All Sphere Files (*.sph)|*.sph|"
		"All Texture Files (*.txm)|*.txm|"
		"All Audio Files (*.utf)|*.utf|"
		"All VMesh Files (*.vms)|*.vms|"
		"All Files|*||");
	dlg.m_ofn.lStructSize = 0x58;

	if (dlg.DoModal() == IDOK)
	{
		m_UtfFilenames = dlg.GetPathName();
		m_UtfFilenamesComboBox.SetWindowText(m_UtfFilenames);
		CFile f;
		f.SetFilePath(m_UtfFilenames);
		CString path = f.GetFilePath();
		CString name = f.GetFileName();
		m_SourcePath =	path.Left(path.GetLength() - name.GetLength() - 1);
		SetDlgItemText(IDC_SOURCE_PATH, m_SourcePath);
	}
}


void UTFXMLDlg::OnSelchangeUtfFilenames()
{
	m_UtfFilenamesComboBox.GetWindowText(m_UtfFilenames);
}


void UTFXMLDlg::OnOptionRgb() 
{
	LPCSTR text;

	switch (IsDlgButtonChecked(IDC_OPTION_RGB))
	{
	case BST_UNCHECKED : text = "RGB (decimal)"; break;
	case BST_CHECKED :	 text = "RGB (integer)"; break;
	default :			 text = "RGB (hex)";	 break;
	}
	SetDlgItemText(IDC_OPTION_RGB, text);
}


void UTFXMLDlg::OnHelpButton()
{
}


void UTFXMLDlg::OnGenerate()
{
	UpdateData(TRUE);

	if (m_StringsList.IsEmpty())
	{
		m_StringsList.Format("%s\\strings.lst", (LPCSTR) m_DestinationPath);
		SetDlgItemText(IDC_STRINGS_LIST, m_StringsList);
		CreateAndSetPath(m_DestinationPath);
	}
	m_XmlFile = fopen(m_StringsList, "w");
	if (!m_XmlFile)
	{
		MessageBox("Strings list file is invalid.", "Error", MB_ICONERROR);
		return;
	}
	m_FindingMaterials = true;
	OnConvert();
	m_FindingMaterials = false;
	mat_crc.clear();

	if (common && SetCurrentDirectory(m_SourcePath) && SetCurrentDirectory("AUDIO"))
	{
		// Add a blank line to separate materials and msgs.
		fprintf(m_XmlFile, "\n");
		INI_Reader ini;
		WIN32_FIND_DATA fdata;
		HANDLE h = FindFirstFile("voice*.ini", &fdata);
		if (h != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!ini.open(fdata.cFileName))
				{
					continue;
				}
				m_NumDataFilesExtracted++;
				while (ini.read_header())
				{
					while (ini.read_value())
					{
						if (ini.is_value("msg"))
						{
							LPCSTR msg = ini.get_value_string();
							DWORD hash = CreateID(msg);
							if (msg_hash.find(hash) == msg_hash.end())
							{
								fprintf(m_XmlFile, "%s\n", msg);
								msg_hash[hash] = "";
							}
							break;
						}
					}
				}
				ini.close();
			} while (FindNextFile(h, &fdata));
		}
	}

	fclose(m_XmlFile);

	CString msg;
	msg.Format("Examined %d UTF files.\n"
			   "Found %d material names.\n"
			   "Examined %d ini files.\n"
			   "Found %d audio names.",
			   m_NumUtfFilesOpened,
			   m_NumXmlFilesCreated,
			   m_NumDataFilesExtracted,
			   msg_hash.size());
	MessageBox(msg, "Strings List Results", MB_ICONINFORMATION);

	msg_hash.clear();
}


void UTFXMLDlg::OnConvert()
{
	char *mat_names = NULL;

	UpdateData(TRUE);

	if (m_FindingMaterials)
	{
		m_Recursive			   = true;
		m_GenerateXmlFiles	   = false;
		m_CreateSubfolders	   = false;
		m_SkipAudioFolder	   = true;
		m_SkipBasesFolder	   = false;
		m_SkipCharactersFolder = false;
		m_SkipFxFolder		   = false;
	}
	else
	{
		m_Recursive			   = IsDlgButtonChecked(IDC_RECURSIVE);
		m_GenerateXmlFiles	   = IsDlgButtonChecked(IDC_GENERATE_XML_FILES);
		m_CreateSubfolders	   = IsDlgButtonChecked(IDC_CREATE_SUBFOLDERS);
		m_SkipAudioFolder	   = IsDlgButtonChecked(IDC_SKIP_AUDIO);
		m_SkipBasesFolder	   = IsDlgButtonChecked(IDC_SKIP_BASES);
		m_SkipCharactersFolder = IsDlgButtonChecked(IDC_SKIP_CHARACTERS);
		m_SkipFxFolder		   = IsDlgButtonChecked(IDC_SKIP_FX);
		m_ExtractAudioFiles    = IsDlgButtonChecked(IDC_EXTRACT_AUDIO_FILES);
		m_ExtractTextureFiles  = IsDlgButtonChecked(IDC_EXTRACT_TEXTURE_FILES);
		m_ExtractIncludeFiles  = IsDlgButtonChecked(IDC_EXTRACT_INCLUDE_FILES);
		m_OptionTimestamps	   = IsDlgButtonChecked(IDC_OPTION_TIMESTAMPS);
		m_OptionDegrees		   = IsDlgButtonChecked(IDC_OPTION_DEGREES);
		m_OptionRotation	   = IsDlgButtonChecked(IDC_OPTION_ROTATION);
		m_OptionRGB			   = IsDlgButtonChecked(IDC_OPTION_RGB);
	}

	if (!SetCurrentDirectory(m_SourcePath))
	{
		MessageBox("Source Path is invalid", "Error", MB_ICONERROR);
		return;
	}

	char source_path[MAX_PATH];
	GetCurrentDirectory(sizeof(source_path), source_path);
	char *data = stristr(source_path, "DATA");
	if (data && (data == source_path || data[-1] == '\\') &&
				(data[4] == '\\' || !data[4]))
	{
		m_PathTrim = data + 5 - source_path;
	}
	else
	{
		m_PathTrim = strlen(source_path) + 1;
	}

	CreateAndSetPath(m_DestinationPath);
	GetCurrentDirectory(sizeof(m_CurrentDestinationPath), m_CurrentDestinationPath);

    // Open the log file
	char temppath[MAX_PATH];
	GetTempPath(MAX_PATH, temppath);
	SetCurrentDirectory(temppath);
    m_LogFile = fopen("UTFXML.log", "w");

    if (!m_LogFile)
    {
        MessageBox("Unable to create UTFXML.log in temporary folder!", "Error", MB_ICONERROR);
        return;
    }

    fprintf(m_LogFile, "%s\n\n", UTFXML_VERSION);
	fprintf(m_LogFile, "Source path: %s\n", source_path);
	fprintf(m_LogFile, "Destination path: %s\n", m_CurrentDestinationPath);

	if (!m_Quiet)
	{
		REGSAM sam = KEY_READ + KEY_WRITE;
		HKEY hKey = NULL;
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, FREELANCER_KEYPATH, 0, sam, &hKey);
		RegSetValueEx(hKey, "UTFXML DestPath", 0, REG_SZ, (BYTE *) m_CurrentDestinationPath, strlen(m_CurrentDestinationPath));
		RegCloseKey(hKey);
	}

    SetCurrentDirectory(m_SourcePath);

	// Initialize variables
	if (!m_FindingMaterials)
	{
		m_XmlFile = NULL;

		if (!m_StringsList.IsEmpty())
		{
			FILE *f = fopen(m_StringsList, "rb");
			if (f)
			{
				fseek(f, 0, SEEK_END);
				int len = ftell(f);
				rewind(f);
				mat_names = new char[len];
				fread(mat_names, 1, len, f);
				fclose(f);
				bool do_hash = false;
				char *n = mat_names;
				char *end = mat_names + len;
				while (n < end)
				{
					char *e = strchr(n, '\n');
					if (e[-1] == '\r')
					{
						e[-1] = '\0';
					}
					else
					{
						*e = '\0';
					}
					if (!*n)
					{
						if (!common)
						{
							break;
						}
						do_hash = true;
						n = e + 1;
						continue;
					}
					if (do_hash)
					{
						msg_hash[CreateID(n)] = n;
					}
					else
					{
						mat_crc[fl_crc32(n)] = n;
					}
					n = e + 1;
				}
			}
		}
	}
	m_Tree = NULL;
	m_String = NULL;
	m_Data = NULL;
	m_Depth = 0;
	m_tga_file = NULL;
	m_bmp_file = NULL;
	m_vms_file = NULL;
	m_3db_file = NULL;
	m_TextureLibrary = false;
	m_VMeshLibrary = false;
	m_Animation = false;
	m_GeneratedOutput = false;
	m_ConversionAborted = false;
	m_ConversionInProgress = true;
	m_CountingUtfFiles = false;
	m_UtfFileCount = 0;
	m_NumUtfFilesOpened = 0;
	m_NumXmlFilesCreated = 0;
	m_NumDataFilesExtracted = 0;

	// Did the user specify a file name?
	bool all = (m_FindingMaterials || m_UtfFilenames.Left(4) == "All ");
	if (all)
	{
		ShowWindow(SW_HIDE);

		// Launch the background conversion thread
		CWinThread *winthread = AfxBeginThread(BackgroundConversionThread, this, THREAD_PRIORITY_BELOW_NORMAL);

		// Display the progress bar
		ProgressDialog progress(this);
		progress.DoModal();
	}
	else
	{
		// Process a single file
		BeginWaitCursor();
		ProcessUTFFile(m_UtfFilenames);
		EndWaitCursor();
	}

	if (m_ConversionAborted)
	{
		Log("\nConversion Aborted\n");
		MessageBox("Conversion Aborted", "UTF to XML", MB_ICONINFORMATION);
	}
	else
	{
		Log("\n------------------------------------------------------------\n");
		// Display the conversion summary
		CString msg;
		if (!m_FindingMaterials)
		{
			msg.Format("Examined %d UTF files.\n"
					   "Created %d XML files.\n"
					   "Extracted %d data files.",
					   m_NumUtfFilesOpened,
					   m_NumXmlFilesCreated,
					   m_NumDataFilesExtracted);
			Log("%s\n", (LPCSTR) msg);
			if (!m_Quiet)
				MessageBox(msg, "UTF to XML Conversion Results", MB_ICONINFORMATION);
		}
		else
		{
			m_NumXmlFilesCreated = mat_crc.size();
		}
	}

    if (m_LogFile)
    {
        fclose(m_LogFile);
        m_LogFile = NULL;
    }

	if (all)
	{
		ShowWindow(SW_NORMAL);
	}

	delete[] mat_names;
	mat_crc.clear();
	msg_hash.clear();
}


UINT UTFXMLDlg::RunBackgroundConversionThread()
{
	// Finding material names is quite quick, so don't bother with the count
	if (!m_FindingMaterials)
	{
		// Count all files in the folder
		m_CountingUtfFiles = true;
		ProcessFolder(".");

		// Update the file count for the progress bar
		EnterCriticalSection(&m_Mutex);
		m_UtfFileCount = m_NumUtfFilesOpened;
		m_NumUtfFilesOpened = 0;
		LeaveCriticalSection(&m_Mutex);
	}

	// Process all files in the folder
	m_CountingUtfFiles = false;
	ProcessFolder(".");

	// Set the completion indicator
	m_ConversionInProgress = false;

	return (0);
}


bool UTFXMLDlg::ProcessFolder(LPCSTR folder)
{
	bool generated_output = false;
	m_GeneratedOutput = false;

	// Does the user want to skip this folder?
	if ((m_SkipAudioFolder		&& STRIEQ(folder, "AUDIO")) ||
		(m_SkipBasesFolder		&& STRIEQ(folder, "BASES")) ||
		(m_SkipCharactersFolder && STRIEQ(folder, "CHARACTERS")) ||
		(m_SkipFxFolder			&& STRIEQ(folder, "FX")))
	{
		// User wants to skip this folder
		return (false);
	}

	// Save the current source path
	char current_source_path[MAX_PATH];
	GetCurrentDirectory(sizeof(current_source_path), current_source_path);

	// Update the current source path
	EnterCriticalSection(&m_Mutex);
	strcpy(m_CurrentSourcePath, current_source_path);
	LeaveCriticalSection(&m_Mutex);

	if (m_CreateSubfolders && !m_CountingUtfFiles)
	{
		// Create the folder and drill down into the destination path
		SetCurrentDirectory(m_CurrentDestinationPath);
		CreateDirectory(folder, NULL);
		SetCurrentDirectory(folder);
		GetCurrentDirectory(sizeof(m_CurrentDestinationPath), m_CurrentDestinationPath);

		// Restore the current source path
		SetCurrentDirectory(current_source_path);
	}

	// Process all files in this directory
	WIN32_FIND_DATA fdata;
	HANDLE h = FindFirstFile("*", &fdata);
	do
	{
		if (m_ConversionAborted)
		{
			break;
		}

		if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// This is a directory
			if (m_Recursive && (fdata.cFileName[0] != '.'))
			{
				// Do recursive processing on the subfolder
				SetCurrentDirectory(fdata.cFileName);
				if (ProcessFolder(fdata.cFileName))
				{
					generated_output = true;
				}
				SetCurrentDirectory("..");

				// Update the current source path
				EnterCriticalSection(&m_Mutex);
				strcpy(m_CurrentSourcePath, current_source_path);
				LeaveCriticalSection(&m_Mutex);
			}
		}
		else
		{
			if (IsFileTypeSelected(fdata.cFileName))
			{
				ProcessUTFFile(fdata.cFileName);
				if (m_GeneratedOutput)
				{
					generated_output = true;
					m_GeneratedOutput = false;
				}
			}
		}
	} while (FindNextFile(h, &fdata));

	if (m_CreateSubfolders && !m_CountingUtfFiles)
	{
		char current_source_path[MAX_PATH];
		// Save the current source path
		GetCurrentDirectory(sizeof(current_source_path), current_source_path);

		// Move up one level from the current destination path
		SetCurrentDirectory(m_CurrentDestinationPath);
		SetCurrentDirectory("..");

		if (!generated_output)
		{
			RemoveDirectory(folder);
		}

		GetCurrentDirectory(sizeof(m_CurrentDestinationPath), m_CurrentDestinationPath);

		// Restore the current source path
		SetCurrentDirectory(current_source_path);
	}

	return (generated_output);
}


bool UTFXMLDlg::IsFileTypeSelected(LPCSTR filename)
{
	const char* ext = strrchr(filename, '.');
	if (!ext)
	{
		return (false);
	}
	++ext;

	if (m_FindingMaterials)
	{
		if (STRIEQ(ext, "3db") ||
			STRIEQ(ext, "cmp") ||
			STRIEQ(ext, "mat"))
		{
			return (true);
		}
	}
	else if (m_UtfFilenames[4] == 'U')
	{
		if (STRIEQ(ext, "3db") ||
			STRIEQ(ext, "ale") ||
			STRIEQ(ext, "anm") ||
			STRIEQ(ext, "cmp") ||
			STRIEQ(ext, "dfm") ||
			STRIEQ(ext, "mat") ||
			STRIEQ(ext, "sph") ||
			STRIEQ(ext, "txm") ||
			STRIEQ(ext, "utf") ||
			STRIEQ(ext, "vms"))
		{
			return (true);
		}
	}
	else if (m_UtfFilenames.Mid(m_UtfFilenames.GetLength()-4,3).CompareNoCase(ext) == 0)
	{
		return (true);
	}

	return (false);
}


void UTFXMLDlg::CreateAndSetPath(LPCSTR path)
{
	if (SetCurrentDirectory(path))
		return;

	char dir[MAX_PATH];
	char *folder = dir;
	strcpy(dir, path);
	// Skip over the root.
	if (*folder == '\\')
	{
		++folder;
		SetCurrentDirectory("\\");
	}
	else if (folder[1] == ':' && folder[2] == '\\')
	{
		folder += 3;
		char save = *folder;
		*folder = '\0';
		SetCurrentDirectory(dir);
		*folder = save;
	}
	folder = strtok(folder, "\\");
	while (folder && *folder)
	{
		CreateDirectory(folder, NULL);
		SetCurrentDirectory(folder);
		folder = strtok(NULL, "\\");
	}
}


void UTFXMLDlg::Log(LPCSTR format, ...)
{
	char buffer[512];

	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	TRACE(buffer);

	if (m_LogFile)
	{
		fprintf(m_LogFile, buffer);
	}
}


void UTFXMLDlg::ProcessUTFFile(LPCSTR filename)
{
	bool generated_output = false;

	FILE *f = fopen(filename, "rb");
	if (f)
	{
		// Determine the file size
		fseek(f, 0, SEEK_END);
		DWORD file_size = ftell(f);
		fseek(f, 0, SEEK_SET);

		// Try to read the UTF header
		if (fread(&m_Header, sizeof(m_Header), 1, f) != 1)
		{
			fclose(f);
			return;
		}

		// Look for the UTF signature
		if (!STRBEG(m_Header.UTF, "UTF "))
		{
			fclose(f);
			return;
		}

		// Increment the number of UTF files opened
		m_NumUtfFilesOpened++;

		// If we are just counting the files, we are done
		if (m_CountingUtfFiles)
		{
			fclose(f);
			return;
		}

		Log("\n------------------------------------------------------------\n");
		if (strchr(filename, '\\'))
		{
			Log("Processing \"%s\"\n", filename + m_PathTrim);
		}
		else
		{
			char current_path[MAX_PATH];
			GetCurrentDirectory(sizeof(current_path), current_path);
			Log("Processing \"%s\\%s\"\n", current_path + m_PathTrim, filename);
		}

		// Check the UTF version number
		if (m_Header.version != 0x0101)
		{
			Log("   *** Error: Unrecognized UTF version %04X ***\n", m_Header.version);
			fclose(f);
			return;
		}

		// Check the header size
		if (m_Header.node_size != 44)
		{
			Log("   *** Error: Invalid node size %d (0x%X) ***\n", m_Header.node_size, m_Header.node_size);
			fclose(f);
			return;
		}

		// Verify that the data follows the header, tree, and string data
		DWORD data_offset = sizeof(UTFHeader) +
							m_Header.tree_size +
							m_Header.string_alloc;

		if (data_offset != m_Header.data_offset)
		{
			Log("   *** Error: Invalid data offset (found %08X, expected %08X) ***\n", m_Header.data_offset, data_offset);
			fclose(f);
			return;
		}

		// Do sanity check on the string size
		if (m_Header.string_alloc < m_Header.string_size)
		{
			Log("   *** Error: String Size (%u) greater than String Alloc (%u) ***\n", m_Header.string_size, m_Header.string_alloc);
			fclose(f);
			return;
		}

		// The tree data OR the string data must follow the header (the order varies)
		if ((m_Header.tree_offset != sizeof(m_Header)) &&
			(m_Header.string_offset != sizeof(m_Header)))
		{
			Log("   *** Error: Invalid tree/string offset ***\n");
			fclose(f);
			return;
		}

		// Allocate memory for the tree data
		m_Tree = new BYTE[m_Header.tree_size];
		if (!m_Tree)
		{
			Log("   *** Error: Unable to allocate space for tree ***\n");
			fclose(f);
			return;
		}

		// Allocate memory for the string data
		m_String = new char[m_Header.string_alloc];
		if (!m_String)
		{
			delete m_Tree;
			Log("   *** Error: Unable to allocate space for string ***\n");
			fclose(f);
			return;
		}

		DWORD data_size;
		if (!m_FindingMaterials)
		{
			// Allocate memory for the data
			data_size = file_size - data_offset;
			m_Data = new BYTE[data_size];
			if (!m_Data)
			{
				delete m_String;
				delete m_Tree;
				Log("   *** Error: Unable to allocate space for data ***\n");
				fclose(f);
				return;
			}
		}

		// Read the Tree
		fseek(f, m_Header.tree_offset, SEEK_SET);
		fread(m_Tree, 1, m_Header.tree_size, f);

		// Read the Strings
		fseek(f, m_Header.string_offset, SEEK_SET);
		fread(m_String, 1, m_Header.string_alloc, f);

		UTFNode *root = NODE(0);

		if (m_FindingMaterials)
		{
			UTFNode *node = FindNode(root, "Material Library");
			if (node && (node->flags & BRANCH) && node->branch)
			{
				node = NODE(node->branch);
				while (true)
				{
					char *name = NAME(node->name);
					if (node->flags & BRANCH)
					{
						DWORD crc = fl_crc32(name);
						if (mat_crc.find(crc) == mat_crc.end())
						{
							fprintf(m_XmlFile, "%s\n", name);
							mat_crc[crc] = "";
						}
					}
					if (!node->next)
					{
						break;
					}
					node = NODE(node->next);
				}
			}

			delete m_String;
			delete m_Tree;
			fclose(f);
			return;
		}

		// Read the Data
		fseek(f, m_Header.data_offset, SEEK_SET);
		fread(m_Data, 1, data_size, f);

		////////////////////////////////////////////////////////////
		// If the user wants to generate an XML file, open it now
		////////////////////////////////////////////////////////////
		if (m_GenerateXmlFiles)
		{
			OpenXmlFile(filename);

			// Create a list of Rev parts, for Frames.
			m_rev_parts.clear();
			UTFNode *rev = FindNode(FindNode(FindNode(root, "Cmpnd"), "Cons"), "Rev");
			if (rev)
			{
				REV *part = (REV *) DATA(rev->branch);
				int size = rev->size1 / sizeof(REV);
				while (size--)
				{
					m_rev_parts.insert(part->child);
					++part;
				}
			}
		}

		m_is_anm = m_is_dfm = false;
		const char* dot = strrchr(filename, '.');
		if (dot)
		{
			// If this is a 3DB file, save the name for later use
			if (STRIEQ(dot, ".3db"))
			{
				// Strip the path
				const char* sep = strrchr(filename, '\\');
				m_3db_file = (sep) ? sep+1 : (char *) filename;
			}
			else if (STRIEQ(dot, ".anm"))
			{
				m_is_anm = true;
			}
			else if (STRIEQ(dot, ".dfm"))
			{
				m_is_dfm = true;
			}
		}

		mesh_crc.clear();
		// Always add the meshes in DATA\INTERFACE\interface.generic.vms
		mesh_crc[0xE296602F] = "interface.generic-2.vms";
		mesh_crc[0x1351B6D4] = "interface.generic-102.vms";

		m_time1 = m_time2 = m_time3 = 0;

		// Process the root of the tree (and everything below it)
		ProcessUTFNode(root);

		// If we have an open XML file, close it now
		if (m_XmlFile)
		{
			CloseXmlFile();
		}

		// We are done with the data, string, and tree
		delete m_Data;
		delete m_String;
		delete m_Tree;

		// Close the input file
		fclose(f);
	}
}


void UTFXMLDlg::ProcessUTFNode(UTFNode *node)
{
	bool done = false;
	char xml_name[MAX_PATH]; // xml compliant tag name
	char lc_name[MAX_PATH];  // lowercase name
	char xml_tag[MAX_PATH];  // contains compliant and non-compliant names
	char *include;
	FILE *old_XmlFile;
	int  old_Depth;
	static char wire_name[MAX_PATH];
	static int vmeshwire_cnt;
	DWORD time1, time2, time3;

	// Loop through all the nodes at this level until we are done
	while (!done)
	{
		char *name = NAME(node->name);

		// Sanity check on the two sizes
		if (node->size1 != node->size2)
		{
		   Log("   *** Error: (size1 != size2) (%d != %d) ***\n", node->size1, node->size2);
		}

		bool bad_xml_tag = false;
		int name_length = strlen(name);
		for (int i = 0; i < name_length+1; i++)
		{
			char c = name[i];
			lc_name[i] = tolower(c);
			xml_tag[i] = xml_name[i] = c;

			// Does the name contain any invalid characters?
			if (!((isalpha(c)) ||
				  ((isdigit(c)) && (i != 0)) ||
				  (c == '.') ||
				  (c == '_') ||
				  (c == '-') ||
				  (c == 0)))
			{
				// This is not a valid XML character
				xml_tag[i] = xml_name[i] = '_';
				bad_xml_tag = true;
			}
		}

		// Use the XML compliant name but save the original name
		if (bad_xml_tag)
		{
			sprintf(xml_tag+name_length, " name=\"%s\"", name);
		}

		if (strstr(lc_name, ".tga"))
		{
			m_tga_file = name;
			m_bmp_file = NULL;
			m_vms_file = NULL;
			m_3db_file = NULL;
		}
		else if (strstr(lc_name, ".bmp"))
		{
			m_bmp_file = name;
			m_tga_file = NULL;
			m_vms_file = NULL;
			m_3db_file = NULL;
		}
		else if (strstr(lc_name, ".vms"))
		{
			m_vms_file = name;
			m_tga_file = NULL;
			m_bmp_file = NULL;
		}
		else if (strstr(lc_name, ".3db"))
		{
			m_3db_file = name;
			m_tga_file = NULL;
			m_bmp_file = NULL;
			m_vms_file = NULL;
		}
		else if (m_TextureLibrary)
		{
			if (!(STRBEG(lc_name, "mip") && (name_length == 4)))
			{
				m_tga_file = name;
				m_bmp_file = NULL;
				m_vms_file = NULL;
				m_3db_file = NULL;
			}
		}

		// Make a special note when we begin a texture library branch
		if (STREQ(lc_name, "texture library"))
		{
			m_TextureLibrary = true;
		}
		// Similarly with VMeshLibrary
		else if (STREQ(lc_name, "vmeshlibrary"))
		{
			m_VMeshLibrary = true;
		}
		// Similarly with VMeshLibrary
		else if (STREQ(lc_name, "animation"))
		{
			m_Animation = true;
		}
		// Check if it's a Rev part being animated.
		else if (STRBEG(lc_name, "joint map "))
		{
			m_is_rev = false;
			if (!m_rev_parts.empty())
			{
				UTFNode *child = FindNode(node, "Child name");
				if (child && m_rev_parts.find((LPCSTR) DATA(child->branch)) != m_rev_parts.end())
					m_is_rev = true;
			}
		}
		// Need to see Header first in order to interpret Frames
		else if (STREQ(lc_name, "channel"))
		{
			m_AnimHeader = NULL;
			UTFNode *frames = NODE(node->branch);
			if (STRIEQ(NAME(frames->name), "Frames"))
			{
				DWORD next = frames->next;
				UTFNode *header = NODE(next);
				frames->next = header->next;
				header->next = node->branch;
				node->branch = next;
			}
		}
		// When we see ALEffectLib, preprocess AlchemyNodeLibrary to
		// find the CRCs.
		else if (STREQ(lc_name, "aleffectlib"))
		{
			UTFNode *lib = NODE(node->next);
			lib = NODE(lib->branch);
			if (STRIEQ(NAME(lib->name), "alchemynodelibrary"))
			{
				ExtractAlchemyNodes(DATA(lib->branch));
			}
		}

		if (node->flags == 0xFFFFFFFF)
		{
			Log("   *** Error: Node in tree is marked \"unused\"!\n");
		}

		// Is this a leaf?
		if (node->flags & LEAF)
		{
			// Process the Leaf Node
			BYTE *leaf = DATA(node->branch);
			m_Node = node;
			m_xml_tag = xml_tag;
			m_xml_name = xml_name;
			ProcessLeaf(name, leaf, node->size1);
		}
		else if (node->branch)
		{
			include = NULL;
			// This is a Branch Node
			// Create the XML record for the beginning
			if (m_XmlFile)
			{
				if (STREQ(name, "\\"))
				{
					// This is the root
					Indent(1);
					fprintf(m_XmlFile, "<UTF_ROOT");
				}
				else
				{
					if (m_VMeshLibrary && m_Depth == 3)
					{
						DWORD crc = fl_crc32(name);
						mesh_crc[crc] = name;
					}
					Indent();
					fprintf(m_XmlFile, "<%s", xml_tag);
					// Give certain nodes their own file.
					if ((!m_is_anm && STREQ(lc_name, "animation")) ||
						(m_is_anm && m_Depth == 4) ||
						(m_is_dfm && STRBEG(lc_name, "mesh")) ||
						(m_VMeshLibrary && m_Depth == 3) ||
						(STREQ(lc_name, "vmeshwire")))
					{
						if (STREQ(lc_name, "vmeshwire"))
						{
							if (m_3db_file)
							{
								// Remove the timestamp
								int i, j;
								i = strlen(m_3db_file)-4;
								for (j = 12; j > 0; --j)
								{
									if (!isdigit(m_3db_file[i-j]))
									{
										break;
									}
								}
								if (j == 0)
								{
									i -= 12;
								}
								sprintf(wire_name, "%.*s.vwd", i, m_3db_file);
							}
							else
							{
								sprintf(wire_name, "VMeshWire%d", ++vmeshwire_cnt);
							}
							include = wire_name;
						}
						else if (m_VMeshLibrary)
						{
							// Remove the path
							include = stristr(name, "lod");
							if (!include)
							{
								include = name;
							}
						}
						else if(m_Animation)
						{
							sprintf(xml_tag, "%s", name);
							UTFNode *branch = NODE(node->branch);
						}
						else
						{
							include = name;
						}
						if(!m_Animation)
							fprintf(m_XmlFile, " include=\"%s\\%s.xml\"", m_ExtractPath, include);
					}
				}

				WriteNodeUnk(node);
				fprintf(m_XmlFile, "%s>", (include) ? "/" : "");
			}
			if (include)
			{
				old_XmlFile = m_XmlFile;
				old_Depth = m_Depth;
				OpenIncludeFile(include, xml_name);
			}

			time1 = m_time1;
			time2 = m_time2;
			time3 = m_time3;

			m_time1 = node->time1;
			m_time2 = node->time2;
			m_time3 = node->time3;

			// Indent another level
			m_Depth++;

			// Process the Branch Node
			UTFNode *branch = NODE(node->branch);
			ProcessUTFNode(branch);

			// Decrease the indent level
			m_Depth--;

			m_time1 = time1;
			m_time2 = time2;
			m_time3 = time3;

			// Create the XML record for the end (except for the root)
			if (include)
			{
				CloseIncludeFile(xml_name);
				m_XmlFile = old_XmlFile;
				m_Depth = old_Depth;
			}
			else if (m_XmlFile)
			{
				Indent();
				if (STREQ(name, "\\"))
				{
					fprintf(m_XmlFile, "</UTF_ROOT>\n");
					m_Depth--;
				}
				else
				{
					fprintf(m_XmlFile, "</%s>", xml_name);
				}
			}
		}
		else
		{
			// This is a branch which has no leaves
			if (m_XmlFile && !STREQ(name, "\\"))
			{
				Indent();
				fprintf(m_XmlFile, "<%s", xml_tag);
				WriteNodeUnk(node);
				fprintf(m_XmlFile, "/>");
			}
		}

		// Make a special note when we are done with a texture library branch
		if (STREQ(lc_name, "texture library"))
		{
			m_TextureLibrary = false;
			m_tga_file = false;
		}
		// Similarly with VMeshLibrary
		else if (STREQ(lc_name, "vmeshlibrary"))
		{
			m_VMeshLibrary = false;
		}
		// Similarly with Animation
		else if (STREQ(lc_name, "animation"))
		{
			m_Animation = false;
		}

		// Tidy up after ALEffectLib
		else if (!node_crc.empty())
		{
			CIterCRCName iter, end = node_crc.end();
			for (iter = node_crc.begin(); iter != end; ++iter)
			{
				free((void *) iter->second);
			}
			node_crc.clear();
		}

		// Have more nodes in the chain?
		if (node->next)
		{
			// Move to the next node
			node = NODE(node->next);
		}
		else
		{
			// No more nodes in this chain - we are done at this level
			done = true;
		}
	}
}


void UTFXMLDlg::ProcessLeaf(LPCSTR name, BYTE *leaf, DWORD size)
{
	int    node_type = NODE_TYPE_UNDETERMINED;
	char   fname[MAX_PATH];
	char  *filename = fname;
	DWORD  wrap = 40;
	DWORD  i;
	bool   MADeltas = false;
	bool   BWC = false;
	bool   use_hex = false;
	bool   use_deg = false;
	int    array_wrap = 0;
	int    left = -2;

	Ptrs p;
	p.b = leaf;

	DWORD hash = CreateID(name);
	NameNodeIter iter = node_map.find(hash);
	if (iter != node_map.end())
	{
		node_type  = NodeType[iter->second].type;
		array_wrap = NodeTypeFormat[NodeType[iter->second].format].wrap;
		left	   = NodeTypeFormat[NodeType[iter->second].format].left;
		use_hex    = NodeTypeFormat[NodeType[iter->second].format].hex;
		if (m_OptionDegrees)
		{
			use_deg = NodeTypeFormat[NodeType[iter->second].format].deg;
		}

		// We can only handle Frames if we've seen its Header
		if (node_type == NODE_TYPE_FRAMES && !m_AnimHeader && m_ExtractIncludeFiles)
		{
			Log("   *** Warning: Frames should be immediately after Header\n");
			node_type = NODE_TYPE_UNDETERMINED;
		}
		MADeltas = (hash == 0x8A29CA4A); // CreateID("madeltas")
		BWC 	 = (hash == 0xB22529CD); // CreateID("bone_weight_chain")
	}

	else if (hash == 0xBA5494C1) // CreateID("constant")
	{
		// Constant may be either RGB or Alpha
		node_type = (size == 12) ? NODE_TYPE_RGB : NODE_TYPE_FLOAT;
	}

	// Look for certain nodes containing Texture file data
	else if (hash == 0x8BA7E34B) // STRIEQ(name, "mips"))
	{
		if (m_tga_file)
		{
			node_type = NODE_TYPE_FILE;
			i = strlen(m_tga_file);
			if (i > 4 && STRIEQ(m_tga_file+i-4, ".tga"))
			{
				i -= 4;
			}
			// Let's just assume it's the timestamp after the extension.
			else if (i > 16 && STRIBEG(m_tga_file+i-16, ".tga"))
			{
				i -= 16;
			}
			sprintf(fname, "%.*s.dds", i, m_tga_file);
			filename = SaveDataFile(fname, leaf, size, m_ExtractTextureFiles);
			m_tga_file = NULL;
		}
	}
	else if (STRIBEG(name, "mip") && (strlen(name) == 4))
	{
		LPCSTR tex = NULL;
		LPCSTR ext;
		if (m_tga_file)
		{
			tex = m_tga_file;
			ext = ".tga";
		}
		else if (m_bmp_file)
		{
			tex = m_bmp_file;
			ext = ".bmp";
		}
		if (tex)
		{
			node_type = NODE_TYPE_FILE;
			i = strlen(tex);
			if (i > 4 && STRIEQ(tex+i-4, ext))
			{
				i -= 4;
			}
			else if (i > 16 && strnicmp(tex+i-16, ext, 4) == 0)
			{
				i -= 16;
			}
			sprintf(fname, "%.*s_mip%s.tga", i, tex, name+3);
			filename = SaveDataFile(fname, leaf, size, m_ExtractTextureFiles);
		}
	}
	else if (hash == 0xB9FD4247) // STRIEQ(name, "cube"))
	{
		node_type = NODE_TYPE_FILE;
		strcpy(fname, "cube.dds");
		filename = SaveDataFile(fname, leaf, size, m_ExtractTextureFiles);
	}

	// Look for certain nodes containing Audio file data
	else if ((memcmp(leaf,	 "RIFF", 4) == 0) &&
			 (memcmp(leaf+8, "WAVE", 4) == 0))
	{
		node_type = NODE_TYPE_FILE;
		char *end;
		hash = strtoul(name, &end, 0);
		if (*end == '\0')
		{
			// Fair chance it's one of the audio files;
			// see if it's one that's been mapped.
			sprintf(fname, "%s.wav", crc_name(msg_hash, hash));
		}
		else
		{
			strcpy(fname, name);
			i = strlen(fname);
			if (i > 4 && STRIEQ(fname+i-4, ".wav"))
			{
				i -= 4;
			}
			strcpy(fname+i, ".wav");
		}
		filename = SaveDataFile(fname, leaf, size, m_ExtractAudioFiles);
	}

	// Now that we have determined the data type, dump the data to the XML file
	if (m_XmlFile)
	{
		Indent();
		switch (node_type)
		{
		case NODE_TYPE_TEXT :
			OpenNode("text");
			fprintf(m_XmlFile, (leaf[0] == ' ') ? "\"%.*s\"" : "%.*s", size, leaf);
			CloseNode();
			break;

		case NODE_TYPE_FILE :
			OpenNode("file", "filename=\"%s\"", filename);
			// Closes it, too
			break;

		case NODE_TYPE_BYTE :
			OpenNode("byte");
			fprintf(m_XmlFile, /*(!use_hex) ? "%d" :*/ "0x%X", *p.b);
			CloseNode();
			break;

		case NODE_TYPE_WORD_ARRAY :
			size /= sizeof(WORD);
			OpenNode("short array", "count=\"%d\"", size);
			if (array_wrap)
			{
				Indent(1);
			}
			for (i = 0; i < size; i++)
			{
				if (i)
				{
					fprintf(m_XmlFile, ",");
					if (array_wrap && i % array_wrap == 0)
					{
						Indent();
					}
					else
					{
						fprintf(m_XmlFile, " ");
					}
				}
				fprintf(m_XmlFile, "%*d", left, p.w[i]);
			}
			if (array_wrap)
			{
				Indent(-1);
			}
			CloseNode();
			break;

		case NODE_TYPE_DWORD :
			OpenNode("int");
			fprintf(m_XmlFile, (use_hex) ? "0x%X" : "%d", *p.d);
			CloseNode();
			break;

		case NODE_TYPE_DWORD_ARRAY :
			size /= sizeof(DWORD);
			OpenNode("int array", "count=\"%d\"", size);
			if (array_wrap)
			{
				Indent(1);
			}
			for (i = 0; i < size; i++)
			{
				if (i)
				{
					fprintf(m_XmlFile, ",");
					if (array_wrap && i % array_wrap == 0)
					{
						Indent();
					}
					else
					{
						fprintf(m_XmlFile, " ");
					}
				}
				/*if (use_hex)
				{
					fprintf(m_XmlFile, "0x%08X", p.d[i]);
				}
				else*/
				{
					fprintf(m_XmlFile, "%*d", left, p.d[i]);
				}
			}
			if (array_wrap)
			{
				Indent(-1);
			}
			CloseNode();
			break;

		case NODE_TYPE_FLOAT :
			OpenNode("float");
			fprintf(m_XmlFile, "%s", (use_deg) ? rtoa(*p.f) : ftoa(*p.f, 0));
			CloseNode();
			break;

		case NODE_TYPE_MATRIX :
		case NODE_TYPE_TRANSFORM :
			OpenNode((node_type == NODE_TYPE_MATRIX) ? "Matrix" : "Transform");
			if (m_OptionRotation)
			{
				Vector rot;
				Ornt_to_Rot(rot, *p.m);
				if (node_type == NODE_TYPE_TRANSFORM)
				{
					Indent(1);
					fprintf(m_XmlFile, "%s,", rtoa(rot));
					Indent();
					fprintf(m_XmlFile, "%s", ftoa(3, p.f+9, 0));
					Indent(-1);
				}
				else
				{
					fprintf(m_XmlFile, "%s", rtoa(rot));
				}
				CloseNode();
				break;
			}
			goto float_array;

		case NODE_TYPE_FLOAT_ARRAY :
			OpenNode("float array", "count=\"%d\"", size/sizeof(float));
		float_array:
			size /= sizeof(float);
			if (array_wrap)
			{
				Indent(1);
			}
			LPCSTR suffix;
			for (i = 0; i < size; i++)
			{
				if (i)
				{
					fprintf(m_XmlFile, "%s", suffix);
					if (array_wrap && i % array_wrap == 0)
					{
						Indent();
					}
					else
					{
						fprintf(m_XmlFile, " ");
					}
				}
				suffix = ",";
				if (BWC)
				{
					fprintf(m_XmlFile, "%.3f", p.f[i]);
				}
				else if (MADeltas && i % array_wrap == 0)
				{
					fprintf(m_XmlFile, "%s", ftoa(p.f[i], 3));
					suffix = ":";
				}
				else
				{
					fprintf(m_XmlFile, "%s", ftoa(p.f[i], left));
				}
			}
			if (array_wrap)
			{
				Indent(-1);
			}
			CloseNode();
			break;

		case NODE_TYPE_VECTOR :
			OpenNode("Vector");
			fprintf(m_XmlFile, "%s", ftoa(3, p.f, 0));
			CloseNode();
			break;

		case NODE_TYPE_RGB :
			// This contains red-green-blue color information
			// Each value must be between 0.0 and 1.0 (inclusive)
			OpenNode("RGB");
			fprintf(m_XmlFile, "%s", ftoc(p.f, m_OptionRGB));
			CloseNode();
			break;

		case NODE_TYPE_VMESHDATA :
			WriteVMeshData(leaf, size);
			break;

		case NODE_TYPE_VWIREDATA :
			WriteVWireData(leaf, size);
			break;

		case NODE_TYPE_VMESHREF :
			if (size != 60)
			{
				Log("   *** Warning: VMeshRef size = %u, expecting 60\n", size);
			}
			WriteVMeshRef(leaf);
			break;

		case NODE_TYPE_HEADER :
			WriteHeader(leaf);
			break;

		case NODE_TYPE_FRAMES :
			WriteFrames(leaf, size);
			break;

		case NODE_TYPE_FIX :
			WriteFix(leaf, size);
			break;

		case NODE_TYPE_PRIS :
		{
			// Bit of a kludge to prevent conversion to degrees.
			BOOL old_deg = m_OptionDegrees;
			m_OptionDegrees = FALSE;
			WriteRev(leaf, size);
			m_OptionDegrees = old_deg;
			break;
		}

		case NODE_TYPE_REV :
			WriteRev(leaf, size);
			break;

		case NODE_TYPE_CYL :
			WriteCyl(leaf, size);
			break;

		case NODE_TYPE_SPHERE :
			WriteSphere(leaf, size);
			break;

		case NODE_TYPE_FRAME_RECTS :
			size /= sizeof(FrameRects);
			OpenNode("Frame_rects", "count=\"%d\"", size);
			m_Depth++;
			for (i = 0; i < size; i++)
			{
				Indent();
				// 0 to 1 in quarters.
				fprintf(m_XmlFile, "%2u: %.2f, %.2f, %.2f, %.2f",
								   p.fr[i].frame, p.fr[i].rect[0], p.fr[i].rect[1], p.fr[i].rect[2], p.fr[i].rect[3]);
			}
			Indent(-1);
			CloseNode();
			break;

		case NODE_TYPE_ALEFFECTLIB :
			WriteALEffectLib(leaf);
			break;

		case NODE_TYPE_ALCHEMYNODELIBRARY :
			// Can only handle version 1.1, which they all are.
			if (*p.f != 1.1f)
			{
				Log("   *** Warning: AlchemyNodeLibrary version = %g, expecting 1.1\n", *p.f);
			}
			WriteAlchemyNodeLibrary(leaf);
			break;

		default :
			// Default type is a hexadecimal dump
			OpenNode("hex", "size=\"%d\"", size);
			Indent(1);
			for (i = 0; i < size; i++)
			{
				if (i && ((i % wrap) == 0))
				{
					Indent();
				}
				fprintf(m_XmlFile, "%02x", leaf[i]);
			}
			Indent(-1);
			CloseNode();
			break;
		}
	}
}


void UTFXMLDlg::Indent(int depth /* = 0 */)
{
	fprintf(m_XmlFile, "\n");
	m_Depth += depth;
	if (m_Depth)
	{
		fprintf(m_XmlFile, "%*c", m_Depth*3, ' ');
	}
}


char * UTFXMLDlg::SaveDataFile(LPCSTR filename, BYTE *data, DWORD size, int save_data)
{
	sprintf(fname, "%s\\%s", m_ExtractPath, filename);

	// Does the caller want to save the data?
	if (save_data)
	{
		char current_source_path[MAX_PATH];
		GetCurrentDirectory(sizeof(current_source_path), current_source_path);

		// Switch to the destination path
		SetCurrentDirectory(m_CurrentDestinationPath);
		// Make the extraction directory
		CreateDirectory(m_ExtractPath, NULL);

		FILE *f = fopen(fname, "wb");
		if (f)
		{
			fwrite(data, size, 1, f);
			fclose(f);
			// Log("   Created %s\n", fname);
			m_NumDataFilesExtracted++;
		}

		// Restore the current source path
		SetCurrentDirectory(current_source_path);

		// Note that we generated output in this folder
		m_GeneratedOutput = true;
	}

	return (fname);
}


void UTFXMLDlg::OpenXmlFile(LPCSTR filename)
{
	char xml_filename[MAX_PATH];
	const char *shortname = filename;

	const char *tmp = strrchr(filename, '\\');
	if (tmp)
	{
		shortname = tmp + 1;
	}

	sprintf(xml_filename, "%s\\%s.xml", m_CurrentDestinationPath, shortname);

	strcpy(m_ExtractPath, shortname);
	char *dot = strrchr(m_ExtractPath, '.');
	if (dot)
	{
		*dot = '_';
	}
	else
	{
		strcat(m_ExtractPath, "_");
	}

	// Try to open the XML file for write access
	m_XmlFile = fopen(xml_filename, "w");
	if (m_XmlFile)
	{
		char current_path[MAX_PATH];
		GetCurrentDirectory(sizeof(current_path), current_path);
		strcat(current_path, "\\");
		char *relative_path = current_path + m_PathTrim;
		fprintf(m_XmlFile, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
		fprintf(m_XmlFile, "<!-- %s -->\n", UTFXML_VERSION);
		fprintf(m_XmlFile, "<UTFXML filename=\"%s\"", filename);

		// If relative path is not an empty string
		if (*relative_path)
		{
			// Chop off trailing backslash
			if (relative_path[strlen(relative_path)-1] == '\\')
			{
				relative_path[strlen(relative_path)-1] = 0;
			}

			// Add the relative path to the XML tag
			fprintf(m_XmlFile, " path=\"%s\"", relative_path);
		}

		WriteUnk(1, m_Header.unk1);
		WriteUnk(2, m_Header.unk2);
		WriteUnk(3, m_Header.unk3);

		if (m_OptionTimestamps)
		{
			WriteTime(0, m_Header.timestamp);
		}

		fprintf(m_XmlFile, ">");

		m_NumXmlFilesCreated++;

		// Note that we generated output in this folder
		m_GeneratedOutput = true;
	}
}


void UTFXMLDlg::CloseXmlFile()
{
	// Is the XML file open?
	if (m_XmlFile)
	{
		// Yes, write the final record and close the XML file
		fprintf(m_XmlFile, "</UTFXML>\n");
		fclose(m_XmlFile);
		m_XmlFile = NULL;
	}
}


char * UTFXMLDlg::OpenIncludeFile(LPCSTR filename, LPCSTR xml_name)
{
	sprintf(fname, "%s\\%s.xml", m_ExtractPath, filename);

	if (!m_ExtractIncludeFiles)
	{
		m_XmlFile = NULL;
		return fname;
	}

	char current_source_path[MAX_PATH];
	GetCurrentDirectory(sizeof(current_source_path), current_source_path);

	// Switch to the destination path
	SetCurrentDirectory(m_CurrentDestinationPath);
	// Make the extraction directory
	CreateDirectory(m_ExtractPath, NULL);

	m_XmlFile = fopen(fname, "w");
	if (m_XmlFile)
	{
		fprintf(m_XmlFile, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
		fprintf(m_XmlFile, "<!-- %s -->\n", UTFXML_VERSION);
		fprintf(m_XmlFile, "<%s>", xml_name);
		m_Depth = 0;
	}

	SetCurrentDirectory(current_source_path);

	return fname;
}


void UTFXMLDlg::CloseIncludeFile(LPCSTR xml_name)
{
	if (m_XmlFile)
	{
		fprintf(m_XmlFile, "\n</%s>\n", xml_name);
		fclose(m_XmlFile);
		m_NumXmlFilesCreated++;
	}
}


void UTFXMLDlg::OpenNode(LPCSTR type, LPCSTR attr /* = NULL */, ...)
{
	fprintf(m_XmlFile, "<%s type=\"%s\"", m_xml_tag, type);
	if (attr)
	{
		fprintf(m_XmlFile, " ");
		va_list args;
		va_start(args, attr);
		vfprintf(m_XmlFile, attr, args);
		va_end(args);
	}
	WriteNodeUnk(m_Node);
	if (STREQ(type, "file"))
	{
		fprintf(m_XmlFile, "/>");
	}
	else
	{
		fprintf(m_XmlFile, ">");
	}
}


void UTFXMLDlg::CloseNode()
{
	fprintf(m_XmlFile, "</%s>", m_xml_name);
}


void UTFXMLDlg::WriteTime(int index, const FILETIME &ft)
{
	char idx[4];
	if (index)
		sprintf(idx, "%d", index);
	else
		*idx = '\0';
	SYSTEMTIME st;
	FileTimeToSystemTime(&ft, &st);
	fprintf(m_XmlFile, " time%s=\"%d-%02d-%02d %02d:%02d:%02d\"", idx,
						st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}


void UTFXMLDlg::WriteTime(int index, DWORD dt)
{
	FILETIME ft;
	DosDateTimeToFileTime((WORD) dt, (WORD) (dt >> 16), &ft);
	WriteTime(index, ft);
}


void UTFXMLDlg::WriteUnk(int index, DWORD value)
{
	// Don't print "unk" if the value is zero
	if (value)
	{
		fprintf(m_XmlFile, " unk%d=\"0x%08X\"", index, value);
	}
}


void UTFXMLDlg::WriteNodeUnk(UTFNode *node)
{
	WriteUnk(1, node->unk1);

	if (!m_OptionTimestamps)
	{
		return;
	}
	if ((node->time1 == node->time2) &&
		(node->time2 == node->time3) &&
		(node->time1 != m_time1))
	{
		// All three are the same, so just write one attribute for all
		WriteTime(0, node->time1);
	}
	else
	{
		// Write each of the three unknown values as XML attributes
		if (node->time1 != m_time1) WriteTime(1, node->time1);
		if (node->time2 != m_time2) WriteTime(2, node->time2);
		if (node->time3 != m_time3) WriteTime(3, node->time3);
	}
}


// Search the immediate children of ROOT for NAME.
UTFNode *UTFXMLDlg::FindNode(UTFNode *root, LPCSTR name)
{
	if (root == NULL || !(root->flags & BRANCH) || !root->branch)
		return NULL;

	UTFNode *node = NODE(root->branch);
	while (true)
	{
		if (STRIEQ(name, NAME(node->name)))
			return node;

		if (!node->next)
			return NULL;

		node = NODE(node->next);
	}
}


// Comparison function for the set - compare the strings, not the pointers.
bool std::less<LPCSTR>::operator()(const LPCSTR &s1, const LPCSTR &s2) const
{
	return (stricmp(s1, s2) < 0);
}


//////////////////////////////////////////////////////////////////////
// End UTFXMLDlg.cpp
//////////////////////////////////////////////////////////////////////
