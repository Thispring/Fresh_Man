#include "pch.h"
#include "AGraphicShader.h"
#include "Device.h"
#include "PathMgr.h"

namespace
{
	HRESULT CompileShader(const wstring& relativePath, const string& entryPoint, const char* target, ID3DBlob** bytecode)
	{
		const wstring path = CONTENT_PATH + relativePath;
		ComPtr<ID3DBlob> errors;
		const HRESULT result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint.c_str(), target, D3DCOMPILE_DEBUG, 0, bytecode, errors.GetAddressOf());
		if (SUCCEEDED(result))
			return result;

		wchar_t resultCode[11] = {};
		swprintf_s(resultCode, L"0x%08lX", static_cast<unsigned long>(result));
		wstring message = path + L"\n" + wstring(entryPoint.begin(), entryPoint.end()) + L" (" + resultCode + L")";
		if (errors)
		{
			const char* text = static_cast<const char*>(errors->GetBufferPointer());
			const int length = MultiByteToWideChar(CP_ACP, 0, text, -1, nullptr, 0);
			if (length > 1)
			{
				wstring details(length, L'\0');
				MultiByteToWideChar(CP_ACP, 0, text, -1, details.data(), length);
				message += L"\n\n";
				message += details.c_str();
			}
		}
		MessageBoxW(nullptr, message.c_str(), L"쉐이더 컴파일 실패", MB_OK | MB_ICONERROR);
		return result;
	}
}

AGraphicShader::AGraphicShader()
	: Asset(ASSET_TYPE::GRAPHICSHADER)
	, m_Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_RSType(RS_TYPE::CULL_BACK)
	, m_DSType(DS_TYPE::LESS)
	, m_BSType(BS_TYPE::DEFAULT)
{
}

AGraphicShader::~AGraphicShader()
{
}

int AGraphicShader::CreateVertexShader(const wstring& _RelativeFilePath, const string& _FuncName)
{
	const HRESULT result = CompileShader(_RelativeFilePath, _FuncName, "vs_5_0", m_VSBlob.GetAddressOf());
	if (FAILED(result))
		return result;

	// 컴파일한 Shader 코드로 VertexShader 만들기
	if (FAILED(DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer()
		, m_VSBlob->GetBufferSize(), nullptr
		, m_VS.GetAddressOf())))
	{
		return E_FAIL;
	}

	// 0		12    20
	// |		|	  |
	// v		v     v
	// ---12---  --8--  ---16----
	// Position |  UV |   Color |
	// --------  -----  ---------
	// Input Layout 생성하기
	D3D11_INPUT_ELEMENT_DESC InputDesc[3] = {};

	InputDesc[0].SemanticName = "POSITION";
	InputDesc[0].SemanticIndex = 0;								// 시멘틱 이름이 중복된 경우, 구별하기위한 숫자
	InputDesc[0].AlignedByteOffset = 0;							// 메모리 시작 위치(오프셋)
	InputDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;			// 오프셋으로부터 크기
	InputDesc[0].InputSlot = 0;									// 설명하는 정점이 들어있는 버퍼의 위치
	InputDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// 설명하는 정점이 들어있는 버퍼의 위치
	InputDesc[0].InstanceDataStepRate = 0;						// 인스턴스당 증가하는 오프셋(인스턴싱에 사용)

	InputDesc[1].SemanticName = "TEXCOORD";
	InputDesc[1].SemanticIndex = 0;								// 시멘틱 이름이 중복된 경우, 구별하기위한 숫자
	InputDesc[1].AlignedByteOffset = 12;						// 메모리 시작 위치(오프셋)
	InputDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;				// 오프셋으로부터 크기
	InputDesc[1].InputSlot = 0;									// 설명하는 정점이 들어있는 버퍼의 위치
	InputDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// 설명하는 정점이 들어있는 버퍼의 위치
	InputDesc[1].InstanceDataStepRate = 0;						// 인스턴스당 증가하는 오프셋(인스턴싱에 사용)

	InputDesc[2].SemanticName = "COLOR";
	InputDesc[2].SemanticIndex = 0;								// 시멘틱 이름이 중복된 경우, 구별하기위한 숫자
	InputDesc[2].AlignedByteOffset = 20;						// 메모리 시작 위치(오프셋)
	InputDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;		// 오프셋으로부터 크기
	InputDesc[2].InputSlot = 0;									// 설명하는 정점이 들어있는 버퍼의 위치
	InputDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// 설명하는 정점이 들어있는 버퍼의 위치
	InputDesc[2].InstanceDataStepRate = 0;						// 인스턴스당 증가하는 오프셋(인스턴싱에 사용)

	// 레이아웃을 사용할 VertexShader 의 코드 정보가 필요
	if (FAILED(DEVICE->CreateInputLayout(InputDesc, 3
		, m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize()
		, m_Layout.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int AGraphicShader::CreatePixelShader(const wstring& _RelativeFilePath, const string& _FuncName)
{
	const HRESULT result = CompileShader(_RelativeFilePath, _FuncName, "ps_5_0", m_PSBlob.GetAddressOf());
	if (FAILED(result))
		return result;

	// 컴파일한 Shader 코드로 PixelShader 만들기
	if (FAILED(DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer()
		, m_PSBlob->GetBufferSize(), nullptr
		, m_PS.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

void AGraphicShader::Binding()
{
	/**************************
	* [Graphic Pipeline]:
	* 
	* 1. IA(Input Assembler)
	* 렌더링에 필요한 리소스 입력
	**************************/

	// 정점으로 구성할 도형의 상태, 모양
	// TriangleList 는 점 3개를 이어서 만든 삼각형(내부까지 색을 채움)
	CONTEXT->IASetPrimitiveTopology(m_Topology);

	// 정점 하나 안에 들어있는 데이터를 구분해주는 정보
	CONTEXT->IASetInputLayout(m_Layout.Get());

	// 2. Vertex Shader(함수) - 정점 당 연산 수행
	// HLSL(High Level Shader Language) 5.0
	CONTEXT->VSSetShader(m_VS.Get(), nullptr, 0);

	/********************************************************
	* 3. Rasterizer State 설정
	* Vertex Shader에서 반환한 정점의 위치를 NDC 좌표 기준으로 기록
	* 정점 내부에 들어오는 픽셀을 계산해서 Pixel Shader로 연계
	* nullptr 입력 : 기본 설정 옵션으로 동작
	* CULL_MODE : CULL_BACK
	* FILL_MODE : SOLID
	********************************************************/
	CONTEXT->RSSetState(Device::GetInst()->GetRSState(m_RSType).Get());

	/***********************************************************************
	* 4. Pixel Shader 설정
	* Rasterizer를 거쳐서, 호출될 픽셀마다 실행되는 함수
	* Pixel Shader에서 리턴한 값이, Render Target 내에서의 해당 픽셀 위치에 색상이 출력된다.
	***********************************************************************/
	CONTEXT->PSSetShader(m_PS.Get(), nullptr, 0);

	// 5. DepthStencilState 설정
	CONTEXT->OMSetDepthStencilState(Device::GetInst()->GetDSState(m_DSType).Get(), 0);
	
	// 6. BlendState 설정
	CONTEXT->OMSetBlendState(Device::GetInst()->GetBSState(m_BSType).Get(), nullptr, 0xffffffff);
}
