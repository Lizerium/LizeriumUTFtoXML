//////////////////////////////////////////////////////////////////////
// UTFXMLDlg.h
//
// Refer to UTFXMLDlg.cpp for software documentation
//////////////////////////////////////////////////////////////////////

#ifndef _UTF_XML_DIALOG_H_INCLUDED_
#define _UTF_XML_DIALOG_H_INCLUDED_

#include <map>
#include <set>


//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

#define FREELANCER_KEYPATH "SOFTWARE\\Microsoft\\Microsoft Games\\Freelancer\\1.0"

#define LEAF   (1<<7)
#define BRANCH (1<<4)

#define STREQ(s1, s2)  (strcmp(s1, s2) == 0)
#define STRIEQ(s1, s2) (stricmp(s1, s2) == 0)

// This only works with static strings!
#define STRBEG(s1, s2)	(strncmp(s1, s2, sizeof(s2)-1) == 0)
#define STRIBEG(s1, s2) (strnicmp(s1, s2, sizeof(s2)-1) == 0)


//////////////////////////////////////////////////////////////////////
// Data Structures
//////////////////////////////////////////////////////////////////////

struct UTFHeader
{
    char UTF[4];            // "UTF "
    DWORD version;          // 0x04
    DWORD tree_offset;      // 0x08
    DWORD tree_size;        // 0x0c
    DWORD unk1;             // 0x10
    DWORD node_size;		// 0x14
    DWORD string_offset;    // 0x18
    DWORD string_alloc;     // 0x1c
    DWORD string_size;      // 0x20
    DWORD data_offset;      // 0x24
    DWORD unk2;             // 0x28
    DWORD unk3;             // 0x2c
    FILETIME timestamp;     // 0x30
};							// size=0x38

struct UTFNode
{
    DWORD next;             // 0x00 : next node on same level (sibling)
    DWORD name;             // 0x04 : string for this node
    DWORD flags;            // 0x08 : bit 4 set = branch, bit 7 set = leaf
    DWORD unk1;             // 0x0c
    DWORD branch;           // 0x10 : offset to branch node (child), offset to data if leaf
    DWORD alloc_size;       // 0x14 : leaf node only, 0 for branch
    DWORD size1;            // 0x18 : leaf node only, 0 for branch
    DWORD size2;            // 0x1c : leaf node only, 0 for branch
    DWORD time1;            // 0x20
    DWORD time2;            // 0x24
    DWORD time3;            // 0x28
};							// size=0x2c

struct VMeshData
{
	DWORD mesh_type;
	DWORD surf_type;
	 WORD mesh_count;
	 WORD index_count;
	 WORD FVF;
	 WORD vertex_count;
};

struct VWireData
{
	DWORD size;				// 16
	DWORD vmesh_hash;		// crc of vms name
	 WORD base;
	 WORD used;
	 WORD count;
	 WORD span;
};

struct VMeshRef
{
    DWORD header_size;      // 60
    DWORD vmesh_hash;       // crc of 3db name
    WORD  start_vert;
    WORD  end_vert;
    WORD  start_vert_ref;
    WORD  end_vert_ref;
    WORD  mesh_no;
    WORD  no_of_meshes;
    float bounding_box[6];
    float center[3];
    float radius;
};

struct Header
{
	DWORD count;
	float interval;
	DWORD flags;
};

struct Vector
{
	float x, y, z;
};

struct Matrix
{
	Vector i, j, k;
};

// \Cmpnd\Cons\{Fix,Trans,Loose}	// Trans isn't in any of the vanilla files
struct FIX
{
  char	 parent[64];
  char	 child[64];
  Vector position;
  Matrix orientation;
};

// \Cmpnd\Cons\{Pris,Rev}
struct REV
{
  char	 parent[64];
  char	 child[64];
  Vector position;
  Vector offset;
  Matrix orientation;
  Vector axis;
  float  minmax[2]; 			// don't seem to be used
};

