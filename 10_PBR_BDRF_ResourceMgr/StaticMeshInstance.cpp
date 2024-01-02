#include "StaticMeshInstance.h"
#include "Mesh.h"

StaticMeshInstance::StaticMeshInstance()
{
}

StaticMeshInstance::~StaticMeshInstance()
{

}


void StaticMeshInstance::Create(Mesh* pMeshResource, Math::Matrix* pWorld, Material* pMaterial)
{
	m_pMeshResource = pMeshResource;
	m_pMaterial = pMaterial;
	m_pNodeWorldTransform = pWorld;
}

void StaticMeshInstance::Render(ID3D11DeviceContext* deviceContext)
{
	//ToDo IASetVertexBuffers 이 부분은 Skeletal 부분도 따로 만들어 줘야함. 현재는 스태틱 메쉬만 적용 중.

	deviceContext->IASetIndexBuffer(m_pMeshResource->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetVertexBuffers(0, 1, &m_pMeshResource->m_pVertexBuffer, &m_pMeshResource->m_VertexBufferStride, &m_pMeshResource->m_VertexBufferOffset);
	deviceContext->DrawIndexed(m_pMeshResource->m_IndexCount, 0, 0);
}
