#include "Graphics.hpp"
#include "MathUtils.hpp"
#include "Logger.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
}

bool Graphics::Init(uint16_t screenWidth, uint16_t screenHeight, HWND hwnd)
{
	LOGI("Initializing Graphics");
	// Create the Direct3D object.
	mD3d = std::make_unique<Direct3d>();

	// Initialize the Direct3D object.
	bool result = mD3d->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		LOGE("DirectX3D initialization FAILED!");
		MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}

	// Setup Dear ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(mD3d->GetDevice(), mD3d->GetDeviceContext());

	mCamera = std::make_unique<Camera>(
		Vector(0.f, 1.f, -55.f),	//< Camera position
		Vector(0.f)				//< Camera rotation
	);


	// MATERIALS
	// TODO: Move to Manager
	std::shared_ptr<MaterialProperties> defaultMaterial = std::make_shared<MaterialProperties>();
	defaultMaterial->mMaterial.mEmissive = Float4(0.5f, 0.5f, 0.5f, 0.5f);
	mMaterialProperties.push_back(defaultMaterial);

	std::shared_ptr<MaterialProperties> brickMaterial = std::make_shared<MaterialProperties>();
	brickMaterial->mMaterial.mUseTexture = true;
	brickMaterial->mTexture = std::make_shared<Texture>();
	brickMaterial->mTexture->Initialize(mD3d->GetDevice(), "E:/Projects/Engine/src/resources/brick.png");
	mMaterialProperties.push_back(brickMaterial);

	// green
	std::shared_ptr<MaterialProperties> greenMaterial = std::make_shared<MaterialProperties>();
	greenMaterial->mMaterial.mAmbient = Float4(0.07568f, 0.61424f, 0.07568f, 1.0f);
	greenMaterial->mMaterial.mDiffuse = Float4(0.07568f, 0.61424f, 0.07568f, 1.0f);
	greenMaterial->mMaterial.mSpecular = Float4(0.07568f, 0.61424f, 0.07568f, 1.0f);
	greenMaterial->mMaterial.mSpecularPower = 76.8f;
	mMaterialProperties.push_back(greenMaterial);

	// red plastic
	std::shared_ptr<MaterialProperties> redPlasticMaterial = std::make_shared<MaterialProperties>();
	redPlasticMaterial->mMaterial.mDiffuse = Float4(0.6f, 0.1f, 0.1f, 1.0f);
	redPlasticMaterial->mMaterial.mSpecular = Float4(1.0f, 0.2f, 0.2f, 1.0f);
	redPlasticMaterial->mMaterial.mAmbient = Float4(0.3f, 0.05f, 0.05f, 1.0f);
	redPlasticMaterial->mMaterial.mSpecularPower = 32.0f;
	mMaterialProperties.push_back(redPlasticMaterial);

	// brown
	std::shared_ptr<MaterialProperties> brownMaterial = std::make_shared<MaterialProperties>();
	brownMaterial->mMaterial.mDiffuse = Float4(0.545f, 0.27f, 0.0745f, 1.0f);
	brownMaterial->mMaterial.mSpecular = (Vector(0.545f, 0.27f, 0.0745f, 1.0f) * 2).Normalized3().ToFloat4();
	brownMaterial->mMaterial.mAmbient = Float4(0.2725f, 0.135f, 0.03725f, 1.0f);
	brownMaterial->mMaterial.mSpecularPower = 20.0f;
	mMaterialProperties.push_back(brownMaterial);

	// pearl
	std::shared_ptr<MaterialProperties> pearlMaterial = std::make_shared<MaterialProperties>();
	pearlMaterial->mMaterial.mAmbient = Float4(0.25f, 0.20725f, 0.20725f, 1.0f);
	pearlMaterial->mMaterial.mDiffuse = Float4(1.0f, 0.829f, 0.829f, 1.0f);
	pearlMaterial->mMaterial.mSpecular = Float4(0.296648f, 0.296648f, 0.296648f, 1.0f);
	pearlMaterial->mMaterial.mSpecularPower = 11.264f;
	mMaterialProperties.push_back(pearlMaterial);


	// MODELS
	// TODO: Move to manager

	mModels = Model::LoadFromObjFile("../../../../src/resources/sponza/sponza.obj", mD3d->GetDevice());
	for (int i = 0; i < mModels.size(); i++)
	{
		mModels[i]->SetPosition({ 0 });
		mModels[i]->Init(mD3d->GetDevice());
	}
	/*
	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadCube({ 2.f, 4.f, 2.f });
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->SetPosition(Vector(4, 4, 4, 0));
	mModels.back()->SetRotation(Matrix::MakeRotationNormal(Vector(0,1,0), RADS(45.f)));
	mModels.back()->SetMaterial(mMaterialProperties[3]);


	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadIcoSphere(2.f, 3);
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->SetPosition(Vector(-4, 2, -4, 0));
	mModels.back()->SetMaterial(mMaterialProperties[5]);
	
	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadIcoSphere(2.f, 3);
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->SetPosition(Vector(4, 0.5, -4, 0));
	mModels.back()->SetMaterial(mMaterialProperties[2]);

	//floor
	
	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadPlane(10.f, 10.f);
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->SetPosition(Vector(0, 0, 0, 0));
	mModels.back()->SetMaterial(mMaterialProperties[4]);

	// walls

	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadPlane(10.f, 10.f);
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->SetRotation(Matrix::MakeRotationNormal(Vector(1, 0, 0), RADS(90.f)));
	mModels.back()->SetPosition(Vector(0, 10, 10, 0));
	mModels.back()->SetMaterial(mMaterialProperties[1]);

	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadPlane(10.f, 10.f);
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->SetRotation(Matrix::MakeRotationNormal(Vector(1, 0, 0), RADS(90.f)) * Matrix::MakeRotationNormal(Vector(0, 1, 0), RADS(90.f)));
	mModels.back()->SetPosition(Vector(-10, 10, 0, 0));
	mModels.back()->SetMaterial(mMaterialProperties[1]);

	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadPlane(10.f, 10.f);
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->SetRotation(Matrix::MakeRotationNormal(Vector(1, 0, 0), RADS(90.f)) * Matrix::MakeRotationNormal(Vector(0, 1, 0), RADS(-90.f)));
	mModels.back()->SetPosition(Vector(10, 10, 0, 0));
	mModels.back()->SetMaterial(mMaterialProperties[1]);

	*/

	mShader = std::make_unique<LightShader>();
	mShader->Initialize(mD3d->GetDevice(), hwnd);

	mLight = std::make_unique<LightManager>();

	LOGI("Graphics initialized!");

	return true;
}

