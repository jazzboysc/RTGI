#include "opengl_21_tutorials_win32_framework.h"

// ----------------------------------------------------------------------------------------------------------------------------

CBuffer::CBuffer()
{
	SetDefaults();
}

CBuffer::~CBuffer()
{
	Empty();
}

void CBuffer::AddData(void *Data, int DataSize)
{
	int Remaining = BufferSize - Position;

	if(DataSize > Remaining)
	{
		BYTE *OldBuffer = Buffer;
		int OldBufferSize = BufferSize;

		int Needed = DataSize - Remaining;

		BufferSize += Needed > BUFFER_SIZE_INCREMENT ? Needed : BUFFER_SIZE_INCREMENT;

		Buffer = new BYTE[BufferSize];

		memcpy(Buffer, OldBuffer, OldBufferSize);

		delete [] OldBuffer;
	}

	memcpy(Buffer + Position, Data, DataSize);

	Position += DataSize;
}

void CBuffer::Empty()
{
	delete [] Buffer;

	SetDefaults();
}

void *CBuffer::GetData()
{
	return Buffer;
}

int CBuffer::GetDataSize()
{
	return Position;
}

void CBuffer::SetDefaults()
{
	Buffer = NULL;

	BufferSize = 0;
	Position = 0;
}

// ----------------------------------------------------------------------------------------------------------------------------

int gl_max_texture_size = 0, gl_max_texture_max_anisotropy_ext = 0;

// ----------------------------------------------------------------------------------------------------------------------------

CTexture::CTexture()
{
	Texture = 0;
}

CTexture::~CTexture()
{
}

CTexture::operator GLuint ()
{
	return Texture;
}

bool CTexture::LoadTexture2D(char *FileName)
{
	CString DirectoryFileName = ModuleDirectory + FileName;

	int Width, Height, BPP;

	FIBITMAP *dib = GetBitmap(DirectoryFileName, Width, Height, BPP);

	if(dib == NULL)
	{
		ErrorLog.Append("Error loading texture " + DirectoryFileName + "!\r\n");
		return false;
	}

	GLenum Format = 0;

	if(BPP == 32) Format = GL_BGRA;
	if(BPP == 24) Format = GL_BGR;

	if(Format == 0)
	{
		ErrorLog.Append("Unsupported texture format (%s)!\r\n", FileName);
		FreeImage_Unload(dib);
		return false;
	}

	Destroy();

	glGenTextures(1, &Texture);

	glBindTexture(GL_TEXTURE_2D, Texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(GLEW_EXT_texture_filter_anisotropic)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, Format, GL_UNSIGNED_BYTE, FreeImage_GetBits(dib));

	glBindTexture(GL_TEXTURE_2D, 0);

	FreeImage_Unload(dib);

	return true;
}

bool CTexture::LoadTextureCubeMap(char **FileNames)
{
	int Width, Height, BPP;

	FIBITMAP *dib[6];

	bool Error = false;
	
	for(int i = 0; i < 6; i++)
	{
		CString DirectoryFileName = ModuleDirectory + FileNames[i];

		dib[i] = GetBitmap(DirectoryFileName, Width, Height, BPP);

		if(dib[i] == NULL)
		{
			ErrorLog.Append("Error loading texture " + DirectoryFileName + "!\r\n");
			Error = true;
		}
	}

	if(Error)
	{
		for(int i = 0; i < 6; i++)
		{
			FreeImage_Unload(dib[i]);
		}

		return false;
	}

	GLenum Format = 0;
	
	if(BPP == 32) Format = GL_BGRA;
	if(BPP == 24) Format = GL_BGR;

	if(Format == 0)
	{
		ErrorLog.Append("Unsupported texture format (%s)!\r\n", FileNames[5]);

		for(int i = 0; i < 6; i++)
		{
			FreeImage_Unload(dib[i]);
		}

		return false;
	}

	Destroy();

	glGenTextures(1, &Texture);

	glBindTexture(GL_TEXTURE_CUBE_MAP, Texture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(GLEW_EXT_texture_filter_anisotropic)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

	for(int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, Width, Height, 0, Format, GL_UNSIGNED_BYTE, FreeImage_GetBits(dib[i]));
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	for(int i = 0; i < 6; i++)
	{
		FreeImage_Unload(dib[i]);
	}

	return true;
}

void CTexture::Destroy()
{
	glDeleteTextures(1, &Texture);
	Texture = 0;
}

FIBITMAP *CTexture::GetBitmap(char *FileName, int &Width, int &Height, int &BPP)
{
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(FileName);

	if(fif == FIF_UNKNOWN)
	{
		fif = FreeImage_GetFIFFromFilename(FileName);
	}

	if(fif == FIF_UNKNOWN)
	{
		return NULL;
	}

	FIBITMAP *dib = NULL;

	if(FreeImage_FIFSupportsReading(fif))
	{
		dib = FreeImage_Load(fif, FileName);
	}

	if(dib != NULL)
	{
		int OriginalWidth = FreeImage_GetWidth(dib);
		int OriginalHeight = FreeImage_GetHeight(dib);

		Width = OriginalWidth;
		Height = OriginalHeight;

		if(Width == 0 || Height == 0)
		{
			FreeImage_Unload(dib);
			return NULL;
		}

		BPP = FreeImage_GetBPP(dib);

		if(Width > gl_max_texture_size) Width = gl_max_texture_size;
		if(Height > gl_max_texture_size) Height = gl_max_texture_size;

		if(!GLEW_ARB_texture_non_power_of_two)
		{
			Width = 1 << (int)floor((log((float)Width) / log(2.0f)) + 0.5f); 
			Height = 1 << (int)floor((log((float)Height) / log(2.0f)) + 0.5f);
		}

		if(Width != OriginalWidth || Height != OriginalHeight)
		{
			FIBITMAP *rdib = FreeImage_Rescale(dib, Width, Height, FILTER_BICUBIC);
			FreeImage_Unload(dib);
			dib = rdib;
		}
	}

	return dib;
}

// ----------------------------------------------------------------------------------------------------------------------------

CShaderProgram::CShaderProgram()
{
	SetDefaults();
}

CShaderProgram::~CShaderProgram()
{
}

CShaderProgram::operator GLuint ()
{
	return Program;
}

bool CShaderProgram::Load(char *VertexShaderFileName, char *FragmentShaderFileName)
{
	bool Error = false;

	Destroy();

	Error |= ((VertexShader = LoadShader(VertexShaderFileName, GL_VERTEX_SHADER)) == 0);
	Error |= ((FragmentShader = LoadShader(FragmentShaderFileName, GL_FRAGMENT_SHADER)) == 0);

	if(Error)
	{
		Destroy();
		return false;
	}

	Program = glCreateProgram();
	glAttachShader(Program, VertexShader);
	glAttachShader(Program, FragmentShader);
	glLinkProgram(Program);

	int LinkStatus;
	glGetProgramiv(Program, GL_LINK_STATUS, &LinkStatus);

	if(LinkStatus == GL_FALSE)
	{
		ErrorLog.Append("Error linking program (%s, %s)!\r\n", VertexShaderFileName, FragmentShaderFileName);

		int InfoLogLength = 0;
		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &InfoLogLength);
	
		if(InfoLogLength > 0)
		{
			char *InfoLog = new char[InfoLogLength];
			int CharsWritten  = 0;
			glGetProgramInfoLog(Program, InfoLogLength, &CharsWritten, InfoLog);
			ErrorLog.Append(InfoLog);
			delete [] InfoLog;
		}

		Destroy();

		return false;
	}

	return true;
}

