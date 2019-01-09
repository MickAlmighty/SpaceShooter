#include <GameManager.h>
#include <GLFW/glfw3.h>
#include <ctime>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <sstream>

GameManager::GameManager(shared_ptr<GraphNode> graph, float* hDir, float* vDir) {
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

		movePlayer();
		moveBullets();
		moveEnemy();
		moveAsteroids();

		DoCollision();
		removeObjectOutsideTheCamera(bulletList);
		removeObjectOutsideTheCamera(enemyList);
		removeAsteroidsOutsideTheCamera(asteroidList);
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
		//cout << bulletList.size() << endl;
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
	bullet = shared_ptr<GraphNode>(bulletPtr);
}

void GameManager::setEnemyShip(GraphNode* enemy)
{
	//enemyShip = std::make_shared<GraphNode>(*enemy);
	enemyShip = shared_ptr<GraphNode>(enemy);
}

void GameManager::SetTextRenderer(shared_ptr<TextRenderer>& text) {
	Text = text;
}

void GameManager::SetAsteroid(GraphNode * ast)
{
	asteroid = shared_ptr<GraphNode>(ast);
}

void GameManager::addBullet(glm::vec3& position, glm::vec3& direction, GraphNode* shootingObject)
{
	shared_ptr<GraphNode> tmp = std::make_shared<GraphNode>(bullet.get());
	
	tmp->setPosition(position.x, position.y, position.z);
	tmp->SetDirection(direction);
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
	SoundEngine->play2D("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\sounds\\Blast.mp3", GL_FALSE);
}

void GameManager::spawnEnemy()
{
	float time = (float)glfwGetTime();
	if( time >= enemyCooldown)
	{
		enemyCooldown = (float)glfwGetTime() + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 1.0f;;
		float y = -11.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-11.0f) )));
		
		shared_ptr<GraphNode> tmp = std::make_shared<GraphNode>(enemyShip.get());
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
		asteroidCooldown = (float)glfwGetTime() + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 3.0f;;
		float y = -11.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-11.0f))));
		float z = -30.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-5.0f - (-30.0f))));
		shared_ptr<GraphNode> tmp = std::make_shared<GraphNode>(asteroid.get());
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
	for (shared_ptr<GraphNode>& bullet : bulletList) {
		glm::vec3 bulletPos = bullet->getPosition();
		
		bullet->setPosition(bulletPos.x + bullet->GetDirection().x * bulletSpeed, bulletPos.y + bullet->GetDirection().y * bulletSpeed, bulletPos.z);
	}
}

void GameManager::moveEnemy()
{
	for (shared_ptr<GraphNode>& enemy : enemyList)
	{
		glm::vec3 enemyPos = enemy->getPosition();
		enemy->setPosition(enemyPos.x - enemySpeed, enemyPos.y, enemyPos.z);
	}
}

void GameManager::moveAsteroids()
{
	for (shared_ptr<GraphNode>& asteroid : asteroidList)
	{
		glm::vec3 asteroidPos = asteroid->getPosition();
		asteroid->setPosition(asteroidPos.x + asteroid->GetDirection().x * asteroid->GetSpeed(), 
			asteroidPos.y + asteroid->GetDirection().y * asteroid->GetSpeed(), 
			asteroidPos.z);
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

void GameManager::removeObjectOutsideTheCamera(vector<shared_ptr<GraphNode>>& v)
{
	for (shared_ptr<GraphNode>& object : v) 
	{
		glm::vec3 xPos = object->getPosition();
		
		if (xPos.x < -30.0f || xPos.x > 30.0f) {
			sceneGraph->RemoveNode(object.get());
			removeNode(v, object);
			break;
		}
	}
}

void GameManager::removeAsteroidsOutsideTheCamera(vector<shared_ptr<GraphNode>>& v)
{
	for (shared_ptr<GraphNode>& object : v)
	{
		glm::vec3 xPos = object->getPosition();

		if (xPos.x < -100.0f || xPos.x > 100.0f) {
			sceneGraph->RemoveNode(object.get());
			removeNode(v, object);
			break;
		}
	}
}

void GameManager::removeNode(vector<shared_ptr<GraphNode>>& v, shared_ptr<GraphNode>& node) {

	std::vector<shared_ptr<GraphNode>>::iterator i = std::find_if(v.begin(), v.end(),
		[&node](shared_ptr<GraphNode>& x) //lambda
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
	for (shared_ptr<GraphNode>& bullet : bulletList) {
		for (shared_ptr<GraphNode>& enemy : enemyList) {
			if (CheckCollision(bullet.get(), enemy.get()))
			{
				bullet->Active(false);
				enemy->Active(false);
				SoundEngine->play2D("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\sounds\\Explosion.mp3", GL_FALSE);
				score += 10;
				break;
			}
				
		}
		if (CheckCollision(bullet.get(), player.get()))
		{
			bullet->Active(false);

			playerLifes -= 1;
			SoundEngine->play2D("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\sounds\\Explosion.mp3", GL_FALSE);
			cout << "Gracz trafiony" << endl;
			if (playerLifes == 0)
				gameState = IN_MENU;
			break;
		}
	}
}


bool GameManager::CheckCollision(GraphNode* one, GraphNode* two)
{
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
	for (shared_ptr<GraphNode>& enemy : enemyList) {
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
