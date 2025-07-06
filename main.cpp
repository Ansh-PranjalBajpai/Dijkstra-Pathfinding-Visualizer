#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <string>
#include <bits/stdc++.h>

class TextBox
{
private:
    sf::RectangleShape box;
    sf::RectangleShape cursor;
    sf::Text text;
    std::string input;

public:
    bool isSelected;

    TextBox(float x, float y, float width, float height, const sf::Font &font)
        : isSelected(false)
    {
//__________Building the box_________________//        
        box.setPosition(x, y);
        box.setSize(sf::Vector2f(width, height));
        box.setFillColor(sf::Color::White);
//__________Building the text_________________//
        text.setFont(font);
        text.setFillColor(sf::Color::Black);
        text.setCharacterSize(14);
        text.setPosition(x + 5, y + 5);
    }

    void render(sf::RenderWindow &window)
    {
        window.draw(box);
        window.draw(text);
    }

    bool contains(float x, float y)
    {
        return box.getGlobalBounds().contains(x, y);
    }

    void setSelected(bool selected)
    {
        isSelected = selected;
        if(isSelected){
            text.setString("_");
        }
        else text.setString("");
    }
    std::string getInput()
    {
        return input;
    }

    void clear()
    {
        input.clear();
        text.setString(input + "_");
    }
//_________________Taking the text input_______________________//
    void handleInput(sf::Event &event)
    {
        if (event.type == sf::Event::TextEntered)
        {
            if (isSelected)
            {
                //___if delete key is pressed___//
                if (event.text.unicode == 8)
                {
                    if (!input.empty())
                    {
                        input.pop_back();
                    }
                }
        //______Typeable ASCII char._____//
                else if (event.text.unicode >= 32 && event.text.unicode <= 126)
                {
                    input += static_cast<char>(event.text.unicode);
                }
                std::string s = "_"; // Added to get the location of the next to be typed character.
                text.setString(input + s);
            }
        }
    }
};
class Label
{
public:
    sf::Text text;
    Label(int charsize, float x, float y, sf::Font &font)
    {
        text.setFont(font);
        text.setCharacterSize(charsize);
        text.setFillColor(sf::Color::White);
        text.setPosition(x, y);
    }
};
struct Edge
{
    int startNodeIndex;
    int endNodeIndex;
    int weight;
    sf::Text label;
    sf::VertexArray line;
    sf::VertexArray arrowhead;

    Edge(int start, int end, float w, const sf::Font &font, float startX, float startY, float endX, float endY)
        : startNodeIndex(start), endNodeIndex(end), weight(w), line(sf::Lines, 4), arrowhead(sf::Triangles, 3)
    {

        line[0].position = sf::Vector2f(startX, startY);
        line[0].color = sf::Color::Cyan;
        line[1].position = sf::Vector2f(endX, endY);
        line[1].color = sf::Color::Cyan;

        sf::Vector2f direction = line[1].position - line[0].position;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f unitDirection = direction / length;

        float arrowheadLength = 10.0f;
        float arrowheadWidth = 5.0f;

        sf::Vector2f arrowTip = line[1].position - unitDirection * 25.0f;
        sf::Vector2f arrowBase1 = line[1].position - unitDirection * 25.0f - unitDirection * arrowheadLength +
                                  sf::Vector2f(-unitDirection.y, unitDirection.x) * arrowheadWidth;
        sf::Vector2f arrowBase2 = line[1].position - unitDirection * 25.0f - unitDirection * arrowheadLength -
                                  sf::Vector2f(-unitDirection.y, unitDirection.x) * arrowheadWidth;

        // Add the arrowhead vertices
        arrowhead[0].position = arrowTip;
        arrowhead[0].color = sf::Color::White;
        arrowhead[1].position = arrowBase1;
        arrowhead[1].color = sf::Color::White;
        arrowhead[2].position = arrowBase2;
        arrowhead[2].color = sf::Color::White;

        label.setFont(font);
        label.setString(std::to_string(weight));
        label.setCharacterSize(24);
        label.setFillColor(sf::Color::White);
        label.setPosition(endX - (endX - startX) / 3 + 0.3, endY - (endY - startY) / 3 + 0.2);
    }
};
struct Node
{
    sf::CircleShape shape;
    sf::Text label;
    float x, y;
    bool color;
    Node(float x, float y, const sf::Font &font, const std::string &labelText) : x(x), y(y), color(0)
    {
        shape.setRadius(25.0f);
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(x - shape.getRadius(), y - shape.getRadius());

        label.setFont(font);
        label.setString(labelText);
        label.setCharacterSize(24);
        label.setFillColor(sf::Color::Black);
        label.setPosition(x - shape.getRadius() / 2, y - shape.getRadius() / 2);
        sf::FloatRect textBounds = label.getLocalBounds();
        label.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
        label.setPosition(x, y);
    }
    bool contains(float px, float py)
    {
        float dx = px - (x);
        float dy = py - (y);
        return dx * dx + dy * dy <= shape.getRadius() * shape.getRadius();
    }

