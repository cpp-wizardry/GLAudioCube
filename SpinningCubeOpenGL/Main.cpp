#include "functions.h"
#include "GlobalVar.h"
#include "GlobalShader.h"


int main() {
    GLFWwindow* window = nullptr;
    if (!initialize(window)) {
        std::cerr << "GLFW initialization failed\n";
        return -1;
    }

    std::string objFilePath = openFileDialog(MODEL_3D);
    std::vector<float> vertices = loadOBJ(objFilePath);
    if (vertices.empty()) {
        std::cerr << "Failed to load OBJ file\n";
        return -1;
    }
    centerAndNormalizeOBJ(vertices);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);


    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, IvertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, IfragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glEnable(GL_DEPTH_TEST);


    std::string imagePath = openFileDialog(TEXTURE);
    unsigned int texture = loadTexture(imagePath.c_str());


    AudioManager audio;
    if (!audio.init()) {
        std::cerr << "PortAudio init failed\n";
        return -1;
    }

    std::string audioPath = openFileDialog(AUDIO);
    if (!audioPath.empty()) {
        if (!audio.loadWavFile(audioPath)) std::cerr << "WAV load failed\n";
        else audio.playBack();
    }
    else {
        audio.startMicrophone();
    }

    size_t audioCursorAdvance = 1024;
    Cube cube;

    while (!glfwWindowShouldClose(window)) {
        processInputs(window, cube);

        float amp = audio.getAmplitude(audioCursorAdvance);
        float scale = cube.scale + amp * 2.0f;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, cube.rotX, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, cube.rotY, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, cube.rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(scale));

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;

        int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    audio.stop();
    return 0;
}