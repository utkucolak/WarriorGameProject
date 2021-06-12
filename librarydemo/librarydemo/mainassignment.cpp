#include "SFML/Graphics.hpp"
#include <string>
#include <iostream>
#include "SFML/Window.hpp"
#include <math.h>
using namespace std;

int b_num, s_num, p_num;
int sandbag_rx = 33;
int sandbag_ry = 30;

int barrel_rx = 32;
int barrel_ry = 30;


enum WalkDirection { //initializing the direction enum. object so we can easily redirect the walking directions
	Left,
	Up,
	Right,
	Down
};
int soldierstate = Up;
int soldierstate2 = Up;
int ellipse_bounds(int ex, int ey, int x, int y, int rx, int ry) {
	int p = (pow((x - ex), 2) / pow(rx, 2)) + (pow((y - ey), 2) / pow(ry, 2));
	return p;
}
class Coord {  //coordination class
public:
	float x;
	float y;
	Coord(); // default constructor
	Coord(float x, float y); //non default constructor

};


Coord::Coord() {  //if there is no argument passed through the object, coordination will be initialized as 0.
	x = 0;
	y = 0;

}


Coord::Coord(float x, float y) { //if there is any given argument exists, it will be initialized
	this->x = x;
	this->y = y;
}


class Object { //object class. Main class for most of the other objects. Abstract class
	friend class BulletList;
protected:

	sf::RenderWindow *window;
	sf::Sprite sprite;
	sf::Texture texture;
	Coord pos;
public:
	virtual void init() = 0;
	virtual void paint() = 0;
};
void show(Object *obj) { //polymorphism. It will trigger the paint function of each object that passed as an arg.
	obj->paint();
}


class Sandbag : public Object {  //sandbag object, just a blocker where soldier can not walk while it stands on his way.
public: //declaring the public class functions.
	void init(sf::RenderWindow *window, string texturePath, Coord pos);
	void init();
	Coord getPosition();
	void paint();
	sf::FloatRect bounds();
};

sf::FloatRect Sandbag::bounds() { //we need to return the bounds of the object so we can detect whether if it intersects with another object's or not
	return sprite.getGlobalBounds();
}
void Sandbag::init() {
	cout << "no arg. passed. Error" << endl;
}
void Sandbag::init(sf::RenderWindow *window, string texturePath, Coord pos) { //initializer object, we need to define its main construct.
	texture.loadFromFile(texturePath + "\\bags.png");
	sprite.setTexture(texture);
	sprite.setPosition(pos.x, pos.y);
	this->window = window;//I want to use the window object that passed as a whole class attribute so I wrote this code. All this->x = x; type lines are used for same goal.
	this->pos = pos; //same here.
}
Coord Sandbag::getPosition() { //returns the coordinates of sandbag object.
	return pos;
}
void Sandbag::paint() { //draws the sprite
	window->draw(sprite);
}


class Barrel :public Object { //Almost identical with Sandbag, only difference is texture.
	int isVisible;
public:
	void init(sf::RenderWindow *window, string texturePath, Coord pos);
	void init();
	Coord getPosition();
	void paint();
	sf::FloatRect bounds();
	bool getVisible(void);
	void setVisible(bool visible);
};

void Barrel::init(sf::RenderWindow *window, string texturePath, Coord pos) {
	texture.loadFromFile(texturePath + "\\barrel.png");
	sprite.setTexture(texture);
	sprite.setPosition(pos.x, pos.y);
	this->window = window;
	this->pos = pos;
	isVisible = 1;

}

bool Barrel::getVisible(void) {
	if (isVisible == 1) return true;
	else return false;
}
void Barrel::setVisible(bool visible) {
	if (visible == true) isVisible = 1;
	if (visible == false) isVisible = 0;
}
void Barrel::init() {
	cout << "barrel error" << endl;
}
sf::FloatRect Barrel::bounds() {
	return sprite.getGlobalBounds();
}
Coord Barrel::getPosition() {
	return pos;
}
void Barrel::paint() {
	window->draw(sprite);
}