// \Cmpnd\Cons\Cyl
struct CYL						// not in any of the vanilla files
{
  char	 parent[64];
  char	 child[64];
  Vector position;
  Vector offset;
  Matrix orientation;
  Vector axis;
  float  minmax[4]; 			// don't seem to be used
};

// \Cmpnd\Cons\Sphere
struct SPHERE
{
  char	 parent[64];
  char	 child[64];
  Vector position;
  Vector offset;
  Matrix orientation;
  float  minmax[6]; 			// don't seem to be used
};

// \Material Library\*\Frame rects
struct FrameRects
{
	DWORD frame;
	float rect[4];
};

// \ALEffectLib\ALEffectLib
struct ALEffectLib
{
	DWORD flag;
	DWORD CRC;
	DWORD parent;
	DWORD index;
};

union Ptrs
{
	BYTE		*b;
	char		*c;
	short		*s;
	WORD		*w;
	DWORD		*d;
	float		*f;
	int 		*i;
	Matrix		*m;
	VMeshData	*vmd;
	VWireData	*vwd;
	FrameRects	*fr;
	ALEffectLib *ael;
};


// Create a set of Rev parts.
typedef std::set<LPCSTR> SetName;

// Map CRCs to their names.
typedef std::map<DWORD, LPCSTR> MapCRCName;
typedef MapCRCName::const_iterator CIterCRCName;
extern MapCRCName fx_crc, node_crc;
extern MapCRCName mat_crc;
extern MapCRCName mesh_crc;
extern MapCRCName msg_hash;
extern MapCRCName ale_type;

DWORD  fl_crc32_b(const BYTE *data, DWORD size);
DWORD  fl_crc32(LPCSTR string);
void   init_fx_crc();
void   Init_Ale_Types();
LPCSTR crc_name(const MapCRCName &crcmap, DWORD crc);

char *ftoa(float num, int left = -2);
char *ftoa(int count, const float *array, int left = -2);
#define vtoal(v, l) ftoa(3, (float *) &(v), l)
#define vtoa(v) vtoal(v, -2)
DWORD ftoc(float col);
char *ftoc(float *col, UINT flag);
char *rtoa(float r, bool symbol = true, bool align = false);
char *rtoa(const Vector &rot);
char *qtoa(const float *quat, bool align);

void Ornt_to_Rot(Vector &rot, const Matrix &ornt);

char *stristr(LPCSTR s1, LPCSTR s2);


//////////////////////////////////////////////////////////////////////
// UTFXMLDlg class definition
//////////////////////////////////////////////////////////////////////

