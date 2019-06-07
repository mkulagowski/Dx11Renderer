#pragma once

class Scene
{
public:
	Scene();
	~Scene();

private:
	std::vector<std::shared_ptr<SceneObject>> mObjects;
};