class Bullet : public Object { //bullet class 
	friend class BulletList; //friend class with bulletlist. So bulletlist can access the private members of bullet class like angle or sprite etc.
	float speed;
	int angle;
	Bullet *next;

public:
	Bullet();
	void move(void);
	void setSpeed(float speed);
	void init(sf::RenderWindow *window, int angle, Coord pos);
	void init() {
		angle = 90;
		texture.loadFromFile("textures\\bullet.png");
		sprite.setTexture(texture);
		sprite.setPosition(pos.x, pos.y);
		sprite.setRotation(angle);
	}
	void paint();

};

Bullet::Bullet() { //constructor. usual stuffs
	texture.loadFromFile("textures\\bullet.png");
	sprite.setTexture(texture);
}


void Bullet::setSpeed(float speed) { //set the speed of the bullet
	this->speed = speed;
}
void Bullet::init(sf::RenderWindow *window, int angle, Coord pos) { //initalizer func. We need to initialize the bullet object as set its parameters.
	this->window = window;
	this->angle = angle;
	this->pos = pos;
	switch (angle) {
	case 90:
		sprite.setPosition(pos.x + 95, pos.y + 75); //set the position according to soldier state
		break;
	case 270:
		sprite.setPosition(pos.x + 5, pos.y + 40);
		break;
	case 180:
		sprite.setPosition(pos.x + 30, pos.y + 120);
		break;
	case 0:
		sprite.setPosition(pos.x + 62, pos.y);
		break;
	}

	sprite.setRotation(angle); //rotate the bullet
}

void Bullet::paint() {
	window->draw(sprite); //draw the bullet

}

void Bullet::move(void) {
	switch (angle) {
	case 90:
		sprite.move(speed, 0.0f);
		pos.x = sprite.getPosition().x;
		pos.y = sprite.getPosition().y;
		this->pos = pos;
		sprite.setPosition(pos.x, pos.y);
		break;

	case 270:
		sprite.move(-1 * speed, 0.0f);
		pos.x = sprite.getPosition().x;
		pos.y = sprite.getPosition().y;
		this->pos = pos;
		sprite.setPosition(pos.x, pos.y);
		break;
	case 180:
		sprite.move(0.0f, speed);
		pos.x = sprite.getPosition().x;
		pos.y = sprite.getPosition().y;
		this->pos = pos;
		sprite.setPosition(pos.x, pos.y);
		break;
	case 0:
		sprite.move(0.0f, -1 * speed);
		pos.x = sprite.getPosition().x;
		pos.y = sprite.getPosition().y;
		this->pos = pos;
		sprite.setPosition(pos.x, pos.y);
		break;
	}
}



class Player : public Object {  //our soldier. One of the main parts of the projects, soldier must walk, rotate etc. in the warzone.

protected:
	int width; //protected attributes
	int height;
	int state;
	int s;
	int score;
public:
	sf::Sprite sprite;
	sf::Texture textures[14];
	void init(sf::RenderWindow *window, string textBasePath, int NumTextures, Coord pos, int width, int height); //initalizer function
	void setPosition(Coord pos); //set the position
	bool checkCollision(Barrel *barrels, Sandbag *sandbags, int nb, int ns); //determine if the player object collides with any other object or not except the other player.
	void walk(float speed, WalkDirection dir, sf::Texture *walking, Barrel *barrels, Sandbag *sandbags, int nb, int ns, int clock); //move the player sprite.
	void paint(); //draw the sprite
	void init(); //empty function
	void incrementScore(void) { // increment the score attribute by 1
		score++;
		return;
	}
	int getScore(void) { //return the score of the player
		return score;
	}
	void setScore(int score) { //set the score by any int value
		this->score = score;
	}
	sf::FloatRect bounds();
	Coord getPosition(); //get its position
};
void Player::init() {
	cout << "player error. no args given";
}
sf::FloatRect Player::bounds() { //get the bounds of sprite
	return sprite.getGlobalBounds();
}

