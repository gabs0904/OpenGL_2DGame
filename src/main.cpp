#include <OpenGLPrj.hpp>

#include <GLFW/glfw3.h>

#include <Shader.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Ground.hpp>
#include <Platform.hpp>
#include <Player.hpp>
#include <Coin.hpp>
#include <ShaderProgram.h>
#include <ft2build.h>
#include <Sprite.hpp>
#include FT_FREETYPE_H

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window);
void mouse_button_callback1(GLFWwindow* window, int button, int action);
void ProcessKeyboardInput(GLFWwindow* window,Player& player,float deltaTime);

std::string FormatTime(int timeNow);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 500;

// Player
Player player;
 float circleRadius = 0.09f;
 float circleRadius1 = 0.16f;
const int numOfCircleVertices = 20;
float circleVertices[(numOfCircleVertices + 1) * 6];
void CalculatePlayerData();


// Ground
unsigned int rectangleIndices[6];
float groundVertices[(4 * 3)+(4*2)]; // 4 points, 3 coordinates each
float textureVertices[4 * 2];
void CalculateGroundData();

// Platforms
const int numOfPlatforms = 4;
float platformsVertices[5 * 3]; // 4 points, 3 coordinates each + texture coords
glm::vec3 platformsPositions[numOfPlatforms];
void CalculatePlatformsData();

//Coins
const int numOfCoinVertices = 20;
const int numOfCoins = 10;
float coinVertices[(numOfCoinVertices + 1) * 3]; // 4 points, 3 coordinates each + texture coords
glm::vec3 coinPositions[numOfCoins];
void CalculateCoinData();

float lastFrame = 0.0f;
float deltaTime = 0.0f;