void CShaderProgram::Destroy()
{
	glDetachShader(Program, VertexShader);
	glDetachShader(Program, FragmentShader);

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	glDeleteProgram(Program);

	delete [] UniformLocations;
	delete [] AttribLocations;

	SetDefaults();
}

GLuint CShaderProgram::LoadShader(char *FileName, GLenum Type)
{
	CString DirectoryFileName = ModuleDirectory + FileName;

	FILE *File;

	if(fopen_s(&File, DirectoryFileName, "rb") != 0)
	{
		ErrorLog.Append("Error loading file " + DirectoryFileName + "!\r\n");
		return 0;
	}

	fseek(File, 0, SEEK_END);
	long Size = ftell(File);
	fseek(File, 0, SEEK_SET);
	char *Source = new char[Size + 1];
	fread(Source, 1, Size, File);
	fclose(File);
	Source[Size] = 0;

	GLuint Shader = glCreateShader(Type);

	glShaderSource(Shader, 1, (const char**)&Source, NULL);
	delete [] Source;
	glCompileShader(Shader);

	int CompileStatus;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);

	if(CompileStatus == GL_FALSE)
	{
		ErrorLog.Append("Error compiling shader %s!\r\n", FileName);

		int InfoLogLength = 0;
		glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	
		if(InfoLogLength > 0)
		{
			char *InfoLog = new char[InfoLogLength];
			int CharsWritten  = 0;
			glGetShaderInfoLog(Shader, InfoLogLength, &CharsWritten, InfoLog);
			ErrorLog.Append(InfoLog);
			delete [] InfoLog;
		}

		glDeleteShader(Shader);

		return 0;
	}

	return Shader;
}

void CShaderProgram::SetDefaults()
{
	VertexShader = 0;
	FragmentShader = 0;

	Program = 0;

	UniformLocations = NULL;
	AttribLocations = NULL;
}

// ----------------------------------------------------------------------------------------------------------------------------

CCamera::CCamera()
{
	ViewMatrix = NULL;
	ViewMatrixInverse = NULL;

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
}

CCamera::~CCamera()
{
}

void CCamera::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void CCamera::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

vec3 CCamera::OnKeys(BYTE Keys, float FrameTime)
{
	float Speed = 5.0f;

	if(Keys & 0x40) Speed *= 2.0f;
	if(Keys & 0x80) Speed *= 0.5f;

	float Distance = Speed * FrameTime;

	vec3 Up(0.0f, 1.0f, 0.0f);
	vec3 Right = X;
	vec3 Forward = cross(Up, Right);

	Up *= Distance;
	Right *= Distance;
	Forward *= Distance;

	vec3 Movement;

	if(Keys & 0x01) Movement += Forward;
	if(Keys & 0x02) Movement -= Forward;
	if(Keys & 0x04) Movement -= Right;
	if(Keys & 0x08) Movement += Right;
	if(Keys & 0x10) Movement += Up;
	if(Keys & 0x20) Movement -= Up;

	return Movement;
}

void CCamera::OnMouseMove(int dx, int dy)
{
	float Sensitivity = 0.25f;

	Position -= Reference;

	if(dx != 0)
	{
		float DeltaX = (float)dx * Sensitivity;

		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	}

	if(dy != 0)
	{
		float DeltaY = (float)dy * Sensitivity;

		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if(Y.y < 0.0f)
		{
			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);
		}
	}

	Position = Reference + Z * length(Position);

	CalculateViewMatrix();
}

void CCamera::OnMouseWheel(float zDelta)
{
	Position -= Reference;

	if(zDelta < 0 && length(Position) < 500.0f)
	{
		Position += Position * 0.1f;
	}

	if(zDelta > 0 && length(Position) > 0.05f)
	{
		Position -= Position * 0.1f;
	}

	Position += Reference;

	CalculateViewMatrix();
}

void CCamera::SetViewMatrixPointer(float *ViewMatrix, float *ViewMatrixInverse)
{
	this->ViewMatrix = (mat4x4*)ViewMatrix;
	this->ViewMatrixInverse = (mat4x4*)ViewMatrixInverse;

	CalculateViewMatrix();
}

void CCamera::CalculateViewMatrix()
{
	if(ViewMatrix != NULL)
	{
		*ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);

		if(ViewMatrixInverse != NULL)
		{
			*ViewMatrixInverse = inverse(*ViewMatrix);
		}
	}
}

// ----------------------------------------------------------------------------------------------------------------------------

CCamera Camera;

// ----------------------------------------------------------------------------------------------------------------------------

CWave::CWave()
{
	Speed = 1.0f;
	FrequencyMPIM2 = 4.0f * (float)M_PI * 2.0f;
}

CWave::~CWave()
{
}

// ----------------------------------------------------------------------------------------------------------------------------

COpenGLRenderer::COpenGLRenderer()
{
	Pause = false;
	WireFrame = false;
	Water = true;
	Caustic = true;

	WaterLevel = 0.5f;

	Wave = 0;

	Camera.SetViewMatrixPointer(&ViewMatrix, &ViewMatrixInverse);
}

COpenGLRenderer::~COpenGLRenderer()
{
}