Coord Player::getPosition() {
	return pos;
}
void Player::init(sf::RenderWindow *window, string textBasePath, int NumTextures, Coord pos, int width, int height) {
	//initializing the player object
	this->width = width;
	this->height = height;
	score = 0;
	NumTextures = 14;
	this->pos = pos;
	int i;
	for (i = 0; i < NumTextures; i++)
		textures[i].loadFromFile(textBasePath + "\\soldier" + to_string(i) + ".png"); //load all the possible states textures of soldier
	sprite.setTexture(textures[0], true);
	//sprite.setPosition(pos.x, pos.y);
	this->window = window; //use the passed window param as a class attribute.
	//window->draw(sprite);

}
void Player::setPosition(Coord pos) { //set the soldier coordinates -choose where will he start.
	this->pos = pos;
	sprite.setPosition(pos.x, pos.y);
}
void Player::paint() {
	window->draw(sprite); //draw soldier

}

bool Player::checkCollision(Barrel *barrels, Sandbag *sandbags, int nb, int ns) { //detect the collision
	for (b_num = 0; b_num < nb; b_num++) {
		if (barrels[b_num].getVisible() == true) {
			if (ellipse_bounds(barrels[b_num].getPosition().x - 5, barrels[b_num].getPosition().y - 13, pos.x, pos.y, barrel_rx, barrel_ry) <= 1) //if player reaches the edges of an euclidian ellipse of an object, collision will be detected.
				return true;
		}
	}

	for (s_num = 0; s_num < ns; s_num++) {

		if (ellipse_bounds(sandbags[s_num].getPosition().x, sandbags[s_num].getPosition().y - 15, pos.x, pos.y, sandbag_rx, sandbag_ry) <= 1)
			return true;

	}

	if (pos.x >= width - 30 || pos.y >= height - 30 || pos.x <= 30 || pos.y <= 30) { //soldier can't leave the screen


		return true;
	}
	//sf::FloatRect boundingBox = barrels
	return false;

}

void Player::walk(float speed, WalkDirection dir, sf::Texture *walking, Barrel *barrels, Sandbag *sandbags, int nb, int ns, int clock) { //function to walk
	int b_num, s_num;
	switch (dir) { //switch-case statement. We shall determine the action of character according to  given direction.
	case Left: //for instance, if given direction is Left,
		sprite.move(-1 * speed, 0.0f); //character will move to the left.
		pos.x = sprite.getPosition().x;
		pos.y = sprite.getPosition().y;
		//this->pos = pos;
		sprite.setPosition(pos.x, pos.y);
		while (checkCollision(barrels, sandbags, nb, ns)) { //but if there is any collision 
			sprite.move(speed, 0.0f); //character moves back to the old place.
			pos.x = sprite.getPosition().x;
			pos.y = sprite.getPosition().y;
			this->pos = pos;
			sprite.setPosition(pos.x, pos.y);
		}
		break;

	case Right: //also same situation.
		sprite.move(speed, 0.0f);
		pos.x = sprite.getPosition().x;
		pos.y = sprite.getPosition().y;
		//this->pos = pos;
		sprite.setPosition(pos.x, pos.y);
		while (checkCollision(barrels, sandbags, nb, ns)) {
			sprite.move(-1 * speed, 0.0f);
			pos.x = sprite.getPosition().x;
			pos.y = sprite.getPosition().y;
			this->pos = pos;
			sprite.setPosition(pos.x, pos.y);
		}
		break;

	case Up:
		sprite.move(0.0f, -1 * speed);
		pos.x = sprite.getPosition().x;
		pos.y = sprite.getPosition().y;
		//this->pos = pos;
		sprite.setPosition(pos.x, pos.y);
		while (checkCollision(barrels, sandbags, nb, ns))
		{
			sprite.move(0.0f, speed);
			pos.x = sprite.getPosition().x;
			pos.y = sprite.getPosition().y;
			this->pos = pos;
			sprite.setPosition(pos.x, pos.y);
		}
		break;

	case Down:
		sprite.move(0.0f, speed);
		pos.x = sprite.getPosition().x;
		pos.y = sprite.getPosition().y;
		//this->pos = pos;
		sprite.setPosition(pos.x, pos.y);
		while (checkCollision(barrels, sandbags, nb, ns)) {
			sprite.move(0.0f, -1 * speed);
			pos.x = sprite.getPosition().x;
			pos.y = sprite.getPosition().y;
			this->pos = pos;
			sprite.setPosition(pos.x, pos.y);
		}
		break;
	}
	sprite.setTexture(walking[clock]); //set the movement animation while walking
}

