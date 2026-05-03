#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

#include "shader.h"
#include "camera.h"

const unsigned int SCR_WIDTH = 1700;
const unsigned int SCR_HEIGHT = 1000;

Camera camera(glm::vec3(0.0f, 5.0f, 8.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// cube data
float vertices[] = {
    -0.5,-0.5,-0.5,  0.5,-0.5,-0.5,  0.5, 0.5,-0.5, -0.5, 0.5,-0.5,
    -0.5,-0.5, 0.5,  0.5,-0.5, 0.5,  0.5, 0.5, 0.5, -0.5, 0.5, 0.5
};

unsigned int indices[] = {
    0,1,2,2,3,0,
    4,5,6,6,7,4,
    0,4,7,7,3,0,
    1,5,6,6,2,1,
    3,2,6,6,7,3,
    0,1,5,5,4,0
};

unsigned int VAO, VBO, EBO;

void setupCube() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void drawCube(Shader& shader, glm::mat4 model, glm::vec3 color) {
    shader.setMat4("model", model);
    shader.setVec3("color", color);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// Room (walls, floor, ceiling)
void drawRoom(Shader& shader) {
    // Floor - warm wooden color
    glm::mat4 floor = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.1, 0));
    floor = glm::scale(floor, glm::vec3(12, 0.1, 12));
    drawCube(shader, floor, glm::vec3(0.35f, 0.25f, 0.18f));

    // Back Wall - warm cream
    glm::mat4 backWall = glm::translate(glm::mat4(1.0f), glm::vec3(0, 2.5, -6));
    backWall = glm::scale(backWall, glm::vec3(12, 5, 0.2));
    drawCube(shader, backWall, glm::vec3(0.88f, 0.82f, 0.76f));

    // Left Wall
    glm::mat4 leftWall = glm::translate(glm::mat4(1.0f), glm::vec3(-6, 2.5, 0));
    leftWall = glm::scale(leftWall, glm::vec3(0.2, 5, 12));
    drawCube(shader, leftWall, glm::vec3(0.85f, 0.78f, 0.72f));

    // Right Wall
    glm::mat4 rightWall = glm::translate(glm::mat4(1.0f), glm::vec3(6, 2.5, 0));
    rightWall = glm::scale(rightWall, glm::vec3(0.2, 5, 12));
    drawCube(shader, rightWall, glm::vec3(0.85f, 0.78f, 0.72f));

    // Ceiling - white
    glm::mat4 ceiling = glm::translate(glm::mat4(1.0f), glm::vec3(0, 5, 0));
    ceiling = glm::scale(ceiling, glm::vec3(12, 0.1, 12));
    drawCube(shader, ceiling, glm::vec3(0.95f, 0.95f, 0.95f));
}

// Table with border (different color)
void drawTable(Shader& shader) {
    // Center table
    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1.5f));

    // Table top - main surface (light oak)
    glm::mat4 top = glm::translate(m, glm::vec3(0, 1.5, 0));
    top = glm::scale(top, glm::vec3(3.2, 0.12, 2.2));
    drawCube(shader, top, glm::vec3(0.55f, 0.45f, 0.35f)); // Light oak color

    // Table border/edge - contrasting dark color (mahogany)
    // Front border
    glm::mat4 frontBorder = glm::translate(m, glm::vec3(0, 1.56, 1.1));
    frontBorder = glm::scale(frontBorder, glm::vec3(3.2, 0.06, 0.08));
    drawCube(shader, frontBorder, glm::vec3(0.35f, 0.2f, 0.12f)); // Dark mahogany

    // Back border
    glm::mat4 backBorder = glm::translate(m, glm::vec3(0, 1.56, -1.1));
    backBorder = glm::scale(backBorder, glm::vec3(3.2, 0.06, 0.08));
    drawCube(shader, backBorder, glm::vec3(0.35f, 0.2f, 0.12f));

    // Left border
    glm::mat4 leftBorder = glm::translate(m, glm::vec3(-1.6, 1.56, 0));
    leftBorder = glm::scale(leftBorder, glm::vec3(0.08, 0.06, 2.2));
    drawCube(shader, leftBorder, glm::vec3(0.35f, 0.2f, 0.12f));

    // Right border
    glm::mat4 rightBorder = glm::translate(m, glm::vec3(1.6, 1.56, 0));
    rightBorder = glm::scale(rightBorder, glm::vec3(0.08, 0.06, 2.2));
    drawCube(shader, rightBorder, glm::vec3(0.35f, 0.2f, 0.12f));

    // Corner accents
    std::vector<glm::vec3> corners = {
        {-1.6, 1.56, 1.1}, {1.6, 1.56, 1.1},
        {-1.6, 1.56, -1.1}, {1.6, 1.56, -1.1}
    };

    for (auto p : corners) {
        glm::mat4 corner = glm::translate(m, p);
        corner = glm::scale(corner, glm::vec3(0.12, 0.08, 0.12));
        drawCube(shader, corner, glm::vec3(0.45f, 0.28f, 0.18f));
    }

    // Table legs - matching dark mahogany
    std::vector<glm::vec3> legs = {
        {1.4, 0.7, 0.9}, {-1.4, 0.7, 0.9},
        {1.4, 0.7, -1.0}, {-1.4, 0.7, -1.0}
    };

    for (auto p : legs) {
        glm::mat4 leg = glm::translate(m, p);
        leg = glm::scale(leg, glm::vec3(0.14, 1.4, 0.14));
        drawCube(shader, leg, glm::vec3(0.25f, 0.15f, 0.08f));
    }
}

