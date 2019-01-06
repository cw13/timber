#include <sstream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "ResourcePath.hpp"


const int NUM_BRANCHES=6;
sf::Sprite branches[NUM_BRANCHES];

enum class side { LEFT, RIGHT, NONE };
side branchPoisitions[NUM_BRANCHES];

void updateBranches(int seed)
{
    // Move all the branches down one place
    for (int j = NUM_BRANCHES-1; j > 0; j--)
    {
        branchPoisitions[j] = branchPoisitions[j-1];
    }
    
    // Spawn a new branch at pos 0
    srand((int)time(0)+seed);
    int r = (rand() % 5);
    switch (r) {
        case 0:
            branchPoisitions[0] = side::LEFT;
            break;
            
        case 1:
            branchPoisitions[0] = side::RIGHT;
            break;
            
        default:
            branchPoisitions[0] = side::NONE;
            break;
    }
};


int main ()  //, char const**)
{
    std::string resources = resourcePath();

    // Create the main window
    sf::VideoMode vm(1920, 1080);
    sf::RenderWindow window(vm, "Timber!!!");

    window.setVerticalSyncEnabled(true);

    // Set the Icon
//    sf::Image icon;
//    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
//        return EXIT_FAILURE;
//    }
//    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Load a sprite to display
    sf::Texture textureBackground;
    if (!textureBackground.loadFromFile(resources + "graphics/background.png")) {
        return EXIT_FAILURE;
    }

    // Make the background
    sf::Sprite spriteBackground;
    spriteBackground.setTexture(textureBackground);
    spriteBackground.setPosition(0, 0);

    // Make a tree sprite
    sf::Texture textureTree;
    textureTree.loadFromFile(resources + "graphics/tree.png");

    sf::Sprite spriteTree;
    spriteTree.setTexture(textureTree);
    spriteTree.setPosition(810, 0);

    // Make a bee
    sf::Texture textureBee;
    textureBee.loadFromFile(resources + "graphics/bee.png");
    sf::Sprite spriteBee;
    spriteBee.setTexture(textureBee);
    spriteBee.setPosition(0, 800);

    bool beeActive = false;
    
    float beeSpeed = 0.0f;
    
    // Clouds n that
    sf::Texture textureCloud;
    textureCloud.loadFromFile(resources + "graphics/cloud.png");
    
    const int NUM_CLOUDS = 10;
    sf::Sprite clouds[NUM_CLOUDS];
    
    float cloudSpeeds[NUM_CLOUDS];
    bool cloudsActive[NUM_CLOUDS];
    
    for (int i = 0; i < NUM_CLOUDS; i++)
    {
        clouds[i].setTexture(textureCloud);
        clouds[i].setPosition(-300, i*150);
        cloudsActive[i] = false;
        cloudSpeeds[i] = 0;
    }
    
    // Variabled to control time
    sf::Clock clock;

    // Time bar
    sf::RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    
    timeBar.setSize(sf::Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(sf::Color::Red);
    timeBar.setPosition(1920/2 - timeBarStartWidth, 980);
    
    sf::Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;
    
    bool paused = true;
    
    // Draw some text
    int score = 0;
    
    sf::Text messageText;
    sf::Text scoreText;
    
    // Create a graphical text to display
    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "fonts/KOMIKAP_.ttf")) {
        return EXIT_FAILURE;
    }
    
    messageText.setFont(font);
    scoreText.setFont(font);

    messageText.setString("Press P to Start!");
    scoreText.setString("Score = 0");
    
    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);
    
    messageText.setFillColor(sf::Color::White);
    scoreText.setFillColor(sf::Color::White);
    
    sf::FloatRect textRect = messageText.getLocalBounds();
    
    messageText.setOrigin(
                          textRect.left + textRect.width / 2.0f,
                          textRect.top + textRect.height /2.0f
                          );
    
    messageText.setPosition(1920/2.0f, 1080/2.0f);
    scoreText.setPosition(20, 20);
    
    sf::Texture textureBranch;
    textureBranch.loadFromFile(resourcePath() + "graphics/branch.png");
    
    for (int i = 0; i < NUM_BRANCHES; i++)
    {
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(-2000, -2000);
        // set sprites origin to the center
        branches[i].setOrigin(220, 20);
    }

    // Load a music to play
    sf::Music music;
    if (!music.openFromFile(resourcePath() + "nice_music.ogg")) {
        return EXIT_FAILURE;
    }

    // Play the music
    music.play();
    
    // Prepare the player
    sf::Texture texturePlayer;
    texturePlayer.loadFromFile(resourcePath() + "graphics/player.png");
    
    sf::Sprite spritePlayer;
    spritePlayer.setTexture(texturePlayer);
    spritePlayer.setPosition(580, 720);
    
    // Player starts on left
    side playerSide = side::LEFT;
    
    // Prepare the gravestone
    sf::Texture textureRIP;
    textureRIP.loadFromFile(resourcePath() + "graphics/rip.png");
    
    sf::Sprite spriteRIP;
    spriteRIP.setTexture(textureRIP);
    spriteRIP.setPosition(600, 860);
    
    // Prepare the axe
    sf::Texture textureAxe;
    textureAxe.loadFromFile(resourcePath() + "graphics/axe.png");
    
    sf::Sprite spriteAxe;
    spriteAxe.setTexture(textureAxe);
    spriteAxe.setPosition(700, 830);
    
    // Line up axe with tree
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;
    
    // Prepare the flying log
    sf::Texture textureLog;
    textureLog.loadFromFile(resourcePath() + "graphics/log.png");
    
    sf::Sprite spriteLog;
    spriteLog.setTexture(textureLog);
    spriteLog.setPosition(810, 720);
    
    // Some other useful log related variables
    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;
    
    bool acceptInput = false;
    
    // prepare the sound
    sf::SoundBuffer chopBuffer;
    chopBuffer.loadFromFile(resourcePath() + "sound/chop.wav");
    sf::Sound chop;
    chop.setBuffer(chopBuffer);
    
    sf::SoundBuffer deathBuffer;
    deathBuffer.loadFromFile(resourcePath() + "sound/death.wav");
    sf::Sound death;
    death.setBuffer(deathBuffer);
    
    // Out of time (oot)
    sf::SoundBuffer ootBuffer;
    ootBuffer.loadFromFile(resourcePath() + "sound/out_of_time.wav");
    sf::Sound outOfTime;
    outOfTime.setBuffer(ootBuffer);

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyReleased && !paused)
            {
                acceptInput = true;
                
                spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
            }
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
            {
                paused = false;
                    
                score = 0;
                timeRemaining = 5;
                
                for (int i = 1; i < NUM_BRANCHES; i++)
                {
                    branchPoisitions[i] = side::NONE;
                }
                
                // Make sure the gravestone is hidden
                spriteRIP.setPosition(675, 2000);
                
                spritePlayer.setPosition(580, 720);
                
                acceptInput = true;
            }
        }
        
        // Only accept player controls if we are accepting input
        if (acceptInput)
        {
            if (
                sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::D)
                )
            {
                playerSide = side::RIGHT;
                score ++;
                
                // add to the amount of time remaining
                timeRemaining += (2/score) + .15;
                
                spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);
                
                spritePlayer.setPosition(1200, 720);
                
                updateBranches(score);

                chop.play();
                spriteLog.setPosition(810, 720);
                logSpeedX = -3500;
                logActive = true;
                
                acceptInput = false;
            }
            
            if (
                sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
                )
            {
                // Put the player on the left
                playerSide = side::LEFT;
                score++;
                
                timeRemaining += (2 / score) + .15;
                
                spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);
                
                spritePlayer.setPosition(580, 720);
                
                // update the branches
                updateBranches(score);
                
                chop.play();

                spriteLog.setPosition(810, 720);
                
                logSpeedX = 3500;
                logActive = true;
                
                acceptInput = false;
            }
            
            // handle the left cursor key
        }

        // Clear screen
        window.clear();

        // Update the scene
        
        if (!paused)
        {
            // Measure time
            sf::Time dt = clock.restart();
            
            const float dtSeconds = dt.asSeconds();
            
            timeRemaining -= dt.asSeconds();
            printf("%f\n", dtSeconds);
            
            timeBar.setSize(sf::Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));
            
            if (timeRemaining <= 0.0f)
            {
                paused = true;
                
                messageText.setString("Out of time!!!");
                
                // reposition the text based on new size
                sf::FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(
                                      textRect.left + textRect.width / 2.0f,
                                      textRect.top + textRect.height / 2.0f
                                      );
                
                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
                
                outOfTime.play();
            }
        
            // Setup the bee
            if (!beeActive)
            {
                // How fast is the bee
                srand((int)time(0));
                beeSpeed = (rand() % 200) + 200;
                
                // How high is the bee
                srand((int)time(0));
                float height = (rand() % 500) + 500;
                spriteBee.setPosition(2000, height);
                beeActive = true;
            } else {
                spriteBee.setPosition(
                    spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),
                    spriteBee.getPosition().y);
                
                // If the bee is off the screen reset the bee
                if (spriteBee.getPosition().x < -100)
                {
                    beeActive = false;
                }
            }
        
            // Setup the clouds
        
            for (int i = 0; i < NUM_CLOUDS; i++)
            {
                if (!cloudsActive[i])
                {
                    // set up the cloud speed
                    srand((int)time(0) * i);
                    cloudSpeeds[i] = (rand() % 200);
                    
                    // set up the cloud height
                    srand((int)time(0) * i);
                    float height = (rand() % 150);
                    clouds[i].setPosition(-200, height);
                    cloudsActive[i] = true;
                }
                else
                {
                    clouds[i].setPosition(
                        clouds[i].getPosition().x + (cloudSpeeds[i] * dt.asSeconds()),
                        clouds[i].getPosition().y);
                    
                    if (clouds[i].getPosition().x > 1920)
                    {
                        cloudsActive[i] = false;
                    }
    
                }
            }

            
            // update the score text
            std::stringstream ss;
            ss << "Score = " << score;
            scoreText.setString(ss.str());
            
            // update the branch sprites
            for (int i = 0; i < NUM_BRANCHES; i++)
            {
                float height = i * 150;
                if (branchPoisitions[i] == side::LEFT)
                {
                    // Move and flip
                    branches[i].setPosition(610, height);
                    branches[i].setRotation(180);
                }
                else if (branchPoisitions[i] == side::RIGHT)
                {
                    branches[i].setPosition(1330, height);
                    branches[i].setRotation(0);
                }
                else
                {
                    // Hide the branch
                    branches[i].setPosition(3000, height);
                
            }
            
            if (logActive)
            {
                spriteLog.setPosition(
                                      spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
                                      spriteLog.getPosition().y + (logSpeedY * dt.asSeconds())
                                      );
                
                // stop log if reaches the edge
                if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000)
                {
                    logActive = false;
                    spriteLog.setPosition(810, 720);
                }
            }
                
            // Is the player squashed by a branch
            if (branchPoisitions[5] == playerSide)
            {
                paused = true;
                acceptInput = false;
                
                spriteRIP.setPosition(525, 760);
                
                spritePlayer.setPosition(2000, 660);
                
                messageText.setString("YOU DIED!");
                
                sf::FloatRect textRect = messageText.getLocalBounds();
                
                messageText.setOrigin(
                                      textRect.left + textRect.width/2.0f,
                                      textRect.top + textRect.height/2.0f
                                      );
                
                messageText.setPosition(1920/2.0f, 1080/2.0f);
                
                death.play();
            }
        
            }
        }
        
        // Draw the scene

        // Draw the background
        window.draw(spriteBackground);

        for (int i = 0; i < NUM_CLOUDS; i++)
        {
            window.draw(clouds[i]);
        }

        
        // draw the branches
        for (int i = 0; i < NUM_BRANCHES; i++)
        {
            window.draw(branches[i]);
        }

        window.draw(spriteTree);

        window.draw(spritePlayer);

        window.draw(spriteAxe);

        window.draw(spriteLog);

        window.draw(spriteRIP);

        window.draw(spriteBee);

        // Draw the string
        window.draw(scoreText);
        
        // Draw time bar
        window.draw(timeBar);
        
        if (paused)
        {
            // Draw our message
            window.draw(messageText);
        }

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