bool gameRunning = true;
class BulletList { //Linked list for all the bullets
	//friend class Game;
	Object* sprite;
	Bullet* head;
	sf::RenderWindow* window;
public:
	BulletList(sf::RenderWindow* window);
	void add(Coord pos, int state, float speed, int shooter);
	void update(Player* players, Barrel* barrels, Sandbag *sandbags, int np, int nb, int ns, int width, int height);
	void checkCollision(Player* players, Barrel* barrels, Sandbag* sandbags, int np, int nb, int ns, int width, int height);
	void remove(Bullet *node);
	void paint();
	void move();
};
BulletList::BulletList(sf::RenderWindow* window) { //initializing the params.
	this->window = window;
	this->head = NULL;

}
void BulletList::checkCollision(Player* players, Barrel* barrels, Sandbag *sandbags, int np, int nb, int ns, int width, int height) { //determine whether the bullet collides with any other object or edges of the screen
	Bullet * current = head;
	while(current != NULL ){ //traverse all the linked list from beginning.
	for (b_num = 0; b_num < nb; b_num++) {
		float initcoord_x = barrels[b_num].getPosition().x; //we try to get the rectangle of the objects bounds.
		float initcoord_y = barrels[b_num].getPosition().y;
		float finalcoord_x = initcoord_x + barrels[b_num].sprite.getGlobalBounds().width;
		float finalcoord_y = initcoord_y + barrels[b_num].sprite.getGlobalBounds().height;
		if (barrels[b_num].getVisible() == true) { //if barrel has not been destroyed yet, a bullet can destroy it. Otherwise it will pass through.
			if (current->pos.x > initcoord_x && current->pos.y > initcoord_y && current->pos.x < finalcoord_x && current->pos.y < finalcoord_y) { //if bullet is in the rectangle of an object 
				remove(current); //free the memory, bullet will be destroyed
				barrels[b_num].setVisible(false); //barrel will be destroyed.
				return;
			}
		}
	}

	for (s_num = 0; s_num < ns; s_num++) {
		float initcoord_x = sandbags[s_num].getPosition().x;
		float initcoord_y = sandbags[s_num].getPosition().y;
		float finalcoord_x = initcoord_x + sandbags[s_num].sprite.getGlobalBounds().width;
		float finalcoord_y = initcoord_y + sandbags[s_num].sprite.getGlobalBounds().height;
		if (current->pos.x > initcoord_x && current->pos.y > initcoord_y && current->pos.x < finalcoord_x && current->pos.y < finalcoord_y) {
			remove(current); //we must destroy only bullet in this case, sandbag will remain.
			return;
		}
	}
	
	for (p_num = 0; p_num < np; p_num++) {
		float initcoord_x = players[p_num].getPosition().x;
		float initcoord_y = players[p_num].getPosition().y;
		float finalcoord_x = initcoord_x + players[p_num].sprite.getGlobalBounds().width;
		float finalcoord_y = initcoord_y + players[p_num].sprite.getGlobalBounds().height;
		if (current->pos.x > initcoord_x && current->pos.y > initcoord_y && current->pos.x < finalcoord_x && current->pos.y < finalcoord_y) {
			
			int newcoordx = (rand() % (width - 50));
			int newcoordy = (rand() % (height - 50));
			
			Coord newpos(newcoordx, newcoordy);
			players[p_num].setPosition(newpos);
			int b_bar = 0;
			int s_bar = 0;
			for(b_bar; b_bar < nb; b_bar++){
				for (s_bar; s_bar < ns; s_bar++) {
					while (players[p_num].sprite.getGlobalBounds().intersects(barrels[b_bar].bounds()) || players[p_num].sprite.getGlobalBounds().intersects(sandbags[s_num].bounds())) {
						int newcoordx = (rand() % (width - 50));
						int newcoordy = (rand() % (height - 50));

						Coord newpos(newcoordx, newcoordy);
						players[p_num].setPosition(newpos);
					}
				}
			}
			//when a player dies, he will respawn in random location.
			
			if (p_num == 1) { //if bullet hits the player 1
				players[0].incrementScore(); //the score of player 0 will be incremented.
			}
			if (p_num == 0) { //vice versa
				players[1].incrementScore();
			}
			remove(current); //destroy the bullet
			return;
		}
	}

	if (current->pos.x >= width || current->pos.y >= height || current->pos.x <= 0 || current->pos.y <= 0) { //if bullet lefts the screen
	
		remove(current);
		return;
	}

	current = current->next;
	}


	return;
}

