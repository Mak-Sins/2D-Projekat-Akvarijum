#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "../Header/Util.h"
// Main fajl funkcija sa osnovnim komponentama OpenGL programa

// Projekat je dozvoljeno pisati počevši od ovog kostura
// Toplo se preporučuje razdvajanje koda po fajlovima (i eventualno potfolderima) !!!
// Srećan rad!

int main()
{
    if (!glfwInit()) return endProgram("GLFW nije uspeo da se inicijalizuje");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Kreiranje full-screen prozora
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Akvarijum", primary, NULL);
    if (window == NULL) return endProgram("Prozor nije uspeo da se kreira.");
    glfwMakeContextCurrent(window);

    // Frame limiter
    const double FPS = 75.0;
    double lastTime = 0.0;


    if (glewInit() != GLEW_OK) return endProgram("GLEW nije uspeo da se inicijalizuje.");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glClearColor(0.31f, 0.76f, 0.93f, 0.8f);

    // --- Strukture za objekte u akvarijumu ---
    struct Object2D {
        float x, y;
        float width, height;
        unsigned int textureID;
        float alpha;
    };

    struct Fish {
        float x, y;
        float width, height;
        float speed;
        bool facingRight;
        unsigned int textureID;

        float left() const {
            return facingRight ? x : x - width;
        }
        float right() const {
            return facingRight ? x + width : x;
        }
        float top() const {
            return y + height;
        }
        float bottom() const {
            return y;
        }
    };

    struct Bubble {
        float x, y;
        float speed;
    };

    struct FoodParticle {
        float x, y;
        bool eaten;
        float size;
        float rotation;
    };
    std::vector<Bubble> bubbles;
    std::vector<FoodParticle> foodParticles;
 

    // --- Učitavanje tekstura i kursora ---
    
  
    unsigned int sandTexture = loadImageToTexture("Resources/Textures/sanddd.png");
	unsigned int seaweedTexture = loadImageToTexture("Resources/Textures/seaweed1.png");
    unsigned int goldFishTexture = loadImageToTexture("Resources/Textures/fishgold1.png");
	unsigned int clownFishTexture = loadImageToTexture("Resources/Textures/fishclown.png");

    unsigned int chestClosedTex = loadImageToTexture("Resources/Textures/chestclosed.png");
    unsigned int chestOpenTex = loadImageToTexture("Resources/Textures/chestopen.png");
    
	unsigned int bubbleTexture = loadImageToTexture("Resources/Textures/bubble.png");
	unsigned int sushiRollTexture = loadImageToTexture("Resources/Textures/sushiroll.png");

	unsigned int coinsTexture = loadImageToTexture("Resources/Textures/coinsstack.png");
	unsigned int gemTexture = loadImageToTexture("Resources/Textures/bluegem.png");


    // Kursor u obliku sidra - resicemo kasnije
    GLFWcursor* anchorCursor = loadImageToCursor("Resources/Cursor/anchor_cursor.png");
    if (anchorCursor) glfwSetCursor(window, anchorCursor);
   


    // --- Učitavanje šejdera ---
    unsigned int shaderProgram = createShader("basic.vert", "basic.frag");
    glUseProgram(shaderProgram);

    float quadVertices[] = {
        // pozicija   // texCoords
        0.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 0.0f,   1.0f, 0.0f,
        0.0f, 0.0f,   0.0f, 0.0f,
        0.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 1.0f,   1.0f, 1.0f,
        1.0f, 0.0f,   1.0f, 0.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    // Atribut 0 (pozicija):
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Atribut 1 (boja):
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // --- Projekcija ---
    int width = mode->width;
    int height = mode->height;
    Object2D water = {
        0.0f,                // x
        0.0f,                // y (donja ivica ekrana)
        (float)width,        // širina
        (float)height * 0.75f,// visina vode (75% visine ekrana)
        0,                   // tekstura (nema)
        0.2f                 // providnost
    };
    Object2D seaweed1 = {
        width * 0.25f,               // x
        0.0f, // y (na dnu vode)
        120.0f,               // širina
        220.0f,             // visina
        seaweedTexture, // tekstura
        1.0f                 // alpha
	};
    Object2D seaweed2 = {
        width * 0.75f,               // x
        0.0f, // y (na dnu vode)
        120.0f,               // širina
        200.0f,             // visina
        seaweedTexture, // tekstura
        1.0f                 // alpha
    };
    Fish goldFish = {

        width / 2.0f,  // x
        height * 0.3f, // y
        140.0f,         // širina
        90.0f,         // visina
        180.0f,       // brzina
        true,          // okrenut desno
        goldFishTexture       // tekstura
    };
    Fish clownFish = {
        width / 3.0f,  // x
        height * 0.4f, // y
        140.0f,         // širina
        90.0f,         // visina
        180.0f,       // brzina
        true,         // okrenut levo
        clownFishTexture       // tekstura
	};
    auto spawnBubbles = [&](float startX, float startY) {
        for (int i = 0; i < 3; i++) {
            Bubble b;
            b.x = startX + (rand() % 40 - 20); // malo nasumično levo-desno
            b.y = startY;
            b.speed = 150.0f + rand() % 50;    // brzina između 150-200
            bubbles.push_back(b);
        }
    };
    auto spawnFood = [&]() {
        for (int i = 0; i < 8; i++) {
            FoodParticle f;
            f.x = rand() % width;
            f.y = height; // počinje iznad ekrana
            f.eaten = false;
            f.size = 25.0f + rand() % 10; // veličina između 25 i 35 piksela
            foodParticles.push_back(f);
            f.rotation = (rand() % 360) * 3.14159f / 180.0f; // u radijanima
        }
    };
    auto checkCollision = [](const Fish& fish, const FoodParticle& food) {
        return !(fish.right() < food.x || fish.left() > food.x + 10.0f ||
            fish.top() < food.y || fish.bottom() > food.y + 10.0f);
        };



    bool chestOpen = false;

    Object2D chest = {
        width * 0.5f - 160.0f,   // x (centar dna)
        0.0f,                   // y (na pesku)
        320.0f,                 // širina
        240.0f,                 // visina
        chestClosedTex,         // početno zatvoren
        1.0f                    // alpha
    };
    // --- NOVČIĆI I DRAGULJ (pojavljuju se samo kad je kovčeg otvoren) ---
    std::vector<Object2D> chestItems;

    // Zlatni novčići (3 komada)
    for (int i = 0; i < 6; i++) {
        chestItems.push_back({
            chest.x + 60.0f + i * 30.0f,  // pomereni horizontalno
            chest.y + 35.0f,              // malo iznad dna kovčega
            45.0f, 45.0f,
            loadImageToTexture("Resources/Textures/coinsstack.png"),
            1.0f
            });
    }
    for (int i = 0; i < 5; i++) {
        chestItems.push_back({
            chest.x + 80.0f + i * 30.0f,
            chest.y + 65.0f,
            45.0f, 45.0f,
            loadImageToTexture("Resources/Textures/coinsstack.png"),
            1.0f
        });
    }
    
    // Dragulj u sredini
    chestItems.push_back({
        chest.x + chest.width / 2.0f - 17.0f,
        chest.y + 120.0f,
        35.0f, 35.0f,
        loadImageToTexture("Resources/Textures/bluegem.png"),
        1.0f
        });

    float projection[16] = {
        2.0f / width, 0, 0, 0,
        0, 2.0f / height, 0, 0,
        0, 0, 1, 0,
        -1, -1, 0, 1
    };

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    int alphaLoc = glGetUniformLocation(shaderProgram, "alpha");
	int colorLoc = glGetUniformLocation(shaderProgram, "overrideColor");
	int timeLoc = glGetUniformLocation(shaderProgram, "time");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
	float moveSpeed = 400.0f; // brzina kretanja ribe
    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;

        if (currentTime - lastTime < 1.0 / FPS) continue;
        lastTime = currentTime;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        // --- KONTROLE ZA RIBU (WASD) ---
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            goldFish.y += moveSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            goldFish.y -= moveSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            goldFish.x -= moveSpeed * deltaTime;
            goldFish.facingRight = false;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            goldFish.x += moveSpeed * deltaTime;
            goldFish.facingRight = true;
        }
        // --- KONTROLE ZA KLOVN RIBU (strelice) ---
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            clownFish.y += moveSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            clownFish.y -= moveSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            clownFish.x -= moveSpeed * deltaTime;
            clownFish.facingRight = false;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            clownFish.x += moveSpeed * deltaTime;
            clownFish.facingRight = true;
        }
        // --- Mehurići ---
        // Z za zlatnu ribu
        static bool zPressedLastFrame = false;
        bool zPressedNow = glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS;
        if (zPressedNow && !zPressedLastFrame) {
            float bubbleX = goldFish.facingRight
                ? goldFish.x + goldFish.width * 0.5f   // ako ide desno
                : goldFish.x - goldFish.width * 0.5f;  // ako ide levo
            float bubbleY = goldFish.y + goldFish.height * 0.5f; // sredina visine
            spawnBubbles(bubbleX, bubbleY);
        }

        zPressedLastFrame = zPressedNow;

        // K za klovnovu ribu
        static bool kPressedLastFrame = false;
        bool kPressedNow = glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS;
        if (kPressedNow && !kPressedLastFrame) {
            float bubbleX = clownFish.facingRight
                ? clownFish.x + clownFish.width * 0.5f
                : clownFish.x - clownFish.width * 0.5f;
            float bubbleY = clownFish.y + clownFish.height * 0.5f;
            spawnBubbles(bubbleX, bubbleY);
        }

        kPressedLastFrame = kPressedNow;

        // Granice da ne izleti iz akvarijuma
        if (goldFish.x < goldFish.width) goldFish.x = goldFish.width;
        if (goldFish.x + goldFish.width > width) goldFish.x = width - goldFish.width;
        if (goldFish.y < 0) goldFish.y = 0;
        if (goldFish.y + goldFish.height > height * 0.75) goldFish.y = height * 0.75f - goldFish.height;

        if (clownFish.x < goldFish.width) clownFish.x = clownFish.width;
        if (clownFish.x + clownFish.width > width) clownFish.x = width - clownFish.width;
        if (clownFish.y < 0) clownFish.y = 0;
        if (clownFish.y + clownFish.height > height * 0.75f) clownFish.y = height * 0.75f - clownFish.height;

        // Pomeri mehuriće nagore
        for (auto& b : bubbles)
            b.y += b.speed * deltaTime;

        // Ukloni mehuriće koji su iznad vode
        bubbles.erase(
            std::remove_if(bubbles.begin(), bubbles.end(),
                [&](const Bubble& b) { return b.y > height * 0.6f; }),
            bubbles.end()
        );
        // --- Padanje hrane (ENTER) ---
        static bool enterPressedLast = false;
        bool enterNow = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;

        if (enterNow && !enterPressedLast)
            spawnFood();

        enterPressedLast = enterNow;

        for (auto& f : foodParticles) {
            if (!f.eaten) {
                f.y -= 150.0f * deltaTime; // brzina padanja
                if (f.y < height * 0.11f)  // dodiruje pesak
                    f.y = height * 0.11f;
            }
        }

        for (auto& f : foodParticles) {
            if (f.eaten) continue;

            // Zlatna riba jede
            if (checkCollision(goldFish, f)) {
                f.eaten = true;
                goldFish.height *= 1.01f; // poveća se za 1%
                continue;
            }

            // Klovn riba jede (ako postoji)
            if (checkCollision(clownFish, f)) {
                f.eaten = true;
                clownFish.height *= 1.01f;
                continue;
            }
        }

        // --- OTVARANJE/ZATVARANJE KOVČEGA ---
        static bool cPressedLastFrame = false;
        bool cPressedNow = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;

        if (cPressedNow && !cPressedLastFrame) {
            chestOpen = !chestOpen; // promeni stanje
            chest.textureID = chestOpen ? chestOpenTex : chestClosedTex;
        }

        cPressedLastFrame = cPressedNow;



        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Aktiviraj teksturni slot i uniformu
        glActiveTexture(GL_TEXTURE0);
        int texLoc = glGetUniformLocation(shaderProgram, "texture1");
        glUniform1i(texLoc, 0);
        int colorLoc = glGetUniformLocation(shaderProgram, "overrideColor");

        // ---------------------
        // PESAK
        // ---------------------
        glBindTexture(GL_TEXTURE_2D, sandTexture);

        float sandModel[16] = {
            (float)width, 0, 0, 0,
            0, (float)height * 0.11f, 0, 0, // visina peska
            0, 0, 1, 0,
            0, 0, 0, 1
        };
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, sandModel);
        glUniform1f(alphaLoc, 1.0f);
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // reset override boje
        glDrawArrays(GL_TRIANGLES, 0, 6);
		// ---------------------
        // --- TRAVA ---
		// ---------------------
        float sway1 = sin(glfwGetTime() * 1.5f) * 15.0f;
        float sway2 = sin(glfwGetTime() * 1.2f + 1.0f) * 10.0f;

        // leva trava
        float seaweedModel1[16] = {
            seaweed1.width, 0, 0, 0,
            0, seaweed1.height, 0, 0,
            0, 0, 1, 0,
            seaweed1.x + sway1, seaweed1.y, 0, 1
        };
        glBindTexture(GL_TEXTURE_2D, seaweed1.textureID);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, seaweedModel1);
        glUniform1f(alphaLoc, seaweed1.alpha);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // desna trava
        float seaweedModel2[16] = {
            seaweed2.width, 0, 0, 0,
            0, seaweed2.height, 0, 0,
            0, 0, 1, 0,
            seaweed2.x + sway2, seaweed2.y, 0, 1
        };
        glBindTexture(GL_TEXTURE_2D, seaweed2.textureID);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, seaweedModel2);
        glUniform1f(alphaLoc, seaweed2.alpha);
        glDrawArrays(GL_TRIANGLES, 0, 6); 
		// ---------------------
        // --- ZLATNA RIBA ---
		// ---------------------
        float fishModel[16] = {
            goldFish.facingRight ? goldFish.width : -goldFish.width, 0, 0, 0,
            0, goldFish.height, 0, 0,
            0, 0, 1, 0,
            goldFish.x, goldFish.y, 0, 1
        };
        glBindTexture(GL_TEXTURE_2D, goldFish.textureID);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, fishModel);
        glUniform1f(alphaLoc, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // ---------------------
        // --- KLOVN RIBA ---
        // ---------------------
        float clownModel[16] = {
            clownFish.facingRight ? clownFish.width : -clownFish.width, 0, 0, 0,
            0, clownFish.height, 0, 0,
            0, 0, 1, 0,
            clownFish.x, clownFish.y, 0, 1
        };
        glBindTexture(GL_TEXTURE_2D, clownFish.textureID);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, clownModel);
        glUniform1f(alphaLoc, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // ---------------------
        // --- MEHURIĆI ---
        // ---------------------
        glBindTexture(GL_TEXTURE_2D, bubbleTexture); // mehurići nemaju teksturu, samo boju
        for (const auto& b : bubbles) {
            float bubbleModel[16] = {
                45.0f, 0, 0, 0,
                0, 45.0f, 0, 0,
                0, 0, 1, 0,
                b.x, b.y, 0, 1
            };
        for (auto& b : bubbles) {
            b.y += b.speed * deltaTime * 0.2f;
            b.x += sin(glfwGetTime() * 2.0f + b.y * 0.05f) * 5.0f * deltaTime; // blago njihanje
        }
            bubbles.erase(std::remove_if(bubbles.begin(), bubbles.end(),
                [&](const Bubble& b) { return b.y > height * 0.6f; }), bubbles.end());

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, bubbleModel);
            glUniform1f(alphaLoc, 0.9f - (b.y / (height * 0.6f)) * 0.5f); // postepeno blede
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // ---------------------
        // --- HRANA ---
        // ---------------------
        glBindTexture(GL_TEXTURE_2D, sushiRollTexture); // bez teksture, samo boja
        for (const auto& f : foodParticles) {
            if (f.eaten) continue;
            float cosA = cos(f.rotation);
            float sinA = sin(f.rotation);
            float size = f.size > 0 ? f.size : 30.0f;

            float foodModel[16] = {
                cosA * size, -sinA * size, 0, 0,
                sinA * size,  cosA * size, 0, 0,
                0, 0, 1, 0,
                f.x, f.y, 0, 1
            };

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, foodModel);
            glUniform1f(alphaLoc, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

		// ---------------------
        // --- KOVČEG ---
		// ---------------------
        glBindTexture(GL_TEXTURE_2D, chest.textureID);

        float chestModel[16] = {
            chest.width, 0, 0, 0,
            0, chest.height, 0, 0,
            0, 0, 1, 0,
            chest.x, chest.y, 0, 1
        };

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, chestModel);
        glUniform1f(alphaLoc, chest.alpha);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // --- NOVČIĆI I DRAGULJ UNUTAR KOVČEGA ---
        if (chestOpen) {
            for (const auto& item : chestItems) {
                glBindTexture(GL_TEXTURE_2D, item.textureID);
                float itemModel[16] = {
                    item.width, 0, 0, 0,
                    0, item.height, 0, 0,
                    0, 0, 1, 0,
                    item.x, item.y, 0, 1
                };
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, itemModel);
                glUniform1f(alphaLoc, item.alpha);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }


        // ---------------------
        // VODA
        // ---------------------
        float waveAlpha = 0.2f + 0.05f * sin(glfwGetTime() * 3.0f);  //dodatno: presijavanje akvarijumskog stakla

        float waterModel[16] = {
            water.width, 0, 0, 0,
            0, water.height, 0, 0,
            0, 0, 1, 0,
            water.x, water.y, 0, 1
        };
        glBindTexture(GL_TEXTURE_2D, 0); // nema teksture, samo boja
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, waterModel);
		glUniform1f(alphaLoc, waveAlpha);                           //dodatno: water.alpha umesto waveAlpha ako se ne zeli efekat presijavanja
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // bela boja vode
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ---------------------
        // IVICE AKVARIJUMA (crne)
        // ---------------------
        glUniform1f(alphaLoc, 1.0f);
        glUniform3f(colorLoc, 0.0f, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Donja ivica
        float borderBottom[16] = {
            (float)width, 0, 0, 0,
            0, 10.0f, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, borderBottom);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Leva ivica
        float borderLeft[16] = {
            10.0f, 0, 0, 0,
            0, water.height, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, borderLeft);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Desna ivica
        float borderRight[16] = {
            10.0f, 0, 0, 0,
            0, water.height, 0, 0,
            0, 0, 1, 0,
            (float)width - 10.0f, 0, 0, 1
        };
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, borderRight);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ---------------------
        // ZAVRŠETAK FRAME-A
        // ---------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}