    void toggleColor(int c)
    {
        // std::cout << "togglecalled\n";
        if (c == 1)
        {
            shape.setFillColor(sf::Color::Green);
            label.setFillColor(sf::Color::Black);
        }
        else if (c == 2)
        {
            shape.setFillColor(sf::Color::Blue);
            label.setFillColor(sf::Color::White);
        }
        else
        {
            shape.setFillColor(sf::Color::Yellow);
            label.setFillColor(sf::Color::Black);
        }
    }
};
class Array
{
private:
    std::vector<sf::RectangleShape> blocks;
    float blockWidth = 50.0f;
    float blockHeight = 50.0f;
    float spacing = 10.0f;
    float x = 50.0f;
    float y = 60.0f;

public:
    std::vector<sf::Text> texts;
    Array(std::vector<int> &arr, const sf::Font &font)
    {
        int si = arr.size();
        for (int i = 0; i < si; i++)
        {
            sf::RectangleShape block(sf::Vector2f(blockWidth, blockHeight));
            block.setFillColor(sf::Color::White);
            block.setOutlineThickness(1.0f);
            block.setOutlineColor(sf::Color::Black);
            block.setPosition(x + i * (blockWidth + spacing), y); // Position the block in a row

            sf::Text text;
            text.setFont(font);
            if (arr[i] == INT_MAX)
                text.setString("-1");
            else
                text.setString(std::to_string(arr[i]));
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::Black);

            sf::FloatRect textBounds = text.getLocalBounds();
            text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            text.setPosition(block.getPosition().x + blockWidth / 2.0f, block.getPosition().y + blockHeight / 2.0f);

            blocks.push_back(block);
            texts.push_back(text);
        }
    }

    void render(sf::RenderWindow &window)
    {
        for (size_t i = 0; i < blocks.size(); i++)
        {
            window.draw(blocks[i]);
            window.draw(texts[i]);
        }
    }
    void toggleColor(int i, int c)
    {
        if (c == 1)
        {
            blocks[i].setFillColor(sf::Color::Red);
            texts[i].setFillColor(sf::Color::White);
        }
        else if (c == 2)
        {
            blocks[i].setFillColor(sf::Color::Green);
            texts[i].setFillColor(sf::Color::Black);
        }
        else
        {
            blocks[i].setFillColor(sf::Color::White);
            texts[i].setFillColor(sf::Color::Black);
        }
    }
    void changeValue(int i, int c)
    {
        texts[i].setString(std::to_string(c));
    }
};
class Graph
{
public:
    std::vector<std::pair<int, int>> adj[100];
    std::vector<Node> *nodes;
    std::vector<int> dist;
    Graph(std::vector<Node> &nodes)
    {
        this->nodes = &nodes;
    }
    void dijkstra(int src, sf::RenderWindow &window, std::function<void()> &fullRender, Array &distArray)
    {
        int n = dist.size();
        src--;
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
        (*nodes)[src].toggleColor(2);
        distArray.changeValue(src, 0);
        distArray.toggleColor(src, 1);
        fullRender();
        sf::sleep(sf::seconds(2));
        (*nodes)[src].toggleColor(0);
        distArray.toggleColor(src, 0);
        pq.push({0, src});
        dist[src] = 0;
        while (!pq.empty())
        {
            std::pair<int, int> x = pq.top();
            pq.pop();
            int s = x.second;
            int dists = x.first;
            if (dists > dist[s])
                continue;
            (*nodes)[s].toggleColor(1);
            distArray.toggleColor(s, 2);
            fullRender();
            sf::sleep(sf::seconds(2));
            for (auto &u : adj[s])
            {
                if (dist[u.first] > dists + u.second)
                {
                    (*nodes)[u.first].toggleColor(2);
                    distArray.toggleColor(u.first, 1);
                    dist[u.first] = dists + u.second;
                    distArray.changeValue(u.first, dist[u.first]);
                    fullRender();
                    pq.push({dist[u.first], u.first});
                    sf::sleep(sf::seconds(2));
                    distArray.toggleColor(u.first, 0);
                    (*nodes)[u.first].toggleColor(0);
                }
            }
        }
    }
};
class Button
{
public:
    sf::RectangleShape box;
    sf::Text text;
    Button(float x, float y, int width, int height, sf::Font &font)
    {
        box.setSize(sf::Vector2f(width, height));
        box.setPosition(x, y);
        box.setFillColor(sf::Color::Green);
        box.setOutlineThickness(2.0f);
        box.setOutlineColor(sf::Color::Black);

        text.setFont(font);
        text.setString("Run!");
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);