void BulletList::paint() { //traverse the linked list and draw each object in it.
	Bullet * current = head;
	while (current != NULL) {
		window->draw(current->sprite);
		current = current->next;
	}
}

void BulletList::update(Player* players, Barrel* barrels, Sandbag *sandbags, int np, int nb, int ns, int width, int height) { //take actions at each screen refresh
	paint();
	checkCollision(players, barrels, sandbags, np, nb, ns, width, height);
	move();

}
void BulletList::move() { //traverse the linked list and move each object in it.
	Bullet * current = head;
	while (current != NULL) {
		current->move();
		current = current->next;
	}
}

void BulletList::add(Coord pos, int state, float speed, int shooter) { //add an object to the linked list.
	Bullet *current, *new_bullet;
	new_bullet = new Bullet;
	new_bullet->setSpeed(speed);
	float angle;
	if (!new_bullet) cout << "hata";
	current = head;
	head = new_bullet;
	new_bullet->next = current;
	

	switch (state) { //determine the angle of the bullet according to current soldier state.
	case Left:
		angle = 270;
		break;
	case Right:
		angle = 90;
		break;
	case Up:
		angle = 0;
		break;
	case Down:
		angle = 180;
		break;

	}
	new_bullet->init(window, angle, pos); //initialize the bullet
	this->sprite = sprite;
	//show(new_bullet);

}

void BulletList::remove(Bullet *node) { //remove a bullet from linked list. -Destroy the bullet-
	if (node == NULL) {
		return;
	}
	if (head == NULL) {
		return;
	}
	if (node == head) { //if node is the first node
		if (head->next == NULL) { //if node is the only node
			
			delete node;
			head = NULL; //new head pointer will point to null
			return;
		}
		
		head = head->next; //else, head pointer will point to next node
		delete node;
		return;
	}
	Bullet *temp = head; //if node is not the fist node
	while (temp->next != NULL && temp->next != node) { //an temporary pointer will traverse the list from the beginning until its next pointer shows the node to be deleted
		temp = temp->next;
	}
	temp->next = node->next; //we will connect the list  and we are ready to delete node
	delete node; //delete the node.
	return;

}
class Game :public Player { //Game class, also we can consider this as base class. -not the base from base-derived-
	float speed;
	int numBarrels;
	int numSandbags;
	int numPlayers;
	int width;
	int height;
	//pointers for each game parts like window, soldier(s), barrels etc.
	sf::RenderWindow* window;
	sf::Texture bgTexture;
	sf::Sprite bgSprite;
	Barrel *barrels;
	Sandbag *sandbags;
	Player* players;
	BulletList* bullets;
	sf::Text text;
	sf::Font font;
public:
	//public methods
	Game(float speed, int w, int h, int nb, int ns, int np);//non default constructor.
	void drawBackground(void);
	void update();
	~Game();
};

