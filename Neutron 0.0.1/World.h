#pragma once
class World {
public:
	float xPos[1], yPos[1], heights[1];
	World();
	~World();
	void load(const char* filepath); // Get from NBT loader
	void setupTextures(); // Setup textures from tiles and/or ext. images
	void tick();
	void render();
private:
	unsigned int image, backgrounds[1], foregrounds[1];

};

