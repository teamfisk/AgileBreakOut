#include "PrecompiledHeader.h"
#include "Rendering/Renderer.h"

#include "Rendering/Model.h"
#include "Rendering/Skeleton.h"
#include "Rendering/ShaderProgram.h"

void dd::Renderer::Initialize()
{
	StaticSystem::Set(this);

	// Initialize GLFW
	if (!glfwInit()) {
		LOG_ERROR("GLFW: Initialization failed");
		exit(EXIT_FAILURE);
	}

	// Create a window
	GLFWmonitor* monitor = nullptr;
	if (m_Fullscreen) {
		monitor = glfwGetPrimaryMonitor();
	}
	glfwWindowHint(GLFW_SAMPLES, 8);
	m_Window = glfwCreateWindow(m_Resolution.Width, m_Resolution.Height, "Squidout! (DirectX 11)", monitor, nullptr);
	if (!m_Window) {
		LOG_ERROR("GLFW: Failed to create window");
		exit(EXIT_FAILURE);
	}
	//glfwMakeContextCurrent(m_Window);

	// Create default camera
	m_DefaultCamera = std::unique_ptr<dd::Camera>(new dd::Camera((float)m_Resolution.Width / m_Resolution.Height, 45.f, 0.01f, 5000.f));
	m_DefaultCamera->SetPosition(glm::vec3(0, 0, 0));
	if (m_Camera == nullptr) {
		m_Camera = m_DefaultCamera.get();
	}

	DXGI_SWAP_CHAIN_DESC scd = { 0 };
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = glfwGetWin32Window(m_Window);
	scd.SampleDesc.Count = 4;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&m_SwapChain,
		&m_Device,
		NULL,
		&m_DeviceContext);

	// Set the back buffer render target
	ID3D11Texture2D* pBackBuffer;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	m_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_BackBuffer);
	pBackBuffer->Release();
	// Depth buffer
	D3D11_TEXTURE2D_DESC texd = { };
	texd.Width = m_Resolution.Width;
	texd.Height = m_Resolution.Height;
	texd.ArraySize = 1;
	texd.MipLevels = 1;
	texd.SampleDesc.Count = 4;
	texd.Format = DXGI_FORMAT_D32_FLOAT;
	texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ID3D11Texture2D* pDepthBuffer;
	m_Device->CreateTexture2D(&texd, NULL, &pDepthBuffer);
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = { };
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	m_Device->CreateDepthStencilView(pDepthBuffer, &dsvd, &m_DepthBuffer);
	pDepthBuffer->Release();
	// Set back and depth buffer
	m_DeviceContext->OMSetRenderTargets(1, &m_BackBuffer, m_DepthBuffer);

	// Rasterizer settings
	setDefaultRasterState();

	D3D11_BLEND_DESC blend = { 0 };
	for (int i = 0; i < 1; i++) {
		blend.RenderTarget[i].BlendEnable = true;
		blend.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blend.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	ID3D11BlendState* blendState;
	m_Device->CreateBlendState(&blend, &blendState);
	m_DeviceContext->OMSetBlendState(blendState, nullptr, 0xffffffff);


	D3D11_INPUT_ELEMENT_DESC VertexIED[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BLENDINDICES", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BLENDWEIGHT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	m_ForwardShaderProgram = new ShaderProgram();
	m_ForwardShaderProgram->CompileVertexShader(L"Shaders/DirectX/vertex.hlsl");
	m_ForwardShaderProgram->CompilePixelShader(L"Shaders/DirectX/pixel.hlsl");
	m_ForwardShaderProgram->CreateInputLayout(VertexIED, sizeof(VertexIED) / sizeof(VertexIED[0]));
	m_GUIShaderProgram = new ShaderProgram();
	m_GUIShaderProgram->CompileVertexShader(L"Shaders/DirectX/vertex.hlsl");
	m_GUIShaderProgram->CompilePixelShader(L"Shaders/DirectX/Flat.pixel.hlsl");
	m_GUIShaderProgram->CreateInputLayout(VertexIED, sizeof(VertexIED) / sizeof(VertexIED[0]));

	// Create constant buffer
	// TODO: Put this in shader
	D3D11_BUFFER_DESC bd = { };
	bd.ByteWidth = sizeof(ConstantBufferStruct);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_Device->CreateBuffer(&bd, NULL, &constantBuffer);
	m_DeviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	m_DeviceContext->PSSetConstantBuffers(0, 1, &constantBuffer);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 1.f;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 1.f;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_Device->CreateSamplerState(&samplerDesc, &m_SamplerState);
	
	m_UnitQuad = ResourceManager::Load<Model>("Models/Core/UnitQuad.obj");
	m_WhiteSphereTexture = ResourceManager::Load<Texture>("Textures/Test/Water.png");
}

void dd::Renderer::Draw(RenderQueueCollection& rq)
{
	using namespace DirectX::SimpleMath;

	rq.Forward.Jobs.sort(dd::Renderer::DepthSort);

	m_DeviceContext->OMSetRenderTargets(1, &m_BackBuffer, m_DepthBuffer);
	float color[] = { 0.f, 0.f, 0.f, 1.f };
	m_DeviceContext->ClearRenderTargetView(m_BackBuffer, color);
	m_DeviceContext->ClearDepthStencilView(m_DepthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Set the viewport
	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = m_Resolution.Width;
	viewport.Height = m_Resolution.Height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	m_DeviceContext->RSSetViewports(1, &viewport);
	
	m_ForwardShaderProgram->Bind();

	Matrix proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(m_Camera->m_FOV, m_Camera->m_AspectRatio, m_Camera->m_NearClip, m_Camera->m_FarClip); // Right-handed
	Matrix view = Matrix::CreateTranslation(Vector3(m_Camera->Position().x, m_Camera->Position().y, -m_Camera->Position().z));
	
	int i = 0;
	constantBufferStruct.NumLights = 0;
	for (auto &job : rq.Lights) {
		auto pointLightJob = std::dynamic_pointer_cast<PointLightJob>(job);
		if (pointLightJob) {
			glm::vec3 p = pointLightJob->Position;
			constantBufferStruct.LightPositions[i] = Vector4(p.r, p.g, p.b, 0.f);
			glm::vec3 c = pointLightJob->DiffuseColor;
			constantBufferStruct.LightColors[i] = Vector4(c.r, c.g, c.b, 1.f);
			constantBufferStruct.LightRadii[i] = pointLightJob->Radius;
			constantBufferStruct.NumLights++;
			i++;
		}
	}

	for (auto &job : rq.Deferred) {
		auto modelJob = std::dynamic_pointer_cast<ModelJob>(job);
		if (modelJob) {
			constantBufferStruct.MVP = Matrix(glm::value_ptr(modelJob->ModelMatrix)) * view * proj;
			constantBufferStruct.InverseTransposeViewModel = (Matrix(glm::value_ptr(modelJob->ModelMatrix)) * view).Transpose().Invert();
			constantBufferStruct.Color = Vector4(modelJob->Color.r, modelJob->Color.g, modelJob->Color.b, modelJob->Color.a);

			m_DeviceContext->UpdateSubresource(constantBuffer, 0, 0, &constantBufferStruct, 0, 0);
			m_DeviceContext->PSSetSamplers(0, 1, &m_SamplerState);

			m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			UINT stride = sizeof(Model::Vertex);
			UINT offset = 0;
			m_DeviceContext->IASetVertexBuffers(0, 1, &modelJob->Model->VertexBuffer, &stride, &offset);
			m_DeviceContext->IASetIndexBuffer(modelJob->Model->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			if (modelJob->DiffuseTexture != nullptr) {
				m_DeviceContext->PSSetShaderResources(0, 1, &modelJob->DiffuseTexture->m_ShaderResourceView);
			}
			//m_DeviceContext->Draw(modelJob->EndIndex - modelJob->StartIndex + 1, modelJob->StartIndex);
			m_DeviceContext->DrawIndexed(modelJob->EndIndex - modelJob->StartIndex + 1, modelJob->StartIndex, 0);

			continue;
		}
	}

	for (auto &job : rq.Forward) {
		auto spriteJob = std::dynamic_pointer_cast<SpriteJob>(job);
		if (spriteJob) {
			constantBufferStruct.MVP = Matrix(glm::value_ptr(spriteJob->ModelMatrix)) * view * proj;
			constantBufferStruct.InverseTransposeViewModel = (Matrix(glm::value_ptr(spriteJob->ModelMatrix)) * view).Transpose().Invert();
			constantBufferStruct.Color = Vector4(glm::value_ptr(spriteJob->Color));
			
			m_DeviceContext->UpdateSubresource(constantBuffer, 0, 0, &constantBufferStruct, 0, 0);
			m_DeviceContext->PSSetSamplers(0, 1, &m_SamplerState);

			m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			UINT stride = sizeof(Model::Vertex);
			UINT offset = 0;
			m_DeviceContext->IASetVertexBuffers(0, 1, &m_UnitQuad->VertexBuffer, &stride, &offset);
			m_DeviceContext->IASetIndexBuffer(m_UnitQuad->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			if (spriteJob->DiffuseTexture != nullptr) {
				m_DeviceContext->PSSetShaderResources(0, 1, &spriteJob->DiffuseTexture->m_ShaderResourceView);
			}
			//m_DeviceContext->Draw(modelJob->EndIndex - modelJob->StartIndex + 1, modelJob->StartIndex);
			m_DeviceContext->DrawIndexed(m_UnitQuad->m_Indices.size(), 0, 0);

			continue;
		}

		auto waterJob = std::dynamic_pointer_cast<WaterParticleJob>(job);
		if (waterJob) {
			constantBufferStruct.MVP = Matrix(glm::value_ptr(waterJob->ModelMatrix)) * view * proj;
			constantBufferStruct.InverseTransposeViewModel = (Matrix(glm::value_ptr(waterJob->ModelMatrix)) * view).Transpose().Invert();
			constantBufferStruct.Color = Vector4(glm::value_ptr(waterJob->Color));

			m_DeviceContext->UpdateSubresource(constantBuffer, 0, 0, &constantBufferStruct, 0, 0);
			m_DeviceContext->PSSetSamplers(0, 1, &m_SamplerState);

			m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			UINT stride = sizeof(Model::Vertex);
			UINT offset = 0;
			m_DeviceContext->IASetVertexBuffers(0, 1, &m_UnitQuad->VertexBuffer, &stride, &offset);
			m_DeviceContext->IASetIndexBuffer(m_UnitQuad->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			m_DeviceContext->PSSetShaderResources(0, 1, &m_WhiteSphereTexture->m_ShaderResourceView);
			//m_DeviceContext->Draw(modelJob->EndIndex - modelJob->StartIndex + 1, modelJob->StartIndex);
			m_DeviceContext->DrawIndexed(m_UnitQuad->m_Indices.size(), 0, 0);

			continue;
		}
	}

	m_DeviceContext->OMSetRenderTargets(1, &m_BackBuffer, nullptr);
	m_GUIShaderProgram->Bind();

	for (auto &job : rq.GUI) {
		auto frameJob = std::dynamic_pointer_cast<FrameJob>(job);
		if (frameJob) {
			Rectangle& vp = frameJob->Viewport;
			glViewport(vp.X, m_Resolution.Height - vp.Y - vp.Height, vp.Width, vp.Height);
			Rectangle& sc = frameJob->Scissor;
			if (sc == Rectangle()) {
				glDisable(GL_SCISSOR_TEST);
			} else {
				glEnable(GL_SCISSOR_TEST);
				glScissor(sc.X, m_Resolution.Height - sc.Y - sc.Height, sc.Width, sc.Height);
			}

			D3D11_VIEWPORT viewport = { 0 };
			viewport.TopLeftX = vp.X;
			viewport.TopLeftY = vp.Y;
			viewport.Width = vp.Width;
			viewport.Height = vp.Height;
			viewport.MinDepth = 0;
			viewport.MaxDepth = 1;
			m_DeviceContext->RSSetViewports(1, &viewport);

			proj = Matrix::Identity;
			view = Matrix::Identity;
			Matrix model = Matrix::CreateScale(2.f);
			constantBufferStruct.MVP = model * view * proj;
			constantBufferStruct.InverseTransposeViewModel = (model * view).Transpose().Invert();
			constantBufferStruct.Color = Vector4(glm::value_ptr(frameJob->Color));
			
			m_DeviceContext->UpdateSubresource(constantBuffer, 0, 0, &constantBufferStruct, 0, 0);
			m_DeviceContext->PSSetSamplers(0, 1, &m_SamplerState);

			m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			UINT stride = sizeof(Model::Vertex);
			UINT offset = 0;
			m_DeviceContext->IASetVertexBuffers(0, 1, &m_UnitQuad->VertexBuffer, &stride, &offset);
			m_DeviceContext->IASetIndexBuffer(m_UnitQuad->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			if (frameJob->DiffuseTexture != nullptr) {
				m_DeviceContext->PSSetShaderResources(0, 1, &frameJob->DiffuseTexture->m_ShaderResourceView);
			}
			//m_DeviceContext->Draw(modelJob->EndIndex - modelJob->StartIndex + 1, modelJob->StartIndex);
			m_DeviceContext->DrawIndexed(m_UnitQuad->m_Indices.size(), 0, 0);

			continue;
		}
	}
	//constantBufferStruct.ModelMatrix = GLMtoDX(glm::mat4(1.f));
	//constantBufferStruct.ViewMatrix = GLMtoDX(m_Camera->ViewMatrix());
	//constantBufferStruct.ProjectionMatrix = proj;
	//constantBufferStruct.InverseTransposeViewModel = GLMtoDX(glm::mat4(1.f));

	m_SwapChain->Present(0, 0);
	//glfwSwapBuffers(m_Window);
}

//void dd::Renderer::Resize(int width, int height)
//{
//	m_DeviceContext->OMSetRenderTargets(0, 0, 0);
//	m_BackBuffer->Release();
//
//	HRESULT hr; // TODO: Error handling
//	// Preserve the existing buffer count and format.
//	hr = m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
//
//	ID3D11Texture2D* pBuffer;
//	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
//	hr = m_Device->CreateRenderTargetView(pBuffer, NULL, &m_BackBuffer);
//	pBuffer->Release();
//	m_DeviceContext->OMSetRenderTargets(1, &BackBuffer, NULL);
//
//	// Set the viewport
//	D3D11_VIEWPORT viewport = { 0 };
//	viewport.TopLeftX = 0;
//	viewport.TopLeftY = 0;
//	viewport.Width = width;
//	viewport.Height = height;
//	m_DeviceContext->RSSetViewports(1, &viewport);
//}

dd::Renderer::~Renderer()
{
	m_SwapChain->Release();
	m_BackBuffer->Release();
	m_Device->Release();
	m_DeviceContext->Release();
}

DirectX::SimpleMath::Matrix dd::Renderer::GLMtoDXModelView(glm::mat4 gm)
{
	gm = glm::transpose(gm);
	DirectX::SimpleMath::Matrix dm(
		gm[0][0], gm[0][1], -gm[0][2], gm[0][3],
		gm[1][0], gm[1][1], -gm[1][2], gm[1][3],
		-gm[2][0], -gm[2][1], gm[2][2], gm[2][3],
		gm[3][0], gm[3][1], -gm[3][2], gm[3][3]
	);
	return dm;
}

DirectX::SimpleMath::Matrix dd::Renderer::GLMtoDXProjection(glm::mat4 gm)
{
	gm = glm::transpose(gm);
	DirectX::SimpleMath::Matrix dm(
		gm[0][0], gm[0][1], gm[0][2], gm[0][3],
		gm[1][0], gm[1][1], gm[1][2], gm[1][3],
		-gm[2][0], -gm[2][1], -gm[2][2], -gm[2][3],
		gm[3][0], gm[3][1], gm[3][2], gm[3][3]
	);
	return dm;
}

void dd::Renderer::setDefaultRasterState()
{
	if (m_RasterState != nullptr) {
		m_RasterState->Release();
		m_RasterState = nullptr;
	}

	D3D11_RASTERIZER_DESC rd;
	memset(&rd, 0, sizeof(D3D11_RASTERIZER_DESC));
	rd.AntialiasedLineEnable = false;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.DepthClipEnable = true;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.FrontCounterClockwise = true;
	rd.MultisampleEnable = false;
	rd.ScissorEnable = false;
	rd.SlopeScaledDepthBias = 0.0f;
	m_Device->CreateRasterizerState(&rd, &m_RasterState);
	m_DeviceContext->RSSetState(m_RasterState);
}

void dd::Renderer::setGUIRasterState()
{
	if (m_RasterState != nullptr) {
		m_RasterState->Release();
		m_RasterState = nullptr;
	}

	D3D11_RASTERIZER_DESC rd;
	memset(&rd, 0, sizeof(D3D11_RASTERIZER_DESC));
	rd.AntialiasedLineEnable = false;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.DepthClipEnable = false;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.FrontCounterClockwise = true;
	rd.MultisampleEnable = false;
	rd.ScissorEnable = false;
	rd.SlopeScaledDepthBias = 0.0f;
	m_Device->CreateRasterizerState(&rd, &m_RasterState);
	m_DeviceContext->RSSetState(m_RasterState);
}