Game::Game(float speed, int w, int h, int nb, int ns, int np) { //Game class, also we can consider this as base class. -not the base from base-derived-
	bool gameRestart = false;
	width = w;
	height = h;
	window = new sf::RenderWindow(sf::VideoMode(width, height), "The Game");
	numSandbags = ns;
	numPlayers = np;
	this->speed = speed;
	players = new Player[np]; //allocate the memory dynamically for each player, barrel and sandbag objects.
	barrels = new Barrel[nb];
	sandbags = new Sandbag[ns];
	numBarrels = nb;
	bullets = new BulletList(window);
	numSandbags = ns;
	numPlayers = np;
	Coord pos(width / 2, 800);
	players[0].init(window, "textures", 14, pos, width, height); //init. the player object
	players[0].setPosition(pos); //set the beginning position.
	Coord pos2(width / 2 - 200, 60);
	players[1].init(window, "textures", 14, pos2, width, height);
	players[1].setPosition(pos2);
	font.loadFromFile("font.ttf");
	text.setFont(font);
	//player coordinates. These coordinates are chosen arbitarily.
}
Game::~Game() { //destructor, free the memory when object is deleted -game is closed so it is OK if we don't do it in this case but still i wanted to do-
	delete[] players;
	delete[] sandbags;
	delete[] barrels;
}
void Game::drawBackground(void) { //drawing background, loading the texture as one big pic was causing problems so i loaded it part by part. Each image was 350x350 so i divided the screen and load it part by part.
	bgTexture.loadFromFile("textures\\grass.png");
	bgSprite.setTexture(bgTexture);
	int i, j;
	for (i = 0; i <= width / 350; i++) {
		for (j = 0; j <= height / 350; j++) {
			bgSprite.setPosition(sf::Vector2f(i * 350, j * 350));
			window->draw(bgSprite);
		}
	}
}

