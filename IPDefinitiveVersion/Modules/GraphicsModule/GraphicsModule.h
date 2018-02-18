#ifndef _GRAPHICSMODULE_H_
#define _GRAPHICSMODULE_H_
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <boost/config.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <unordered_map>
#include "libheaders.h"
#include "Utils.h"
#include <IGraphics_API.h>
//#define GLEW_STATIC
// This class is exported from the Plugin2.dll
class GraphicsModule : public IGraphics_API {
public:
	GraphicsModule(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	bool startUp();// {/*	m_info.dependencies.getDep<IPrinter_API>("printer")->printStuffToSomething(m_info.identifier + " successfully started up as " + m_info.iam); */return true;  } //do stuff?
	void render();
	void render(ipengine::TaskContext& c);
	virtual void setCameraEntity(ipengine::ipid v) override;
	virtual void setFOV(uint32_t v) override;
	virtual void setResolution(uint32_t x, uint32_t y) override;
	virtual void setClipRange(float n,float f) override;
	virtual ipengine::ipid getCameraEntity() override;
	virtual uint32_t getFOV() override;
	virtual void getResolution(uint32_t &, uint32_t &) override;
	virtual void getClipRange(float &, float&) override;
	
private:
	//module system data	
	ModuleInformation m_info;
	std::string DataDepName;
	std::string m_scmID = "SCM";
	boost::shared_ptr<SCM::ISimpleContentModule_API> m_scm;
	std::vector<ipengine::Scheduler::SubHandle> handles;

	//window
	SDL_Window* window;
	SDL_GLContext context;
	HGLRC wincontext;
	SDL_SysWMinfo info;	
	
	//render data -------------------------------------------------------------------------------------------------
	//camera
	ipengine::ipid cameraentity = IPID_INVALID;
	glm::vec4 m_clearcolor = {0.20f, 0.15f, 0.18f, 1.0f};
	glm::vec4 m_shadowclearcolor = {1.0f, 1.0f, 0.0f, 0.0f};
	float m_cleardepth = 0.0f;
	float width = 1280; float height = 720; float znear = 0.1f; float zfar = 100;
	float m_fov = glm::pi<float>() / 2;
	glm::vec3 camerapos = glm::vec3(3, 3, 20);
	glm::mat4 projmat = glm::perspective(m_fov, width / height, znear, zfar);
	glm::mat4 viewmat = glm::mat4(glm::quat(1.0f, 0.0f, .0f, .0f))*translate(glm::mat4(1.0f), -camerapos);

	//material texture map params
	GLint m_mtexMinFilter;
	GLint m_mtexMagFilter;
	bool m_mtexAniso;
	int m_mtexMaxAnisoLevel;

	//light stuff
	glm::vec3 m_ambientLight;
	int m_max_dirlights;
	int m_max_pointlights;
	int m_max_spotlights;
	float m_exposure;

	//skybox settings
	bool m_display_envmap;
	int m_envmap_type; //0: cubmemap, 1: equirectangular map
	bool m_envmap_hdr;

	//ibl settings
	bool m_ibl;
	bool m_ibldiffuse;
	bool m_iblspecular;

	//shadow settings
	bool m_shadows;
	int m_shadow_res_x;
	int m_shadow_res_y;
	int m_shadow_blur_passes;
	float m_shadow_variance_bias;
	float m_light_bleed_reduction;

	//environment textures
	std::shared_ptr<TextureCube> m_cube_envmap;
	std::shared_ptr<Texture2D> m_er_envmap;
	std::shared_ptr<TextureCube> m_ibl_irradiance;
	std::shared_ptr<TextureCube> m_ibl_specularradiance;
	std::shared_ptr<Texture2D> m_ibl_brdfresponse;

	//global shaders
	std::shared_ptr<ShaderProgram> m_s_pbrforward;
	std::shared_ptr<ShaderProgram> m_s_pbriblforward;
	std::shared_ptr<ShaderProgram> m_s_gblur;
	std::shared_ptr<ShaderProgram> m_s_skybox;
	std::shared_ptr<ShaderProgram> m_s_shadow;
	std::shared_ptr<ShaderProgram> m_s_ibldiff;
	std::shared_ptr<ShaderProgram> m_s_iblspec;
	std::shared_ptr<ShaderProgram> m_s_iblbrdf;

	//framebuffers
	std::shared_ptr<FrameBuffer> m_fb_shadow;
	std::shared_ptr<FrameBuffer> m_fb_gblur1;
	std::shared_ptr<FrameBuffer> m_fb_gblur2;
	std::shared_ptr<FrameBuffer> m_fb_iblgenirradiance;
	std::shared_ptr<FrameBuffer> m_fb_iblgenspecular;
	std::shared_ptr<FrameBuffer> m_fb_iblgenbrdf;

	//framebuffer output
	std::shared_ptr<Texture2D> m_ot_shadowmap;
	
	//framebufferdescs
	FrameBufferDesc m_fd_shadow;
	FrameBufferDesc m_fd_gblur1;
	FrameBufferDesc m_fd_gblur2;
	FrameBufferDesc m_fd_iblgenirradiance;
	FrameBufferDesc m_fd_iblgenspecular;
	FrameBufferDesc m_fd_iblgenbrdf;

	//SCM<=>graphicsmodule resource mapping
	std::unordered_map<ipengine::ipid, std::shared_ptr<VAO>> m_scmmeshtovao;
	std::unordered_map<ipengine::ipid, std::shared_ptr<ShaderProgram>> m_scmshadertoprogram;
	std::unordered_map <ipengine::ipid, std::shared_ptr<Texture2D>> m_scmtexturetot2d;

	//light matrices
	glm::mat4 m_dirLightMat;
	//later a list of light matrices for each shadow light

	//setup --------------------------------------------------------------------------------------------------
	void setup();
	void setupSDL();
	void loadShaders();
	void setupFrameBuffers();
	void renderIBLMaps();
	void readSettings();
	void prepareAssets();
	void setDefaultGLState();

	//update -------------------------------------------------------------------------------------------------
	void updateData();

	//rendering ----------------------------------------------------------------------------------------------
	void setMaterialTexParams(GLuint tex);
	void drawSCMMesh(ipengine::ipid);
	glm::mat4 ViewFromTransData(const SCM::TransformData*);
	void recalcProj();
	void drawScene(ShaderProgram* shader);
	void drawSceneShadow(ShaderProgram* shader);
	void setSceneUniforms(ShaderProgram* shader);
	void setLightUniforms(ShaderProgram* shader);
	void setMaterialUniforms(SCM::MaterialData* mdata, ShaderProgram* shader);
	void drawEntity(SCM::ThreeDimEntity* entity, ShaderProgram* shader);
	void drawEntityShadow(SCM::ThreeDimEntity* entity, ShaderProgram* shader);
	void renderDirectionalLightShadowMap();
	void renderEnvMap();
	void lightMatDirectionalLight(glm::mat4& view, glm::mat4& proj, SCM::DirectionalLight& dirLight, const glm::vec3& min, const glm::vec3& max);
	
	//helpers ------------------------------------------------------------------------------------------------
	std::vector<ipengine::ipid> getActiveEntityNames(SCM::ISimpleContentModule_API & scm);
};

extern "C" BOOST_SYMBOL_EXPORT GraphicsModule module;
GraphicsModule module;

#endif