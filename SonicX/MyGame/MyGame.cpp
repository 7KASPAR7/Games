#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

int ground = 576;

bool isgameover = false;
bool iswin = false;

float offsetX = 0, offsetY = 0;

const int H = 12;
const int W = 75;

sf::String TileMap[H] = {
"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
"B                                                               BBBBBBBBBB",
"B                                                               BBBBBBBBBB",
"B                                                                  BBBBBBB",
"B O                    O                                     OOOO FBBBBBBB",
"BB               OBB    O                                 O BBBBBBBBBBBBBB",
"BBB            O BBBB    O                          O BBBBBBBBBBBBBBBBBBBB",
"BBBB         O BBBBBBBB   O          O         O  BBBBBBBBBBBBBBBBBBBBBBBB",
"BBBBB  O   O BBBBBBBBBBBB  O       BBB   OO  BBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB O BBBBBBBBOOBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
};


class PLAYER {
public:
	float dx, dy;
	sf::FloatRect rect;
	bool onGround;
	sf::Sprite sprite;
	float currentFrameL, currentFrameR;
	int score = 0;
	
	PLAYER(sf::Texture & image)
	{

		sprite.setTexture(image);
		rect = sf::FloatRect(300, 300, 85, 92);
		dx = dy = float(0.1);
		currentFrameL = currentFrameR = 0;
	}
	void update(float time)
	{
		rect.left += dx*time;
		Collision(0);

		if (!onGround) dy = dy + float(0.025) * time;
		rect.top += dy*time;
		onGround = false;
		Collision(1);

		currentFrameR += float(0.1) * time;
		currentFrameL += float(0.1) * time;
		if (currentFrameR > 23) currentFrameR = 15;
		if (currentFrameL > 23) currentFrameL = 15;

		if (dx > 0) sprite.setTextureRect(sf::IntRect(85 * int(currentFrameR), 0, 85, 92));
		if (dx < 0) sprite.setTextureRect(sf::IntRect(85 * int(currentFrameL) + 85, 0, -85, 92));

		sprite.setPosition(rect.left - offsetX, rect.top - offsetY);

		dx = 0;
	}

	void Collision(int dir)
	{
		for (int i = rect.top / 64; i < (rect.top + rect.height) / 64; i++)
			for (int j = rect.left / 64; j < (rect.left + rect.width) / 64; j++)
			{
				if (TileMap[i][j] == 'B')
				{
					if ((dx > 0) && (dir == 0)) rect.left = j * 64 - rect.width;
					if ((dx < 0) && (dir == 0)) rect.left = j * 64 + 64;
					if ((dy > 0) && (dir == 1)) { rect.top = i * 64 - rect.height; dy = 0; onGround = true; }
					if ((dy < 0) && (dir == 1)) { rect.top = i * 64 + 64; dy = 0; }
				}
				if (TileMap[i][j] == 'O')
				{
					TileMap[i][j] = ' ';
					score += 1;
					
				}
			}
	}
};

class ENEMY
{

public:
	float dx, dy;
	sf:: FloatRect rect;
	sf::Sprite sprite;
	float currentFrame;
	bool life;


	void set(sf::Texture& image, int x, int y)
	{
		sprite.setTexture(image);
		rect = sf::FloatRect(x, y, 128, 64);

		dx = 0.5;
		currentFrame = 0;
		life = true;
	}

	void update(float time)
	{
		rect.left += dx * time;

		Collision();


		currentFrame += time * 0.005;
		if (currentFrame > 4) currentFrame -= 4;

		if (dx>0) sprite.setTextureRect(sf::IntRect(128 * int(currentFrame) + 128, 129, -128, 128));
		else sprite.setTextureRect(sf::IntRect(128 * int(currentFrame) , 129, 128, 128));
		if (!life) sprite.setTextureRect(sf::IntRect(0, 0, 10, 10));
	

		sprite.setPosition(rect.left - offsetX, rect.top - offsetY);

	}


