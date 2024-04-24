#include "SpaceShooterActionGame.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Mesh.h"
struct vertex
{
	Vector3D position;
	Vector2D texcoord;
};


__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	Vector4D m_light_direction;
	Vector4D m_camera_position;
	Vector4D m_light_position = Vector4D(0, 1, 0, 0);
	float m_light_radius = 4.0f;
	float m_time = 0.0f;
};


SpaceShooterActionGame::SpaceShooterActionGame()
{
}

void SpaceShooterActionGame::render()
{
	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.3f, 0.4f, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);


	m_list_materials.clear();
	m_list_materials.push_back(m_barrel_mat);
	m_list_materials.push_back(m_brick_mat);
	m_list_materials.push_back(m_windows_mat);
	m_list_materials.push_back(m_wood_mat);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			updateModel(Vector3D(-14.0f + 14.0f * i, 0, -14.0f + 14.0f * j), m_list_materials);
			drawMesh(m_house_mesh, m_list_materials);
		}
	}


	m_list_materials.clear();
	m_list_materials.push_back(m_terrain_mat);
	updateModel(Vector3D(0, 0, 0), m_list_materials);
	drawMesh(m_terrain_mesh, m_list_materials);


	m_list_materials.clear();
	m_list_materials.push_back(m_sky_mat);

	//RENDER SKYBOX/SPHERE
	drawMesh(m_sky_mesh, m_list_materials);



	m_swap_chain->present(true);


	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
	m_time += m_delta_time;
}

void SpaceShooterActionGame::update()
{
	updateCamera();
	updateLight();
	updateSkyBox();
}

void SpaceShooterActionGame::updateModel(Vector3D position, const std::vector<MaterialPtr> & list_materials)
{
	constant cc;




	cc.m_world.setIdentity();
	cc.m_world.setTranslation(position);
	cc.m_view = m_view_cam;
	cc.m_proj = m_proj_cam;
	cc.m_camera_position = m_world_cam.getTranslation();

	cc.m_light_position = m_light_position;
	cc.m_light_radius = m_light_radius;
	cc.m_light_direction = m_light_rot_matrix.getZDirection();
	cc.m_time = m_time;
	for (size_t m = 0; m < list_materials.size(); m++)
	{
		list_materials[m]->setData(&cc, sizeof(constant));
	}
}

void SpaceShooterActionGame::updateCamera()
{
	Matrix4x4 world_cam, temp;
	world_cam.setIdentity();

	temp.setIdentity();
	temp.setRotationX(m_rot_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rot_y);
	world_cam *= temp;


	Vector3D new_pos = m_world_cam.getTranslation() + world_cam.getZDirection() * (m_forward * 0.05f);

	new_pos = new_pos + world_cam.getXDirection() * (m_rightward * 0.05f);

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;


	world_cam.inverse();




	m_view_cam = world_cam;

	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);


	m_proj_cam.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 100.0f);

}

void SpaceShooterActionGame::updateSkyBox()
{
	constant cc;

	cc.m_world.setIdentity();
	cc.m_world.setScale(Vector3D(100.0f, 100.0f, 100.0f));
	cc.m_world.setTranslation(m_world_cam.getTranslation());
	cc.m_view = m_view_cam;
	cc.m_proj = m_proj_cam;

	m_sky_mat->setData(&cc, sizeof(constant));
}

void SpaceShooterActionGame::updateLight()
{
	m_light_rot_matrix.setIdentity();

	//Lightを中心に45°回転
	m_light_rot_matrix.setRotationY(0.707f);
}

void SpaceShooterActionGame::drawMesh(const MeshPtr & mesh, const std::vector<MaterialPtr> & list_materials)
{
	//SET THE VERTICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(mesh->getVertexBuffer());
	//SET THE INDICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(mesh->getIndexBuffer());

	for (size_t m = 0; m < mesh->getNumMaterialSlots(); m++)
	{
		if (m >= list_materials.size()) break;
		MaterialSlot mat = mesh->getMaterialSlot(m);
		GraphicsEngine::get()->setMaterial(list_materials[m]);
		// FINALLY DRAW THE TRIANGLE
		GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(mat.num_indices, 0, mat.start_index);
	}

}




SpaceShooterActionGame::~SpaceShooterActionGame()
{
}

