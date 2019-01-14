#include <GameManager.h>
#include <GLFW/glfw3.h>
#include <ctime>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <sstream>

GameManager::GameManager(NodePtr graph, float* hDir, float* vDir) {
	sceneGraph = graph;
	horizontalDirection = hDir;
	verticalDirection = vDir;
	spacebar = false;
	SoundEngine = createIrrKlangDevice();
	SoundEngine->setSoundVolume((float)0.008);
	SoundEngine->play2D("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\sounds\\Space Trip.mp3", GL_TRUE);
	
}

GameManager::~GameManager()
{
	SoundEngine->drop();
}

void GameManager::GameOps()
{
	stringstream ss;
	if (gameState == IN_GAME) 
	{
		ShootIfPossible();
		spawnEnemy();
		EnemyShooting();
		spawnAsteroid();
		spawnPowerUps();

		movePlayer();
		moveBullets();
		moveEnemy();
		moveAsteroids();
		moveHealthPowerUps();

		DoCollision();
		removeObjectOutsideTheCamera(bulletList);
		removeObjectOutsideTheCamera(enemyList);
		removeAsteroidsOutsideTheCamera(asteroidList);
		removeObjectOutsideTheCamera(healthPowerUpList);
		ss << playerLifes;
		Text.get()->RenderText("Lives: " + ss.str(), 10.0f, 10.0f, 1.0f);
		ss.str("");
		ss << score;
		Text.get()->RenderText("Score: " + ss.str(), 10.0f, 55.0f, 1.0f);
	}
	if(gameState == IN_MENU)
	{
		Text->RenderText("Press ENTER to start", 500.0f, 360.0f, 1.0f);
		if (!gameInit)
		{
			ss << score;
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
	player->setPosition(-17.0f, 0.0f, 0.0f);
	playerLifes = 3;
	score = 0;
	gameState = IN_GAME;
	gameInit = false;
}

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

void GameManager::addBullet(glm::vec3& position, glm::vec3& direction, GraphNode* shootingObject)
{
	NodePtr tmp = std::make_shared<GraphNode>(bullet.get());
	
	tmp->setPosition(position.x, position.y, position.z);
	tmp->SetDirection(direction);
	tmp->SetSpeed(bulletSpeed);
	tmp->SetShootingObject(shootingObject);
	tmp->Scale(glm::vec3(0.05f, 0.05f, 0.05f));
	float angleRadians = glm::angle(glm::vec3(1, 0, 0), direction);

	float angle = angleRadians * (float)(180 / M_PI);
	if(player->getPosition().y > shootingObject->getPosition().y)
		tmp.get()->Rotate(angle, glm::vec3(0, 0, 1));
	else
		tmp.get()->Rotate(-angle, glm::vec3(0, 0, 1));
	
	
	bulletList.push_back(tmp);
	sceneGraph->AddChild(tmp.get());
	ISound* soundLoaded = SoundEngine->play2D("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\sounds\\Blast.mp3", GL_FALSE, GL_TRUE);
	if (soundLoaded) {
		soundLoaded->setVolume(0.5f);
		soundLoaded->setIsPaused(false);
		soundLoaded->drop();
	}
}

void GameManager::spawnEnemy()
{
	float time = (float)glfwGetTime();
	if( time >= enemyCooldown)
	{
		enemyCooldown = (float)glfwGetTime() + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 1.0f;;
		float y = -11.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-11.0f) )));
		
		NodePtr tmp = std::make_shared<GraphNode>(enemyShip.get());
		tmp->setPosition(25.0f, y, 0.0f);
		enemyList.push_back(tmp);
		sceneGraph->AddChild(tmp.get());
		
	}
}

void GameManager::spawnAsteroid()
{
	float time = (float)glfwGetTime();
	if (time >= asteroidCooldown)
	{
		asteroidCooldown = (float)glfwGetTime() + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 1.5f;;
		float y = -11.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-11.0f))));
		float z = -30.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-5.0f - (-30.0f))));
		NodePtr tmp = std::make_shared<GraphNode>(asteroid.get());
		tmp->setPosition(50.0f, y, z);
		tmp->SetSpeed(static_cast <float> ((rand()) / static_cast <float> (RAND_MAX))/5);
		float i = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) + 1;
		float j = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - (-1.0f))));
		j /= 2;
		tmp->SetDirection(glm::vec3(-i, j, 0));
		asteroidList.push_back(tmp);
		sceneGraph->AddChild(tmp.get());
	}
}

