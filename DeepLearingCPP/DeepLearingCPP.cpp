#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>

using namespace std;
using namespace sf;

//Single Neuron
class Neuron
{
public:
	Neuron(float x, float y, float r)
	{
		//radius of neuron
		shape.setPosition(x, y);
		shape.setRadius(r);

		//set the fill color to white
		shape.setFillColor(sf::Color::White);

		//set the outline color to black
		shape.setOutlineColor(sf::Color::Black);

		//set the outline thickness to 2px
		shape.setOutlineThickness(2.f);

		//initialize the activation to 0
		activation = 0.f;
	}

	//method to draw the neuron on a window
	void draw(sf::RenderWindow& window)
	{
		//set the fill color based on the activation
		shape.setFillColor(sf::Color(255, 255 * (1 - activation), 255 * (1 - activation)));
		window.draw(shape);

		// Draw text showing neuron's activation
		sf::Font font;
		font.loadFromFile("arial.ttf");  // Provide the path to a font file

		sf::Text text;
		text.setFont(font);
		text.setString(to_string(activation));
		text.setCharacterSize(12);
		text.setFillColor(sf::Color::Black);
		text.setPosition(shape.getPosition().x - 5, shape.getPosition().y - 15);

		window.draw(text);
	}

	//method to set the activation of neuron
	void setActivation(float value)
	{
		//clamp the value between 0 and 1
		activation = max(0.f, min(1.f, value));
	}

	float getActivation()
	{
		return activation;
	}

	//method to get the position of the neuron 
	sf::Vector2f getPosition()
	{
		return shape.getPosition();
	}

	// method to get the radius of the neuron
	float getRadius() const
	{
		return shape.getRadius();
	}

	//method to update the activation of the neuron
	void update()
	{
		//increase the activation by a small amount
		activation += 0.01f;

		//if the activation is greater than 1, reset it to 0
		if (activation > 1.f)
		{
			activation = 0.f;
		}
	}

private:
	//a circle shape to represent the neuron
	sf::CircleShape shape;

	//a float to store the activation of the neuron
	float activation;
};

//a class to represent a connection between two neurons
class Connection
{
public:
	//constructor
	Connection(Neuron* from, Neuron* to, float w)
	{
		//set the pointers to the source end target neurons
		source = from;
		target = to;

		//set the weight of the connectuion
		weight = w;

		//set the color the connection
		//the higher the weight, the more green the connection
		//the lower the weight, the more red the connection
		color = sf::Color(255 * (1 - weight), 255 * weight, 0);
	}

	//a method to draw the connection on a window
	void draw(sf::RenderWindow& window)
	{
		// get the center of the source and target neurons
		sf::Vector2f sourceCenter = source->getPosition() + sf::Vector2f(source->getRadius(), source->getRadius());
		sf::Vector2f targetCenter = target->getPosition() + sf::Vector2f(target->getRadius(), target->getRadius());

		// create a line shape to represent the connection
		sf::Vertex line[] =
		{
			Vertex(sourceCenter, color),
			Vertex(targetCenter, color)
		};

		// draw the line on the window
		window.draw(line, 2, Lines);

		// Draw text showing connection's weight
		sf::Font font;
		font.loadFromFile("arial.ttf");  // Provide the path to a font file

		sf::Text text;
		text.setFont(font);
		text.setString(to_string(weight));
		text.setCharacterSize(12);
		text.setFillColor(sf::Color::Black);
		text.setPosition((sourceCenter.x + targetCenter.x) / 2, (sourceCenter.y + targetCenter.y) / 2);

		window.draw(text);
	}

	//a method to get the source neuron
	Neuron* getSource()
	{
		return source;
	}

	//a method to get the target neuron
	Neuron* getTarget()
	{
		return target;
	}

	//a method to get the weight of the connection
	float getWeight()
	{
		return weight;
	}

private:
	//pointers to the source and target neurons
	Neuron* source;
	Neuron* target;

	//a float to store the weight of the connection
	float weight;

	//a color to represent the weight of the connection
	sf::Color color;
};

//a class to represent a neural network
class NeuralNetwork
{
public:
	//constructor
	NeuralNetwork(int inputSize, int hiddenSize, int outputSize)
	{
		//set the sizes of the layers
		inputLayerSize = inputSize;
		hiddenLayerSize = hiddenSize;
		outputLayerSize = outputSize;

		//create the neurons for each layer
		createNeurons();

		//create the connections for each layer
		createConnections();
	}

