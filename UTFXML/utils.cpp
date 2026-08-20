//////////////////////////////////////////////////////////////////////
// Utility functions.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UTFXML.h"
#include "UTFXMLDlg.h"
#include <cmath>


// Display a number with ten decimal places, but using eight significant
// digits.	Small or large values will use engineering notation (the exponent
// is a multiple of 3).
char *ftoa(float num, int left /* = -2 */)
{
	// Separate rotating buffers for each thread.
	static thread_local char buf[9][32] = {};
	static thread_local int idx = 0;

	// Defensive protection against corrupted index.
	if (idx < 0 || idx >= 9)
		idx = 0;

	char *rc = buf[idx];

	idx = (idx + 1) % 9;

	bool neg = false;
	bool fix = false;
	bool sgn = false;

	if (left)
	{
		if (left < 0)
		{
			sgn = true;
			left = -left;
		}

		if (left < 2)
			left = 2;
		else if (left > 6)
			left = 6;

		left += 12;
		fix = true;
	}

	// Some VMeshData texture coordinates don't really look like floats,
	// so use hex instead.
	BYTE hi = (reinterpret_cast<BYTE*>(&num))[3] & 0x7F;

	if ((hi == 0 && num) || hi == 0x7F)
	{
		if (fix)
		{
			left -= 10;

			if (left > 0)
				memset(rc, ' ', left);
		}

		sprintf_s(
			rc + ((left > 0) ? left : 0),
			32 - ((left > 0) ? left : 0),
			"0x%08X",
			*reinterpret_cast<DWORD*>(&num)
		);

		return rc;
	}

	if (num < 0)
	{
		num = -num;
		neg = true;
	}

	if (num && (num < 1e-3f || num >= ((fix) ? 1e6f : 1e8f)))
	{
		int exp = static_cast<int>(floor(log10(num)));

		int adj = exp % 3;

		if (adj < 0)
			adj += 3;

		exp -= adj;

		double n = num / pow(10.0, exp);

		if (n > 999.99999)
		{
			n /= 1000;
			exp += 3;
		}

		if (neg)
			n = -n;

		if (fix)
		{
			sprintf_s(
				rc,
				32,
				(sgn) ? "% #*.8ge%+.2d" : "%#*.8ge%+.2d",
				left - 4,
				n,
				exp
			);
		}
		else
		{
			sprintf_s(
				rc,
				32,
				"%.8ge%d",
				n,
				exp
			);
		}
	}
	else
	{
		bool round = (num < 100);

		if (neg)
			num = -num;

		if (fix)
		{
			char temp[32] = {};

			int written = sprintf_s(
				temp,
				sizeof(temp),
				(sgn) ? "% #.8g" : "%#.8g",
				num
			);

			if (written < 0)
			{
				rc[0] = '\0';
				return rc;
			}

			char *z = temp + written;

			if (round && written >= 4)
			{
				if (STRBEG(z - 4, "999") ||
					STRBEG(z - 4, "000"))
				{
					written = sprintf_s(
						temp,
						sizeof(temp),
						(sgn) ? "% #g" : "%#g",
						num
					);

					if (written < 0)
					{
						rc[0] = '\0';
						return rc;
					}

					z = temp + written;
				}
			}

			char *decimal = strchr(temp, '.');

			if (decimal)
			{
				char *p = decimal + 1 + 10;
				char *end = temp + sizeof(temp) - 1;

				if (p > end)
					p = end;

				while (z < p)
					*z++ = '0';

				*z = '\0';
			}

			sprintf_s(
				rc,
				32,
				"%*s",
				left,
				temp
			);

			if (strstr(rc, "NAN"))
				return rc;
		}
		else
		{
			int len = sprintf_s(
				rc,
				32,
				"%.8g",
				num
			);

			if (len >= 9 && round)
			{
				if (STRBEG(rc + len - 4, "999") ||
					STRBEG(rc + len - 4, "000"))
				{
					sprintf_s(
						rc,
						32,
						"%g",
						num
					);
				}
			}
		}
	}

	return rc;
}