bool COpenGLRenderer::Init()
{
	bool Error = false;

	if(!GLEW_ARB_texture_non_power_of_two)
	{
		ErrorLog.Append("GL_ARB_texture_non_power_of_two not supported!\r\n");
		Error = true;
	}

	if(!GLEW_ARB_texture_float)
	{
		ErrorLog.Append("GL_ARB_texture_float not supported!\r\n");
		Error = true;
	}

	if(!GLEW_EXT_framebuffer_object)
	{
		ErrorLog.Append("GL_EXT_framebuffer_object not supported!\r\n");
		Error = true;
	}

	char *TextureFileName[] = {"kocka.jpg", "podlaha.jpg", "stena.jpg"};

	for(int i = 0; i < 3; i++)
	{
		Error |= !Texture[i].LoadTexture2D(TextureFileName[i]);
	}

	Error |= !SceneProgram.Load("scene.vs", "scene.fs");
	Error |= !WaterBumpMapProgram.Load("waterbumpmap.vs", "waterbumpmap.fs");
	Error |= !WaterNormalMapProgram.Load("waternormalmap.vs", "waternormalmap.fs");
	Error |= !WaterProgram.Load("water.vs", "water.fs");

	if(Error)
	{
		return false;
	}

	glUseProgram(SceneProgram);
	glUniform1i(glGetUniformLocation(SceneProgram, "Texture"), 0);
	glUniform1i(glGetUniformLocation(SceneProgram, "WaterNormalBumpMapTexture"), 1);
	glUniform1f(glGetUniformLocation(SceneProgram, "ODWMS"), 1.0f / WMS);
	glUseProgram(0);

	glUseProgram(WaterBumpMapProgram);
	glUniform1f(glGetUniformLocation(WaterBumpMapProgram, "WMS"), WMS);
	glUniform1f(glGetUniformLocation(WaterBumpMapProgram, "WMSD2"), WMS / 2.0f);
	glUseProgram(0);

	glUseProgram(WaterNormalMapProgram);
	glUniform1f(glGetUniformLocation(WaterNormalMapProgram, "ODWNBMTR"), 1.0f / WNBMTR);
	glUniform1f(glGetUniformLocation(WaterNormalMapProgram, "WMSDWNBMTRM2"), WMS / WNBMTR * 2.0f);
	glUseProgram(0);

	glUseProgram(WaterProgram);
	glUniform1i(glGetUniformLocation(WaterProgram, "WaterNormalBumpMapTexture"), 0);
	glUniform1i(glGetUniformLocation(WaterProgram, "ReflectionTexture"), 1);
	glUniform1i(glGetUniformLocation(WaterProgram, "RefractionTexture"), 2);
	glUniform1f(glGetUniformLocation(WaterProgram, "ODWMS"), 1.0f / WMS);
	glUseProgram(0);

	SceneProgram.UniformLocations = new GLuint[6];
	SceneProgram.UniformLocations[0] = glGetUniformLocation(SceneProgram, "ClipType");
	SceneProgram.UniformLocations[1] = glGetUniformLocation(SceneProgram, "WaterLevel");
	SceneProgram.UniformLocations[2] = glGetUniformLocation(SceneProgram, "NormalMatrix");
	SceneProgram.UniformLocations[3] = glGetUniformLocation(SceneProgram, "ModelMatrix");
	SceneProgram.UniformLocations[4] = glGetUniformLocation(SceneProgram, "Texturing");
	SceneProgram.UniformLocations[5] = glGetUniformLocation(SceneProgram, "Caustic");

	WaterBumpMapProgram.UniformLocations = new GLuint[1 + MAX_WAVES * 5];
	WaterBumpMapProgram.UniformLocations[0] = glGetUniformLocation(WaterBumpMapProgram, "Time");
	CString Variable;
	for(int Wave = 0; Wave < MAX_WAVES; Wave++)
	{
		Variable.Set("Waves[%d].Position", Wave);
		WaterBumpMapProgram.UniformLocations[1 + Wave * 5 + 0] = glGetUniformLocation(WaterBumpMapProgram, Variable);
		Variable.Set("Waves[%d].StartTime", Wave);
		WaterBumpMapProgram.UniformLocations[1 + Wave * 5 + 1] = glGetUniformLocation(WaterBumpMapProgram, Variable);
		Variable.Set("Waves[%d].Speed", Wave);
		WaterBumpMapProgram.UniformLocations[1 + Wave * 5 + 2] = glGetUniformLocation(WaterBumpMapProgram, Variable);
		Variable.Set("Waves[%d].MaxY", Wave);
		WaterBumpMapProgram.UniformLocations[1 + Wave * 5 + 3] = glGetUniformLocation(WaterBumpMapProgram, Variable);
		Variable.Set("Waves[%d].FrequencyMPIM2", Wave);
		WaterBumpMapProgram.UniformLocations[1 + Wave * 5 + 4] = glGetUniformLocation(WaterBumpMapProgram, Variable);
	}

	WaterProgram.UniformLocations = new GLuint[3];
	WaterProgram.UniformLocations[0] = glGetUniformLocation(WaterProgram, "WaterLevel");
	WaterProgram.UniformLocations[1] = glGetUniformLocation(WaterProgram, "CameraPosition");
	WaterProgram.UniformLocations[2] = glGetUniformLocation(WaterProgram, "NormalMatrix");

	glGenBuffers(1, &TexCoordsVBO);
	glGenBuffers(1, &NormalsVBO);
	glGenBuffers(1, &VerticesVBO);
	glGenBuffers(1, &WaterVerticesVBO);

	InitSceneVertexBuffers();
	InitWaterVertexBuffer();

	glGenTextures(1, &WaterNormalBumpMapTexture);

	glBindTexture(GL_TEXTURE_2D, WaterNormalBumpMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, WNBMTR, WNBMTR, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &ReflectionTexture);
	glGenTextures(1, &RefractionTexture);
	glGenTextures(1, &DepthTexture);

	glGenFramebuffersEXT(1, &FBO);

	LightColor = vec3(1.0f, 1.0f, 1.0f);
	LightPosition = vec3(0.0f, 2.75f, -4.75f);

	glLightfv(GL_LIGHT0, GL_AMBIENT, &vec4(LightColor * 0.25f, 1.0f));
	glLightfv(GL_LIGHT0, GL_DIFFUSE, &vec4(LightColor * 0.75f, 1.0f));
	glLightfv(GL_LIGHT0, GL_SPECULAR, &vec4(LightColor, 1.0f));
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0f / 128.0f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0f / 256.0f);

	Camera.Look(vec3(0.0f, 1.75f, 5.0f), vec3(0.0f, 1.5f, 0.0f), true);
	// Camera.Look(vec3(0.0f, 1.75f, 4.0f), vec3(0.0f, 0.5f, 0.0f));

	srand(GetTickCount());

	return true;
}