Sprite sprite;
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "The Owl House", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    std::string shader_location("../res/shaders/");

    std::string used_shaders("welcome");

    Shader ourShader(shader_location + used_shaders + std::string(".vert"),
        shader_location + used_shaders + std::string(".frag")
    );

    //float vertices[] = {
    //   -1.8f, -1.0f, -0.5f,  0.0f, 0.0f,
    //    1.8f, -1.0f, -0.5f,  1.0f, 0.0f,
    //    1.8f,  1.0f, -0.5f,  1.0f, 1.0f,
    //    1.8f,  1.0f, -0.5f,  1.0f, 1.0f,
    //   -1.8f,  1.0f, -0.5f,  0.0f, 1.0f,
    //   -1.8f, -1.0f, -0.5f,  0.0f, 0.0f,

    //};




    float vertices[] = {
        // positions             // colors           // texture coordinates
            0.999f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // top right
            0.999f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // bottom right
           -0.999f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // bottom left
           -0.999f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f  // top left

    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load("../res/textures/photo.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int texturebackground;
    glGenTextures(1, &texturebackground);
    glBindTexture(GL_TEXTURE_2D, texturebackground); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("../res/textures/sequence1.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int texturebackground1;
    glGenTextures(1, &texturebackground1);
    glBindTexture(GL_TEXTURE_2D, texturebackground1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("../res/textures/sequence2.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int texturebackground2;
    glGenTextures(1, &texturebackground2);
    glBindTexture(GL_TEXTURE_2D, texturebackground2); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("../res/textures/sequence3.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);



    unsigned int gamebackground;
    glGenTextures(1, &gamebackground);
    glBindTexture(GL_TEXTURE_2D, gamebackground); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("../res/textures/level.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int endbackground;
    glGenTextures(1, &endbackground);
    glBindTexture(GL_TEXTURE_2D, endbackground); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("../res/textures/end.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);





    float verticesbutton[] = {
        // positions             // colors           // texture coordinates
            0.3f,  -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // top right
            0.3f, -0.9f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // bottom right
           -0.3f, -0.9f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // bottom left
           -0.3f, - 0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f  // top left

    };


    unsigned int VBO1, VAO1, EBO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO1);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesbutton), verticesbutton, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    std::string bshader_location("../res/shaders/");

    std::string used_bshaders("button");

    Shader ourButtonShader(bshader_location + used_bshaders + std::string(".vert"),
        bshader_location + used_bshaders + std::string(".frag")
    );
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   unsigned char* data1 = stbi_load("../res/textures/buttongame.png", &width, &height, &nrChannels, 0);
    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data1);






    CalculatePlayerData();
    player.Setup(numOfCircleVertices, circleVertices, "../res/shaders/hootyShader.vert", "../res/shaders/hootyShader.frag", circleRadius);
   // Text timerText(0, 36, "../res/shaders/text.vert", "../res/shaders/text.frag", SCR_WIDTH, SCR_HEIGHT);
    CalculateGroundData();
    Ground ground(groundVertices, rectangleIndices, "../res/shaders/ground.vert", "../res/shaders/ground.frag");
    CalculatePlatformsData();
    std::vector<Platform> platforms;
    for (int i = 0; i < numOfPlatforms; i++)
        platforms.push_back(Platform(platformsVertices,rectangleIndices, "../res/shaders/platform.vert", "../res/shaders/platform.frag"));

 
    CalculateCoinData();
    std::vector<Coin> coins;
    for (int i = 0; i < numOfCoins; i++)
        coins.push_back(Coin(coinVertices, rectangleIndices, "../res/shaders/coinShader.vert", "../res/shaders/coinShader.frag"));

  
    sprite.Setup("../res/shaders/EdaHand.vert", "../res/shaders/EdaHand.frag");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);
 
        // render container
        ourShader.use();
        glBindVertexArray(VAO);
       
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture1);

        // render container
        ourButtonShader.use();
        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
       // mouse_button_callback(window);
        double xpos, ypos;
        int timeNow = (int)round(glfwGetTime());
       // timerText.RenderText(FormatTime(timeNow), 550.0f, 650.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        //getting cursor position
        glfwGetCursorPos(window, &xpos, &ypos);
      //  std::cout << "Cursor Position at (" << xpos << " : " << ypos << std::endl;
        bool canPlay = false;
        static int clicked = 0;
      
        if (xpos < 544 && xpos>480 && ypos > 375 && ypos < 474 ) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

               
                canPlay = true;
                clicked = 1;
                //std::cout << "Button found and clicked" << std::endl;
            }

        }
        static int nextscene = 0;
        static int nextscenehoot = 0;
        static int level = 0;
        if (clicked==1 ) {
          
    
            glBindTexture(GL_TEXTURE_2D, texturebackground);

            // render container
            ourShader.use();
            glBindVertexArray(VAO);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            if (xpos < 936 && xpos>850 && ypos > 426 && ypos < 460 ){
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                    nextscene = 1;
                   
                }

            }
            if (nextscene == 1) {
                glBindTexture(GL_TEXTURE_2D, texturebackground1);
                ourShader.use();
                glBindVertexArray(VAO);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
                double currentFrame = glfwGetTime();
                if (xpos < 936 && xpos>850 && ypos > 426 && ypos < 460) {
                    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                        nextscenehoot = 1;

                    }

                }
                if (nextscenehoot == 1 && currentFrame>4) {
                    glBindTexture(GL_TEXTURE_2D, texturebackground2);
                    ourShader.use();
                    glBindVertexArray(VAO);

                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
                    if (xpos < 936 && xpos>850 && ypos > 426 && ypos < 460) {
                        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                            level = 1;

                        }

                    }

                }
                currentFrame = 0;
               
                if (level == 1) {
                    float crntFrame = (float)glfwGetTime();
                     deltaTime = crntFrame - lastFrame;
                    lastFrame = crntFrame;

                    glBindTexture(GL_TEXTURE_2D, gamebackground);
                    ourShader.use();
                    glBindVertexArray(VAO);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
                    ground.Draw();
                    for (int i = 0; i < numOfPlatforms; i++) platforms[i].Draw(platformsPositions[i]);
                    for (int i = 0; i < numOfCoins; i++) coins[i].Draw(coinPositions[i]);

                    sprite.DrawSprite();
                    player.Draw(deltaTime, numOfPlatforms, platforms,coins,sprite);
                    ProcessKeyboardInput(window,player,deltaTime);
                    
                    if (player.DetectHandCollision(sprite) && player.collected) {
                        currentFrame = glfwGetTime();
                        if (currentFrame > 15) {
                            glBindTexture(GL_TEXTURE_2D, endbackground);
                            ourShader.use();
                            glBindVertexArray(VAO);

                            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
                        }
                      
                    }

                }
            }
           
            
        }
       
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

  
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void ProcessKeyboardInput(GLFWwindow* window, Player& player, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); // close window when esc is pressed

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // jump
        player.Move(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // move right
        player.Move(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // move left
        player.Move(LEFT, deltaTime);


    glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ? player.GetHyper() : player.BeNormal();
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void mouse_button_callback(GLFWwindow* window )
{
   
        double xpos, ypos;
        //getting cursor position
        glfwGetCursorPos(window, &xpos, &ypos);
        if (xpos < 544 && xpos>528 && ypos>375 && ypos<474 ) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                std::cout << "Button found and clicked" << std::endl;
           }
            std::cout << "Cursor Position at (" << xpos << " : " << ypos << std::endl;
           
        }
   
    
}
void mouse_button_callback1(GLFWwindow* window, int button, int action)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        //getting cursor position
        glfwGetCursorPos(window, &xpos, &ypos);
        std::cout << "Cursor Position at (" << xpos << " : " << ypos << std::endl;
    }
}


