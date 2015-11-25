#pragma once

class FBO
{	
public:
	GLuint frameBuffer;
	GLuint renderTexture;
	GLuint depthTexture;
	FBO():frameBuffer(0),renderTexture(0),depthTexture(0){};
	void Free();
	int Init(int w,int h, bool linear = false);
    void GetSize(int& w, int& h) const;
	void BindFBO();
	void UnBindFBO();
	void BindColorTexture(int unit);
	void BindDepthTexture(int unit);
	~FBO();
private:
	int w, h;
};