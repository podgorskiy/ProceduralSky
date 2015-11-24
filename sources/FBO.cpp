#include "SBOpenGLHeaders.h"
#include "SBCommon.h"
#include "FBO.h"

extern void ResetRenderBuffer();

int FBO::Init(int w,int h, bool linear){
	this->w = w;
	this->h = h;
	glGenFramebuffers(1, &frameBuffer);
	glGenTextures(1, &renderTexture);
	glGenTextures(1, &depthTexture);

	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,	0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (linear)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h,	0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	
	// specify texture as color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_2D, renderTexture, 0);

	// specify depth texture as depth attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	GL_TEXTURE_2D, depthTexture, 0);
	
	int err;
	if((err=glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE){
		switch(err){
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT			:LOGE("INCOMPLETE_ATTACHMENT");break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT	:LOGE("MISSING_ATTACHMENT");break;
		//case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS			:esLogMessage("DIMENSIONS");break;
		//case GL_FRAMEBUFFER_INCOMPLETE_FORMATS				:esLogMessage("FORMATS");break;
		case GL_FRAMEBUFFER_UNSUPPORTED						:LOGE("UNSUPPORTED");break;
		default												:LOGE("UKNOWN");break;
		}
		return -1;
	}
	
	UnBindFBO();
	return 0;
}


void FBO::BindFBO(){
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
}

void FBO::UnBindFBO(){
#ifndef IPHONE
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else
    ResetRenderBuffer();
#endif
}

void FBO::BindColorTexture(int unit){
	glActiveTexture(GL_TEXTURE0+unit);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
}

void FBO::BindDepthTexture(int unit){
	glActiveTexture(GL_TEXTURE0+unit);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
}

FBO::~FBO(){	
	if (glIsFramebuffer(frameBuffer)){
		glDeleteFramebuffers(1,&frameBuffer);
		frameBuffer = 0;
	}
	if (glIsTexture(renderTexture)){
		glDeleteTextures(1,&renderTexture);
		renderTexture = 0;
	}
	if (glIsTexture(depthTexture)){
		glDeleteTextures(1,&depthTexture);
		depthTexture = 0;
	}
};


void FBO::GetSize(int& w, int& h) const
{
    w = this->w;
    h = this->h;
}