void COpenGLRenderer::Render(float FrameTime)
{
	// add wave ---------------------------------------------------------------------------------------------------------------

	static DWORD LastTime = GetTickCount();

	if(!Pause)
	{
		DWORD Time = GetTickCount();

		if(Time - LastTime > 125)
		{
			Waves[Wave].Position.x = -WMS / 2.0f + WMS * (float)rand() / (float)RAND_MAX;
			Waves[Wave].Position.y = -WMS / 2.0f + WMS * (float)rand() / (float)RAND_MAX;

			Waves[Wave].StartTime = (float)Time * 0.001f;

			Waves[Wave].MaxY = MAXY * (float)rand() / (float)RAND_MAX;

			LastTime = Time;

			SetWaveInShaders(Wave++);

			Wave %= MAX_WAVES;
		}
	}

	// render water normal / bump map texture ---------------------------------------------------------------------------------

	glViewport(0, 0, WNBMTR, WNBMTR);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, WaterNormalBumpMapTexture, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);

	glUseProgram(WaterBumpMapProgram);
	glUniform1f(WaterBumpMapProgram.UniformLocations[0], (float)GetTickCount() * 0.001f);
	glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(0.0f, 1.0f);
	glEnd();
	glUseProgram(0);

	glBindTexture(GL_TEXTURE_2D, WaterNormalBumpMapTexture);
	glUseProgram(WaterNormalMapProgram);
	glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(0.0f, 1.0f);
	glEnd();
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// set viewport, reset modelview matrix and set light position ------------------------------------------------------------

	glViewport(0, 0, Width, Height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(GL_LIGHT0, GL_POSITION, &vec4(LightPosition, 1.0f));

	// render reflection texture ----------------------------------------------------------------------------------------------

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, ReflectionTexture, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, DepthTexture, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadMatrixf(&ViewMatrix);

	RenderScene(1);

	glTranslatef(0.0f, WaterLevel * 2.0f, 0.0f);
	glScalef(1.0f, -1.0f, 1.0f);

	glCullFace(GL_FRONT);

	RenderScene(1);

	glCullFace(GL_BACK);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// render scene / refraction texture --------------------------------------------------------------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadMatrixf(&ViewMatrix);

	RenderScene(0);

	glBindTexture(GL_TEXTURE_2D, RefractionTexture);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, Width, Height);
	glBindTexture(GL_TEXTURE_2D, 0);

	// render water surface ------------------------------------------------------------------------------------------------------

	if(Water)
	{
		glEnable(GL_DEPTH_TEST);

		if(WireFrame)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, WaterNormalBumpMapTexture);
		glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, ReflectionTexture);
		glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, RefractionTexture);
		glBindBuffer(GL_ARRAY_BUFFER, WaterVerticesVBO);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 8, (void*)0);
		glUseProgram(WaterProgram);
		glUniform1f(WaterProgram.UniformLocations[0], WaterLevel);
		glUniform3fv(WaterProgram.UniformLocations[1], 1, &Camera.Position);
		vec3 X = Camera.X, Y = vec3(0.0f, 1.0f, 0.0f), Z = cross(X, Y);
		NormalMatrix = mat3x3(X.x, Y.x, Z.x, X.y, Y.y, Z.y, X.z, Y.z, Z.z);
		glUniformMatrix3fv(WaterProgram.UniformLocations[2], 1, GL_FALSE, &NormalMatrix);
		glDrawArrays(GL_QUADS, 0, WaterVerticesCount);
		glUseProgram(0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);

		if(WireFrame) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_DEPTH_TEST);
	}

	// rotate object ----------------------------------------------------------------------------------------------------------

	if(!Pause)
	{
		static float a = 0.0f;

		ModelMatrix = translate(0.0f, 1.5f, 0.0f) * rotate(a, vec3(0.0f, 1.0f, 0.0f)) * rotate(a, vec3(1.0f, 0.0f, 0.0f));

		a += 22.5f * FrameTime;
	}
}

void COpenGLRenderer::Resize(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;

	ProjectionMatrix = perspective(45.0f, (float)Width / (float)Height, 0.125f, 512.0f);
	ProjectionBiasMatrixInverse = inverse(ProjectionMatrix) * BiasMatrixInverse;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&ProjectionMatrix);

	glBindTexture(GL_TEXTURE_2D, ReflectionTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, RefractionTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, DepthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void COpenGLRenderer::Destroy()
{
	for(int i = 0; i < 3; i++)
	{
		Texture[i].Destroy();
	}

	SceneProgram.Destroy();
	WaterBumpMapProgram.Destroy();
	WaterNormalMapProgram.Destroy();
	WaterProgram.Destroy();

	glDeleteBuffers(1, &TexCoordsVBO);
	glDeleteBuffers(1, &NormalsVBO);
	glDeleteBuffers(1, &VerticesVBO);
	glDeleteBuffers(1, &WaterVerticesVBO);

	glDeleteTextures(1, &WaterNormalBumpMapTexture);
	glDeleteTextures(1, &ReflectionTexture);
	glDeleteTextures(1, &RefractionTexture);
	glDeleteTextures(1, &DepthTexture);

	if(GLEW_EXT_framebuffer_object)
	{
		glDeleteFramebuffersEXT(1, &FBO);
	}
}

void COpenGLRenderer::RenderScene(int ClipType)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glBindBuffer(GL_ARRAY_BUFFER, TexCoordsVBO);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 8, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NormalsVBO);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 12, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VerticesVBO);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 12, (void*)0);

	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, WaterNormalBumpMapTexture);

	glActiveTexture(GL_TEXTURE0);

	glUseProgram(SceneProgram);
	glUniform1i(SceneProgram.UniformLocations[0], ClipType);
	glUniform1f(SceneProgram.UniformLocations[1], WaterLevel);
	glUniform1i(SceneProgram.UniformLocations[5], Caustic ? 1 : 0);

	glUniformMatrix3fv(SceneProgram.UniformLocations[2], 1, GL_FALSE, &mat3x3());
	glUniformMatrix4fv(SceneProgram.UniformLocations[3], 1, GL_FALSE, &mat4x4());

	glUniform1i(SceneProgram.UniformLocations[4], 1);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	glDrawArrays(GL_QUADS, 0, 96);

	glBindTexture(GL_TEXTURE_2D, Texture[1]);
	glDrawArrays(GL_QUADS, 96, 4);

	glBindTexture(GL_TEXTURE_2D, Texture[2]);
	glDrawArrays(GL_QUADS, 100, 80);

	glUniform1i(SceneProgram.UniformLocations[4], 0);

	glDrawArrays(GL_QUADS, 180, 4);

	glUniformMatrix3fv(SceneProgram.UniformLocations[2], 1, GL_FALSE, &mat3x3(ModelMatrix));
	glUniformMatrix4fv(SceneProgram.UniformLocations[3], 1, GL_FALSE, &ModelMatrix);

	glColor3f(0.33f, 0.66f, 1.0f);
	glDrawArrays(GL_QUADS, 184, 72);

	glUseProgram(0);

	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
}

