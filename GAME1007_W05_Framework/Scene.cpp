#include "Scene.h"
#include "tinyxml2.h"
#include <iostream>
#include <cassert>
using namespace std;
using namespace tinyxml2;

Scene::Type Scene::sCurrent;
std::array<Scene*, Scene::COUNT> Scene::sScenes;

void Scene::Init()
{
	sScenes[TITLE] = new TitleScene;
	sScenes[GAME] = new GameScene;
	sScenes[LAB_1A] = new Lab1AScene;
	sCurrent = LAB_1A;
	sScenes[sCurrent]->OnEnter();
}

void Scene::Exit()
{
	for (size_t i = 0; i < sScenes.size(); i++)
		delete sScenes[i];
}

void Scene::Update(float dt)
{
	sScenes[sCurrent]->OnUpdate(dt);
}

void Scene::Render()
{
	sScenes[sCurrent]->OnRender();
}

void Scene::Change(Type type)
{
	assert(sCurrent != type);
	sScenes[sCurrent]->OnExit();
	sCurrent = type;
	sScenes[sCurrent]->OnEnter();
}

void OnTitleGui(void* data);

void TitleScene::OnEnter()
{
	SetGuiCallback(OnTitleGui, this);
}

void TitleScene::OnExit()
{
	SetGuiCallback(nullptr, nullptr);
}

void TitleScene::OnUpdate(float dt)
{
}

void TitleScene::OnRender()
{
	DrawRect(mBackRec, { 0, 0, 0, 255 });
	DrawRect(mFrontRec, { 255, 255, 255, 255 });
}

void OnTitleGui(void* data)
{
	TitleScene& scene = *(TitleScene*)data;
	if (ImGui::Button("Begin!"))
	{
		Scene::Change(Scene::GAME);
	}
}

void OnGameGui(void* data);

GameScene::GameScene()
{
	mShipTex = LoadTexture("../Assets/img/enterprise.png");
}

GameScene::~GameScene()
{
	UnloadTexture(mShipTex);
}

void GameScene::OnEnter()
{
	SetGuiCallback(OnGameGui, this);

	XMLDocument doc;
	doc.LoadFile("Game.xml");

	XMLElement* gameData = doc.FirstChildElement();
	XMLElement* shipData = gameData->FirstChildElement();
	shipData->QueryAttribute("x", &mShipRec.x);
	shipData->QueryAttribute("y", &mShipRec.y);
	shipData->QueryAttribute("w", &mShipRec.w);
	shipData->QueryAttribute("h", &mShipRec.h);
	shipData->QueryAttribute("speed", &mShipSpeed);
}

void GameScene::OnExit()
{
	XMLDocument doc;
	XMLNode* root = doc.NewElement("Game");
	doc.InsertEndChild(root);

	XMLElement* ship = doc.NewElement("Ship");
	ship->SetAttribute("x", mShipRec.x);
	ship->SetAttribute("y", mShipRec.y);
	ship->SetAttribute("w", mShipRec.w);
	ship->SetAttribute("h", mShipRec.h);
	ship->SetAttribute("speed", mShipSpeed);
	root->InsertEndChild(ship);

	doc.SaveFile("Game.xml");

	SetGuiCallback(nullptr, nullptr);
}

void GameScene::OnUpdate(float dt)
{
	if (IsKeyDown(SDL_SCANCODE_A))
	{
		mShipRec.x -= mShipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_D))
	{
		mShipRec.x += mShipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_W))
	{
		mShipRec.y -= mShipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_S))
	{
		mShipRec.y += mShipSpeed * dt;
	}
}

void GameScene::OnRender()
{
	DrawTexture(mShipTex, mShipRec);
}

void OnGameGui(void* data)
{
	GameScene& scene = *(GameScene*)data;

	if (ImGui::Button("End"))
	{
		Scene::Change(Scene::TITLE);
	}
}

Lab1AScene::Lab1AScene()
{
	mTexEnterprise = LoadTexture("../Assets/img/enterprise.png");
	mTexD7 = LoadTexture("../Assets/img/d7.png");

	for (size_t i = 0; i < mShips.size(); i++)
	{
		Ship& ship = mShips[i];
		ship.rec.x = SCREEN_WIDTH * 0.1f;

		const float space = SCREEN_HEIGHT / mShips.size();
		ship.rec.y = i * space + space * 0.5f;

		ship.rec.w = 60.0f;
		ship.rec.h = 40.0f;

		ship.dir = { 1.0f, 1.0f };

		if (i % 2 == 0)
			ship.tex = mTexEnterprise;
		else
			ship.tex = mTexD7;
	}
}

Lab1AScene::~Lab1AScene()
{
	UnloadTexture(mTexD7);
	UnloadTexture(mTexEnterprise);
}

void Lab1AScene::OnUpdate(float dt)
{
	const float speed = 500.0f;
	for (Ship& ship : mShips)
	{
		Rect& rec = ship.rec;

		if (rec.x + rec.w >= SCREEN_WIDTH)
			ship.dir.x = -1.0f;
		else if (rec.x <= 0.0f)
			ship.dir.x = 1.0f;

		if (rec.y + rec.h >= SCREEN_HEIGHT)
			ship.dir.y = -1.0f;
		else if (rec.y <= 0.0f)
			ship.dir.y = 1.0f;

		rec.x += ship.dir.x * speed * dt;
		rec.y += ship.dir.y * speed * dt;
	}
}

void Lab1AScene::OnRender()
{
	for (const Ship& ship : mShips)
		DrawTexture(ship.tex, ship.rec);
}