// Boss Chair - Behind the table, facing forward (away from table, towards camera/room)
void drawBossChair(Shader& shader) {
    // Position behind the table (negative Z side)
    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.2f));

    // Boss chair faces forward (towards the room, away from table)
    // No rotation needed as default forward is positive Z

    // Larger seat cushion - executive leather (dark brown)
    glm::mat4 seat = glm::translate(m, glm::vec3(0, 0.55, 0));
    seat = glm::scale(seat, glm::vec3(1.0, 0.14, 1.0));
    drawCube(shader, seat, glm::vec3(0.35f, 0.22f, 0.18f));

    // Chair legs - polished dark wood
    std::vector<glm::vec3> legs = {
        {0.45, 0.1, 0.45}, {-0.45, 0.1, 0.45},
        {0.45, 0.1, -0.45}, {-0.45, 0.1, -0.45}
    };

    for (auto p : legs) {
        glm::mat4 leg = glm::translate(m, p);
        leg = glm::scale(leg, glm::vec3(0.09, 0.55, 0.09));
        drawCube(shader, leg, glm::vec3(0.2f, 0.12f, 0.08f));
    }

    // Tall backrest - executive style
    glm::mat4 back = glm::translate(m, glm::vec3(0, 1.05, -0.52));
    back = glm::scale(back, glm::vec3(1.0, 1.1, 0.1));
    drawCube(shader, back, glm::vec3(0.35f, 0.22f, 0.18f));

    // Armrests - wider and more comfortable
    glm::mat4 armLeft = glm::translate(m, glm::vec3(-0.55, 0.85, 0));
    armLeft = glm::scale(armLeft, glm::vec3(0.12, 0.1, 0.75));
    drawCube(shader, armLeft, glm::vec3(0.3f, 0.18f, 0.14f));

    glm::mat4 armRight = glm::translate(m, glm::vec3(0.55, 0.85, 0));
    armRight = glm::scale(armRight, glm::vec3(0.12, 0.1, 0.75));
    drawCube(shader, armRight, glm::vec3(0.3f, 0.18f, 0.14f));

    // Headrest
    glm::mat4 headrest = glm::translate(m, glm::vec3(0, 1.62, -0.55));
    headrest = glm::scale(headrest, glm::vec3(0.7, 0.2, 0.08));
    drawCube(shader, headrest, glm::vec3(0.35f, 0.22f, 0.18f));
}

