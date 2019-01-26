#include <GameManager.h>
#include <GLFW/glfw3.h>
#include <ctime>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <sstream>

GameManager::GameManager(NodePtr graph, string path, float* hDir, float* vDir) {
	sceneGraph = graph;
	horizontalDirection = hDir;
	verticalDirection = vDir;
	spacebar = false;
	SoundEngine = createIrrKlangDevice();
	SoundEngine->setSoundVolume((float)0.008);
	SOUND_PATH = path;
	string str = SOUND_PATH;
	SoundEngine->play2D(str.append("Space Trip.mp3").c_str(), GL_TRUE);
}

GameManager::~GameManager()
{
	SoundEngine->drop();
}

void GameManager::GameOps()
{
	stringstream ss;
	if (GAME_STATE == IN_GAME) 
	{
		ShootIfPossible();
		spawnEnemy();
		EnemyShooting();
		spawnAsteroid();
		spawnPowerUps(HEALTH_POWERUP_COOLDOWN, healthPowerUpList, healthPowerUp);
		spawnPowerUps(DOUBLE_SHOTS_POWERUP_COOLDOWN, doubleShotsPowerUpList, doubleShotPowerUp);
		
		movePlayer();
		moveObjects(bulletList);
		moveObjects(enemyList);
		moveObjects(asteroidList);
		moveObjects(healthPowerUpList);
		moveObjects(doubleShotsPowerUpList);

		DoCollision();
		removeObjectOutsideTheCamera(bulletList);
		removeObjectOutsideTheCamera(enemyList);
		removeObjectOutsideTheCamera(healthPowerUpList);
		removeObjectOutsideTheCamera(doubleShotsPowerUpList);
		removeAsteroidsOutsideTheCamera(asteroidList);
		ss << PLAYER_LIFES;
		Text.get()->RenderText("Lives: " + ss.str(), 10.0f, 10.0f, 1.0f);
		ss.str("");
		ss << SCORE;
		Text.get()->RenderText("Score: " + ss.str(), 10.0f, 55.0f, 1.0f);
	}
	if(GAME_STATE == IN_MENU)
	{
		Text->RenderText("Press ENTER to start", 500.0f, 360.0f, 1.0f);
		if (!gameInit)
		{
			ss << SCORE;
			Text.get()->RenderText("Score: " + ss.str(), 590.0f, 400.0f, 1.0f);
		}
		if (EnterIsPushed()) {
			ResetGame();
		}
	}
	if (bulletList.size() >= 00) {
		//cout << bulletList.size() + enemyList.size() + asteroidList.size()<< " "<< sceneGraph->GetChildren().size() << endl;
		//cout << enemyList.size() << endl;
		//cout << sceneGraph->GetChildren().size();
	}
	
}

void GameManager::ResetGame()
{
	sceneGraph->GetChildren().clear();
	enemyList.clear();
	bulletList.clear();
	asteroidList.clear();
	player->transform.setPosition(-17.0f, 0.0f, 0.0f);
	PLAYER_LIFES = 3;
	SCORE = 0;
	GAME_STATE = IN_GAME;
	gameInit = false;
}

void GameManager::SetFrameTime(float frameTime)
{
	FRAME_TIME = frameTime;
}

//void GameManager::SetSoundPath(string& path)
//{
//	GameManager::SOUND_PATH = path;
//}

void GameManager::setPlayer(GraphNode* playerPtr)
{
	player = shared_ptr<GraphNode>(playerPtr);
}

void GameManager::setBullet(GraphNode* bulletPtr)
{
	//bullet = std::make_shared<GraphNode>(*bulletPtr);
	bullet = NodePtr(bulletPtr);
}

void GameManager::setEnemyShip(GraphNode* enemy)
{
	//enemyShip = std::make_shared<GraphNode>(*enemy);
	enemyShip = NodePtr(enemy);
}

void GameManager::SetTextRenderer(shared_ptr<TextRenderer>& text) {
	Text = text;
}

void GameManager::SetAsteroid(GraphNode * ast)
{
	asteroid = NodePtr(ast);
}