void COpenGLRenderer::AddWaveByMouseClick(int x, int y)
{
	float s = (float)x / (float)(Width - 1);
	float t = 1.0f - (float)y / (float)(Height - 1);

	vec4 Position = ViewMatrixInverse * (ProjectionBiasMatrixInverse * vec4(s, t, 0.5f, 1.0f));
	Position /= Position.w;

	vec3 Ray = normalize(*(vec3*)&Position - Camera.Position);

	vec3 Normal = vec3(0.0f, 1.0f, 0.0f);
	float D = -dot(Normal, vec3(0.0f, WaterLevel, 0.0f));

	float NdotR = -dot(Normal, Ray);

	if(NdotR != 0.0f)
	{
		float Distance = (dot(Normal, Camera.Position) + D) / NdotR;

		if(Distance > 0.0)
		{
			vec3 Position = Ray * Distance + Camera.Position;

			float WMSD2 = WMS / 2.0f, MWMSD2 = -WMSD2;

			if(Position.x >= MWMSD2 && Position.x <= WMSD2 && Position.z >= MWMSD2 && Position.z <= WMSD2)
			{
				Waves[Wave].Position.x = Position.x;
				Waves[Wave].Position.y = Position.z;

				Waves[Wave].StartTime = (float)GetTickCount() * 0.001f;

				Waves[Wave].MaxY = MAXY;

				SetWaveInShaders(Wave++);

				Wave %= MAX_WAVES;
			}
		}
	}
}

void COpenGLRenderer::SetWaveInShaders(int Wave)
{
	glUseProgram(WaterBumpMapProgram);
	glUniform2fv(WaterBumpMapProgram.UniformLocations[1 + Wave * 5 + 0], 1, &Waves[Wave].Position);
	glUniform1f(WaterBumpMapProgram.UniformLocations[1 + Wave * 5 + 1], Waves[Wave].StartTime);
	glUniform1f(WaterBumpMapProgram.UniformLocations[1 + Wave * 5 + 2], Waves[Wave].Speed);
	glUniform1f(WaterBumpMapProgram.UniformLocations[1 + Wave * 5 + 3], Waves[Wave].MaxY);
	glUniform1f(WaterBumpMapProgram.UniformLocations[1 + Wave * 5 + 4], Waves[Wave].FrequencyMPIM2);
	glUseProgram(0);
}

void COpenGLRenderer::IncreaseWaterLevel(float Increment)
{
	WaterLevel += Increment;

	if(WaterLevel < 0.25f) WaterLevel = 0.25f;
	if(WaterLevel > 2.75f) WaterLevel = 2.75f;
}

