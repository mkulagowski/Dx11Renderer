#include "Graphics.hpp"

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
}

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




	std::shared_ptr<Texture> texViolet = std::make_shared<Texture>();
	texViolet->Initialize(mD3d->GetDevice(), Vector(1, 0, 1, 1));
	std::shared_ptr<Texture> texLime = std::make_shared<Texture>();
	texLime->Initialize(mD3d->GetDevice(), Vector(0.749f, 1, 0, 1));
	std::shared_ptr<Texture> texBlue = std::make_shared<Texture>();
	texBlue->Initialize(mD3d->GetDevice(), Vector(0, 0.749f, 1, 1));


	mModels.push_back(std::make_unique<Model>());
	//mModels.back()->LoadFromObjFile("E:/Projects/Engine/src/resources/model.obj");
	mModels.back()->LoadIcoSphere(20.f, 3);
	mModels.back()->Init(mD3d->GetDevice(), texViolet);
	

	mModels.push_back(std::make_unique<Model>());
	mModels.back()->LoadIcoSphere(20.f, 3);
	mModels.back()->Init(mD3d->GetDevice(), texLime);
	mModels.back()->SetPosition(Vector(100, 0, 50, 0));

	mModels.push_back(std::make_unique<Model>());
	//mModels.back()->LoadFromObjFile("E:/Projects/Engine/src/resources/model.obj");
	mModels.back()->LoadIcoSphere(20.f, 3);
	mModels.back()->Init(mD3d->GetDevice(), texBlue);
	//mModels.back()->Init(mD3d->GetDevice(), "E:/Projects/Engine/src/resources/brick.png");
	mModels.back()->SetPosition(Vector(50, 0, 100, 0));



	//mModel->Init(mD3d->GetDevice(), "E:/Projects/Engine/src/resources/brick.png");

	//mModels.push_back(model);

	mShader = std::make_unique<LightShader>();
	mShader->Initialize(mD3d->GetDevice(), hwnd);

	mLight = std::make_unique<Light>();
	mLight->SetDiffuseColor(0.5f, 0.5f, 0.5f, 1.f);
	mLight->SetAmbient(0.15f, 0.15f, 0.15f, 1.0f);
	mLight->SetDirection(0.0f, -1.0f, 1.0f);
	mLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	mLight->SetSpecularPower(32.f);

	return true;
}

bool Graphics::Frame()
{
	static float rotation = 0.0f;


	// Update the rotation variable each frame.
	rotation += 3.1415f * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the graphics scene.
	return RenderWithRotation(rotation);
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
/*
bool Graphics::Render()
{
	// Clear the buffers to begin the scene.
	mD3d->BeginScene(Vector(0.f, 0.f, 0.f, 1.f));

	// Generate the view matrix based on the camera's position.
	//mCamera->RecalculateMatrix();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	Matrix viewMatrix = mCamera->GetViewMatrix();
	Matrix worldMatrix = mD3d->GetWorldMatrix();
	Matrix projectionMatrix = mD3d->GetProjectionMatrix();

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	for (auto &i : mModels)
	{
		i.Bind(mD3d->GetDeviceContext());

	}
	//mModel->Bind(mD3d->GetDeviceContext());

	// Render the model using the color shader.
	bool result = static_cast<TextureShader*>(mShader.get())->Render(mD3d->GetDeviceContext(), mModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, mModel->GetTexture()->GetShaderResourceView());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	mD3d->EndScene();

	return true;
}
*/

bool Graphics::RenderWithRotation(float rot)
{
	// Clear the buffers to begin the scene.
	mD3d->BeginScene(Vector(0.0f, 0.0f, 0.0f, 1.0f));

	// Generate the view matrix based on the camera's position.
	//mCamera->RecalculateMatrix();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	Matrix viewMatrix = mCamera->GetViewMatrix();
	Matrix projectionMatrix = mD3d->GetProjectionMatrix();
	Matrix worldMatrix;//Matrix::MakeRotationNormal(Vector(0.f, 1.f, 0.f), rot);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	for (auto &i : mModels)
	{
		i->Bind(mD3d->GetDeviceContext());
		Vector pos = i->GetPosition();

		mShader->Render(mD3d->GetDeviceContext(), i->GetIndexCount(), Matrix::MakeTranslation3(pos), viewMatrix, projectionMatrix,
			i->GetTexture()->GetShaderResourceView(), mLight->GetDirection(), mLight->GetAmbient(), mLight->GetDiffuseColor(), mCamera->GetPosition(), mLight->GetSpecularColor(), mLight->GetSpecularPower());
	}

	// Render the model using the color shader.
	/*bool result = mShader->Render(mD3d->GetDeviceContext(), mModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		mModel->GetTexture()->GetShaderResourceView(), mLight->GetDirection(), mLight->GetAmbient(), mLight->GetDiffuseColor(), mCamera->GetPosition(), mLight->GetSpecularColor(), mLight->GetSpecularPower());
	if (!result)
	{
		return false;
	}
	*/
	// Present the rendered scene to the screen.
	mD3d->EndScene();

	return true;
}
