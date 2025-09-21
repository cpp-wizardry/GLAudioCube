#include "functions.h"
#include "GlobalVar.h"
#include "GlobalShader.h"

/*
int changeColor = 0;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (changeColor > 18)
        {
            changeColor = 0;
        }
        changeColor +=1;    
    }
}
*/


int main() {
    
    GLFWwindow* window = nullptr;
    if (initialize(window) == false)
    {
        std::cerr << "initialisation failed";
        return -1;
    }

    Cube Cube;
    

    //creation Vertex Array et Buffer
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
   

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
    
    WAV_HEADER header;
    std::vector<int16_t> data;

    std::string audioPath = openFileDialog(AUDIO);
    for (size_t i = 0; i < data.size(); i++)
    {
        //std::cout << "data :" << data[i];
    }

    if (!loadAudio(audioPath.c_str(), header, data))
    {
        std::cerr << "Failed to load wav file at : " << audioPath.c_str();
    }

    size_t audioCursor = 0;
    size_t chunkSize = 1024;

    AudioData audioData; 
    PaStream* stream = playBack(data, header,&audioData);
    if (!stream)
    {
        std::cerr << "failed the playback";
    }

    while (!glfwWindowShouldClose(window)) {
        processInputs(window, Cube, data);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float fileAmplitude = normalizeAudioData(data, audioCursor, chunkSize);
        audioCursor = (audioCursor + chunkSize) % data.size();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, Cube.rotX, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, Cube.rotY, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, Cube.rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
        float scale = 0.5f + fileAmplitude * 2.0f;
        //scale by microphone float scale = 0.5f + gAmplitude * 2.0f;
        model = glm::scale(model, glm::vec3(scale));


        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        glm::mat4 mvp = projection * view * model;

        int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        int colorLoc = glGetUniformLocation(shaderProgram, "uColor");
        glUniform3f(colorLoc, 0.2f, 0.7f, 0.3f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }




    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    if (stream) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
    }
    Pa_Terminate();
    return 0;
}
