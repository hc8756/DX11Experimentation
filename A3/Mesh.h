#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
/*Class that creates and uses index & vertex buffers*/
class Mesh
{
public: 
	Mesh(Vertex* vertexArray, int vertNum, unsigned int* indexArray, int indNum, Microsoft::WRL::ComPtr<ID3D11Device>, Microsoft::WRL::ComPtr<ID3D11DeviceContext>);//pointer is similar to array
	~Mesh();
	void Draw();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	
private:
	//Buffers to hold mesh data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	//Device context to call rendering pipeline w/ drawing function
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	//Size of index buffer (used when drawing)
	int ibSize;
};
