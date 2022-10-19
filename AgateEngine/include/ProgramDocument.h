#pragma once
#include <vector>
#include <bitserializer/bit_serializer.h>
#include <bitserializer/rapidjson_archive.h>

using namespace BitSerializer;
using JsonArchive = BitSerializer::Json::RapidJson::JsonArchive;

class ProgramDocument
{
public:
    ProgramDocument():_Y{}, _X{}
    {

    }

    void SetValue(int x, int y)
    {
        _X = x;
        _Y = y;
    }

	template <class TArchive>
	void Serialize(TArchive& archive)
	{
        archive << MakeAutoKeyValue("y", _Y);
        archive << MakeAutoKeyValue("x", _X);
        /*
        if constexpr (TArchive::IsLoading()) {

        }
        else {
            const int y = testThirdPartyClass.GetY();
            archive << MakeAutoKeyValue("y", y);
        }
        */
	}

    void SavetoFile()
    {
        BitSerializer::SaveObjectToFile<JsonArchive>(*this, "D:\test_obj.json");
    }
private:
    int _Y;
    int _X;
    std::wstring _Name;
};