	void Collision()
	{

		for (int i = rect.top / 64; i < (rect.top + rect.height) / 64; i++)
			for (int j = rect.left / 64; j < (rect.left + rect.width) / 64; j++)
				if ((TileMap[i][j] == 'B') || (TileMap[i][j] == '0'))
				{
					if (dx > 0)
					{
						rect.left = j * 64 - rect.width; dx *= -1;
					}
					else if (dx < 0)
					{
						rect.left = j * 64 + 64;  dx *= -1;
					}

				}
	}

};


int WinMain()
{

	sf::RenderWindow window(sf::VideoMode(1280, 800), "Sonic!!!");
	sf::Texture t;
	sf::Texture e;
	sf::Texture g;
	sf::Texture y;
	sf::Texture m;
	t.loadFromFile("Sonic.png");
	e.loadFromFile("Enemy.png");
	g.loadFromFile("gameover.png");
	y.loadFromFile("youwin.png");
	m.loadFromFile("map.png");
	sf::Sprite gameover;
	gameover.setTexture(g);
	gameover.setPosition(416, 254);
	sf::Sprite youwin;
	youwin.setTexture(y);
	youwin.setPosition(304, 204);
	sf::Sprite map;
	map.setTexture(m);

	sf::Font font;
	font.loadFromFile("score.ttf");
	sf::Text text("",font, 40);
	text.setFillColor(sf::Color::Red);


	int fx;

	
	sf::SoundBuffer kill;
	kill.loadFromFile("kill.ogg");
	sf::Sound getkill(kill);

	sf::Music game;
	game.openFromFile("game.ogg");
	game.play();


	PLAYER Sonic(t);

	ENEMY  enemy1, enemy2;
	enemy1.set(e, 400, 450);
	enemy2.set(e, 1700, 450);


	sf::Clock clock;

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		time = time / 3000;

		if (time > 10) time = 0;

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			Sonic.currentFrameR = 0;
			Sonic.dx = -1;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			Sonic.currentFrameL = 0;
			Sonic.dx = 1;
		}



		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			if (Sonic.onGround)
			{
				Sonic.dy = -3;
				Sonic.onGround = false; 
			}


		}

		Sonic.update(time);
		enemy1.update(time);
		enemy2.update(time);

		if (Sonic.rect.left > 640)  offsetX = Sonic.rect.left - 640;
		offsetY = Sonic.rect.top - 400;

		if (Sonic.rect.intersects(enemy1.rect))
		{
			if (enemy1.life) {
				if (Sonic.dy > 0) { enemy1.dx = 0; enemy1.life = false; getkill.play(); }
				else isgameover = true;
			}
		}
		if (Sonic.rect.intersects(enemy2.rect))
		{
			if (enemy2.life) {
				if (Sonic.dy > 0) { enemy2.dx = 0; enemy2.life = false; getkill.play();}
				else isgameover = true;
			}
		}

		window.clear(sf::Color::White);

		for (int i=0; i < H; i++)
			for (int j = 0; j < W; j++)
			{
				if (TileMap[i][j] == 'B')
				{
					map.setTextureRect(sf::IntRect(181, 0, 64, 64));
				}
				if (TileMap[i][j] == 'O')
				{
					map.setTextureRect(sf::IntRect(0, 0, 64, 64));
				}
				if (TileMap[i][j] == 'F') {
					map.setTextureRect(sf::IntRect(84, 0, 64, 64));

					fx = j * 64 - 64;
				}
				if (TileMap[i][j] == ' ') continue;
				map.setPosition(j * 64 - offsetX , i * 64 - offsetY );
				window.draw(map);
			}
		if (Sonic.rect.left >= fx) {
			iswin = true;

		}
		if (isgameover) {
			window.draw(gameover);
			Sonic.currentFrameL = 0;
			Sonic.currentFrameR = 0;
			Sonic.rect.left = 256;
			Sonic.rect.top = 420;
		}
		if (iswin) {
			window.draw(youwin);
			Sonic.currentFrameL = 0;
			Sonic.currentFrameR = 0;
			Sonic.rect.left = fx;
			Sonic.rect.top = 228;
		}

		text.setString("SCORE: " + std::to_string(Sonic.score));;
		text.setPosition(40, 10);

		window.draw(text);
		window.draw(Sonic.sprite);
		window.draw(enemy1.sprite);
		window.draw(enemy2.sprite);
		window.display();

	}
	return 0;
}
