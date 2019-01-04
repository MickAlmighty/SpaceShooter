#include <GameManager.h>
#include <GLFW/glfw3.h>
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
	movePlayer();
	moveBullets();
	removeBulletsOutsideTheCamera();
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

void GameManager::movePlayer()
{
	glm::vec3 shipPosition = player->getPosition();
	float xVal = shipPosition.x + *horizontalDirection;
	float yVal = shipPosition.y + *verticalDirection;
	//cout << xVal << " " << yVal << endl;
	if( xVal >= -19.0f && xVal <= 16.0f && yVal <= 11.0f && yVal >= -12.0f)
		player->setPosition(xVal, shipPosition.y + *verticalDirection, 0.0f);
	//cout << shipPosition.x << " " << shipPosition.y << " " << shipPosition.z << endl;
}

void GameManager::addBullet()
{
	playerShot = true;
	shared_ptr<GraphNode> tmp = std::make_shared<GraphNode>(bullet.get());
	
	glm::vec3 playerPosition = player->getPosition();
	
	tmp.get()->setPosition(playerPosition.x, playerPosition.y, playerPosition.z);
	playerPosition = tmp->getPosition();

	bulletList.push_back(tmp);
	sceneGraph->AddChild(tmp.get());
}

void GameManager::moveBullets()
{
	//glm::vec3 bulletPos;
	for (shared_ptr<GraphNode>& b : bulletList) {
		glm::vec3 bulletPos = b->getPosition();
		b->setPosition(bulletPos.x + bulletSpeed, bulletPos.y, bulletPos.z);
		//cout << bulletPos.x + bulletSpeed << " " << bulletPos.y << " " << bulletPos.z << endl;
	}
	//cout << "za petla przesuwania pociskow" << endl;
}

void GameManager::spacebarPushed(bool pushed)
{
	spacebar = pushed;
}

void GameManager::removeBulletsOutsideTheCamera()
{
	for (shared_ptr<GraphNode>& b : bulletList) {
		glm::vec3 xPos = b->getPosition();
		if (xPos.x < -30.0f || xPos.x > 30.0f) {
			sceneGraph->RemoveNode(b.get());
			if (removeNode(b.get())) {
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
