#include "Graphics.hpp"

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
}
#include <cstdio>
bool Graphics::Init(uint16_t screenWidth, uint16_t screenHeight, HWND hwnd)
{
	// Create the Direct3D object.
	mD3d = std::make_unique<Direct3d>();

	// Initialize the Direct3D object.
	bool result = mD3d->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}

	mCamera = std::make_unique<Camera>(
		Vector(0.f, 1.f, -55.f),	//< Camera position
		Vector(0.f)				//< Camera rotation
	);


	
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
	brownMaterial->mMaterial.mSpecular = Float4((Vector(0.545f, 0.27f, 0.0745f, 1.0f) * 2).Normalized3());
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




	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadCube({ 2.f, 4.f, 2.f });
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->SetPosition(Vector(4, 4, 4, 0));
	mModels.back()->mRot = Matrix::MakeRotationNormal(Vector(0,1,0), 45.f  * 3.1415f / 180.f);
	mModels.back()->mMat = mMaterialProperties[3];


	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadIcoSphere(2.f, 3);
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->SetPosition(Vector(-4, 2, -4, 0));
	mModels.back()->mMat = mMaterialProperties[5];
	
	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadIcoSphere(2.f, 3);
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->SetPosition(Vector(4, 0.5, -4, 0));
	mModels.back()->mMat = mMaterialProperties[2];

	//floor
	
	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadPlane(10.f, 10.f);
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->SetPosition(Vector(0, 0, 0, 0));
	mModels.back()->mMat = mMaterialProperties[4];

	// walls

	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadPlane(10.f, 10.f);
	mModels.back()->Init(mD3d->GetDevice());
	mModels.back()->mRot = Matrix::MakeRotationNormal(Vector(1, 0, 0), 90.f  * 3.1415f / 180.f);
	mModels.back()->SetPosition(Vector(0, 10, 10, 0));
	mModels.back()->mMat = mMaterialProperties[1];

	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadPlane(10.f, 10.f);
	mModels.back()->Init(mD3d->GetDevice());// , "E:/Projects/Engine/src/resources/brick.png");
	mModels.back()->mRot = Matrix::MakeRotationNormal(Vector(1, 0, 0), 90.f  * 3.1415f / 180.f) * Matrix::MakeRotationNormal(Vector(0, 1, 0), 90.f  * 3.1415f / 180.f);
	mModels.back()->SetPosition(Vector(-10, 10, 0, 0));
	mModels.back()->mMat = mMaterialProperties[1];

	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadPlane(10.f, 10.f);
	mModels.back()->Init(mD3d->GetDevice());// , "E:/Projects/Engine/src/resources/brick.png");
	mModels.back()->mRot = Matrix::MakeRotationNormal(Vector(1, 0, 0), 90.f  * 3.1415f / 180.f) * Matrix::MakeRotationNormal(Vector(0, 1, 0), -90.f  * 3.1415f / 180.f);
	mModels.back()->SetPosition(Vector(10, 10, 0, 0));
	mModels.back()->mMat = mMaterialProperties[1];



	mShader = std::make_unique<LightShader>();
	mShader->Initialize(mD3d->GetDevice(), hwnd);

	mLight = std::make_unique<LightBufferType>();
	mLight->GlobalAmbient = Float4(0.1f, 0.1f, 0.1f, 1.0f);

	static const LightType LightTypes[8] = {
		PointLight, SpotLight, SpotLight, PointLight, SpotLight, SpotLight, SpotLight, PointLight
	};

	static const bool LightEnabled[8] = {
		false, false, false, false, true, false, false, false
	};

	const int numLights = 8;
	float radius = 8.0f;
	float offset = 2.0f * 3.1415f / numLights;
	for (int i = 0; i < numLights; ++i)
	{
		Light light;
		light.Enabled = static_cast<int>(LightEnabled[i]);
		light.LightType = LightTypes[i];
		Vector col = Vector(1,0,1,1);//((i + 1) % 3, (i + 1) % 2, (i + 1), 1);
		light.Color = Float4(col);
		light.SpotAngle = 45.0f * 3.1415f / 180.f;
		light.ConstantAttenuation = 0.5f;
		light.LinearAttenuation = 0.08f;
		light.QuadraticAttenuation = 0.0f;
		Vector LightPosition = Vector(std::sin(offset * i) * radius, 9.0f, std::cos(offset * i) * radius, 1.0f);
		light.Position = LightPosition;
		Vector LightDirection(-LightPosition.f[0], -LightPosition.f[1], -LightPosition.f[2], 0.0f);
		LightDirection = LightDirection.Normalized3();
		light.Direction = LightDirection;
		mLight->Lights[i] = light;
		//printf("Light %d pos: [%f, %f, %f]\n", i, LightPosition.f[0], LightPosition.f[1], LightPosition.f[2]);
		//printf("Light %d dir: [%f, %f, %f]\n", i, LightDirection.f[0], LightDirection.f[1], LightDirection.f[2]);

		if (light.Enabled) {
			std::shared_ptr<MaterialProperties> lightMat = std::make_shared<MaterialProperties>();
			lightMat->mMaterial.mEmissive = light.Color;
			if (light.LightType == LightType::PointLight)
			{
				mModels.push_back(std::make_unique<Model>());
				mModels.back()->LoadIcoSphere(1.f, 2);
				mModels.back()->Init(mD3d->GetDevice());
				mModels.back()->SetPosition(LightPosition);

				mMaterialProperties.push_back(lightMat);
				mModels.back()->mMat = mMaterialProperties.back();
			}
			else if (light.LightType == LightType::SpotLight)
			{
				mModels.push_back(std::make_unique<Model>());
				mModels.back()->LoadPyramid({ 1.f, 1.f, 1.f });
				mModels.back()->Init(mD3d->GetDevice());
				mModels.back()->SetPosition(LightPosition);

				mMaterialProperties.push_back(lightMat);
				mModels.back()->mMat = mMaterialProperties.back();

				Vector vec(0, -1, 0);
				float cos = Vector::Dot3(vec, LightDirection);
				float sin = sqrt(1.f - (cos*cos));
				Vector axis = Vector::Cross3(LightDirection, vec);
				float angle = atan(sin / cos);
				mModels.back()->mRot = Matrix::MakeRotationNormal(axis, angle);
			}
		}
	}

	return true;
}

bool Graphics::Frame()
{
	// Render the graphics scene.
	return Render();
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

bool Graphics::Render()
{
	// Clear the buffers to begin the scene.
	mD3d->BeginScene(Vector(0.0f, 0.0f, 0.0f, 1.0f));

	// Get the world, view, and projection matrices from the camera and d3d objects.
	Matrix viewMatrix = mCamera->GetViewMatrix();
	Matrix projectionMatrix = mD3d->GetProjectionMatrix();
	Matrix worldMatrix;

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	for (auto &i : mModels)
	{
		i->Bind(mD3d->GetDeviceContext());
		Vector pos = i->GetPosition();

		mShader->Render(mD3d->GetDeviceContext(), i->GetIndexCount(), i->mRot * Matrix::MakeTranslation3(pos), viewMatrix, projectionMatrix,
			mCamera->GetPosition(), *mLight.get(), i->mMat.get());
	}

	// Present the rendered scene to the screen.
	mD3d->EndScene();

	return true;
}