// Client Chair - On opposite side of boss chair, facing the table
void drawClientChair(Shader& shader, float xOffset, bool isLeft) {
    // Position on the opposite side of the table (positive Z side) - facing the table
    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(xOffset, 0, 2.2f));

    // Rotate to face the table (towards negative Z)
    m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0, 1, 0));

    // Seat cushion - professional gray/blue
    glm::vec3 seatColor = isLeft ? glm::vec3(0.4f, 0.45f, 0.55f) : glm::vec3(0.5f, 0.55f, 0.45f);
    glm::mat4 seat = glm::translate(m, glm::vec3(0, 0.55, 0));
    seat = glm::scale(seat, glm::vec3(0.85, 0.12, 0.85));
    drawCube(shader, seat, seatColor);

    // Chair legs
    std::vector<glm::vec3> legs = {
        {0.4, 0.1, 0.4}, {-0.4, 0.1, 0.4},
        {0.4, 0.1, -0.4}, {-0.4, 0.1, -0.4}
    };

    for (auto p : legs) {
        glm::mat4 leg = glm::translate(m, p);
        leg = glm::scale(leg, glm::vec3(0.08, 0.55, 0.08));
        drawCube(shader, leg, glm::vec3(0.25f, 0.15f, 0.08f));
    }

    // Backrest
    glm::mat4 back = glm::translate(m, glm::vec3(0, 1.0, -0.48));
    back = glm::scale(back, glm::vec3(0.85, 0.85, 0.08));
    drawCube(shader, back, seatColor);

    // Armrests
    glm::mat4 armLeft = glm::translate(m, glm::vec3(-0.48, 0.8, 0));
    armLeft = glm::scale(armLeft, glm::vec3(0.1, 0.08, 0.65));
    drawCube(shader, armLeft, glm::vec3(0.3f, 0.2f, 0.15f));

    glm::mat4 armRight = glm::translate(m, glm::vec3(0.48, 0.8, 0));
    armRight = glm::scale(armRight, glm::vec3(0.1, 0.08, 0.65));
    drawCube(shader, armRight, glm::vec3(0.3f, 0.2f, 0.15f));
}

// Original Chair - Now kept as regular office chair (facing table)
void drawOfficeChair(Shader& shader) {
    // Position chair directly opposite the table
    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1.5f));

    // Rotate chair 180 degrees to face the table (towards negative Z)
    m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0, 1, 0));

    // Seat cushion - warm leather color
    glm::mat4 seat = glm::translate(m, glm::vec3(0, 0.55, 0));
    seat = glm::scale(seat, glm::vec3(0.9, 0.12, 0.9));
    drawCube(shader, seat, glm::vec3(0.55f, 0.32f, 0.22f));

    // Chair legs - dark wood
    std::vector<glm::vec3> legs = {
        {0.4, 0.1, 0.4}, {-0.4, 0.1, 0.4},
        {0.4, 0.1, -0.4}, {-0.4, 0.1, -0.4}
    };

    for (auto p : legs) {
        glm::mat4 leg = glm::translate(m, p);
        leg = glm::scale(leg, glm::vec3(0.08, 0.55, 0.08));
        drawCube(shader, leg, glm::vec3(0.25f, 0.15f, 0.08f));
    }

    // Backrest
    glm::mat4 back = glm::translate(m, glm::vec3(0, 1.05, -0.48));
    back = glm::scale(back, glm::vec3(0.9, 0.9, 0.08));
    drawCube(shader, back, glm::vec3(0.55f, 0.32f, 0.22f));

    // Armrests
    glm::mat4 armLeft = glm::translate(m, glm::vec3(-0.5, 0.85, 0));
    armLeft = glm::scale(armLeft, glm::vec3(0.1, 0.08, 0.7));
    drawCube(shader, armLeft, glm::vec3(0.45f, 0.28f, 0.18f));

    glm::mat4 armRight = glm::translate(m, glm::vec3(0.5, 0.85, 0));
    armRight = glm::scale(armRight, glm::vec3(0.1, 0.08, 0.7));
    drawCube(shader, armRight, glm::vec3(0.45f, 0.28f, 0.18f));
}