class UTFXMLDlg : public CDialog
{
// Construction
public:
	UTFXMLDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(UTFXMLDlg)
	enum { IDD = IDD_UTFXML_DIALOG };
	CComboBox	m_UtfFilenamesComboBox;
	CString	m_SourcePath;
	CString	m_DestinationPath;
	CString	m_UtfFilenames;
	CString m_StringsList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UTFXMLDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(UTFXMLDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConvert();
	afx_msg void OnBrowseUtfFilename();
	afx_msg void OnGenerate();
	afx_msg void OnHelpButton();
	afx_msg void OnSelchangeUtfFilenames();
	afx_msg void OnOptionRgb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    UINT   RunBackgroundConversionThread();

private:
	void   CreateAndSetPath(LPCSTR path);
    void   Log(LPCSTR format, ...);
    bool   ProcessFolder(LPCSTR folder);
    bool   IsFileTypeSelected(LPCSTR filename);
    void   ProcessUTFFile(LPCSTR filename);
	void   ProcessUTFNode(UTFNode *node);
	void   ProcessLeaf(LPCSTR name, BYTE *leaf, DWORD size);
	char * OpenIncludeFile(LPCSTR filename, LPCSTR xml_name);
	void   CloseIncludeFile(LPCSTR xml_name);
	char * SaveDataFile(LPCSTR filename, BYTE *data, DWORD size, int save_data);
    void   OpenXmlFile(LPCSTR filename);
    void   CloseXmlFile();
	void   Indent(int depth = 0);
	void   OpenNode(LPCSTR type, LPCSTR attr = NULL, ...);
	void   CloseNode();
	void   WriteTime(int index, const FILETIME &ft);
	void   WriteTime(int index, DWORD dt);
    void   WriteUnk(int index, DWORD value);
	void   WriteNodeUnk(UTFNode *node);
	void   WriteALEffectLib(BYTE *leaf);
	void   WriteAlchemyNodeLibrary(BYTE *leaf);
	BYTE * do_Single(BYTE *leaf);
	void   ExtractAlchemyNodes(BYTE *leaf);
	BYTE * Skip_Single(BYTE *leaf);
	void   WriteHeader(BYTE *leaf);
	void   WriteFrames(BYTE *leaf, DWORD size);
	void   WriteVMeshData(BYTE *leaf, DWORD size);
	void   WriteVWireData(BYTE *leaf, DWORD size);
	void   WriteVMeshRef(BYTE *leaf);
	void   WriteFix(BYTE *leaf, DWORD size);
	void   WriteRev(BYTE *leaf, DWORD size);
	void   WriteCyl(BYTE *leaf, DWORD size);
	void   WriteSphere(BYTE *leaf, DWORD size);
	void   WriteConsOpen(const void *cons, bool has_ofs);
	void   WriteConsOrient(const void *orient, bool has_axis);
	void   WriteConsMinMax(const float *minmax, int count);
	void   WriteConsClose();
	UTFNode *FindNode(UTFNode *root, LPCSTR name);

public:
    bool   m_ConversionAborted;
    bool   m_ConversionInProgress;
    bool   m_CountingUtfFiles;
	bool   m_FindingMaterials;
    int    m_PathTrim;
    CRITICAL_SECTION m_Mutex;
    // The following public attributes are protected by the mutex
    int    m_UtfFileCount;
    int    m_NumUtfFilesOpened;
    int    m_NumXmlFilesCreated;
    int    m_NumDataFilesExtracted;
    char   m_CurrentSourcePath[MAX_PATH];
    // end of public attributes protected by the mutex

private:
    BOOL   m_Recursive;
    BOOL   m_GenerateXmlFiles;
    BOOL   m_CreateSubfolders;
    BOOL   m_SkipAudioFolder;
    BOOL   m_SkipBasesFolder;
    BOOL   m_SkipCharactersFolder;
    BOOL   m_SkipFxFolder;
    BOOL   m_ExtractAudioFiles;
    BOOL   m_ExtractTextureFiles;
    BOOL   m_ExtractIncludeFiles;
	BOOL   m_OptionTimestamps;
	BOOL   m_OptionDegrees;
	BOOL   m_OptionRotation;
	UINT   m_OptionRGB;
    char   m_CurrentDestinationPath[MAX_PATH];
	char   m_ExtractPath[MAX_PATH];
    BYTE * m_Tree;
	char * m_String;
    BYTE * m_Data;
    int    m_Depth;
    char * m_vms_file;
    char * m_tga_file;
    char * m_bmp_file;
    const char* m_3db_file;
	bool   m_is_anm;
	bool   m_is_dfm;
	DWORD  m_time1, m_time2, m_time3;
	Header * m_AnimHeader;
    FILE * m_LogFile;
    FILE * m_XmlFile;
    bool   m_TextureLibrary;
	bool   m_VMeshLibrary;
	bool   m_Animation;
    bool   m_GeneratedOutput;
	bool   m_Quiet;
	UTFHeader m_Header;
	UTFNode *m_Node;
	char * m_xml_tag;
	char * m_xml_name;
	SetName m_rev_parts;
	bool   m_is_rev;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif _UTF_XML_DIALOG_H_INCLUDED_