void GameManager::SetHealthPowerUp(GraphNode * health)
{
	healthPowerUp = NodePtr(health);
}
void GameManager::SetDoubleShotsPowerUp(GraphNode * powerUp) {
	doubleShotPowerUp = NodePtr(powerUp);
}

void GameManager::addBullet(glm::vec3& position, glm::vec3& direction, GraphNode* shootingObject, WEAPON mode)
{
	
	if (mode == SINGLE_SHOT) 
	{
		NodePtr oneBullet = std::make_shared<GraphNode>(bullet.get());
		oneBullet->transform.setPosition(position.x, position.y, position.z);
		oneBullet->SetDirection(direction);
		oneBullet->SetSpeed(BULLET_SPEED);
		oneBullet->SetShootingObject(shootingObject);
		float angleRadians = glm::angle(glm::vec3(1, 0, 0), direction);

		float angle = angleRadians * (float)(180 / M_PI);
		if (player->transform.getPosition().y > shootingObject->transform.getPosition().y)
			oneBullet.get()->transform.Rotate(angle, glm::vec3(0, 0, 1));
		else
			oneBullet.get()->transform.Rotate(-angle, glm::vec3(0, 0, 1));


		bulletList.push_back(oneBullet);
		sceneGraph->AddChild(oneBullet.get());
	}
	else if (mode == DOUBLE_SHOT)
	{
		NodePtr firstBullet = std::make_shared<GraphNode>(bullet.get());
		firstBullet->transform.setPosition(position.x, position.y + 0.2f, position.z);
		firstBullet->SetDirection(direction);
		firstBullet->SetSpeed(BULLET_SPEED);
		firstBullet->SetShootingObject(shootingObject);

		NodePtr secondBullet = std::make_shared<GraphNode>(bullet.get());
		secondBullet->transform.setPosition(position.x, position.y - 0.25f, position.z);
		secondBullet->SetDirection(direction);
		secondBullet->SetSpeed(BULLET_SPEED);
		secondBullet->SetShootingObject(shootingObject);

		bulletList.push_back(firstBullet);
		bulletList.push_back(secondBullet);
		sceneGraph->AddChild(firstBullet.get());
		sceneGraph->AddChild(secondBullet.get());
	}
	string str = SOUND_PATH;
	ISound* soundLoaded = SoundEngine->play2D(str.append("Blast.mp3").c_str(), GL_FALSE, GL_TRUE);
	if (soundLoaded) {
		soundLoaded->setVolume(0.5f);
		soundLoaded->setIsPaused(false);
		soundLoaded->drop();
	}
}

void GameManager::spawnEnemy()
{
	float time = (float)glfwGetTime();
	if( time >= ENEMY_COOLDOWN)
	{
		ENEMY_COOLDOWN = (float)glfwGetTime() + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 1.0f;;
		float y = -11.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-11.0f) )));
		
		NodePtr tmp = std::make_shared<GraphNode>(enemyShip.get());
		tmp->SetDirection(glm::vec3(-1.0f, 0.0f, 0.0f));
		tmp->SetSpeed(ENEMY_SPEED);
		tmp->transform.setPosition(25.0f, y, 0.0f);
		enemyList.push_back(tmp);
		sceneGraph->AddChild(tmp.get());
		
	}
}

void GameManager::spawnAsteroid()
{
	float time = (float)glfwGetTime();
	if (time >= ASTEROID_COOLDOWN)
	{
		ASTEROID_COOLDOWN = (float)glfwGetTime() + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 1.5f;;
		float y = -11.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-11.0f))));
		float z = -30.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-5.0f - (-30.0f))));
		NodePtr tmp = std::make_shared<GraphNode>(asteroid.get());
		tmp->transform.setPosition(50.0f, y, z);
		tmp->SetSpeed(static_cast <float> (((rand()) / static_cast <float> (RAND_MAX)))* 10.0f);
		float i = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) + 1;
		float j = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - (-1.0f))));
		j /= 2;
		tmp->SetDirection(glm::vec3(-i, j, 0));
		asteroidList.push_back(tmp);
		sceneGraph->AddChild(tmp.get());
	}
}