	//a method to create the neurons for each layer 
	void createNeurons()
	{
		//the distance between nerons
		float gap = 50.f;

		//the radius of the neurons
		float radius = 20.f;

		//the x-coordinate of the input layer
		float inputX = 100.f;

		//the x-coordinate of the hidden layer
		float hiddenX = 300.f;

		//the x-coordinate of the output layer
		float outputX = 500.f;

		//the y-coordinate of the first neuron in each layer
		float inputY = 200.f - (inputLayerSize - 1) * gap / 2;
		float hiddenY = 200.f - (hiddenLayerSize - 1) * gap / 2;
		float outputY = 200.f - (outputLayerSize - 1) * gap / 2;
		
		//create the neurons for the input layer
		for (int i = 0; i < inputLayerSize; i++)
		{
			//create a new neuron with the given position and radius
			Neuron* neuron = new Neuron(inputX, inputY + i * gap, radius);

			//add the neuron to the input layer vector
			inputLayer.push_back(neuron);
		}

		//create the neurons for the hidden layer
		for (int i = 0; i < hiddenLayerSize; i++)
		{
			//create a new neuron with the given position and radius
			Neuron* neuron = new Neuron(hiddenX, hiddenY + i * gap, radius);

			//add the neuron to the input layer vector
			hiddenLayer.push_back(neuron);
		}

		//create the neurons for the output layer
		for (int i = 0; i < outputLayerSize; i++)
		{
			//create a new neuron with the given position and radius
			Neuron* neuron = new Neuron(outputX, outputY + i * gap, radius);

			//add the neuron to the output layer vector
			outputLayer.push_back(neuron);
		}
	}

	//a method to create the connection for each layer
	void createConnections()
	{
		//create a random number generator
		random_device rd;
		mt19937 gen(rd());

		uniform_real_distribution<> dis(-1, 1);

		//create the connections for the input to hidden layer
		for (int i = 0; i < inputLayerSize; i++)
		{
			for (int j = 0; j < hiddenLayerSize; j++)
			{
				//create a new connection with a random weight
				Connection* connection = new Connection(inputLayer[i], hiddenLayer[j], dis(gen));

				//add the connection to the input to hidden vetor
				inputToHidden.push_back(connection);
			}
		}

		//create the connections for the hidden layer to ouput layer
		for (int i = 0; i < hiddenLayerSize; i++)
		{
			for (int j = 0; j < outputLayerSize; j++)
			{
				//create a new connection with a random weight
				Connection* connection = new Connection(hiddenLayer[i], outputLayer[j], dis(gen));

				//add the connection to the hidden to output vector
				hiddenToOutput.push_back(connection);
			}
		}
	}

	//a method to draw the neural network on a window
	void draw(RenderWindow& window)
	{
		//draw the connections for the input to hidden layer
		for (int i = 0; i < inputToHidden.size(); i++)
		{
			inputToHidden[i]->draw(window);
		}

		//draw the connections for the hidden to output layer
		for (int i = 0; i < hiddenToOutput.size(); i++)
		{
			hiddenToOutput[i]->draw(window);
		}

		//draw the neurons for the input layer
		for (int i = 0; i < inputLayer.size(); i++)
		{
			inputLayer[i]->draw(window);
		}

		//draw the neurons for the hidden layer
		for (int i = 0; i < hiddenLayer.size(); i++)
		{
			hiddenLayer[i]->draw(window);
		}

		//draw the neurons for the output layer
		for (int i = 0; i < outputLayer.size(); i++)
		{
			outputLayer[i]->draw(window);
		}
	}

	// Method to update the neural network based on user input
	void update(float input1, float input2)
	{
		// Feed forward the inputs through the network
		vector<float> output = feedForward({ input1, input2 });

		// Update the window with the new state of the network
		RenderWindow window(VideoMode(800, 600), "Neural Network Virtualization");
		window.clear();

		// Update activations for all neurons
		for (auto& neuron : inputLayer)
		{
			neuron->update();
			neuron->draw(window);
		}
		for (auto& neuron : hiddenLayer)
		{
			neuron->update();
			neuron->draw(window);
		}
		for (auto& neuron : outputLayer)
		{
			neuron->update();
			neuron->draw(window);
		}

		// Update connections and draw them if needed
		for (auto& connection : inputToHidden)
		{
			connection->draw(window);
		}
		for (auto& connection : hiddenToOutput)
		{
			connection->draw(window);
		}

		// Draw a line representing the output of the network
		sf::Vertex line[] =
		{
			Vertex(Vector2f(600, 300), sf::Color(255 * (1 - output[0]), 255 * output[0], 0)),
			Vertex(Vector2f(700, 300), sf::Color(255 * (1 - output[0]), 255 * output[0], 0))
		};

		window.draw(line, 2, Lines);

		// Display the window
		window.display();
	}

