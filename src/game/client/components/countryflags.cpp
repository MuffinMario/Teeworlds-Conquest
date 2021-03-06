/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/math.h>
#include <base/system.h>

#include <engine/console.h>
#include <engine/graphics.h>
#include <engine/storage.h>
#include <engine/textrender.h>
#include <engine/external/json-parser/json.h>
#include <engine/shared/config.h>

#include "countryflags.h"


void CCountryFlags::LoadCountryflagsIndexfile()
{
	// read file data into buffer
	const char *pFilename = "countryflags/index.json";
	IOHANDLE File = Storage()->OpenFile(pFilename, IOFLAG_READ, IStorage::TYPE_ALL);
	if(!File)
	{
		Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "countryflags", "couldn't open index file");
		return;
	}
	int FileSize = (int)io_length(File);
	char *pFileData = (char *)mem_alloc(FileSize, 1);
	io_read(File, pFileData, FileSize);
	io_close(File);

	// parse json data
	json_settings JsonSettings;
	mem_zero(&JsonSettings, sizeof(JsonSettings));
	char aError[256];
	json_value *pJsonData = json_parse_ex(&JsonSettings, pFileData, FileSize, aError);
	mem_free(pFileData);

	if(pJsonData == 0)
	{
		Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, pFilename, aError);
		return;
	}

	// extract data
	const json_value &rInit = (*pJsonData)["country codes"];
	if(rInit.type == json_object)
	{
		enum
		{
			NUM_INDICES = 2,
		};
		const char *paIndices[NUM_INDICES] = {"custom", "ISO 3166-1"};
		for(int Index = 0; Index < NUM_INDICES; ++Index)
		{
			const json_value &rStart = rInit[(const char *)paIndices[Index]];
			if(rStart.type == json_array)
			{
				for(unsigned i = 0; i < rStart.u.array.length; ++i)
				{
					char aBuf[64];

					// validate country code
					int CountryCode = (json_int_t)rStart[i]["code"];
					if(CountryCode < CODE_LB || CountryCode > CODE_UB)
					{
						str_format(aBuf, sizeof(aBuf), "country code '%i' not within valid code range [%i..%i]", CountryCode, CODE_LB, CODE_UB);
						Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "countryflags", aBuf);
						continue;
					}

					// add entry
					const char *pCountryName = rStart[i]["id"];
					CCountryFlag CountryFlag;
					CountryFlag.m_CountryCode = CountryCode;
					str_copy(CountryFlag.m_aCountryCodeString, pCountryName, sizeof(CountryFlag.m_aCountryCodeString));
					if(g_Config.m_ClLoadCountryFlags)
					{
						// load the graphic file
						CImageInfo Info;
						str_format(aBuf, sizeof(aBuf), "countryflags/%s.png", pCountryName);
						if(!Graphics()->LoadPNG(&Info, aBuf, IStorage::TYPE_ALL))
						{
							char aMsg[64];
							str_format(aMsg, sizeof(aMsg), "failed to load '%s'", aBuf);
							Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "countryflags", aMsg);
							continue;
						}
						CountryFlag.m_Texture = Graphics()->LoadTextureRaw(Info.m_Width, Info.m_Height, Info.m_Format, Info.m_pData, Info.m_Format, 0);
						mem_free(Info.m_pData);
					}
					// blocked?
					CountryFlag.m_Blocked = false;
					const json_value Check = rStart[i]["blocked"];
					if(Check.type == json_boolean && Check)
						CountryFlag.m_Blocked = true;
					m_aCountryFlags.add_unsorted(CountryFlag);

					// print message
					if(g_Config.m_Debug)
					{
						str_format(aBuf, sizeof(aBuf), "loaded country flag '%s'", pCountryName);
						Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "countryflags", aBuf);
					}
				}
			}
		}
	}

	// clean up
	json_value_free(pJsonData);
	m_aCountryFlags.sort_range();

	// find index of default item
	int DefaultIndex = 0, Index = 0;
	for(sorted_array<CCountryFlag>::range r = m_aCountryFlags.all(); !r.empty(); r.pop_front(), ++Index)
		if(r.front().m_CountryCode == -1)
		{
			DefaultIndex = Index;
			break;
		}

	// init LUT
	if(DefaultIndex != 0)
		for(int i = 0; i < CODE_RANGE; ++i)
			m_CodeIndexLUT[i] = DefaultIndex;
	else
		mem_zero(m_CodeIndexLUT, sizeof(m_CodeIndexLUT));
	for(int i = 0; i < m_aCountryFlags.size(); ++i)
		m_CodeIndexLUT[max(0, (m_aCountryFlags[i].m_CountryCode-CODE_LB)%CODE_RANGE)] = i;
}

void CCountryFlags::OnInit()
{
	// load country flags
	m_aCountryFlags.clear();
	LoadCountryflagsIndexfile();
	if(!m_aCountryFlags.size())
	{
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "countryflags", "failed to load country flags. folder='countryflags/'");
		CCountryFlag DummyEntry;
		DummyEntry.m_CountryCode = -1;
		DummyEntry.m_Blocked = false;
		mem_zero(DummyEntry.m_aCountryCodeString, sizeof(DummyEntry.m_aCountryCodeString));
		m_aCountryFlags.add(DummyEntry);
	}
}

int CCountryFlags::Num() const
{
	return m_aCountryFlags.size();
}

const CCountryFlags::CCountryFlag *CCountryFlags::GetByCountryCode(int CountryCode) const
{
	return GetByIndex(m_CodeIndexLUT[max(0, (CountryCode-CODE_LB)%CODE_RANGE)]);
}

const CCountryFlags::CCountryFlag *CCountryFlags::GetByIndex(int Index) const
{
	return &m_aCountryFlags[max(0, Index%m_aCountryFlags.size())];
}

void CCountryFlags::Render(int CountryCode, const vec4 *pColor, float x, float y, float w, float h, bool AllowBlocked)
{
	const CCountryFlag *pFlag = GetByCountryCode(CountryCode);
	if(!AllowBlocked && pFlag->m_Blocked)
		pFlag = GetByCountryCode(-1);
	if(pFlag->m_Texture.IsValid())
	{
		Graphics()->TextureSet(pFlag->m_Texture);
		Graphics()->QuadsBegin();
		Graphics()->SetColor(pColor->r*pColor->a, pColor->g*pColor->a, pColor->b*pColor->a, pColor->a);
		IGraphics::CQuadItem QuadItem(x, y, w, h);
		Graphics()->QuadsDrawTL(&QuadItem, 1);
		Graphics()->QuadsEnd();
	}
	else
	{
		CTextCursor Cursor;
		TextRender()->SetCursor(&Cursor, x, y, 10.0f, TEXTFLAG_RENDER|TEXTFLAG_STOP_AT_END);
		Cursor.m_LineWidth = w;
		TextRender()->TextEx(&Cursor, pFlag->m_aCountryCodeString, -1);
	}
}