void Graphics::MoveCamera(float x, float y, double delta, Camera::Move dir)
{
	mCamera->MoveCamera(x, y, delta, dir);
}

void Graphics::ZoomCamera(float z)
{
	mD3d->ChangeFoV(z);
	mD3d->RecalculateProjectionMatrix();
}

LightManager * Graphics::GetLight() const
{
	return mLight.get();
}

ID3D11Device * Graphics::GetDevice()
{
	return mD3d->GetDevice();
}

bool Graphics::Render()
{
	// Clear the buffers to begin the scene.
	mD3d->BeginScene(Vector(0.0f, 0.0f, 0.0f, 1.0f));

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	for (auto &i : mModels)
	{
		if (i)
		{
			i->Bind(mD3d->GetDeviceContext());

			mShader->Render(mD3d->GetDeviceContext(), i->GetIndexCount(), i->GetWorldMatrix(), mCamera->GetViewMatrix(), mD3d->GetProjectionMatrix(),
				mCamera->GetPosition(), *mLight->GetLightBuffer(), i->GetMaterial());
		}
	}

	for (int i = 0; i < LIGHT_NO; ++i)
	{
		Model* mdl = mLight->GetModel(i);
		if (mdl != nullptr && mLight->GetLight(i)->Enabled == 1)
		{
			mdl->Bind(mD3d->GetDeviceContext());

			mShader->Render(mD3d->GetDeviceContext(), mdl->GetIndexCount(), mdl->GetWorldMatrix(), mCamera->GetViewMatrix(), mD3d->GetProjectionMatrix(),
				mCamera->GetPosition(), *mLight->GetLightBuffer(), mdl->GetMaterial());
		}
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Present the rendered scene to the screen.
	mD3d->EndScene();

	return true;
}