void Game::update() { //this function will behave like our main() function, I could to it all from the main but that way seems easier so I decided to implement all functions in that function.
	//players[0].init(window, "textures", 14, pos); //init. the player object
	//players[0].setPosition(pos); //set the beginning position.
	int bar;
	srand(time(NULL));
	for (bar = 0; bar < numBarrels; bar++) { //set the bars and the bags, they will spawn randomly in each game start. 
		Coord barcoord;
		int coordx = (rand() % (width - 40));
		int coordy = (rand() % (height - 40));

		//int coordx = bar * 60 + 40;
		//int coordy = bar * 50 + 20;

		barcoord.x = coordx;
		barcoord.y = coordy;
		barrels[bar].init(window, "textures", barcoord);
		while (players[0].sprite.getGlobalBounds().intersects(barrels[bar].bounds())) {
			coordx = (rand() % (width - 40)); //copy code with new random values if there is any intersection in the initial position -since if there is any character would never walk.
			coordy = (rand() % (height - 40));
			barcoord.x = coordx;
			barcoord.y = coordy;
			barrels[bar].init(window, "textures", barcoord);

		}
	}
	int bag;
	for (bag = 0; bag < numSandbags; bag++) {
		Coord bagcoord;
		int bcoordx = (rand() % (width - 40));
		int bcoordy = (rand() % (height - 40));

		bagcoord.x = bcoordx;
		bagcoord.y = bcoordy;
		sandbags[bag].init(window, "textures", bagcoord);
		while (players[0].sprite.getGlobalBounds().intersects(barrels[bar].bounds())) {
			bcoordx = (rand() % (width - 40)); //copy code with new random values if there is any intersection in the initial position -since if there is any character would never walk.
			bcoordy = (rand() % (height - 40));
			bagcoord.x = bcoordx;
			bagcoord.y = bcoordy;
			sandbags[bag].init(window, "textures", bagcoord);

		}


	}
	sf::Texture  walkingLeft[3] = { players->textures[12],  players->textures[6], players->textures[13] }, //walking animations. I implemented that since our movement algorithm must be like stand still -> right foot -> stand still -> left foot -> [...]
		walkingRight[3] = { players->textures[9],   players->textures[2], players->textures[10] }, //also walking right animation array
		walkingUp[3] = { players->textures[7],   players->textures[0], players->textures[8] },
		walkingDown[3] = { players->textures[11],  players->textures[4], players->textures[3] };

	int clock = 0;
	int clock2 = 0;//initialize the clock as 0. Clock is just an arbitrary controller to control character rotation and determine the animation.
	while (window->isOpen()) {
		sf::Event event;
		float dir1;
		float dir2;

		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed) { gameRunning = false; window->close(); }
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			if (++clock > 2) clock = 0;
			players[0].walk(11, Left, walkingLeft, barrels, sandbags, numBarrels, numSandbags, clock);//move to the left with walkingLeft animation
			soldierstate = Left;//save the last state of the soldier -the direction where soldier looks-
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			if (++clock > 2) clock = 0;
			players[0].walk(11, Right, walkingRight, barrels, sandbags, numBarrels, numSandbags, clock);
			soldierstate = Right;//save the last state of the soldier -the direction where soldier looks-
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			if (++clock > 2) clock = 0;
			players[0].walk(11, Up, walkingUp, barrels, sandbags, numBarrels, numSandbags, clock);
			soldierstate = Up;//move to the up with walkingUp animation and so on.
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			if (++clock > 2) clock = 0;
			players[0].walk(11, Down, walkingDown, barrels, sandbags, numBarrels, numSandbags, clock);
			soldierstate = Down;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			if (++clock2 > 2) clock2 = 0;
			players[1].walk(11, Left, walkingLeft, barrels, sandbags, numBarrels, numSandbags, clock2);
			soldierstate2 = Left;//same movements for player 1
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			if (++clock2 > 2) clock2 = 0;
			players[1].walk(11, Right, walkingRight, barrels, sandbags, numBarrels, numSandbags, clock2);
			soldierstate2 = Right;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			if (++clock2 > 2) clock2 = 0;
			players[1].walk(11, Up, walkingUp, barrels, sandbags, numBarrels, numSandbags, clock2);
			soldierstate2 = Up;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			if (++clock2 > 2) clock2 = 0;
			players[1].walk(11, Down, walkingDown, barrels, sandbags, numBarrels, numSandbags, clock2);
			soldierstate2 = Down;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			bullets->add(players[0].getPosition(), soldierstate, 50.0, 0); //if Enter is pressed, player 0 will shoot.
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			bullets->add(players[1].getPosition(), soldierstate2, 50.0, 1); //if Space is pressed then player 1 will shoot.
		}
		
		window->clear(sf::Color::Black);
		Game::drawBackground();
		int p0score = players[0].getScore();
		int p1score = players[1].getScore();

		string textToWrite = "Player0  " + to_string(p0score) + " : " + to_string(p1score) + " Player 1";
		
		text.setCharacterSize(24); // in pixels, not points!
		text.setFillColor(sf::Color::White);
		text.setOutlineColor(sf::Color::Black);
		text.setString(textToWrite);
		text.setStyle(sf::Text::Bold | sf::Text::Underlined);
		window->draw(text);
		show(&players[0]); //draw player objects
		show(&players[1]);
		bullets->update(players, barrels, sandbags, numPlayers, numBarrels, numSandbags, width, height); //update each bullet to check its conditions and take appropriate actions like move, remove etc.
		for (bar = 0; bar < numBarrels; bar++) {
			if (barrels[bar].getVisible() == true)
				show(&barrels[bar]); //show barrels
		}
		for (bag = 0; bag < numSandbags; bag++) {

			show(&sandbags[bag]); //show sandbags

		}
		sf::Text wintext; //win text
		wintext.setFont(font);
		wintext.setCharacterSize(45); // in pixels, not points!
		wintext.setFillColor(sf::Color::White);
		wintext.setPosition(width / 2 - 120, height / 2); //we want the text to be appear at the middle of the screen.
		wintext.setStyle(sf::Text::Bold | sf::Text::Underlined);
		if (p0score == 10) { //if player 0 reaches to score 10, then player 0 wins and game will be restarted if players choose so, otherwise game will close.
			wintext.setString("Player 0 wins. Restart (Y/N)?");
			window->draw(wintext);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) {
				players[0].setScore(0); //set scores to 0 before game restarts
				players[1].setScore(0);
				return;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
				gameRunning = false; //close the game
				window->close();
			}
		}

		if (p1score == 10) {
			wintext.setString("Player 1 wins. Restart (Y/N)?");
			window->draw(wintext);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) {
				players[0].setScore(0);
				players[1].setScore(0); 
				return;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
				gameRunning = false;
				window->close();
			}
		}

		//barrels->paint();
		//players->walk(1.1, Right, 0, 0, 0, 0);
		//oyuncu.init(window, "textures", 14, pos);
		//oyuncu.paint();
		window->display();
		window->setFramerateLimit(60); //60fps
		sf::sleep(sf::seconds(0.1));
	}
}


int main()
{
	
	Game oyun(1.0, 1920, 1080, 6, 6, 2); //create the game object
	oyun.drawBackground(); //call the functions
	while (gameRunning)
	{
		oyun.update();
	}
	
	return 0;
}
