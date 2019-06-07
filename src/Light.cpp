
#include "Light.hpp"
#include <string>
#include "imgui/imgui.h"

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
}

Vector LightManager::GetAmbient() const
{
    return Vector(mLightBuffer.GlobalAmbient);
}

void LightManager::SetAmbient(Vector col)
{
    mLightBuffer.GlobalAmbient = col.ToFloat4();
}

Light* LightManager::GetLight(unsigned char i)
{
    return &mLightBuffer.Lights[i];
}

LightBufferType * LightManager::GetLightBuffer()
{
	return &mLightBuffer;
}

void LightManager::InitModels(ID3D11Device* device)
{
	for (int i = 0; i < LIGHT_NO; ++i)
	{
		mLastType[i] = mLightBuffer.Lights[i].LightType;
		Vector vec(0, -1, 0);
		float cos = Vector::Dot3(vec, mLightBuffer.Lights[i].Direction);
		float sin = sqrt(1.f - (cos*cos));
		Vector axis = Vector::Cross3(mLightBuffer.Lights[i].Direction, vec);
		float angle = atan(sin / cos);

		switch (mLastType[i])
		{
		case LightType::PointLight:
			mLightModels[i] = std::make_unique<Model>();
			mLightModels[i]->LoadIcoSphere(1.f, 2);
			mLightModels[i]->Init(device);
			mLightModels[i]->SetMaterial(std::make_shared<MaterialProperties>());
			mLightModels[i]->SetPosition(mLightBuffer.Lights[i].Position);
			mLightModels[i]->GetMaterial()->mMaterial.mEmissive = mLightBuffer.Lights[i].Color;
			break;

		case LightType::SpotLight:
			mLightModels[i] = std::make_unique<Model>();
			mLightModels[i]->LoadPyramid({ 1.f, 1.f, 1.f });
			mLightModels[i]->Init(device);
			mLightModels[i]->SetMaterial(std::make_shared<MaterialProperties>());
			mLightModels[i]->SetPosition(mLightBuffer.Lights[i].Position);
			mLightModels[i]->GetMaterial()->mMaterial.mEmissive = mLightBuffer.Lights[i].Color;
			mLightModels[i]->SetRotation(Matrix::MakeRotationNormal(axis, angle));
			break;

		default:
			mLightModels[i].reset();
			break;
		}
	}
}

std::array<std::unique_ptr<Model>, LIGHT_NO>* LightManager::GetModels()
{
	return &mLightModels;
}

Model * LightManager::GetModel(int i)
{
	return mLightModels[i].get();
}


void LightManager::RenderUI(ID3D11Device* device)
{
    for (int i = 0; i < LIGHT_NO; ++i)
    {
        if (!ImGui::CollapsingHeader((std::string("Light ") + std::to_string(i)).c_str())) {
			ImGui::SameLine();
			ImGui::Checkbox((std::string("##") + std::to_string(i)).c_str(), reinterpret_cast<bool*>(&mLightBuffer.Lights[i].Enabled));
            continue;
        }
		
		ImGui::Separator();
		ImGui::Text((std::string("Light ") + std::to_string(i)).c_str());
        ImGui::Checkbox((std::string("Enable##") + std::to_string(i)).c_str(), reinterpret_cast<bool*>(&mLightBuffer.Lights[i].Enabled));
		ImGui::SameLine();
		ImGui::ColorEdit4((std::string("Color##") + std::to_string(i)).c_str(), reinterpret_cast<float*>(&mLightBuffer.Lights[i].Color));

		ImGui::Text("Light type:");
		ImGui::RadioButton((std::string("Directional##") + std::to_string(i)).c_str(), &mLightBuffer.Lights[i].LightType, LightType::DirectionalLight); ImGui::SameLine();
		ImGui::RadioButton((std::string("Point##") + std::to_string(i)).c_str(), &mLightBuffer.Lights[i].LightType, LightType::PointLight); ImGui::SameLine();
		ImGui::RadioButton((std::string("Spot##") + std::to_string(i)).c_str(), &mLightBuffer.Lights[i].LightType, LightType::SpotLight);

		ImGui::Separator();
		ImGui::Text("Coords:");
		ImGui::SliderFloat3((std::string("Position##") + std::to_string(i)).c_str(), reinterpret_cast<float*>(&mLightBuffer.Lights[i].Position), -500.0f, 500.f);
		ImGui::SliderFloat3((std::string("Direction##") + std::to_string(i)).c_str(), reinterpret_cast<float*>(&mLightBuffer.Lights[i].Direction), -10.0f, 10.f);
		

		if (mLastType[i] != mLightBuffer.Lights[i].LightType)
		{
			mLastType[i] = mLightBuffer.Lights[i].LightType;
			switch (mLastType[i])
			{
				case LightType::PointLight:
					mLightModels[i] = std::make_unique<Model>();
					mLightModels[i]->LoadIcoSphere(1.f, 2);
					mLightModels[i]->Init(device);
					mLightModels[i]->SetMaterial(std::make_shared<MaterialProperties>());
					break;

				case LightType::SpotLight:
					mLightModels[i] = std::make_unique<Model>();
					mLightModels[i]->LoadPyramid({ 1.f, 1.f, 1.f });
					mLightModels[i]->Init(device);
					mLightModels[i]->SetMaterial(std::make_shared<MaterialProperties>());
					break;

				default:
					mLightModels[i].reset();
					break;
			}
			
		}
		if (mLightModels[i])
		{
			mLightModels[i]->SetPosition(mLightBuffer.Lights[i].Position);
			mLightModels[i]->GetMaterial()->mMaterial.mEmissive = mLightBuffer.Lights[i].Color;
		}
		
		if (mLightBuffer.Lights[i].LightType == LightType::SpotLight) {
			ImGui::SliderAngle((std::string("SpotAngle##") + std::to_string(i)).c_str(), &mLightBuffer.Lights[i].SpotAngle, 0.0f, 360.f);
			
			if (mLightModels[i])
			{
				Vector vec(0, -1, 0);
				float cos = Vector::Dot3(vec, mLightBuffer.Lights[i].Direction);
				float sin = sqrt(1.f - (cos*cos));
				Vector axis = Vector::Cross3(mLightBuffer.Lights[i].Direction, vec);
				float angle = atan(sin / cos);
				mLightModels[i]->SetRotation(Matrix::MakeRotationNormal(axis, angle));
			}
		}

		ImGui::Separator();
		ImGui::Text("Attenuation:");
        ImGui::SliderFloat((std::string("Constant##") + std::to_string(i)).c_str(), &mLightBuffer.Lights[i].ConstantAttenuation, 0.0f, 1.f);
        ImGui::SliderFloat((std::string("Linear##") + std::to_string(i)).c_str(), &mLightBuffer.Lights[i].LinearAttenuation, 0.0f, .05f);
        ImGui::SliderFloat((std::string("Quadratic##") + std::to_string(i)).c_str(), &mLightBuffer.Lights[i].QuadraticAttenuation, 0.0f, .02f);
		ImGui::Separator(); ImGui::Separator();
    }
}