char *ftoa(int count, const float *array, int left /* = -2 */)
{
	static char buf[6*20];		// bounding box used to have six values

	if (count < 1 || count > 6)
	{
		return "Oops!";
	}

	char *s = buf;
	while (count--)
	{
		s += sprintf(s, "%s", ftoa(*array++, left));
		if (count)
		{
			s += sprintf(s, ", ");
		}
	}

	return buf;
}


// Convert a decimal color value (0.0 to 1.0) to an integer (0 to 255).
DWORD ftoc(float c)
{
	int i = int(c * 255);
	if (i < 0)
	{
		i = 0;
	}
	else if (i > 255)
	{
		i = 255;
	}
	return i;
}


// Convert a decimal color to integer or hex.
char *ftoc(float *col, UINT flag)
{
	if (flag == BST_UNCHECKED)
	{
		return ftoa(3, col, 0);
	}
	static char buf[16];
	sprintf(buf, (flag == BST_CHECKED) ? "%d, %d, %d" : "#%02X%02X%02X", 
				 ftoc(col[0]), ftoc(col[1]), ftoc(col[2]));
	return buf;
}


// Convert radians to a string in degrees, with at most two decimals.
char *rtoa(float r, bool symbol /* = true */, bool align /* = false */)
{
	static char buf[3][12];	// "-999999.99�"
	static int  idx;

	char *rc = buf[idx];
	if (++idx == 3)
	{
		idx = 0;
	}

	float d = float(r * 180 / 3.1415926535897932);

	if (d < 0)
	{
		// Avoid -0
		if (d > -0.005f)
		{
			d = 0;
		}
		// Avoid -180 (the reverse process just generates 180, anyway)
		else if (!symbol && d <= -179.995f)
		{
			d = 180;
		}
	}

	int len = sprintf(rc, "%7.2f", d);

	if (!align)
	{
		// Remove trailing zeros.
		while (rc[--len] == '0') ;
		if (rc[len] != '.')
		{
			++len;
		}
		if (symbol)
		{
			rc[len++] = '�';
		}
		rc[len] = '\0';
		// Remove leading spaces.
		while (*rc == ' ')
			++rc;
	}
	else if (symbol)
	{
		rc[len++] = '�';
		rc[len] = '\0';
	}

	return rc;
}


// Convert a rotation vector to a string.
char *rtoa(const Vector &rot)
{
	static char buf[2+7+2+7+2+7+1];

	sprintf(buf, "R %s, %s, %s", 
				 rtoa(rot.x, false), rtoa(rot.y, false), rtoa(rot.z, false));

	return buf;
}


// Convert a quaternion to an angle (in degrees) and axis vector.
// See http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
char *qtoa(const float *q, bool align)
{
	static char buf[2+7+3*(2+13)+1];

	if (q[0] > 1.0f || q[0] < -1.0f)
		return ftoa(4, q);

	float angle = 2 * acosf(q[0]);
	double s = sqrt(1 - q[0] * q[0]);
	Vector axis;
	axis.x = q[1];
	axis.y = q[2];
	axis.z = q[3];
	if (s != 0)
	{
		axis.x /= s;
		axis.y /= s;
		axis.z /= s;
	}

	sprintf(buf, "Q %s, %s",
				 rtoa(angle, false, align), vtoa(axis));

	return buf;
}


// Convert an orientation matrix to a rotation vector (pitch/yaw/roll).
// This is what Freelancer does when saving a game.
void Ornt_to_Rot(Vector &rot, const Matrix &ornt)
{
	float h = (float) hypot(ornt.i.x, ornt.j.x);
	if (h > 1/524288.0f)
	{
		rot.x = atan2f(ornt.k.y, ornt.k.z);
		rot.y = atan2f(-ornt.k.x, h);
		rot.z = atan2f(ornt.j.x, ornt.i.x);
	}
	else
	{
		rot.x = atan2f(-ornt.j.z, ornt.j.y);
		rot.y = atan2f(-ornt.k.x, h);
		rot.z = 0;
	}
}


// Case insensitive version of strstr.	(Inefficient, but it's only used twice.)
char *stristr(LPCSTR s1, LPCSTR s2)
{
	char *i1 = strdup(s1);
	char *i2 = strdup(s2);
	strlwr(i1);
	strlwr(i2);
	char *rc = strstr(i1, i2);
	if (rc)
	{
		rc += s1 - i1;
	}
	free(i2);
	free(i1);
	return rc;
}