	//a method to feed forward and input vector through the network
	vector<float> feedForward(vector<float> input)
	{
		//check if the input size matches the input layer size
		if (input.size() != inputLayerSize)
		{
			//return an empty vector
			return {};
		}

		//set the activations of the input layer
		for (int i = 0; i < inputLayerSize; i++)
		{
			inputLayer[i]->setActivation(input[i]);
		}

		//compute the activations of the hidden layer
		for (int i = 0; i < hiddenLayerSize; i++)
		{
			//initialize the sum to zero
			float sum = 0.f;
			
			//loop through the connections from the input layer
			for (int j = 0; j < inputLayerSize; j++)
			{
				//get the connection index
				int index = i * inputLayerSize + j;

				//get the source neuron, target neuron and weight
				Neuron* source = inputToHidden[index]->getSource();
				Neuron* target = inputToHidden[index]->getTarget();
				float weight = inputToHidden[index]->getWeight();

				//add the product of source activation and weight to the sum
				sum += source->getActivation() * weight;
			}

			//set the activation of the hidden layer neuron
			hiddenLayer[i]->setActivation(sigmoid(sum));
		}

		//compute the activations of the output layer
		for (int i = 0; i < outputLayerSize; i++)
		{
			//initialize the sum to zero
			float sum = 0.f;

			//loop through the connections from the hidden layer
			for (int j = 0; j < hiddenLayerSize; j++)
			{
				//get the connection index
				int index = i * hiddenLayerSize + j;

				//get the source neuron, target neuron and weight
				Neuron* source = hiddenToOutput[index]->getSource();
				Neuron* target = hiddenToOutput[index]->getTarget();
				float weight = hiddenToOutput[index]->getWeight();

				//add the product of source activation and weight to the sum
				sum += source->getActivation() * weight;
			}

			//set the activation of the output layer neuron
			outputLayer[i]->setActivation(sigmoid(sum));
		}

		//collect the output activations
		vector<float> outputActivations;
		for (int i = 0; i < outputLayerSize; i++)
		{
			outputActivations.push_back(outputLayer[i]->getActivation());
		}

		return outputActivations;
	}

private:
	//sizes of the layers
	int inputLayerSize;
	int hiddenLayerSize;
	int outputLayerSize;

	//vectors to store neurons for each layer
	vector<Neuron*> inputLayer;
	vector<Neuron*> hiddenLayer;
	vector<Neuron*> outputLayer;

	//vectors to store connections between layers
	vector<Connection*> inputToHidden;
	vector<Connection*> hiddenToOutput;

	//sigmoid activation function
	float sigmoid(float x)
	{
		return 1 / (1 + exp(-x));
	}
};

int main()
{
	RenderWindow window(VideoMode(800, 600), "Neural Network Visualization");
	NeuralNetwork neuralNetwork(2, 3, 1);

	// Dodaj wektor danych wejściowych i oczekiwanych wyników
	vector<vector<float>> inputData = { {0.2, 0.8}, {0.5, 0.3}, {0.7, 0.9} };
	vector<float> expectedOutput = { 0.5, 0.8, 0.2 };

	// Indeks, który będzie używany do iteracji po danych wejściowych
	int dataIndex = 0;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}

			// Move to the next set of input data
			dataIndex = (dataIndex + 1) % inputData.size();

			// Update the random weights for the connections in each iteration
			neuralNetwork.createConnections();
		}

		// Update the neural network with the current input data
		neuralNetwork.update(inputData[dataIndex][0], inputData[dataIndex][1]);

		// Draw the updated neural network
		window.clear();
		neuralNetwork.draw(window);
		window.display();

		// Move to the next set of input data
		dataIndex = (dataIndex + 1) % inputData.size();
	}

	return 0;
}