void CalculateGroundData() {
    // Position data:
    // lower left
    groundVertices[0] = -1.0f;
    groundVertices[1] = -1.0f;
    groundVertices[2] = 0.0f;
    // lower right
    groundVertices[3] = 1.0f;
    groundVertices[4] = -1.0f;
    groundVertices[5] = 0.0f;
    // upper right
    groundVertices[6] = 1.0f;
    groundVertices[7] = -0.8f;
    groundVertices[8] = 0.0f;
    // upper left
    groundVertices[9] = -1.0f;
    groundVertices[10] = -0.8f;
    groundVertices[11] = 0.0f;


    //texture data
    groundVertices[12] = 1.0f;
    groundVertices[13] = 0.0f;
    groundVertices[14] = 1.0f;
    groundVertices[15] = 1.0f;
    groundVertices[16] = 0.0f;
    groundVertices[17] = 1.0f;
    groundVertices[18] = 0.0f;
    groundVertices[19] = 0.0f;




    // Indices data (EBO):
    // first triangle
    rectangleIndices[0] = 0;
    rectangleIndices[1] = 1;
    rectangleIndices[2] = 3;
    // second triangle
    rectangleIndices[3] = 1;
    rectangleIndices[4] = 2;
    rectangleIndices[5] = 3;



    textureVertices[0] = 1.0f;
    textureVertices[1] =  0.0f;
    textureVertices[2] = 1.0f;
    textureVertices[3] =  1.0f;
    textureVertices[4] = -1.0f;
    textureVertices[5] = -1.0f;
    textureVertices[6] = -1.0f;
    textureVertices[7] = -1.0f;

}
void CalculatePlatformsData() {
    // Position data:
    // first vertex
    platformsVertices[0] = -0.25f;
    platformsVertices[1] = -0.1f;
    platformsVertices[2] = 0.0f;
    // second vertex
    platformsVertices[3] = 0.25f;
    platformsVertices[4] = -0.1f;
    platformsVertices[5] = 0.0f;
    // third vertex
    platformsVertices[6] = 0.25f;
    platformsVertices[7] = 0.08f;
    platformsVertices[8] = 0.0f;
    // fourth vertex
    platformsVertices[9] = -0.25f;
    platformsVertices[10] = 0.08f;
    platformsVertices[11] = 0.0f;
    //texture vertices
    platformsVertices[12] = 0.0f;
    platformsVertices[13] = 1.0f;
    platformsVertices[14] = 0.0f;



    // translation vectors
    platformsPositions[0] = glm::vec3(0.75f, -0.2f, 0.0f);
    platformsPositions[1] = glm::vec3(-0.1f, -0.3f, 0.0f);
    platformsPositions[2] = glm::vec3(-0.7f, 0.2f, 0.0f);
    platformsPositions[3] = glm::vec3(0.0f, 0.5f, 0.0f);


}



void CalculatePlayerData() {
    float deg = 0.0f;

    // points on circumference
    for (int idx = 0; idx < 3 * numOfCircleVertices; idx += 3, deg += 360.0f / numOfCircleVertices) {
        circleVertices[idx + 0] = (circleRadius * (float)std::cos(glm::radians(deg))); // x coordinate
        circleVertices[idx + 1] = (circleRadius1 * (float)std::sin(glm::radians(deg))); // y coordinate
        circleVertices[idx + 2] = 0.0f; // z coordinate
        circleVertices[(2 * numOfCircleVertices) + idx] = 0.5+0.5* (float)std::cos(circleRadius);
        circleVertices[(2 * numOfCircleVertices) + idx+1] = 0.5+0.5*(float)std::sin(circleRadius1);
    }

    // center of circle
    circleVertices[3 * numOfCircleVertices + 0] = 0.0f;
    circleVertices[3 * numOfCircleVertices + 1] = 0.0f;
    circleVertices[3 * numOfCircleVertices + 2] = 0.0f;


}


void CalculateCoinData() {
    circleRadius = 0.04;
    circleRadius1 = 0.06;
    float twicePi = 2 * 3.14;
    // points on circumference
    for (int i = 0;i < 3* numOfCoinVertices;i+=3) {
        coinVertices[i] = circleRadius * cos(twicePi * i / numOfCoinVertices);
        coinVertices[i+1] = circleRadius1 * sin(twicePi * i / numOfCoinVertices);
        coinVertices[i+2] = 0.0f;
     
    }
   /* for (int idx = 0; idx < 3 * (numOfCircleVertices); idx += 3, deg += 350.0f / numOfCircleVertices + 45) {
        coinVertices[idx + 0] = (circleRadius * (float)std::cos(glm::radians(deg))); // x coordinate
        coinVertices[idx + 1] = (circleRadius1 * (float)std::sin(glm::radians(deg))); // y coordinate
        coinVertices[idx + 2] = 0.0f; // z coordinate
    }*/


    // translation vectors
    coinPositions[0] = glm::vec3(0.75f, -0.05f, 0.0f);
    coinPositions[1] = glm::vec3(-0.1f, -0.15f, 0.0f);
    coinPositions[2] = glm::vec3(-0.7f, 0.35f, 0.0f);
    coinPositions[3] = glm::vec3(0.0f, 0.65f, 0.0f);
    coinPositions[4] = glm::vec3(0.9f, -0.05f, 0.0f);
    coinPositions[5] = glm::vec3(0.05f, -0.15f, 0.0f);
    coinPositions[6] = glm::vec3(-0.85f, 0.35f, 0.0f);
    coinPositions[7] = glm::vec3(-0.15f, 0.65f, 0.0f);
    coinPositions[8] = glm::vec3(-0.55f, 0.35f, 0.0f);
    coinPositions[9] = glm::vec3(-0.25f, -0.15f, 0.0f);
}

