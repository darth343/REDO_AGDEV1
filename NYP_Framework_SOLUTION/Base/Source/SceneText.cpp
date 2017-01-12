#include "SceneText.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>
#include "KeyboardController.h"
#include "MouseController.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "EntityManager.h"

#include "GenericEntity.h"
#include "GroundEntity.h"
#include "TextEntity.h"
#include "SpriteEntity.h"
#include "Light.h"
#include "SkyBox/SkyBoxEntity.h"
#include "SceneGraph\SceneGraph.h"
#include "SpatialPartition\SpatialPartition.h"
#include "Mortar\MortarSingleton.h"

#include <iostream>
using namespace std;

SceneText* SceneText::sInstance = new SceneText(SceneManager::GetInstance());

SceneText::SceneText()
{
}

SceneText::SceneText(SceneManager* _sceneMgr)
{
	_sceneMgr->AddScene("Start", this);
}

SceneText::~SceneText()
{
	CSpatialPartition::GetInstance()->RemoveCamera();
	CSceneGraph::GetInstance()->Destroy();
}

void SceneText::Init()
{
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//Texture.vertexshader", "Shader//Texture.fragmentshader");
	
	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled");
	currProg->AddUniform("colorTexture");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");
	
	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");

	lights[0] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	lights[0]->position.Set(0, 20, 0);
	lights[0]->color.Set(1, 1, 1);
	lights[0]->power = 1;
	lights[0]->kC = 1.f;
	lights[0]->kL = 0.01f;
	lights[0]->kQ = 0.001f;
	lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	lights[0]->exponent = 3.f;
	lights[0]->spotDirection.Set(0.f, 1.f, 0.f);
	lights[0]->name = "lights[0]";

	lights[1] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[1]", lights[1]);
	lights[1]->type = Light::LIGHT_DIRECTIONAL;
	lights[1]->position.Set(1, 1, 0);
	lights[1]->color.Set(1, 1, 0.5f);
	lights[1]->power = 0.4f;
	lights[1]->name = "lights[1]";

	currProg->UpdateInt("numLights", 1);
	currProg->UpdateInt("textEnabled", 0);
	
	// Create the playerinfo instance, which manages all information about the player
	playerInfo = CPlayerInfo::GetInstance();
	playerInfo->Init();
	playerInfo->SetPos(Vector3(-350, 8, 0));

	// Create and attach the camera to the scene
	//camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	camera.Init(Vector3(-800, 865, -6.5), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	// Load all the meshes
	MeshBuilder::GetInstance()->GenerateAxes("reference");
	MeshBuilder::GetInstance()->GenerateCrossHair("crosshair");
	MeshBuilder::GetInstance()->GenerateQuad("quad", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("quad")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);
	MeshBuilder::GetInstance()->GenerateText("text_noalpha", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text_noalpha")->textureID = LoadTGA("Image//calibri_noalpha.tga");
	MeshBuilder::GetInstance()->GetMesh("text_noalpha")->material.kAmbient.Set(1, 0, 0);
	MeshBuilder::GetInstance()->GenerateOBJ("Chair", "OBJ//chair.obj");
	MeshBuilder::GetInstance()->GetMesh("Chair")->textureID = LoadTGA("Image//chair.tga");
	MeshBuilder::GetInstance()->GenerateRing("ring", Color(1, 0, 1), 36, 1, 0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("lightball", Color(1, 1, 1), 18, 36, 1.f);
	MeshBuilder::GetInstance()->GenerateSphere("sphere", Color(1, 0, 0), 18, 36, 0.5f);
	MeshBuilder::GetInstance()->GenerateCone("cone", Color(0.5f, 1, 0.3f), 36, 10.f, 10.f);
	MeshBuilder::GetInstance()->GenerateCube("cube", Color(1.0f, 1.0f, 0.0f), 1.0f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kDiffuse.Set(0.99f, 0.99f, 0.99f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kSpecular.Set(0.f, 0.f, 0.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRASS_DARKGREEN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GRASS_DARKGREEN")->textureID = LoadTGA("Image//grass_darkgreen.tga");
	MeshBuilder::GetInstance()->GenerateQuad("GEO_GRASS_LIGHTGREEN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GEO_GRASS_LIGHTGREEN")->textureID = LoadTGA("Image//grass_lightgreen.tga");
	MeshBuilder::GetInstance()->GenerateCube("cubeSG", Color(1.0f, 0.64f, 0.0f), 1.0f);

	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_FRONT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BACK", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_LEFT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_RIGHT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_TOP", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BOTTOM", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_FRONT")->textureID = LoadTGA("Image//SkyBox//skybox_front.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BACK")->textureID = LoadTGA("Image//SkyBox//skybox_back.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_LEFT")->textureID = LoadTGA("Image//SkyBox//skybox_left.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_RIGHT")->textureID = LoadTGA("Image//SkyBox//skybox_right.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_TOP")->textureID = LoadTGA("Image//SkyBox//skybox_top.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BOTTOM")->textureID = LoadTGA("Image//SkyBox//skybox_bottom.tga");
	MeshBuilder::GetInstance()->GenerateRay("laser", 10.0f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH_WHITE", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH_RED", Color(1, 0, 0), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH_YELLOW", Color(1, 1, 0), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH_BLUE", Color(0, 0, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH_GREEN", Color(0, 1, 0), 1.f);

	MeshBuilder::GetInstance()->GenerateOBJ("MortarBody", "OBJ//MortarBody.obj");
	MeshBuilder::GetInstance()->GetMesh("MortarBody")->textureID = LoadTGA("Image//Mortar.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("MortarMidHead", "OBJ//MortarHead.obj");
	MeshBuilder::GetInstance()->GetMesh("MortarMidHead")->textureID = LoadTGA("Image//Mortar.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("MortarLeftHead", "OBJ//MortarLeft.obj");
	MeshBuilder::GetInstance()->GetMesh("MortarLeftHead")->textureID = LoadTGA("Image//Mortar.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("MortarRightHead", "OBJ//MortarRight.obj");
	MeshBuilder::GetInstance()->GetMesh("MortarRightHead")->textureID = LoadTGA("Image//Mortar.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("Wall", "OBJ//Wall.obj");
	MeshBuilder::GetInstance()->GetMesh("Wall")->textureID = LoadTGA("Image//Wall.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("WallRed", "OBJ//Wall.obj");
	MeshBuilder::GetInstance()->GetMesh("WallRed")->textureID = LoadTGA("Image//Wall_Red.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("WallGreen", "OBJ//Wall.obj");
	MeshBuilder::GetInstance()->GetMesh("WallGreen")->textureID = LoadTGA("Image//Wall_Green.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("WallYellow", "OBJ//Wall.obj");
	MeshBuilder::GetInstance()->GetMesh("WallYellow")->textureID = LoadTGA("Image//Wall_Yellow.tga");

	// Gun
	MeshBuilder::GetInstance()->GenerateOBJ("gun", "OBJ//gun.obj");
	MeshBuilder::GetInstance()->GetMesh("gun")->textureID = LoadTGA("Image//gun.tga");

	// Set up the Spatial Partition and pass it to the EntityManager to manage
	CSpatialPartition::GetInstance()->Init(100, 100, 10, 10, 0.1f);
	CSpatialPartition::GetInstance()->SetMesh("GRIDMESH");
	CSpatialPartition::GetInstance()->SetCamera(&camera);
	CSpatialPartition::GetInstance()->SetLevelOfDetails(40000.0f, 90000.f);
	EntityManager::GetInstance()->SetSpatialPartition(CSpatialPartition::GetInstance());

	// Create entities into the scene
	Create::Asset("reference", Vector3(0.0f, 10.0f, 0.0f)); // Reference
	Create::Asset("lightball", Vector3(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z)); // Lightball
	
	for (int i = 0; i < 10; i++)
	{
		if (i == 4 || i == 5)
			continue;
		for (int j = 0; j < 2; j++)
		{
			GenericEntity* wall = Create::Entity("Wall", Vector3(-300 + j * 11, 0, -450 + i * 100), Vector3(10, 50, 98), true);
			wall->SetAABB(MeshBuilder::GetInstance()->GetMesh("Wall")->Max, MeshBuilder::GetInstance()->GetMesh("Wall")->Min);
			CSpatialPartition::GetInstance()->Add(wall);

			wall = Create::Entity("WallGreen", Vector3(-322 + j * 11, 0, -450 + i * 100), Vector3(10, 50, 98), true);
			wall->SetAABB(MeshBuilder::GetInstance()->GetMesh("Wall")->Max, MeshBuilder::GetInstance()->GetMesh("Wall")->Min);
			CSpatialPartition::GetInstance()->Add(wall);

			wall = Create::Entity("WallYellow", Vector3(-344 + j * 11, 0, -450 + i * 100), Vector3(10, 50, 98), true);
			wall->SetAABB(MeshBuilder::GetInstance()->GetMesh("Wall")->Max, MeshBuilder::GetInstance()->GetMesh("Wall")->Min);
			CSpatialPartition::GetInstance()->Add(wall);

			wall = Create::Entity("WallRed", Vector3(-366 + j * 11, 0, -450 + i * 100), Vector3(10, 50, 98), true);
			wall->SetAABB(MeshBuilder::GetInstance()->GetMesh("Wall")->Max, MeshBuilder::GetInstance()->GetMesh("Wall")->Min);
			CSpatialPartition::GetInstance()->Add(wall);
		}
	}

	groundEntity = Create::Ground("GRASS_DARKGREEN", "GEO_GRASS_LIGHTGREEN");
	//Create::Text3DObject("text", Vector3(0.0f, 0.0f, 0.0f), "DM2210", Vector3(10.0f, 10.0f, 10.0f), Color(0, 1, 1));
	Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

	SkyBoxEntity* theSkyBox = Create::SkyBox("SKYBOX_FRONT", "SKYBOX_BACK",
											 "SKYBOX_LEFT", "SKYBOX_RIGHT",
											 "SKYBOX_TOP", "SKYBOX_BOTTOM");

	// Customise the ground entity
	groundEntity->SetPosition(Vector3(0, 0, 0));
	groundEntity->SetScale(Vector3(100.0f, 100.0f, 100.0f));
	groundEntity->SetGrids(Vector3(10.0f, 1.0f, 10.0f));
	playerInfo->SetTerrain(groundEntity);

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	for (int i = 0; i < 3; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f,1.0f,0.0f));
	}
	textObj[0]->SetText("HELLO WORLD");
	state = START;
	wave = 1;
	SpawnMortars(wave);
}

void SceneText::SpawnMortars(int count)
{
		MortarHandler::GetInstance()->Mortars.clear();

		CMortar* mortar;
		mortar = new CMortar();
		mortar->Init();
		mortar->SetPosition(Vector3(0, 1000, 0));
		MortarHandler::GetInstance()->Mortars.push_back(mortar);

		if (count > 7)
		{
			count = 7;
		}

		for (int i = 1; i < count; i++)
		{
			float z = i * 100;
			float x = i * 100;

			if (i > 2)
			{
				float num = i - 1;
				x = num * 150;
				z = num * 100;
				
			}

			CMortar* mortar;
			mortar = new CMortar();
			mortar->Init();
			mortar->SetPosition(Vector3(x, 1000, z));
			MortarHandler::GetInstance()->Mortars.push_back(mortar);

			mortar = new CMortar();
			mortar->Init();
			mortar->SetPosition(Vector3(x, 1000, -z));
			MortarHandler::GetInstance()->Mortars.push_back(mortar);
		}
}

void SceneText::Update(double dt)
{
	if (state == START)
	{
		bool transfer = false;

		for (int i = 0; i < MortarHandler::GetInstance()->Mortars.size(); i++)
		{
			Vector3 position;

			position = Vector3(MortarHandler::GetInstance()->Mortars[i]->GetPosition());
			position.y -= dt * 500;
			if (position.y < 5)
			{
				position.y = 5;
				transfer = true;
			}
			MortarHandler::GetInstance()->Mortars[i]->SetPosition(Vector3(position.x, position.y, position.z));
		}
		if (KeyboardController::GetInstance()->IsKeyDown(' '))
		{
			for (int i = 0; i < MortarHandler::GetInstance()->Mortars.size(); i++)
			{
				Vector3 position;
				position = Vector3(MortarHandler::GetInstance()->Mortars[i]->GetPosition());
				MortarHandler::GetInstance()->Mortars[i]->SetPosition(Vector3(position.x, 5, position.z));
			}
			playerInfo->AttachCamera(&camera);
			state = PLAYING;
		}
		
		if (transfer == true)
		{
			playerInfo->AttachCamera(&camera);
			state = DELAY;
			delaytimer = 3;
		}
	}
	else if (state == DELAY)
	{
		if (delaytimer > 0)
		{
			delaytimer -= dt;
			if (delaytimer <= 0)
				state = PLAYING;
		}

		if (KeyboardController::GetInstance()->IsKeyDown(' '))
		{
			playerInfo->AttachCamera(&camera);
			state = PLAYING;
		}
	}
	else if (state == PLAYING)
	{
		// Update our entities
		EntityManager::GetInstance()->Update(dt);
		// Update the player position and other details based on keyboard and mouse inputs
		playerInfo->Update(dt);
		if (MortarHandler::GetInstance()->Mortars.size() == 0)
		{
			state = START;
			playerInfo->DetachCamera();
			GraphicsManager::GetInstance()->AttachCamera(&camera);
			camera.SetCameraPos(Vector3(-800, 865, -6.5));
			camera.SetCameraTarget(Vector3(0, 0, 0));
			camera.SetCameraUp(Vector3(0, 1, 0));
			SpawnMortars(++wave);
		}
	}

	// THIS WHOLE CHUNK TILL <THERE> CAN REMOVE INTO ENTITIES LOGIC! Or maybe into a scene function to keep the update clean
	if(KeyboardController::GetInstance()->IsKeyDown('1'))
		glEnable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('2'))
		glDisable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(KeyboardController::GetInstance()->IsKeyDown('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	if(KeyboardController::GetInstance()->IsKeyDown('5'))
	{
		lights[0]->type = Light::LIGHT_POINT;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('6'))
	{
		lights[0]->type = Light::LIGHT_DIRECTIONAL;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('7'))
	{
		lights[0]->type = Light::LIGHT_SPOT;
	}

	if (EntityManager::GetInstance()->GetEntityHolder())
	{
		if (KeyboardController::GetInstance()->IsKeyDown('I'))
			EntityManager::GetInstance()->GetEntityHolder()->SetPosition(Vector3(EntityManager::GetInstance()->GetEntityHolder()->GetPosition().x, EntityManager::GetInstance()->GetEntityHolder()->GetPosition().y, EntityManager::GetInstance()->GetEntityHolder()->GetPosition().z - 10.f * dt));
		if (KeyboardController::GetInstance()->IsKeyDown('K'))
			EntityManager::GetInstance()->GetEntityHolder()->SetPosition(Vector3(EntityManager::GetInstance()->GetEntityHolder()->GetPosition().x, EntityManager::GetInstance()->GetEntityHolder()->GetPosition().y, EntityManager::GetInstance()->GetEntityHolder()->GetPosition().z + 10.f * dt));
		if (KeyboardController::GetInstance()->IsKeyDown('J'))
			EntityManager::GetInstance()->GetEntityHolder()->SetPosition(Vector3(EntityManager::GetInstance()->GetEntityHolder()->GetPosition().x + 10.f * dt, EntityManager::GetInstance()->GetEntityHolder()->GetPosition().y, EntityManager::GetInstance()->GetEntityHolder()->GetPosition().z));
		if (KeyboardController::GetInstance()->IsKeyDown('L'))
			EntityManager::GetInstance()->GetEntityHolder()->SetPosition(Vector3(EntityManager::GetInstance()->GetEntityHolder()->GetPosition().x - 10.f * dt, EntityManager::GetInstance()->GetEntityHolder()->GetPosition().y, EntityManager::GetInstance()->GetEntityHolder()->GetPosition().z));
	}

	if(KeyboardController::GetInstance()->IsKeyDown('O'))
		lights[0]->position.y -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('P'))
		lights[0]->position.y += (float)(10.f * dt);

	if (KeyboardController::GetInstance()->IsKeyReleased('M'))
	{
		CSceneNode* theNode = CSceneGraph::GetInstance()->GetNode(1);
		Vector3 pos = theNode->GetEntity()->GetPosition();
		theNode->GetEntity()->SetPosition(Vector3(pos.x + 50.0f, pos.y, pos.z + 50.0f));
	}
	if (KeyboardController::GetInstance()->IsKeyReleased('N'))
	{
		CSpatialPartition::GetInstance()->PrintSelf();
	}

	// if the left mouse button was released
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::LMB))
	{
		cout << "Left Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::RMB))
	{
		cout << "Right Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::MMB))
	{
		cout << "Middle Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) != 0.0)
	{
		cout << "Mouse Wheel has offset in X-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) << endl;
	}
	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) != 0.0)
	{
		cout << "Mouse Wheel has offset in Y-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) << endl;
	}
	// <THERE>

	//camera.Update(dt); // Can put the camera into an entity rather than here (Then we don't have to write this)

	GraphicsManager::GetInstance()->UpdateLights(dt);

	// Update the 2 text object values. NOTE: Can do this in their own class but i'm lazy to do it now :P
	// Eg. FPSRenderEntity or inside RenderUI for LightEntity
	std::ostringstream ss;
	ss.precision(5);
	float fps = (float)(1.f / dt);
	ss << "FPS: " << fps;
	textObj[1]->SetText(ss.str());

	std::ostringstream ss1;
	ss1.precision(4);
	ss1 << "Player:" << playerInfo->GetPos();
	textObj[2]->SetText(ss1.str());
}

void SceneText::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(&camera);
	EntityManager::GetInstance()->Render();
	playerInfo->Render();

	// Setup 2D pipeline then render 2D
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();
	EntityManager::GetInstance()->RenderUI();
}

void SceneText::Exit()
{
	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
	playerInfo->DetachCamera();

	if (playerInfo->DropInstance() == false)
	{
#if _DEBUGMODE==1
		cout << "Unable to drop PlayerInfo class" << endl;
#endif
	}

	// Delete the lights
	delete lights[0];
	delete lights[1];
}