// Desktop PC on table
void drawDesktopPC(Shader& shader) {
    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1.5f));

    // Monitor stand
    glm::mat4 stand = glm::translate(m, glm::vec3(-0.7, 1.56, 0.6));
    stand = glm::scale(stand, glm::vec3(0.35, 0.05, 0.28));
    drawCube(shader, stand, glm::vec3(0.15f));

    // Monitor screen
    glm::mat4 monitor = glm::translate(m, glm::vec3(-0.7, 1.82, 0.6));
    monitor = glm::scale(monitor, glm::vec3(0.75, 0.55, 0.04));
    drawCube(shader, monitor, glm::vec3(0.12f, 0.12f, 0.14f));

    // Monitor display
    glm::mat4 screen = glm::translate(m, glm::vec3(-0.7, 1.82, 0.63));
    screen = glm::scale(screen, glm::vec3(0.68, 0.48, 0.02));
    drawCube(shader, screen, glm::vec3(0.25f, 0.45f, 0.85f));

    // CPU Tower
    glm::mat4 cpu = glm::translate(m, glm::vec3(1.1, 1.15, 0.75));
    cpu = glm::scale(cpu, glm::vec3(0.38, 0.75, 0.45));
    drawCube(shader, cpu, glm::vec3(0.12f));

    // CPU LED strip
    glm::mat4 cpuLed = glm::translate(m, glm::vec3(1.12, 1.0, 0.98));
    cpuLed = glm::scale(cpuLed, glm::vec3(0.36, 0.55, 0.02));
    drawCube(shader, cpuLed, glm::vec3(0.9f, 0.2f, 0.2f));

    // Keyboard
    glm::mat4 keyboard = glm::translate(m, glm::vec3(0.1, 1.57, 0.95));
    keyboard = glm::scale(keyboard, glm::vec3(0.65, 0.04, 0.24));
    drawCube(shader, keyboard, glm::vec3(0.08f));

    // Mouse
    glm::mat4 mouse = glm::translate(m, glm::vec3(0.75, 1.57, 1.05));
    mouse = glm::scale(mouse, glm::vec3(0.12, 0.04, 0.18));
    drawCube(shader, mouse, glm::vec3(0.08f));
}

// Books on table
void drawBooks(Shader& shader) {
    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1.5f));

    // Book stack 1
    std::vector<glm::vec3> bookColors = {
        glm::vec3(0.75f, 0.25f, 0.25f),
        glm::vec3(0.25f, 0.35f, 0.75f),
        glm::vec3(0.25f, 0.75f, 0.35f)
    };

    for (int i = 0; i < 3; i++) {
        glm::mat4 book = glm::translate(m, glm::vec3(0.3f, 1.57f + (i * 0.11f), -0.75f));
        book = glm::scale(book, glm::vec3(0.45f, 0.09f, 0.65f));
        drawCube(shader, book, bookColors[i]);
    }

    // Leaning book
    glm::mat4 bookLeaning = glm::translate(m, glm::vec3(-0.4f, 1.62f, -0.85f));
    bookLeaning = glm::rotate(bookLeaning, glm::radians(-15.0f), glm::vec3(0, 0, 1));
    bookLeaning = glm::scale(bookLeaning, glm::vec3(0.4f, 0.1f, 0.6f));
    drawCube(shader, bookLeaning, glm::vec3(0.7f, 0.55f, 0.3f));

    // Notepad
    glm::mat4 notepad = glm::translate(m, glm::vec3(-0.9f, 1.58f, -0.5f));
    notepad = glm::scale(notepad, glm::vec3(0.55f, 0.04f, 0.45f));
    drawCube(shader, notepad, glm::vec3(0.95f, 0.92f, 0.85f));
}

