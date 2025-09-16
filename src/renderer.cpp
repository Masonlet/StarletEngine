#include <glad/glad.h>
#include "StarletGraphics/shaderManager.hpp"
#include "StarletGraphics/textureManager.hpp"
#include "StarletGraphics/meshManager.hpp"
#include "StarletScene/objects/model.hpp"
#include "StarletMath/mat4.hpp"
#include "StarletEngine/renderer.hpp"
#include "StarletParsers/utils/log.hpp"
#include "StarletScene/objects/light.hpp"

bool Renderer::setProgram(unsigned int program) {
	if (program == 0) return error("Renderer", "setProgram", "Program is 0");
	this->program = program;
	glUseProgram(program);

	return cacheUniformLocations() && debugLog("Renderer", "setProgram", "Successfully set shader program and located uniforms", true);
}

bool Renderer::cacheUniformLocations() {
	return cacheTextureUniforms()
		&& cacheCameraUniforms()
		&& cacheModelUniforms()
		&& cacheLightUniforms();
}

bool Renderer::cacheTextureUniforms() {
	glUniform1i(glGetUniformLocation(program, "textSampler2D_00"), 0);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_01"), 1);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_02"), 2);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_03"), 3);

	bool ok = true;
	ok &= getUniformLocation(modelUseTexturesLocation, "bUseTextures");
	ok &= getUniformLocation(modelTexMixRatiosLocation, "texMixRatios");
	ok &= getUniformLocation(skyboxTextureLocation, "skyboxCubeTexture");
	glUniform1i(skyboxTextureLocation, SKYBOX_TU);

	return ok;
}
bool Renderer::cacheCameraUniforms() {
	bool ok = true;
	ok &= getUniformLocation(eyeLocation, "eyePos");
	return ok;
}
bool Renderer::cacheModelUniforms() {
	bool ok = true;
	ok &= getUniformLocation(modelIsSkyboxLocation, "bIsSkybox");
	ok &= getUniformLocation(modelLocation, "mModel");
	ok &= getUniformLocation(modelViewLocation, "mView");
	ok &= getUniformLocation(modelProjectionLocation, "mProj");
	ok &= getUniformLocation(modelInverseTransposeLocation, "mModel_InverseTranspose");
	ok &= getUniformLocation(modelColourModeLocation, "colourMode");
	ok &= getUniformLocation(modelHasVertColourLocation, "hasVertexColour");
	ok &= getUniformLocation(modelColourOverrideLocation, "colourOverride");
	ok &= getUniformLocation(modelSpecularLocation, "vertSpecular");
	ok &= getUniformLocation(modelMinYMaxYLocation, "yMin_yMax");
	ok &= getUniformLocation(modelSeedLocation, "seed");
	ok &= getUniformLocation(modelLightedLocation, "bLighted");
	return ok;
}
bool Renderer::cacheLightUniforms() {
	bool ok = true;
	ok &= getUniformLocation(lightCountLocation, "lightCount");
	ok &= getUniformLocation(ambientLightLocation, "ambientLight");
	glUniform4f(ambientLightLocation, 1.0, 1.0, 1.0, 1.0);

	return ok;
}

bool Renderer::getUniformLocation(int& location, const char* name) const {
	location = glGetUniformLocation(program, name);
	if (location < 0) return error("Renderer", "getUniformLocation", std::string("Could not find uniform: ") + name);
	return true;
}

void Renderer::cacheLightUniformLocations(int maxLights) {
	if (maxLights <= 0) {
		lightUL.clear();
		return;
	}
	lightUL.resize(static_cast<size_t>(maxLights));

	for (int i = 0; i < maxLights; ++i) {
		std::string base = "theLights[" + std::to_string(i) + "].";
		lightUL[i].position = glGetUniformLocation(program, (base + "position").c_str());
		lightUL[i].diffuse = glGetUniformLocation(program, (base + "diffuse").c_str());
		lightUL[i].attenuation = glGetUniformLocation(program, (base + "attenuation").c_str());
		lightUL[i].direction = glGetUniformLocation(program, (base + "direction").c_str());
		lightUL[i].param1 = glGetUniformLocation(program, (base + "param1").c_str());
		lightUL[i].param2 = glGetUniformLocation(program, (base + "param2").c_str());
	}
}

void Renderer::updateLightCount(int count) const {
	if (lightCountLocation != -1) glUniform1i(lightCountLocation, count);
}