        text.setPosition(x + width / 2.0, y + height / 2.0);
    }
};
class Legend
{
private:
    sf::CircleShape shape1;
    Label label1;
    sf::CircleShape shape2;
    Label label2;
    sf::CircleShape shape3;
    Label label3;

public:
    Legend(float x, float y, float gap, sf::Font &font)
        : label1(18, x + 55.0f, y + 15.0f + 0.6f, font), label2(18, x + 55.0f, y + gap + 3 * 15.0f + 0.6f, font), label3(18, x + 55.0f, y + 2 * gap + 5 * 15.0f + 0.6f, font)
    {
        shape1.setRadius(15.0f);
        shape1.setFillColor(sf::Color::Green);
        shape1.setPosition(x + shape1.getRadius(), y + shape1.getRadius());
        label1.text.setString("Visited");

        shape2.setRadius(15.0f);
        shape2.setFillColor(sf::Color::Yellow);
        shape2.setPosition(x + shape1.getRadius(), y + gap + 3 * shape1.getRadius());
        label2.text.setString("Not yet visited");

        shape3.setRadius(15.0f);
        shape3.setFillColor(sf::Color::Blue);
        shape3.setPosition(x + shape1.getRadius(), y + 2 * gap + 5 * shape1.getRadius());
        label3.text.setString("Being pushed to\npriority queue");
    }
    void render(sf::RenderWindow &window)
    {
        window.draw(shape1);
        window.draw(label1.text);
        window.draw(shape2);
        window.draw(label2.text);
        window.draw(shape3);
        window.draw(label3.text);
    }
};

