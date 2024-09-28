#ifndef __COMMON_H__
#define __COMMON_H__

#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define CLASS_PTR(klassName) \
class klassName; \
using klassName ## UPtr = std::unique_ptr<klassName>; \
using klassName ## Ptr = std::shared_ptr<klassName>; \
using klassName ## WPtr = std::weak_ptr<klassName>;

std::optional<std::string> LoadTextFile(const std::string& filename);
glm::vec3 GetAttenuationCoeff(float distance);
std::optional<int> ParseInt(const std::string& str);
std::optional<float> ParseFloat(const std::string& str);

#endif // __COMMON_H__