void Renderer::updateLightUniforms(const std::map<std::string, Light>& lights) const {
	const int n = static_cast<int>(lights.size());
	const int maxCached = static_cast<int>(lightUL.size());

	updateLightCount(n);

	std::map<std::string, Light>::const_iterator it = lights.begin();
	for (int i = 0; i < maxCached; ++i) {
		if (it != lights.end()) {
			const Light& L = it->second;
			if (lightUL[i].position != -1)    glUniform4f(lightUL[i].position, L.pos.x, L.pos.y, L.pos.z, 1.0f);
			if (lightUL[i].diffuse != -1)     glUniform4f(lightUL[i].diffuse, L.diffuse.r, L.diffuse.g, L.diffuse.b, L.diffuse.a);
			if (lightUL[i].attenuation != -1) glUniform4f(lightUL[i].attenuation, L.attenuation.r, L.attenuation.g, L.attenuation.b, L.attenuation.a);
			if (lightUL[i].direction != -1)   glUniform4f(lightUL[i].direction, L.direction.r, L.direction.g, L.direction.b, 1.0f);
			if (lightUL[i].param1 != -1)			glUniform4f(lightUL[i].param1, static_cast<float>(L.type), L.param1.x, L.param1.y, 0.0f);
			if (lightUL[i].param2 != -1)			glUniform4f(lightUL[i].param2, L.enabled ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f);
		}
		else if (lightUL[i].param2 != -1) glUniform4f(lightUL[i].param2, 0.0f, 0.0f, 0.0f, 0.0f);
	}
}
static Vec3 seedFromName(const std::string& s) {
	float r = 0.0f, g = 0.0f, b = 0.0f;
	int i = 0;
	for (unsigned char c : s) {
		if (i % 3 == 0) r += static_cast<float>(c);
		else if (i % 3 == 1) g += static_cast<float>(c);
		else                 b += static_cast<float>(c);
		++i;
	}
	r = fmod(r / 255.0f, 1.0f);
	g = fmod(g / 255.0f, 1.0f);
	b = fmod(b / 255.0f, 1.0f);
	return { r, g, b };
}
void Renderer::updateModelUniforms(const Model& instance, const Mesh& data) const {
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, Mat4::modelMatrix(instance.transform).models);
	glUniformMatrix4fv(modelInverseTransposeLocation, 1, GL_FALSE, Mat4::modelMatrix(instance.transform).inverse().transpose().models);
	glUniform4fv(modelSpecularLocation, 1, &instance.specular.x);

	glUniform1i(modelColourModeLocation, static_cast<int>(instance.colourMode));
	glUniform4fv(modelColourOverrideLocation, 1, &instance.colour.x);
	glUniform1i(modelHasVertColourLocation, data.hasColours ? 1 : 0);
	glUniform2f(modelMinYMaxYLocation, data.minY, data.maxY);

	const Vec3 randSeed = seedFromName(instance.name);
	glUniform3f(modelSeedLocation, randSeed.x, randSeed.y, randSeed.z);

	glUniform1i(modelUseTexturesLocation, instance.useTextures ? 1 : 0);
}
void Renderer::updateCameraUniforms(const Vec3& eye, const Mat4& view, const Mat4& projection) const {
	glUniform3f(eyeLocation, eye.x, eye.y, eye.z);
	glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, view.ptr());
	glUniformMatrix4fv(modelProjectionLocation, 1, GL_FALSE, projection.ptr());
}

bool Renderer::drawModel(MeshManager& meshManager, TextureManager& textureManager, const Model& instance) const {
	if (!instance.isVisible) return true;

	Mesh* data;
	if (!meshManager.getMesh(instance.meshPath, data))
		return error("Renderer", "drawModel", "Could not find mesh: " + instance.meshPath);

	updateModelUniforms(instance, *data);

	if (instance.useTextures) {
		glUniform4f(modelTexMixRatiosLocation, instance.textureMixRatio[0], instance.textureMixRatio[1], instance.textureMixRatio[2], instance.textureMixRatio[3]);

		for (size_t i = 0; i < instance.NUM_TEXTURES; ++i) {
			const std::string& name = instance.textureNames[i];
			if (name.empty()) continue;

			unsigned int textureID = textureManager.getTextureID(name);
			if (textureID == 0) return error("Renderer", "drawModel", "Could not find texture: " + name);

			glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(i));
			glBindTexture(GL_TEXTURE_2D, textureID);
		}
	}

	glUniform1i(modelLightedLocation, instance.isLighted ? 1 : 0);

	if (instance.colour.w < 1.0f)	glDepthMask(GL_FALSE);
	glBindVertexArray(data->VAOID);
	glDrawElements(GL_TRIANGLES, data->numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if (instance.colour.w < 1.0f) glDepthMask(GL_TRUE);

	return true;
}

bool Renderer::drawSkybox(MeshManager& meshManager, TextureManager& textureManager, const Model& skybox, const Vec3& cameraPos) const {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDepthMask(GL_FALSE);

	Model tempSkybox = skybox;
	tempSkybox.transform.pos = { cameraPos, 0.0f };
	tempSkybox.isVisible = true;
	setModelIsSkybox(true);

	bindSkyboxTexture(textureManager.getTextureID(skybox.name));
	drawModel(meshManager, textureManager, tempSkybox);

	setModelIsSkybox(false);
	glDepthMask(GL_TRUE);
	glCullFace(GL_BACK);
	return true;
}

void Renderer::bindSkyboxTexture(unsigned int textureID) const {
	glActiveTexture(GL_TEXTURE0 + SKYBOX_TU);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void Renderer::setModelIsSkybox(bool isSkybox) const {
	glUniform1i(modelIsSkyboxLocation, isSkybox ? 1 : 0);
}