void GameManager::spawnPowerUps(float& cooldown, vector<NodePtr>& powerUps, NodePtr& prefab)
{
	float time = (float)glfwGetTime();
	if (time >= cooldown)
	{
		cooldown = (float)glfwGetTime() + 10.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (20.0f - 10.0f)));
		NodePtr tmp = std::make_shared<GraphNode>(prefab.get());
		
		float y = -11.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-11.0f))));
		
		tmp->transform.setPosition(25.0f, y, -0.5f);
		tmp->SetSpeed(9.0f);
		tmp->SetDirection(glm::vec3(-1, 0, 0));
		powerUps.push_back(tmp);
		sceneGraph->AddChild(tmp.get());
	}
}

void GameManager::movePlayer()
{
	glm::vec3 shipPosition = player->transform.getPosition();
	float xVal = shipPosition.x + *horizontalDirection * PLAYER_SPEED * FRAME_TIME;
	float yVal = shipPosition.y + *verticalDirection * PLAYER_SPEED * FRAME_TIME;
	if (xVal >= -19.0f && xVal <= 16.0f && yVal <= 11.0f && yVal >= -12.0f) {
		player->transform.setPosition(xVal, yVal, 0.0f);
	}
}

void GameManager::moveObjects(vector<NodePtr>& objectContainer)
{
	for (NodePtr& gameObject : objectContainer) {
		glm::vec3 gameObjectPos = gameObject->transform.getPosition();
		float x = gameObjectPos.x + gameObject->GetDirection().x * gameObject->GetSpeed() * FRAME_TIME;
		float y = gameObjectPos.y + gameObject->GetDirection().y * gameObject->GetSpeed() * FRAME_TIME;
		gameObject->transform.setPosition( x, y, gameObjectPos.z);
	}
}

void GameManager::spacebarPushed(bool pushed)
{
	spacebar = pushed;
}

void GameManager::enterPushed(bool pushed)
{
	enter = pushed;
}

void GameManager::removeObjectOutsideTheCamera(vector<NodePtr>& v)
{
	for (NodePtr& object : v) 
	{
		glm::vec3 xPos = object->transform.getPosition();
		
		if (xPos.x < -30.0f || xPos.x > 30.0f) {
			sceneGraph->RemoveNode(object.get());
			removeNode(v, object);
			break;
		}
	}
}

void GameManager::removeAsteroidsOutsideTheCamera(vector<NodePtr>& v)
{
	for (NodePtr& object : v)
	{
		glm::vec3 xPos = object->transform.getPosition();

		if (xPos.x < -100.0f || xPos.x > 100.0f) {
			sceneGraph->RemoveNode(object.get());
			removeNode(v, object);
			break;
		}
	}
}

void GameManager::removeNode(vector<NodePtr>& v, NodePtr& node) {

	std::vector<NodePtr>::iterator i = std::find_if(v.begin(), v.end(),
		[&node](NodePtr& x) //lambda
	{
		return x == node;
	});

	if (i != v.end()) {
		v.erase(i);
	}
		
}

void GameManager::ShootIfPossible()
{
	static volatile bool playerShot = false;
	if (playerShot) {
		COOLDOWN = (float)glfwGetTime() + 0.4f;
	}
	if (SpacebarIsPushed() && (float)glfwGetTime() >= COOLDOWN)
	{
		addBullet(player->transform.getPosition(), glm::vec3(1, 0, 0), player.get(), WEAPON_MODE);
		playerShot = true;
	}
	else
	{
		playerShot = false;
	}
}

