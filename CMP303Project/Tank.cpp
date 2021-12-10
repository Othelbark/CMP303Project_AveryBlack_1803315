#include "Tank.h"



Tank::Tank(std::string color) : sf::Sprite()
{
	m_BodyTexture.loadFromFile("Assets/" + color + "Tank.png");
	m_BarrelTexture.loadFromFile("Assets/" + color + "Barrel.png");
	setTexture(m_BodyTexture);

	setOrigin(getTextureRect().width / 2, getTextureRect().height / 2);

	m_GhostSprite.setTexture(m_BodyTexture);
	m_GhostSprite.setColor(sf::Color(255, 255, 255, 128));
	m_GhostSprite.setOrigin(getTextureRect().width / 2, getTextureRect().height / 2);
	setGhostPosition( getPosition() );

	m_BarrelSprite.setTexture(m_BarrelTexture);
	m_BarrelSprite.setOrigin(6, 2);
	m_BarrelSprite.setPosition( getPosition() );
}


Tank::~Tank()
{
}

//Sets the tank's position to the latest network position
void Tank::Update(float dt)
{
	if (m_Messages.size() < 1)
		return;
	TankMessage latestMessage = m_Messages.back();
	setPosition( latestMessage.x, latestMessage.y );
}

void Tank::setPosition( float x, float y ) {
	sf::Sprite::setPosition(x, y);
	m_BarrelSprite.setPosition(getPosition());
}

//Use this to set the prediction position
void Tank::setGhostPosition( sf::Vector2f pos ) {
	m_GhostSprite.setPosition( pos );
}

//Draw the tank / or the ghost / or both
const void Tank::Render(sf::RenderWindow * window) {
	if(m_RenderMode > 0)
		window->draw(m_GhostSprite);
	if (m_RenderMode != 1) {
		window->draw(*this);
		window->draw(m_BarrelSprite);
	}
}

//Add a message to the tank's network message queue
void Tank::AddMessage(const TankMessage & msg) {
	m_Messages.push_back(msg);

	if (m_Messages.size() > 3)
	{
		m_Messages.erase(m_Messages.begin());
	}
}

void Tank::AddPrediction(const TankMessage& pre)
{
	m_Predictions.push_back(pre);

	if (m_Predictions.size() > 3)
	{
		m_Predictions.erase(m_Predictions.begin());
	}
}

//This method calculates and stores the position, but also returns it immediately for use in the main loop
//This is my where prediction would be... IF I HAD ANY
/*sf::Vector2f Tank::RunPrediction(float gameTime) {
	float predictedX = -1.0f;
	float predictedY = -1.0f;

	const int msize = m_Messages.size();
	if( msize < 3 ) {
		return sf::Vector2f( predictedX, predictedX );
	}
	const TankMessage& msg0 = m_Messages[msize - 1];
	const TankMessage& msg1 = m_Messages[msize - 2];
	const TankMessage& msg2 = m_Messages[msize - 3];

	float xVel1 = (msg1.x - msg2.x) / (msg1.time - msg2.time);
	float yVel1 = (msg1.y - msg2.y) / (msg1.time - msg2.time);

	float xVel0 = (msg0.x - msg1.x) / (msg0.time - msg1.time);
	float yVel0 = (msg0.y - msg1.y) / (msg0.time - msg1.time);

	float xAcc = (xVel0 - xVel1) / (msg0.time - msg1.time);
	float yAcc = (yVel0 - yVel1) / (msg0.time - msg1.time);


	float timeSinceLastMsg = gameTime - msg0.time;
	predictedX = msg0.x + (xVel0 * timeSinceLastMsg) + (0.5f * xAcc * timeSinceLastMsg * timeSinceLastMsg);
	predictedY = msg0.y + (yVel0 * timeSinceLastMsg) + (0.5f * yAcc * timeSinceLastMsg * timeSinceLastMsg);
		
	return sf::Vector2f( predictedX, predictedY );
}*/

//Linear
sf::Vector2f Tank::RunPrediction(float gameTime) {
	//prediction from real data
	float predictedX = -1.0f;
	float predictedY = -1.0f;

	const int msize = m_Messages.size();
	if (msize < 3) {
		return sf::Vector2f(predictedX, predictedX);
	}
	const TankMessage& msg0 = m_Messages[msize - 1];
	const TankMessage& msg1 = m_Messages[msize - 2];
	const TankMessage& msg2 = m_Messages[msize - 3];

	float timeSinceLastMsg = gameTime - msg0.time;

	float xVel = (msg0.x - msg1.x) / (msg0.time - msg1.time);
	float yVel = (msg0.y - msg1.y) / (msg0.time - msg1.time);

	predictedX = msg0.x + (xVel * timeSinceLastMsg);
	predictedY = msg0.y + (yVel * timeSinceLastMsg);

	//prediction from predictions
	float purePredictedX = predictedX;
	float purePredictedY = predictedY;

	const int psize = m_Predictions.size();
	if (psize < 3) { //not enough predictions
		//store prediction
		TankMessage prediction;
		prediction.x = predictedX;
		prediction.y = predictedY;
		prediction.time = gameTime;
		AddPrediction(prediction);

		return sf::Vector2f(predictedX, predictedY);
	}
	const TankMessage& pre0 = m_Predictions[psize - 1];
	const TankMessage& pre1 = m_Predictions[psize - 2];
	const TankMessage& pre2 = m_Predictions[psize - 3];

	float timeSinceLastPre = gameTime - pre0.time;

	float ppxVel = (pre0.x - pre1.x) / (pre0.time - pre1.time);
	float ppyVel = (pre0.y - pre1.y) / (pre0.time - pre1.time);

	purePredictedX = pre0.x + (ppxVel * timeSinceLastPre);
	purePredictedY = pre0.y + (ppyVel * timeSinceLastPre);
	
	float interpX = (purePredictedX + predictedX) / 2.0f;
	float interpY = (purePredictedY + predictedY) / 2.0f;
	//float interpX = purePredictedX + (timeSinceLastPre * (predictedX - purePredictedX));
	//float interpY = purePredictedY + (timeSinceLastPre * (predictedY - purePredictedY));


	//store interpolated prediction
	TankMessage prediction;
	prediction.x = interpX;
	prediction.y = interpY;
	prediction.time = gameTime;
	AddPrediction(prediction);

	return sf::Vector2f(interpX, interpY);
}

void Tank::Reset() {
	m_Messages.clear();
}