void SpaceShooterActionGame::onCreate()
{
	Window::onCreate();

	InputSystem::get()->addListener(this);

	m_play_state = true;
	InputSystem::get()->showCursor(false);

	/*m_wall_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\wall.jpg");
	m_bricks_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\brick.png");
	m_earth_color_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\earth_color.jpg");
*/



	m_sky_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\sky.jpg");
	m_sand_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\sand.jpg");

	m_barrel_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\barrel.jpg");
	m_brick_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\house_brick.jpg");
	m_windows_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\house_windows.jpg");
	m_wood_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\house_wood.jpg");


	/*m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\scene.obj");
	m_torus_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\torus.obj");
	m_suzanne_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\suzanne.obj");
	m_plane_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\plane.obj");
*/

	m_sky_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\sphere.obj");
	m_terrain_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\terrain.obj");
	m_house_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\house.obj");



	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	m_world_cam.setTranslation(Vector3D(0, 0, -1));


	m_mat = GraphicsEngine::get()->createMaterial(L"DirectionalLightVertexShader.hlsl", L"DirectionalLightPixelShader.hlsl");
	m_mat->addTexture(m_wall_tex);
	m_mat->setCullMode(CULL_MODE_BACK);


	/*m_earth_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_earth_mat->addTexture(m_earth_color_tex);
	m_earth_mat->setCullMode(CULL_MODE_BACK);*/

	/*m_bricks_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_bricks_mat->addTexture(m_bricks_tex);
	m_bricks_mat->setCullMode(CULL_MODE_BACK);*/

	m_terrain_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_terrain_mat->addTexture(m_sand_tex);
	m_terrain_mat->setCullMode(CULL_MODE_BACK);



	m_barrel_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_barrel_mat->addTexture(m_barrel_tex);
	m_barrel_mat->setCullMode(CULL_MODE_BACK);

	m_brick_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_brick_mat->addTexture(m_brick_tex);
	m_brick_mat->setCullMode(CULL_MODE_BACK);

	m_windows_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_windows_mat->addTexture(m_windows_tex);
	m_windows_mat->setCullMode(CULL_MODE_BACK);

	m_wood_mat = GraphicsEngine::get()->createMaterial(m_mat);
	m_wood_mat->addTexture(m_wood_tex);
	m_wood_mat->setCullMode(CULL_MODE_BACK);


	m_sky_mat = GraphicsEngine::get()->createMaterial(L"PointLightVertexShader.hlsl", L"SkyBoxPixelShader.hlsl");
	m_sky_mat->addTexture(m_sky_tex);
	m_sky_mat->setCullMode(CULL_MODE_FRONT);


	m_world_cam.setTranslation(Vector3D(0, 0, -2));

	m_list_materials.reserve(32);
}

void SpaceShooterActionGame::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();


	this->update();
	this->render();

}

void SpaceShooterActionGame::onDestroy()
{
	Window::onDestroy();
	m_swap_chain->SetFullScreen(false, 1, 1);
}

void SpaceShooterActionGame::onFocus()
{
	InputSystem::get()->addListener(this);
}

void SpaceShooterActionGame::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void SpaceShooterActionGame::onSize()
{
	RECT rc = this->getClientWindowRect();
	m_swap_chain->resize(rc.right - rc.left, rc.bottom - rc.top);
	this->render();
}

void SpaceShooterActionGame::onKeyDown(int key)
{
	if (key == 'W')
	{
		//m_rot_x += 3.14f*m_delta_time;
		m_forward = 1.0f;
	}
	else if (key == 'S')
	{
		//m_rot_x -= 3.14f*m_delta_time;
		m_forward = -1.0f;
	}
	else if (key == 'D')
	{
		//m_rot_y += 3.14f*m_delta_time;
		m_rightward = 1.0f;
	}
	else if (key == 'A')
	{
		//m_rot_y += 3.14f*m_delta_time;
		m_rightward = -1.0f;
	}
	else if (key == 'O')
	{
		m_light_radius -= 1.0f * m_delta_time;
	}
	else if (key == 'P')
	{
		m_light_radius += 1.0f * m_delta_time;
	}


}

void SpaceShooterActionGame::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_rightward = 0.0f;

	if (key == VK_ESCAPE)
	{
		if (m_play_state) 
		{
			m_play_state = false;
		    InputSystem::get()->showCursor(!m_play_state);
		}
	}
	else if (key == 'F')
	{
		m_fullscreen_state = (m_fullscreen_state) ? false : true;
		RECT size_screen = this->getSizeScreen();

		m_swap_chain->SetFullScreen(m_fullscreen_state, size_screen.right, size_screen.bottom);
	}
}

//マウスカーソルへの処理
void SpaceShooterActionGame::onMouseMove(const Point & mouse_pos)
{
	if (!m_play_state) return;

	RECT win_size = this->getClientWindowRect();

	int width = (win_size.right - win_size.left);
	int height = (win_size.bottom - win_size.top);

	m_rot_x += (mouse_pos.m_y - (win_size.top + (height / 2.0f))) * m_delta_time * 0.1f;
	m_rot_y += (mouse_pos.m_x - (win_size.left + (width / 2.0f))) * m_delta_time * 0.1f;

	InputSystem::get()->setCursorPosition(Point(win_size.left + (int)(width / 2.0f), win_size.top + (int)(height / 2.0f)));
}

void SpaceShooterActionGame::onLeftMouseDown(const Point & mouse_pos)
{
	if (!m_play_state)
	{
		m_play_state = true;
		InputSystem::get()->showCursor(!m_play_state);
	}
}

void SpaceShooterActionGame::onLeftMouseUp(const Point & mouse_pos)
{

}

void SpaceShooterActionGame::onRightMouseDown(const Point & mouse_pos)
{

}

void SpaceShooterActionGame::onRightMouseUp(const Point & mouse_pos)
{
}