// Bookshelf
void drawBookshelf(Shader& shader) {
    // Main bookshelf frame
    glm::mat4 shelfFrame = glm::translate(glm::mat4(1.0f), glm::vec3(-4.2, 1.6, -3.8));
    shelfFrame = glm::scale(shelfFrame, glm::vec3(1.6, 3.2, 0.7));
    drawCube(shader, shelfFrame, glm::vec3(0.55f, 0.4f, 0.25f));

    // Shelves (horizontal dividers)
    for (int i = 0; i < 4; i++) {
        glm::mat4 shelf = glm::translate(glm::mat4(1.0f), glm::vec3(-4.2, 0.45 + (i * 1.05), -3.8));
        shelf = glm::scale(shelf, glm::vec3(1.6, 0.06, 0.7));
        drawCube(shader, shelf, glm::vec3(0.45f, 0.32f, 0.18f));
    }

    // Books on shelves
    std::vector<glm::vec3> bookPositions = {
        {-4.8, 1.0, -3.5}, {-4.5, 1.0, -3.5}, {-4.2, 1.0, -3.5}, {-3.9, 1.0, -3.5},
        {-4.7, 2.05, -3.5}, {-4.4, 2.05, -3.5}, {-4.1, 2.05, -3.5},
        {-4.9, 3.1, -3.5}, {-4.6, 3.1, -3.5}, {-4.3, 3.1, -3.5}, {-4.0, 3.1, -3.5}
    };

    std::vector<glm::vec3> bookColors = {
        glm::vec3(0.85f, 0.3f, 0.3f), glm::vec3(0.3f, 0.85f, 0.3f),
        glm::vec3(0.3f, 0.3f, 0.85f), glm::vec3(0.85f, 0.85f, 0.3f),
        glm::vec3(0.7f, 0.4f, 0.8f), glm::vec3(0.4f, 0.8f, 0.7f),
        glm::vec3(0.8f, 0.5f, 0.3f),
        glm::vec3(0.5f, 0.3f, 0.2f), glm::vec3(0.2f, 0.5f, 0.3f),
        glm::vec3(0.3f, 0.2f, 0.5f), glm::vec3(0.6f, 0.3f, 0.4f)
    };

    for (int i = 0; i < bookPositions.size(); i++) {
        glm::mat4 book = glm::translate(glm::mat4(1.0f), bookPositions[i]);
        book = glm::scale(book, glm::vec3(0.22, 0.4, 0.18));
        drawCube(shader, book, bookColors[i % bookColors.size()]);
    }

    // Decorative vase
    glm::mat4 vase = glm::translate(glm::mat4(1.0f), glm::vec3(-3.8, 3.3, -3.7));
    vase = glm::scale(vase, glm::vec3(0.25, 0.4, 0.25));
    drawCube(shader, vase, glm::vec3(0.7f, 0.5f, 0.3f));
}

// Window
void drawWindow(Shader& shader) {
    // Window frame
    glm::mat4 windowFrame = glm::translate(glm::mat4(1.0f), glm::vec3(5.95, 2.8, -1.8));
    windowFrame = glm::scale(windowFrame, glm::vec3(0.12, 2.8, 2.2));
    drawCube(shader, windowFrame, glm::vec3(0.85f, 0.75f, 0.65f));

    // Window glass
    glm::mat4 windowGlass = glm::translate(glm::mat4(1.0f), glm::vec3(6.02, 2.8, -1.8));
    windowGlass = glm::scale(windowGlass, glm::vec3(0.05, 2.6, 2.0));
    drawCube(shader, windowGlass, glm::vec3(0.5f, 0.7f, 0.95f));

    // Window cross bars
    glm::mat4 crossBar1 = glm::translate(glm::mat4(1.0f), glm::vec3(5.95, 1.8, -1.8));
    crossBar1 = glm::scale(crossBar1, glm::vec3(0.14, 0.1, 2.1));
    drawCube(shader, crossBar1, glm::vec3(0.65f, 0.55f, 0.45f));

    glm::mat4 crossBar2 = glm::translate(glm::mat4(1.0f), glm::vec3(5.95, 3.6, -1.8));
    crossBar2 = glm::scale(crossBar2, glm::vec3(0.14, 0.1, 2.1));
    drawCube(shader, crossBar2, glm::vec3(0.65f, 0.55f, 0.45f));

    glm::mat4 crossBar3 = glm::translate(glm::mat4(1.0f), glm::vec3(5.95, 2.8, -0.9));
    crossBar3 = glm::scale(crossBar3, glm::vec3(0.14, 2.7, 0.12));
    drawCube(shader, crossBar3, glm::vec3(0.65f, 0.55f, 0.45f));

    glm::mat4 crossBar4 = glm::translate(glm::mat4(1.0f), glm::vec3(5.95, 2.8, -2.7));
    crossBar4 = glm::scale(crossBar4, glm::vec3(0.14, 2.7, 0.12));
    drawCube(shader, crossBar4, glm::vec3(0.65f, 0.55f, 0.45f));
}