void COpenGLRenderer::InitWaterVertexBuffer()
{
	WaterVerticesCount = WMR * WMR * 4;

	vec2 *WaterVertices = new vec2[WaterVerticesCount];

	float WMSD2 = WMS / 2.0f, MWMSD2 = -WMSD2, WMSDWMR = WMS / (float)WMR;

	int i = 0;

	for(int y = 0; y < WMR; y++)
	{
		for(int x = 0; x < WMR; x++)
		{
			WaterVertices[i].x = MWMSD2 + x * WMSDWMR;
			WaterVertices[i++].y = WMSD2 - y * WMSDWMR;

			WaterVertices[i].x = MWMSD2 + (x + 1) * WMSDWMR;
			WaterVertices[i++].y = WMSD2 - y * WMSDWMR;

			WaterVertices[i].x = MWMSD2 + (x + 1) * WMSDWMR;
			WaterVertices[i++].y = WMSD2 - (y + 1) * WMSDWMR;

			WaterVertices[i].x = MWMSD2 + x * WMSDWMR;
			WaterVertices[i++].y = WMSD2 - (y + 1) * WMSDWMR;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, WaterVerticesVBO);
	glBufferData(GL_ARRAY_BUFFER, WaterVerticesCount * 2 * 4, WaterVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete [] WaterVertices;
}

void COpenGLRenderer::InitSceneVertexBuffers()
{
	vec2 *TexCoords = new vec2[256];
	vec3 *Normals = new vec3[256];
	vec3 *Vertices = new vec3[256];

	int pos = 0;
	vec3 m;

	// cubes

	m = vec3( 0.0f, 0.5f, 0.0f);

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);

	m = vec3( 0.0f, 1.5f,-4.5f);

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);

	m = vec3(-0.625f, 0.5f,-4.5f);

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);

	m = vec3( 0.625f, 0.5f,-4.5f);

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x,-0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.5f + m.y, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.5f + m.y, 0.5f + m.z);

	// floor

	TexCoords[pos] = vec2( 0.0f,  0.0f); Normals[pos] = vec3(0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-5.0f, 0.0f,  5.0f);
	TexCoords[pos] = vec2(10.0f,  0.0f); Normals[pos] = vec3(0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 5.0f, 0.0f,  5.0f);
	TexCoords[pos] = vec2(10.0f, 10.0f); Normals[pos] = vec3(0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 5.0f, 0.0f, -5.0f);
	TexCoords[pos] = vec2( 0.0f, 10.0f); Normals[pos] = vec3(0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-5.0f, 0.0f, -5.0f);

	// walls

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f,  0.0f); Vertices[pos++] = vec3( 5.0f, 0.0f,-5.0f);
	TexCoords[pos] = vec2(10.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f,  0.0f); Vertices[pos++] = vec3( 5.0f, 0.0f, 5.0f);
	TexCoords[pos] = vec2(10.0f, 3.0f); Normals[pos] = vec3(-1.0f, 0.0f,  0.0f); Vertices[pos++] = vec3( 5.0f, 3.0f, 5.0f);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3(-1.0f, 0.0f,  0.0f); Vertices[pos++] = vec3( 5.0f, 3.0f,-5.0f);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f,  0.0f); Vertices[pos++] = vec3(-5.0f, 0.0f, 5.0f);
	TexCoords[pos] = vec2(10.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f,  0.0f); Vertices[pos++] = vec3(-5.0f, 0.0f,-5.0f);
	TexCoords[pos] = vec2(10.0f, 3.0f); Normals[pos] = vec3( 1.0f, 0.0f,  0.0f); Vertices[pos++] = vec3(-5.0f, 3.0f,-5.0f);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 1.0f, 0.0f,  0.0f); Vertices[pos++] = vec3(-5.0f, 3.0f, 5.0f);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, -1.0f); Vertices[pos++] = vec3( 5.0f, 0.0f, 5.0f);
	TexCoords[pos] = vec2(10.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, -1.0f); Vertices[pos++] = vec3(-5.0f, 0.0f, 5.0f);
	TexCoords[pos] = vec2(10.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f, -1.0f); Vertices[pos++] = vec3(-5.0f, 3.0f, 5.0f);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f, -1.0f); Vertices[pos++] = vec3( 5.0f, 3.0f, 5.0f);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,  1.0f); Vertices[pos++] = vec3(-5.0f, 0.0f,-5.0f);
	TexCoords[pos] = vec2(10.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,  1.0f); Vertices[pos++] = vec3( 5.0f, 0.0f,-5.0f);
	TexCoords[pos] = vec2(10.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f,  1.0f); Vertices[pos++] = vec3( 5.0f, 3.0f,-5.0f);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f,  1.0f); Vertices[pos++] = vec3(-5.0f, 3.0f,-5.0f);

	// pillars

	m = vec3(-2.5f, 0.0f,-2.5f);

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f, 0.5f + m.z);

	m = vec3( 2.5f, 0.0f,-2.5f);

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f, 0.5f + m.z);

	m = vec3( 2.5f, 0.0f, 2.5f);

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f, 0.5f + m.z);

	m = vec3(-2.5f, 0.0f, 2.5f);

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f,-0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 0.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 1.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.5f + m.x, 3.0f, 0.5f + m.z);
	TexCoords[pos] = vec2( 0.0f, 3.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.5f + m.x, 3.0f, 0.5f + m.z);

	// ceiling

	TexCoords[pos] = vec2( 0.0f,  0.0f); Normals[pos] = vec3(0.0f, -1.0f, 0.0f); Vertices[pos++] = vec3(-5.0f, 3.0f,-5.0f);
	TexCoords[pos] = vec2(10.0f,  0.0f); Normals[pos] = vec3(0.0f, -1.0f, 0.0f); Vertices[pos++] = vec3( 5.0f, 3.0f,-5.0f);
	TexCoords[pos] = vec2(10.0f, 10.0f); Normals[pos] = vec3(0.0f, -1.0f, 0.0f); Vertices[pos++] = vec3( 5.0f, 3.0f, 5.0f);
	TexCoords[pos] = vec2( 0.0f, 10.0f); Normals[pos] = vec3(0.0f, -1.0f, 0.0f); Vertices[pos++] = vec3(-5.0f, 3.0f, 5.0f);

	// rotating object

	m = vec3(0.0f);

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.25f + m.x,-0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.25f + m.x,-0.025f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.25f + m.x, 0.025f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.25f + m.x, 0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.25f + m.x,-0.025f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.25f + m.x,-0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.25f + m.x, 0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.25f + m.x, 0.025f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.25f + m.x,-0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.25f + m.x,-0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.25f + m.x,-0.025f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.25f + m.x,-0.025f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.25f + m.x, 0.025f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.25f + m.x, 0.025f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.25f + m.x, 0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.25f + m.x, 0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.25f + m.x,-0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.25f + m.x,-0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.25f + m.x, 0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.25f + m.x, 0.025f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.25f + m.x,-0.025f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.25f + m.x,-0.025f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.25f + m.x, 0.025f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.25f + m.x, 0.025f + m.y, 0.025f + m.z);

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.25f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.25f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.25f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.25f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.25f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.25f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.25f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.25f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.25f + m.y,-0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.25f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.25f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.25f + m.y, 0.025f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.25f + m.y, 0.025f + m.z);

	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.025f + m.y, 0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.025f + m.y, 0.25f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3(-1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.025f + m.y, 0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 1.0f, 0.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.025f + m.y, 0.25f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.025f + m.y, 0.25f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f,-1.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.025f + m.y, 0.25f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.025f + m.y, 0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.025f + m.y, 0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 1.0f, 0.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f,-1.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.025f + m.y,-0.25f + m.z);
	TexCoords[pos] = vec2( 0.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.025f + m.x,-0.025f + m.y, 0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 0.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.025f + m.x,-0.025f + m.y, 0.25f + m.z);
	TexCoords[pos] = vec2( 1.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3( 0.025f + m.x, 0.025f + m.y, 0.25f + m.z);
	TexCoords[pos] = vec2( 0.0f, 1.0f); Normals[pos] = vec3( 0.0f, 0.0f, 1.0f); Vertices[pos++] = vec3(-0.025f + m.x, 0.025f + m.y, 0.25f + m.z);

	glBindBuffer(GL_ARRAY_BUFFER, TexCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, 256 * 2 * 4, TexCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NormalsVBO);
	glBufferData(GL_ARRAY_BUFFER, 256 * 3 * 4, Normals, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VerticesVBO);
	glBufferData(GL_ARRAY_BUFFER, 256 * 3 * 4, Vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete [] TexCoords;
	delete [] Normals;
	delete [] Vertices;
}

// ----------------------------------------------------------------------------------------------------------------------------

COpenGLRenderer OpenGLRenderer;

// ----------------------------------------------------------------------------------------------------------------------------

CString ModuleDirectory, ErrorLog;

// ----------------------------------------------------------------------------------------------------------------------------

void GetModuleDirectory()
{
	char *moduledirectory = new char[256];
	GetModuleFileName(GetModuleHandle(NULL), moduledirectory, 256);
	*(strrchr(moduledirectory, '\\') + 1) = 0;
	ModuleDirectory = moduledirectory;
	delete [] moduledirectory;
}

// ----------------------------------------------------------------------------------------------------------------------------

COpenGLView::COpenGLView()
{
}

COpenGLView::~COpenGLView()
{
}

bool COpenGLView::Init(HINSTANCE hInstance, char *Title, int Width, int Height, int Samples)
{
	this->Title = Title;
	this->Width = Width;
	this->Height = Height;

	WNDCLASSEX WndClassEx;

	memset(&WndClassEx, 0, sizeof(WNDCLASSEX));

	WndClassEx.cbSize = sizeof(WNDCLASSEX);
	WndClassEx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WndClassEx.lpfnWndProc = WndProc;
	WndClassEx.hInstance = hInstance;
	WndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClassEx.lpszClassName = "Win32OpenGLWindowClass";

	if(RegisterClassEx(&WndClassEx) == 0)
	{
		ErrorLog.Set("RegisterClassEx failed!");
		return false;
	}

	DWORD Style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	hWnd = CreateWindowEx(WS_EX_APPWINDOW, WndClassEx.lpszClassName, Title, Style, 0, 0, Width, Height, NULL, NULL, hInstance, NULL);

	if(hWnd == NULL)
	{
		ErrorLog.Set("CreateWindowEx failed!");
		return false;
	}

	HDC hDC = GetDC(hWnd);

	if(hDC == NULL)
	{
		ErrorLog.Set("GetDC failed!");
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int PixelFormat = ChoosePixelFormat(hDC, &pfd);

	if(PixelFormat == 0)
	{
		ErrorLog.Set("ChoosePixelFormat failed!");
		return false;
	}

	static int MSAAPixelFormat = 0;

	if(SetPixelFormat(hDC, MSAAPixelFormat == 0 ? PixelFormat : MSAAPixelFormat, &pfd) == FALSE)
	{
		ErrorLog.Set("SetPixelFormat failed!");
		return false;
	}

	hGLRC = wglCreateContext(hDC);

	if(hGLRC == NULL)
	{
		ErrorLog.Set("wglCreateContext failed!");
		return false;
	}

	if(wglMakeCurrent(hDC, hGLRC) == FALSE)
	{
		ErrorLog.Set("wglMakeCurrent failed!");
		return false;
	}

	if(glewInit() != GLEW_OK)
	{
		ErrorLog.Set("glewInit failed!");
		return false;
	}

	if(!GLEW_VERSION_2_1)
	{
		ErrorLog.Set("OpenGL 2.1 not supported!");
		return false;
	}

	if(MSAAPixelFormat == 0 && Samples > 0)
	{
		if(GLEW_ARB_multisample && WGLEW_ARB_pixel_format)
		{
			while(Samples > 0)
			{
				UINT NumFormats = 0;

				int PFAttribs[] =
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, 32,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
					WGL_SAMPLES_ARB, Samples,
					0
				};

				if(wglChoosePixelFormatARB(hDC, PFAttribs, NULL, 1, &MSAAPixelFormat, &NumFormats) == TRUE && NumFormats > 0) break;

				Samples--;
			}

			wglDeleteContext(hGLRC);
			DestroyWindow(hWnd);
			UnregisterClass(WndClassEx.lpszClassName, hInstance);

			return Init(hInstance, Title, Width, Height, Samples);
		}
		else
		{
			Samples = 0;
		}
	}

	this->Samples = Samples;

	GetModuleDirectory();

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);

	if(GLEW_EXT_texture_filter_anisotropic)
	{
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_max_texture_max_anisotropy_ext);
	}

	if(WGLEW_EXT_swap_control)
	{
		wglSwapIntervalEXT(0);
	}

	return OpenGLRenderer.Init();
}

