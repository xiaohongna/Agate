#pragma once
#include <vector>
#include "ParticlesParameter.h"
#include <bitserializer/bit_serializer.h>
#include <bitserializer/rapidjson_archive.h>
#include <rapidjson/document.h>

using namespace BitSerializer;
using JsonArchive = BitSerializer::Json::RapidJson::JsonArchive;

namespace agate
{

    class ProgramDocument
    {
    public:
        ProgramDocument() :_Y{}, _X{}
        {
            _Parameter.particleCount = 100;
            _Parameter.infinite = true;
            _Parameter.generateInterval.min = 10;
            _Parameter.generateInterval.max = 50;
            rapidjson::Document document;
            auto v = document["1"].GetObject();
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
            archive << MakeAutoKeyValue("paramter", _Parameter);
        }

        void SavetoFile()
        {
            BitSerializer::SaveObjectToFile<JsonArchive>(*this, LR"(D:\test_obj.json)");
           // BitSerializer::LoadObjectFromFile<JsonArchive>()
        }

        void LoadFromFile()
        {
            BitSerializer::LoadObjectFromFile<JsonArchive>(*this, LR"(D:\test_obj.json)");
        }

        
    private:
        int _Y;
        int _X;
        std::wstring _Name;
        ParticleParameter _Parameter;
    };
}