// Door
void drawDoor(Shader& shader) {
    // Door frame
    glm::mat4 doorFrame = glm::translate(glm::mat4(1.0f), glm::vec3(-5.95, 1.6, 3.5));
    doorFrame = glm::scale(doorFrame, glm::vec3(0.15, 3.2, 1.3));
    drawCube(shader, doorFrame, glm::vec3(0.6f, 0.48f, 0.35f));

    // Door panel
    glm::mat4 door = glm::translate(glm::mat4(1.0f), glm::vec3(-6.02, 1.6, 3.5));
    door = glm::scale(door, glm::vec3(0.08, 2.9, 1.2));
    drawCube(shader, door, glm::vec3(0.45f, 0.35f, 0.22f));

    // Door handle
    glm::mat4 handle = glm::translate(glm::mat4(1.0f), glm::vec3(-5.95, 1.7, 4.05));
    handle = glm::scale(handle, glm::vec3(0.08, 0.08, 0.12));
    drawCube(shader, handle, glm::vec3(0.85f, 0.75f, 0.4f));

    // Door panels
    glm::mat4 panel1 = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0, 1.1, 3.52));
    panel1 = glm::scale(panel1, glm::vec3(0.05, 0.8, 0.8));
    drawCube(shader, panel1, glm::vec3(0.55f, 0.45f, 0.32f));

    glm::mat4 panel2 = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0, 2.2, 3.52));
    panel2 = glm::scale(panel2, glm::vec3(0.05, 0.8, 0.8));
    drawCube(shader, panel2, glm::vec3(0.55f, 0.45f, 0.32f));
}

// Desk lamp
void addDeskLamp(Shader& shader) {
    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1.5f));

    // Lamp base
    glm::mat4 base = glm::translate(m, glm::vec3(1.2, 1.56, -0.7));
    base = glm::scale(base, glm::vec3(0.15, 0.04, 0.15));
    drawCube(shader, base, glm::vec3(0.2f));

    // Lamp pole
    glm::mat4 pole = glm::translate(m, glm::vec3(1.2, 1.8, -0.7));
    pole = glm::scale(pole, glm::vec3(0.04, 0.5, 0.04));
    drawCube(shader, pole, glm::vec3(0.15f));

    // Lamp shade
    glm::mat4 shade = glm::translate(m, glm::vec3(1.2, 2.05, -0.7));
    shade = glm::scale(shade, glm::vec3(0.22, 0.1, 0.22));
    drawCube(shader, shade, glm::vec3(0.9f, 0.85f, 0.7f));
}

void processInput(GLFWwindow* window) {
    float speed = 3.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, speed);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow*, double xpos, double ypos) {
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }

    float sensitivity = 0.08f;
    float xoffset = (xpos - lastX) * sensitivity;
    float yoffset = (lastY - ypos) * sensitivity;

    lastX = xpos; lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Office desk Setup-Mujam & Nourin", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    Shader shader("vertexShader.vs", "fragmentShader.fs");
    setupCube();

    while (!glfwWindowShouldClose(window)) {
        float current = glfwGetTime();
        deltaTime = current - lastFrame;
        lastFrame = current;

        processInput(window);

        glClearColor(0.7f, 0.85f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 view = camera.GetViewMatrix();

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // Draw complete conference room with all chairs
        drawRoom(shader);           // Walls, floor, ceiling
        drawTable(shader);          // Main conference table
        drawOfficeChair(shader);    // Regular office chair (facing table)
        drawBossChair(shader);      // Boss chair behind table (facing forward)
        drawClientChair(shader, -1.2f, true);   // Left client chair (facing table)
        drawClientChair(shader, 1.2f, false);   // Right client chair (facing table)
        drawDesktopPC(shader);      // Computer setup on desk
        drawBooks(shader);          // Books on desk
        addDeskLamp(shader);        // Desk lamp
        drawBookshelf(shader);      // Bookshelf with organized books
        drawWindow(shader);         // Window with cross bars
        drawDoor(shader);           // Door with handle and panels

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}