void GameManager::spawnPowerUps()
{
	float time = (float)glfwGetTime();
	if (time >= powerUpCooldown)
	{
		powerUpCooldown = (float)glfwGetTime() + 10.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (20.0f - 10.0f)));
		NodePtr tmp = std::make_shared<GraphNode>(healthPowerUp.get());
		
		float y = -11.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-11.0f))));
		
		tmp->setPosition(25.0f, y, -0.5f);
		tmp->SetSpeed(0.2f);
		tmp->SetDirection(glm::vec3(-1, 0, 0));
		healthPowerUpList.push_back(tmp);
		sceneGraph->AddChild(tmp.get());
	}
}

void GameManager::movePlayer()
{
	glm::vec3 shipPosition = player->getPosition();
	float xVal = shipPosition.x + *horizontalDirection;
	float yVal = shipPosition.y + *verticalDirection;
	if (xVal >= -19.0f && xVal <= 16.0f && yVal <= 11.0f && yVal >= -12.0f) {
		player->setPosition(xVal, shipPosition.y + *verticalDirection, 0.0f);
	}
}

void GameManager::moveBullets()
{
	for (NodePtr& bullet : bulletList) {
		glm::vec3 bulletPos = bullet->getPosition();
		
		bullet->setPosition(bulletPos.x + bullet->GetDirection().x * bullet->GetSpeed(), 
			bulletPos.y + bullet->GetDirection().y * bullet->GetSpeed(), bulletPos.z);
	}
}

void GameManager::moveEnemy()
{
	for (NodePtr& enemy : enemyList)
	{
		glm::vec3 enemyPos = enemy->getPosition();
		enemy->setPosition(enemyPos.x - enemySpeed, enemyPos.y, enemyPos.z);
	}
}

void GameManager::moveAsteroids()
{
	for (NodePtr& asteroid : asteroidList)
	{
		glm::vec3 asteroidPos = asteroid->getPosition();
		asteroid->setPosition(asteroidPos.x + asteroid->GetDirection().x * asteroid->GetSpeed(), 
			asteroidPos.y + asteroid->GetDirection().y * asteroid->GetSpeed(), 
			asteroidPos.z);
	}
}

void GameManager::moveHealthPowerUps()
{
	for (NodePtr& powerUp : healthPowerUpList) {
		glm::vec3 powerUpPos = powerUp->getPosition();

		powerUp->setPosition(powerUpPos.x + powerUp->GetDirection().x * powerUp->GetSpeed(),
			powerUpPos.y + powerUp->GetDirection().y * powerUp->GetSpeed(), powerUpPos.z);
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
		glm::vec3 xPos = object->getPosition();
		
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
		glm::vec3 xPos = object->getPosition();

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
		cooldown = (float)glfwGetTime() + 0.4f;
	}
	if (SpacebarIsPushed() && (float)glfwGetTime() >= cooldown)
	{
		addBullet(player->getPosition(), glm::vec3(1, 0, 0), player.get());
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
			playerLifes += 1;
			ISound* soundLoaded = SoundEngine->play2D("C:/Semestr5/PAG/openGL/scrolling-shooter/Build/src/res/sounds/PowerUp.mp3", GL_FALSE, GL_TRUE);
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
				ISound* soundLoaded = SoundEngine->play2D("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\sounds\\Explosion.mp3", GL_FALSE, GL_TRUE);
				if (soundLoaded) {
					soundLoaded->setVolume(0.5f);
					soundLoaded->setIsPaused(false);
					soundLoaded->drop();
				}
				score += 10;
				break;
			}
				
		}
		if (CheckCollision(bullet.get(), player.get(), true))
		{
			bullet->Active(false);

			playerLifes -= 1;
			ISound* soundLoaded = SoundEngine->play2D("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\sounds\\Explosion.mp3", GL_FALSE, GL_TRUE);
			if (soundLoaded) {
				soundLoaded->setVolume(0.5f);
				soundLoaded->setIsPaused(false);
				soundLoaded->drop();
			}
			cout<<SoundEngine->getSoundSourceCount()<<endl;
			cout << "Gracz trafiony" << endl;
			if (playerLifes == 0)
				gameState = IN_MENU;
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
			if( enemy->getPosition().x >= player->getPosition().x + 1)
			{
				glm::vec3 dir = glm::normalize(player->getPosition() - enemy->getPosition());

				addBullet(enemy->getPosition(), dir, enemy.get());
			}
			else 
			{
				addBullet(enemy->getPosition(), glm::vec3(-1.0f, 0.0f, 0.0f), enemy.get());
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