int main()
{
    int window_width = 1440, window_height = 900;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Graph Visualizer");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        return EXIT_FAILURE;
    }

    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<int> selectedNodes;

    int panel_width = 300;
    int panel_start = window_width - panel_width;
    sf::RectangleShape panel(sf::Vector2f(panel_width, window_height));
    panel.setFillColor(sf::Color(50, 50, 50));
    panel.setPosition(panel_start, 0);

    TextBox inputBoxWeight(panel_start + 10, 80, 160, 30, font);
    bool waitingForWeight = false;
    Label weightLabel(18, panel_start + 10, 20, font);

    TextBox inputBoxSrc(panel_start + 10, 170, 160, 30, font);
    bool waitingForSrc = false;
    Label SrcLabel(18, panel_start + 10, 140, font);
    SrcLabel.text.setString("Source");

    Legend legend(panel_start + 10, 360, 20, font);

    Label ArrayLabel(18, 50, 20, font);
    ArrayLabel.text.setString("Distance Array");
    // TextBox inputBoxDest(panel_start + 10, 250, 160, 30, font);
    // bool waitingForDest = false;
    // Label DestLabel(18, panel_start + 10, 220, font);
    // DestLabel.text.setString("Destination");

    Graph graph(nodes);

    Array distArray(graph.dist, font);

    bool run = false;
    int src = 1, dest = 0;

    Button button(panel_start + 10, 300, 160, 30, font);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    float x = event.mouseButton.x;
                    float y = event.mouseButton.y;
                    bool nodeClicked = false;

                    if (x < panel_start)
                    {
                        for (size_t i = 0; i < nodes.size(); ++i)
                        {
                            if (nodes[i].contains(x, y))
                            {
                                nodes[i].toggleColor(1);
                                if (nodes[i].color == 0)
                                {
                                    selectedNodes.push_back(i);
                                }
                                else
                                {
                                    selectedNodes.erase(std::remove(selectedNodes.begin(), selectedNodes.end(), i), selectedNodes.end());
                                }
                                nodeClicked = true;
                                break;
                            }
                        }

                        if (!nodeClicked)
                        {
                            nodes.emplace_back(x, y, font, std::to_string(nodes.size() + 1));
                            graph.dist.push_back(INT_MAX);
                        }
                        // std::cout << selectedNodes.size() << "\n";
                        if (selectedNodes.size() == 2)
                        {
                            // std::cout << "this working\n";
                            waitingForWeight = true;
                            inputBoxWeight.setSelected(true);
                        }
                    }
                    else
                    {
                        if (inputBoxWeight.contains(x, y))
                            inputBoxWeight.setSelected(true);
                        else
                            inputBoxWeight.setSelected(false);
                        if (inputBoxSrc.contains(x, y))
                        {
                            inputBoxSrc.setSelected(true);
                            waitingForSrc = true;
                        }
                        else
                            inputBoxSrc.setSelected(false);
                        // if (inputBoxDest.contains(x, y))
                        //     inputBoxDest.setSelected(true);
                        // else
                        //     inputBoxDest.setSelected(false);
                        if (button.box.getGlobalBounds().contains(x, y))
                        {
                            run = true;
                        }
                    }
                }
            }
            if (inputBoxWeight.isSelected)
                inputBoxWeight.handleInput(event);
            if (inputBoxSrc.isSelected)
                inputBoxSrc.handleInput(event);
            // if (inputBoxDest.isSelected)
            //     inputBoxDest.handleInput(event);
            if (!waitingForWeight)
                weightLabel.text.setString("Select two nodes to create an\nedge.");
            else
            {
                int startNodeIndex = selectedNodes[0];
                int endNodeIndex = selectedNodes[1];

                std::string x = "Enter weight for the edge\nbetween node " + std::to_string(startNodeIndex + 1) +
                                " and node " + std::to_string(endNodeIndex + 1) + ": ";
                weightLabel.text.setString(x);
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
            {
                if (waitingForWeight)
                {
                    // std::cout << "working" << "\n";
                    std::stringstream ss(inputBoxWeight.getInput());
                    int weight;
                    ss >> weight;
                    int startNodeIndex = selectedNodes[0];
                    int endNodeIndex = selectedNodes[1];
                    graph.adj[startNodeIndex].push_back({endNodeIndex, weight});
                    edges.emplace_back(startNodeIndex, endNodeIndex, weight, font,
                                       nodes[startNodeIndex].x, nodes[startNodeIndex].y,
                                       nodes[endNodeIndex].x, nodes[endNodeIndex].y);

                    nodes[startNodeIndex].toggleColor(0);
                    nodes[endNodeIndex].toggleColor(0);
                    selectedNodes.clear();

                    waitingForWeight = false;
                    inputBoxWeight.clear();
                    inputBoxWeight.setSelected(false);
                }
                if (waitingForSrc)
                {
                    std::stringstream ss(inputBoxSrc.getInput());
                    ss >> src;
                    waitingForSrc = false;
                    inputBoxSrc.clear();
                    inputBoxSrc.setSelected(false);
                }
                // if (waitingForDest)
                // {
                //     std::stringstream ss(inputBoxDest.getInput());
                //     ss >> dest;
                // }
            }
        }
        std::function<void()> fullRender = [&]()
        {
            window.clear();
            for (const auto &edge : edges)
            {
                window.draw(edge.line);
                window.draw(edge.label);
                window.draw(edge.arrowhead);
            }
            for (const auto &node : nodes)
            {
                window.draw(node.shape);
                window.draw(node.label);
            }
            window.draw(panel);
            window.draw(weightLabel.text);
            inputBoxWeight.render(window);
            window.draw(SrcLabel.text);
            inputBoxSrc.render(window);
            // window.draw(DestLabel.text);
            // inputBoxDest.render(window);

            window.draw(button.box);
            window.draw(button.text);
            distArray.render(window);
            if (distArray.texts.size())
            {
                window.draw(ArrayLabel.text);
                legend.render(window);
            }
            window.display();
        };

        if (run)
        {
            Array temp(graph.dist, font);
            distArray = temp;
            graph.dijkstra(src, window, fullRender, distArray);
            run = false;
        }
        else
            fullRender();
    }

    return 0;
}
