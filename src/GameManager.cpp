#include <GameManager.h>
#include <GLFW/glfw3.h>
#include <ctime>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>

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
	if (playerLifes > 0) 
	{
		ShootIfPossible();
		spawnEnemy();
		EnemyShooting();

		movePlayer();
		moveBullets();
		moveEnemy();

		DoCollision();
		removeObjectOutsideTheCamera(bulletList);
		removeObjectOutsideTheCamera(enemyList);
	}
	else {
		ResetGame();
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
	player->setPosition(-19.0f, -0.5f, 0.0f);
	playerLifes = 3;
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

void GameManager::addBullet(glm::vec3& position, glm::vec3& direction, GraphNode* shootingObject)
{
	shared_ptr<GraphNode> tmp = std::make_shared<GraphNode>(bullet.get());
	
	tmp.get()->setPosition(position.x, position.y, position.z);
	tmp.get()->SetDirection(direction);
	tmp.get()->SetShootingObject(shootingObject);
	float angleRadians = glm::angle(glm::vec3(1, 0, 0), direction);

	float angle = angleRadians * (float)(180 / M_PI);
	if(player->getPosition().y > shootingObject->getPosition().y)
		tmp.get()->Rotate(angle, glm::vec3(0, 0, 1));
	else
		tmp.get()->Rotate(-angle, glm::vec3(0, 0, 1));
	
	
	bulletList.push_back(tmp);
	sceneGraph->AddChild(tmp.get());
}

void GameManager::spawnEnemy()
{
	float time = (float)glfwGetTime();
	if( time >= cooldown1)
	{
		enemyCooldown = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 1.0f;
		cooldown1 = (float)glfwGetTime() + enemyCooldown;
		float y = -11.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-11.0f) )));
		
		shared_ptr<GraphNode> tmp = std::make_shared<GraphNode>(enemyShip.get());
		tmp->setPosition(25.0f, y, 0.0f);
		enemyList.push_back(tmp);
		sceneGraph->AddChild(tmp.get());
		
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
		
		bullet->setPosition(bulletPos.x + bullet->GetDirection().x * bulletSpeed, bulletPos.y + bullet->GetDirection().y * bulletSpeed, bulletPos.z);
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
			removeNode(v, object);
			break;
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

void GameManager::removeNode(vector<shared_ptr<GraphNode>>& v, shared_ptr<GraphNode>& node) {

	std::vector<shared_ptr<GraphNode>>::iterator i = std::find_if(v.begin(), v.end(),
		[&node](shared_ptr<GraphNode>& x) //lambda
	{
		return x == node;
	});

	if (i != v.end()) {
		//cout << "usuwanie shared_ptr" << endl;
		v.erase(i);
	}
		
}

void GameManager::ShootIfPossible()
{
	static volatile bool playerShot = false;
	if (playerShot) {
		cooldown = (float)glfwGetTime() + 0.4f;
	}
	cout << "cooldown = " << cooldown << " czas = " << (float)glfwGetTime() << " spacja "<< SpacebarIsPushed() << endl;
	if (SpacebarIsPushed() && (float)glfwGetTime() >= cooldown)
	{
		cout << "spacebar pushed" << endl;
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
				break;
			}
				
		}
		if (CheckCollision(bullet.get(), player.get()))
		{
			bullet->Active(false);

			playerLifes -= 1;
			cout << "Gracz trafiony" << endl;
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

	bool collisionX = one->getPosition().x + one->GetColliderSize().x >= two->getPosition().x &&
		two->getPosition().x + two->GetColliderSize().x >= one->getPosition().x;

	bool collisionY = one->getPosition().y + one->GetColliderSize().y >= two->getPosition().y &&
		two->getPosition().y + two->GetColliderSize().y >= one->getPosition().y;
	// Collision only if on both axes
	return collisionX && collisionY;
}

void GameManager::EnemyShooting()
{
	for (shared_ptr<GraphNode>& enemy : enemyList) {
		float time = (float)glfwGetTime();
		if (time >= enemy->GetShootingCooldown() && enemy->IsActive() && enemy->getPosition().x >= player->getPosition().x + 1)
		{
			enemy->SetShootingCooldown((float)glfwGetTime() + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 1.0f);
						
			glm::vec3 dir = glm::normalize(player->getPosition() - enemy->getPosition());

			addBullet(enemy->getPosition(), dir, enemy.get());
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
