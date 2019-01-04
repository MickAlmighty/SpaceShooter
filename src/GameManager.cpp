#include <GameManager.h>
#include <GLFW/glfw3.h>
#include <ctime>

GameManager::GameManager(shared_ptr<GraphNode> graph, float* hDir, float* vDir) {
	sceneGraph = graph;
	horizontalDirection = hDir;
	verticalDirection = vDir;
	spacebar = false;
}

GameManager::~GameManager()
{
}

void GameManager::GameOps()
{
	ShootIfPossible();
	spawnEnemy();
	movePlayer();
	moveBullets();
	moveEnemy();
	removeObjectOutsideTheCamera(bulletList);
	removeObjectOutsideTheCamera(enemyList);
	/*if (bulletList.size() >= 00) {
		cout << bulletList.size() << endl;
	}*/
}

void GameManager::setPlayer(GraphNode* playerPtr)
{
	player = shared_ptr<GraphNode>(playerPtr);
}

void GameManager::setBullet(GraphNode* bulletPtr)
{
	bullet = std::make_shared<GraphNode>(*bulletPtr);
}

void GameManager::setEnemyShip(GraphNode* enemy)
{
	enemyShip = std::make_shared<GraphNode>(*enemy);
}

void GameManager::addBullet()
{
	playerShot = true;
	shared_ptr<GraphNode> tmp = std::make_shared<GraphNode>(bullet.get());
	
	glm::vec3 playerPosition = player->getPosition();
	
	tmp.get()->setPosition(playerPosition.x, playerPosition.y, playerPosition.z);

	bulletList.push_back(tmp);
	sceneGraph->AddChild(tmp.get());
}

void GameManager::spawnEnemy()
{
	float time = (float)glfwGetTime();
	if( time >= cooldown1)
	{
		shared_ptr<GraphNode> tmp = std::make_shared<GraphNode>(enemyShip.get());
		float r3 = -12.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (11.0f - (-12.0f) )));
		tmp->setPosition(25.0f, r3, 0.0f);

		enemyList.push_back(tmp);
		sceneGraph->AddChild(tmp.get());
		float randomNumber = static_cast <float> (rand());
		enemyCooldown = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 1.0f;
		//cout << enemyCooldown << endl;
		cooldown1 = (float)glfwGetTime() + enemyCooldown;
	}
	
}

void GameManager::movePlayer()
{
	glm::vec3 shipPosition = player->getPosition();
	float xVal = shipPosition.x + *horizontalDirection;
	float yVal = shipPosition.y + *verticalDirection;
	//cout << xVal << " " << yVal << endl;
	if (xVal >= -19.0f && xVal <= 16.0f && yVal <= 11.0f && yVal >= -12.0f)
		player->setPosition(xVal, shipPosition.y + *verticalDirection, 0.0f);
	//cout << shipPosition.x << " " << shipPosition.y << " " << shipPosition.z << endl;
}

void GameManager::moveBullets()
{
	for (shared_ptr<GraphNode>& bullet : bulletList) {
		glm::vec3 bulletPos = bullet->getPosition();
		bullet->setPosition(bulletPos.x + bulletSpeed, bulletPos.y, bulletPos.z);
		//cout << bulletPos.x + bulletSpeed << " " << bulletPos.y << " " << bulletPos.z << endl;
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

void GameManager::spacebarPushed(bool pushed)
{
	spacebar = pushed;
}

void GameManager::removeObjectOutsideTheCamera(vector<shared_ptr<GraphNode>>& v)
{
	for (shared_ptr<GraphNode>& object : v) 
	{
		glm::vec3 xPos = object->getPosition();
		
		if (xPos.x < -30.0f || xPos.x > 30.0f) {
			sceneGraph->RemoveNode(object.get());
			if (removeNode(object.get())) {
				break;
			}
		}
	}
}

bool GameManager::removeNode(GraphNode* node)
{
	for (auto i = bulletList.begin(); i != bulletList.end(); i++) {
		if (i->get()->getPosition().x == node->getPosition().x &&
			i->get()->getPosition().y == node->getPosition().y &&
			i->get()->getPosition().z == node->getPosition().z)
		{
			bulletList.erase(i);
			return true;
		}
	}
	return false;
}

void GameManager::ShootIfPossible()
{
	if (playerShot) {
		cooldown = (float)glfwGetTime() + 0.4f;
	}

	if (SpacebarIsPushed() && (float)glfwGetTime() >= cooldown)
	{
		//cout << "spacebar pushed" << endl;
		addBullet();
		playerShot = true;
	}
	else
	{
		playerShot = false;
	}
}

bool GameManager::SpacebarIsPushed()
{
	if (spacebar)
		return true;
	else
		return false;
}
