#include "imgui-SFML.h"
#include "imgui.h"

#include <SFML/Graphics.hpp>

#include <fstream>
#include <iostream>
#include <memory>

#include <cmath>

class Vec2
{
public:
    float x = 0;
    float y = 0;

    Vec2() = default;

    Vec2(float xin, float yin) : x(xin), y(yin)
    {
    }

    void add(const Vec2& v)
    {
        x += v.x;
        y += v.y;
    }

    Vec2 operator+(const Vec2& rhs) const
    {
        return {x + rhs.x, y + rhs.y};
    }

    Vec2 operator-(const Vec2& rhs) const
    {
        return {x - rhs.x, y - rhs.y};
    }

    Vec2 operator*(const float& scale) const
    {
        return {x * scale, y * scale};
        // return Vec2(x * scale, y * scale);
    }

    [[nodiscard]] float length() const
    {
        return std::sqrt(x * x + y * y);
    }
};

int main(int /*argc*/, char* /*argv*/[])
{
    // Create a new window of size w*h pixels
    // Top-left of the window is (0,0) and bottom-right is (w,h)
    const int        wWidth  = 1280;
    const int        wHeight = 720;
    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML works!");
    window.setFramerateLimit(60); // Limit frame rate to 60 fps

    // Initialize ImGui and create a clock used for its internal timing
    bool success = ImGui::SFML::Init(window);
    if (!success)
    {
        // Handle the error, e.g., print a message or exit the program
        std::cerr << "Failed to initialize ImGui-SFML!" << std::endl;
        return -1;
    }

    sf::Clock deltaClock;

    // Scale the ImGui UI and text size by 2
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;

    // The ImGui color {r, g, b} wheel requires floats from 0-1 instead of ints
    // from 0-255
    float c[3] = {0.0f, 1.0f, 1.0f};

    // Let's make a shape that we will draw to the screen
    float circleRadius   = 50;   // Radius to draw the circle
    int   circleSegments = 32;   // Number of segments to draw the circle with
    float circleSpeedX   = 1.0f; // Speed to move the circle horizontally
    float circleSpeedY   = 0.5f; // Speed to move the circle vertically
    bool  drawCircle     = true; // Whether or not to draw the circle
    bool  drawText       = true; // Whether or not to draw the text

    // Create the SFML circle shape based on our parameters
    sf::CircleShape circle(circleRadius,
                           circleSegments); // Create a circle shape with given radius and segments
    circle.setPosition(10.0f, 10.0f);       // Set the top-left position of the circle

    // Let's load a font so we can display some text
    sf::Font myFont;

    if (!myFont.loadFromFile("fonts/Roboto-Regular.ttf"))
    {
        std::cerr << "Could not load font!\n";
        exit(-1);
    }

    // Set up the text object that will be drawn to the screen
    sf::Text text("Sample Text", myFont, 24);

    // Position the top-left corner of the text so that the text aligns on the bottom
    // Text character size is in pixels, so move the text up from the bottom by its height
    text.setPosition(0, wHeight - (float)text.getCharacterSize());

    // Set up a character array to set the text
    char displayString[255] = "Sample Text";

    // Main loop - continues for each frame while the window is open
    while (window.isOpen())
    {
        // Event handling
        sf::Event event{};
        while (window.pollEvent(event))
        {
            // Pass the event to ImGui to be parsed
            ImGui::SFML::ProcessEvent(window, event);

            // This event triggers when the window is closed
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // This event is triggered when a key is pressed
            if (event.type == sf::Event::KeyPressed)
            {
                // Print the key that was pressed to the console
                std::cout << "Key pressed with code = " << event.key.code << std::endl;

                // Example: what happens when X is pressed
                if (event.key.code == sf::Keyboard::X)
                {
                    // Reverse the x direction of the circle on the screen
                    circleSpeedX *= -1.0f;
                }
            }
        }

        // Update ImGui for this frame with the time that the last frame took
        ImGui::SFML::Update(window, deltaClock.restart());

        // Draw the UI
        ImGui::Begin("Window title");
        ImGui::Text("Window text!");
        ImGui::Checkbox("Draw Circle", &drawCircle);
        ImGui::SameLine();
        ImGui::Checkbox("Draw Text", &drawText);
        ImGui::SliderFloat("Radius", &circleRadius, 0.0f, 300.0f);
        ImGui::SliderInt("Sides", &circleSegments, 3, 64);
        ImGui::ColorEdit3("Color Circle", c);
        ImGui::InputText("Text", displayString, 255);
        if (ImGui::Button("Set Text"))
        {
            text.setString(displayString);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Circle"))
        {
            circle.setPosition(0, 0);
        }
        ImGui::End();

        // Set the circle properties, because they may have been updated with the UI
        circle.setPointCount(circleSegments);
        circle.setRadius(circleRadius);

        // ImGui uses 0-1 float for colors, SFML uses 0-255 for colors
        // We must convert from the UI floats to SFML Uint8
        circle.setFillColor(sf::Color(sf::Uint8(c[0] * 255), sf::Uint8(c[1] * 255), sf::Uint8(c[2] * 255)));

        // Basic animation - move each frame if it's still in frame
        circle.setPosition(circle.getPosition().x + circleSpeedX, circle.getPosition().y + circleSpeedY);

        // Basic rendering function calls
        window.clear(); // Clear the window of anything previously drawn
        if (drawCircle)
        {
            window.draw(circle); // Draw the circle if the boolean is true
        }
        if (drawText)
        {
            window.draw(text); // Draw the text if the boolean is true
        }
        ImGui::SFML::Render(window); // Draw the UI last so it's on top
        window.display();            // Call the window display function
    }

    ImGui::SFML::Shutdown();
    return 0;
}