void GameManager::DoCollision()
{
	for (NodePtr& healtPowerUp : healthPowerUpList)
	{
		if (CheckCollision(player.get(), healtPowerUp.get()))
		{
			healtPowerUp->Active(false);
			PLAYER_LIFES += 1;
			string str = SOUND_PATH;
			ISound* soundLoaded = SoundEngine->play2D(str.append("PowerUp.mp3").c_str(), GL_FALSE, GL_TRUE);
			if (soundLoaded) {
				soundLoaded->setVolume(2.0f);
				soundLoaded->setIsPaused(false);
				soundLoaded->drop();
			}
		}
	}

	for (NodePtr& doubleShotPowerUp : doubleShotsPowerUpList)
	{
		if (CheckCollision(player.get(), doubleShotPowerUp.get()))
		{
			doubleShotPowerUp->Active(false);
			if (WEAPON_MODE == DOUBLE_SHOT) {
				SCORE += 20;
			}
			WEAPON_MODE = DOUBLE_SHOT;
			string str = SOUND_PATH;
			ISound* soundLoaded = SoundEngine->play2D(str.append("PowerUp.mp3").c_str(), GL_FALSE, GL_TRUE);
			if (soundLoaded) {
				soundLoaded->setVolume(2.0f);
				soundLoaded->setIsPaused(false);
				soundLoaded->drop();
			}
		}
	}

	for (NodePtr& bullet : bulletList) {
		for (NodePtr& enemy : enemyList) {
			if (CheckCollision(bullet.get(), enemy.get(), true))
			{
				bullet->Active(false);
				enemy->Active(false);
				string str = SOUND_PATH;
				ISound* soundLoaded = SoundEngine->play2D(str.append("Explosion.mp3").c_str(), GL_FALSE, GL_TRUE);
				if (soundLoaded) {
					soundLoaded->setVolume(0.5f);
					soundLoaded->setIsPaused(false);
					soundLoaded->drop();
				}
				SCORE += 10;
				break;
			}
				
		}
		if (CheckCollision(bullet.get(), player.get(), true))
		{
			bullet->Active(false);

			PLAYER_LIFES -= 1;
			WEAPON_MODE = SINGLE_SHOT;
			string str = SOUND_PATH;
			ISound* soundLoaded = SoundEngine->play2D(str.append("Explosion.mp3").c_str(), GL_FALSE, GL_TRUE);
			if (soundLoaded) {
				soundLoaded->setVolume(0.5f);
				soundLoaded->setIsPaused(false);
				soundLoaded->drop();
			}
			cout << "Gracz trafiony" << endl;
			if (PLAYER_LIFES == 0)
				GAME_STATE = IN_MENU;
			break;
		}
	}
	
}


bool GameManager::CheckCollision(GraphNode* one, GraphNode* two, bool bulletMode)
{
	if(bulletMode)
		if (one->GetShootingObject() == two)
			return false;

	if (one->IsActive() == false || two->IsActive() == false)
		return false;

	float d1x = two->GetMin().x - one->GetMax().x;
	float d1y = two->GetMin().y - one->GetMax().y;
	float d2x = one->GetMin().x - two->GetMax().x;
	float d2y = one->GetMin().y - two->GetMax().y;

	if (d1x > 0.0f || d1y > 0.0f)
		return false;

	if (d2x > 0.0f || d2y > 0.0f)
		return false;

	return true;
}

void GameManager::EnemyShooting()
{
	for (NodePtr& enemy : enemyList) {
		float time = (float)glfwGetTime();
		if (time >= enemy->GetShootingCooldown() && enemy->IsActive())
		{
			enemy->SetShootingCooldown((float)glfwGetTime() + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 1.0f);
			if( enemy->transform.getPosition().x >= player->transform.getPosition().x + 2.5)
			{
				glm::vec3 dir = glm::normalize(player->transform.getPosition() - enemy->transform.getPosition());

				addBullet(enemy->transform.getPosition(), dir, enemy.get(), WEAPON::SINGLE_SHOT);
			}
			else 
			{
				addBullet(enemy->transform.getPosition(), glm::vec3(-1.0f, 0.0f, 0.0f), enemy.get(), WEAPON::SINGLE_SHOT);
			}
			
		}
	}
}

bool GameManager::SpacebarIsPushed()
{
	if (spacebar)
		return true;
	else
		return false;
}

bool GameManager::EnterIsPushed()
{
	if (enter)
		return true;
	else
		return false;
}