void COpenGLView::Show(bool Maximized)
{
	RECT dRect, wRect, cRect;

	GetWindowRect(GetDesktopWindow(), &dRect);
	GetWindowRect(hWnd, &wRect);
	GetClientRect(hWnd, &cRect);

	wRect.right += Width - cRect.right;
	wRect.bottom += Height - cRect.bottom;
	wRect.right -= wRect.left;
	wRect.bottom -= wRect.top;
	wRect.left = dRect.right / 2 - wRect.right / 2;
	wRect.top = dRect.bottom / 2 - wRect.bottom / 2;

	MoveWindow(hWnd, wRect.left, wRect.top, wRect.right, wRect.bottom, FALSE);

	ShowWindow(hWnd, Maximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL);
}

void COpenGLView::MessageLoop()
{
	MSG Msg;

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

void COpenGLView::Destroy()
{
	if(GLEW_VERSION_2_1)
	{
		OpenGLRenderer.Destroy();
	}

	wglDeleteContext(hGLRC);
	DestroyWindow(hWnd);
}

void COpenGLView::OnKeyDown(UINT Key)
{
	switch(Key)
	{
		case VK_F1:
			OpenGLRenderer.WireFrame = !OpenGLRenderer.WireFrame;
			break;

		case VK_F2:
			OpenGLRenderer.Water = !OpenGLRenderer.Water;
			break;

		case VK_F3:
			OpenGLRenderer.Caustic = !OpenGLRenderer.Caustic;
			break;

		case VK_SPACE:
			OpenGLRenderer.Pause = !OpenGLRenderer.Pause;
			break;

		case VK_ADD:
			OpenGLRenderer.IncreaseWaterLevel(0.05f);
			break;

		case VK_SUBTRACT:
			OpenGLRenderer.IncreaseWaterLevel(-0.05f);
			break;
	}
}

void COpenGLView::OnLButtonDown(int X, int Y)
{
	OpenGLRenderer.AddWaveByMouseClick(X, Y);
}

void COpenGLView::OnMouseMove(int X, int Y)
{
	if(GetKeyState(VK_RBUTTON) & 0x80)
	{
		Camera.OnMouseMove(LastX - X, LastY - Y);

		LastX = X;
		LastY = Y;
	}
}

void COpenGLView::OnMouseWheel(short zDelta)
{
	Camera.OnMouseWheel(zDelta);
}

void COpenGLView::OnPaint()
{
	static DWORD LastFPSTime = GetTickCount(), LastFrameTime = LastFPSTime, FPS = 0;

	PAINTSTRUCT ps;

	HDC hDC = BeginPaint(hWnd, &ps);

	DWORD Time = GetTickCount();

	float FrameTime = (Time - LastFrameTime) * 0.001f;

	LastFrameTime = Time;

	if(Time - LastFPSTime > 1000)
	{
		CString Text = Title;

		if(OpenGLRenderer.Text[0] != 0)
		{
			Text.Append(" - " + OpenGLRenderer.Text);
		}

		Text.Append(" - %dx%d", Width, Height);
		Text.Append(", ATF %dx", gl_max_texture_max_anisotropy_ext);
		Text.Append(", MSAA %dx", Samples);
		Text.Append(", FPS: %d", FPS);
		Text.Append(" - %s", glGetString(GL_RENDERER));
		
		SetWindowText(hWnd, Text);

		LastFPSTime = Time;
		FPS = 0;
	}
	else
	{
		FPS++;
	}

	BYTE Keys = 0x00;

	if(GetKeyState('W') & 0x80) Keys |= 0x01;
	if(GetKeyState('S') & 0x80) Keys |= 0x02;
	if(GetKeyState('A') & 0x80) Keys |= 0x04;
	if(GetKeyState('D') & 0x80) Keys |= 0x08;
	if(GetKeyState('R') & 0x80) Keys |= 0x10;
	if(GetKeyState('F') & 0x80) Keys |= 0x20;

	if(GetKeyState(VK_SHIFT) & 0x80) Keys |= 0x40;
	if(GetKeyState(VK_CONTROL) & 0x80) Keys |= 0x80;

	float MinD = 0.25f;

	if(Keys & 0x3F)
	{
		vec3 Movement = Camera.OnKeys(Keys, FrameTime);

		if(Camera.Reference.x + Movement.x < -5.0f + MinD) Movement.x -= Camera.Reference.x + Movement.x - (-5.0f + MinD);
		if(Camera.Reference.x + Movement.x >  5.0f - MinD) Movement.x -= Camera.Reference.x + Movement.x - ( 5.0f - MinD);
		if(Camera.Reference.y + Movement.y <  0.0f + MinD) Movement.y -= Camera.Reference.y + Movement.y - ( 0.0f + MinD);
		if(Camera.Reference.y + Movement.y >  3.0f - MinD) Movement.y -= Camera.Reference.y + Movement.y - ( 3.0f - MinD);
		if(Camera.Reference.z + Movement.z < -5.0f + MinD) Movement.z -= Camera.Reference.z + Movement.z - (-5.0f + MinD);
		if(Camera.Reference.z + Movement.z >  5.0f - MinD) Movement.z -= Camera.Reference.z + Movement.z - ( 5.0f - MinD);

		Camera.Move(Movement);
	}

	if(GetKeyState(VK_NUMPAD4) & 0x80) OpenGLRenderer.LightPosition.x -= FrameTime;
	if(GetKeyState(VK_NUMPAD6) & 0x80) OpenGLRenderer.LightPosition.x += FrameTime;
	if(GetKeyState(VK_NUMPAD2) & 0x80) OpenGLRenderer.LightPosition.y -= FrameTime;
	if(GetKeyState(VK_NUMPAD8) & 0x80) OpenGLRenderer.LightPosition.y += FrameTime;
	if(GetKeyState(VK_NUMPAD9) & 0x80) OpenGLRenderer.LightPosition.z -= FrameTime;
	if(GetKeyState(VK_NUMPAD1) & 0x80) OpenGLRenderer.LightPosition.z += FrameTime;

	if(OpenGLRenderer.LightPosition.x < -5.0f + MinD) OpenGLRenderer.LightPosition.x = -5.0f + MinD;
	if(OpenGLRenderer.LightPosition.x >  5.0f - MinD) OpenGLRenderer.LightPosition.x =  5.0f - MinD;
	if(OpenGLRenderer.LightPosition.y <  0.0f + MinD) OpenGLRenderer.LightPosition.y =  0.0f + MinD;
	if(OpenGLRenderer.LightPosition.y >  3.0f - MinD) OpenGLRenderer.LightPosition.y =  3.0f - MinD;
	if(OpenGLRenderer.LightPosition.z < -5.0f + MinD) OpenGLRenderer.LightPosition.z = -5.0f + MinD;
	if(OpenGLRenderer.LightPosition.z >  5.0f - MinD) OpenGLRenderer.LightPosition.z =  5.0f - MinD;

	if(GetKeyState(VK_NUMPAD5) & 0x80) OpenGLRenderer.LightPosition = vec3(0.0f, 2.75f, -4.75f);

	OpenGLRenderer.Render(FrameTime);

	SwapBuffers(hDC);

	EndPaint(hWnd, &ps);

	InvalidateRect(hWnd, NULL, FALSE);
}

void COpenGLView::OnRButtonDown(int X, int Y)
{
	LastX = X;
	LastY = Y;
}

void COpenGLView::OnSize(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;

	OpenGLRenderer.Resize(Width, Height);
}

// ----------------------------------------------------------------------------------------------------------------------------

COpenGLView OpenGLView;

// ----------------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case WM_KEYDOWN:
			OpenGLView.OnKeyDown((UINT)wParam);
			break;

		case WM_LBUTTONDOWN:
			OpenGLView.OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_MOUSEMOVE:
			OpenGLView.OnMouseMove(LOWORD(lParam), HIWORD(lParam));
			break;

		case 0x020A: // WM_MOUSEWHEEL
			OpenGLView.OnMouseWheel(HIWORD(wParam));
			break;

		case WM_PAINT:
			OpenGLView.OnPaint();
			break;

		case WM_RBUTTONDOWN:
			OpenGLView.OnRButtonDown(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_SIZE:
			OpenGLView.OnSize(LOWORD(lParam), HIWORD(lParam));
			break;

		default:
			return DefWindowProc(hWnd, uiMsg, wParam, lParam);
	}

	return 0;
}

// ----------------------------------------------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR sCmdLine, int iShow)
{
	char *AppName = "Interactive water surface, light reflection and refraction, caustic";

	if(OpenGLView.Init(hInstance, AppName, 800, 600, 0))
	{
		OpenGLView.Show();
		OpenGLView.MessageLoop();
	}
	else
	{
		MessageBox(NULL, ErrorLog, "Error", MB_OK | MB_ICONERROR);
	}

	OpenGLView.Destroy();

	return 0;
}
