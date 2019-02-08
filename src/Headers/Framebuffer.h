

class Framebuffer
{
private:
	unsigned int framebufferID;
	unsigned int framebufferTextureID;
	unsigned int framebufferWidth;
	unsigned int framebufferHeight;

public:
	Framebuffer(unsigned int, unsigned int);
	~Framebuffer();
	void InitFramebuffer();
	void GenerateTexture2D();
	void GenerateCubemapTexture();

	unsigned int GetFramebufferID();
	unsigned int GetFramebufferTextureID();
	unsigned int GetWidth();
	unsigned int GetHeight();
};