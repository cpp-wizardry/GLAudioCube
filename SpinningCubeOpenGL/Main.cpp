#include "stdafx.h"
#include "Functions.h"
#include "GlobalVar.h"
#include "GlobalShader.h"
#include "AudioManager.h"
#include "3DModelLoader.h"
#include "AppCon.h"
#include <glm/gtc/type_ptr.hpp>

int main() {
    GLFWwindow* window = nullptr;
    if (!initialize(window)) return -1;

    Cube cube;
    AudioManager audio;
    if (!audio.init()) return -1;

    std::string objFilePath;
    while (objFilePath.empty()) objFilePath = openFileDialog(MODEL_3D);
    std::vector<float> vertices = loadOBJ(objFilePath);
    if (vertices.empty()) return -1;
    centerAndNormalizeOBJ(vertices);

    unsigned int VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, lightVertexShader);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, lightFragmentShader);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glEnable(GL_DEPTH_TEST);

    std::string imagePath;
    while (imagePath.empty()) imagePath = openFileDialog(TEXTURE);
    unsigned int texture = loadTexture(imagePath.c_str());

    std::string audioPath = openFileDialog(AUDIO);
    if (!audioPath.empty()) {
        if (!audio.loadWavFile(audioPath)) audio.startMicrophone();
        else audio.playBack();
    }
    else {
        audio.startMicrophone();
    }

    AppContext ctx{};
    ctx.VBO = VBO;
    ctx.VAO = VAO;
    ctx.texture = texture;
    ctx.vertexCount = vertices.size() / 8;
    ctx.audio = &audio;
    ctx.currentWavPath = audioPath;

    size_t audioCursorAdvance = 1024;

    while (!glfwWindowShouldClose(window)) {
        processInputs(window, cube, ctx);

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
        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        glm::vec3 viewPos(0.0f, 0.0f, 3.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewPos));
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 1.0f, 1.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ctx.texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        glBindVertexArray(ctx.VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(ctx.vertexCount));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    if (texture) glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);

    audio.stop();
    glfwTerminate();
    return 0;
}
