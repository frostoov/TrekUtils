#include <iostream>
#include "trackglwidget.hpp"


static constexpr double PI    = 3.14159265358979323846;
static constexpr double torad = PI / 180;

using trek::TrekHandler;
using vecmath::Vec3;
using vecmath::Line3;
using std::runtime_error;
using std::begin;
using std::end;

static Vec3 conv(const glm::vec3& vec) {
	return Vec3(vec.x, vec.y, vec.z);
}

static glm::vec3 conv(const Vec3& vec) {
	return glm::vec3(vec.x(), vec.y(), vec.z());
}

TrekGLWidget::TrekGLWidget(TrekHandler* handler, QWidget* parent)
	: QOpenGLWidget(parent), mTrekHandler(handler), mNeedLoad(false) {

	mSelectedChamber = nullptr;
	mCamPosition.offset = 7000;
	mCamPosition.position = Vec3(5960, 14353, 10900);
}

void TrekGLWidget::genBuffers() {
	glFuncs->glGenBuffers(1, &vbo.vertices);
	glFuncs->glGenBuffers(1, &vbo.polygonColors);
	glFuncs->glGenBuffers(1, &vbo.lineColors);
	glFuncs->glGenBuffers(1, &vbo.polygonFace);
	glFuncs->glGenBuffers(1, &vbo.lineFace);
}

void TrekGLWidget::mousePressEvent(QMouseEvent* me) {
	QOpenGLWidget::mousePressEvent(me);
	if(me->button() == Qt::LeftButton) {
		mCursor.pos[0] = me->x();
		mCursor.pos[1] = me->y();
	}
}

void TrekGLWidget::mouseMoveEvent(QMouseEvent* me) {
	QOpenGLWidget::mouseMoveEvent(me);
	if(me->buttons() == Qt::NoButton)
		return;
	if(me->buttons() == Qt::LeftButton) {
		mCursor.delta[0] = me->x() - mCursor.pos[0];
		mCursor.delta[1] = me->y() - mCursor.pos[1];

		if(me->modifiers() != Qt::ShiftModifier) {
			mCursor.delta[0] *= torad;
			mCursor.delta[1] *= torad;

			if(std::abs(mCamPosition.theta + mCursor.delta[1])  < PI / 2 - torad)
				mCamPosition.theta += mCursor.delta[1];
			mCamPosition.phi -= mCursor.delta[0];

			if(mCamPosition.phi > 2 * PI)
				mCamPosition.phi -= 2 * PI;
			if(mCamPosition.phi < -2 * PI)
				mCamPosition.phi += 2 * PI;
		} else {

			auto direction = mCamPosition.getDirection();
			mCamPosition.position.z() += 20 * mCursor.delta[1];

			auto shift = Vec3(0, 0, 1) & direction * mCursor.delta[0] * 20;
			mCamPosition.position += shift;
		}
		mCursor.pos[0] = me->x();
		mCursor.pos[1] = me->y();
	}
	update();
}

void TrekGLWidget::wheelEvent(QWheelEvent* we) {
	auto delta = 20 * we->delta();
	if(mCamPosition.offset - delta >= 0)
		mCamPosition.offset -= delta;
	else
		mCamPosition.offset = 0;
	update();
}

Line3 TrekGLWidget::expandLine(QPoint pnt, const glm::mat4& model, const glm::mat4& proj, const glm::vec4& viewPort) {
	pnt.setY(height() - pnt.y() - 1);

	auto vec1 = glm::unProject(glm::vec3(pnt.x(), pnt.y(), -1), model, proj, viewPort);
	auto vec2 = glm::unProject(glm::vec3(pnt.x(), pnt.y(),  1), model, proj, viewPort);

	return Line3(conv(vec1), conv(vec2));
}

void TrekGLWidget::loadTrack() {
	if(mTrekHandler == nullptr)
		return;
	auto startVertex = mRenderData.vertices.size() / 3;
	loadTrackVertices(*mTrekHandler);
	loadTrackLineColors(*mTrekHandler);
	loadTrackLineFaces(*mTrekHandler, startVertex);
}

void TrekGLWidget::loadVBOs() {
	if(!mNeedLoad)
		return;
	loadVerticesVBO();
	loadColorVBOs();
	loadFaceVBOs();
	mNeedLoad = false;
}

void TrekGLWidget::loadVerticesVBO() {
	glFuncs->glBindBuffer(GL_ARRAY_BUFFER, vbo.vertices);
	glFuncs->glBufferData(GL_ARRAY_BUFFER, mRenderData.vertices.size() * sizeof(float),
						  mRenderData.vertices.data(), GL_STATIC_DRAW);
	clearVerticesData();
}

void TrekGLWidget::loadColorVBOs() {
	loadColorPlgVBO();
	loadColorLineVBO();
	clearColorData();
}

void TrekGLWidget::loadColorPlgVBO() {
	glFuncs->glBindBuffer(GL_ARRAY_BUFFER, vbo.polygonColors);
	glFuncs->glBufferData(GL_ARRAY_BUFFER, mRenderData.polygonColors.size() * sizeof(float),
						  mRenderData.polygonColors.data(), GL_STATIC_DRAW);
}

void TrekGLWidget::loadColorLineVBO() {
	glFuncs->glBindBuffer(GL_ARRAY_BUFFER, vbo.lineColors);
	glFuncs->glBufferData(GL_ARRAY_BUFFER, mRenderData.lineColors.size() * sizeof(float),
						  mRenderData.lineColors.data(), GL_STATIC_DRAW);
}

void TrekGLWidget::loadFaceVBOs() {
	loadFacePlgVBO();
	loadFaceLineVBO();

	mPlgSize  = mRenderData.polygonFace.size();
	mPineSize = mRenderData.lineFaces.size();
	clearFaceData();
}

void TrekGLWidget::loadFacePlgVBO() {
	glFuncs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.polygonFace);
	glFuncs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, mRenderData.polygonFace.size() * sizeof(unsigned),
						  mRenderData.polygonFace.data(), GL_STATIC_DRAW);
}
void TrekGLWidget::loadFaceLineVBO() {
	glFuncs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.lineFace);
	glFuncs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, mRenderData.lineFaces.size() * sizeof(unsigned),
						  mRenderData.lineFaces.data(), GL_STATIC_DRAW);
}

void TrekGLWidget::clearVerticesData() {
	mRenderData.vertices.clear();
}

void TrekGLWidget::clearColorData() {
	mRenderData.polygonColors.clear();
	mRenderData.lineColors.clear();
}

void TrekGLWidget::clearFaceData() {
	mRenderData.polygonFace.clear();
	mRenderData.lineFaces.clear();
}

void TrekGLWidget::loadObjects() {
	if(mTrekHandler == nullptr)
		throw runtime_error("TrekGLWidget::loadObjects: TrekHandler");
	clearVerticesData();
	clearColorData();
	clearFaceData();

	for(auto& cham : mTrekHandler->getChambers() ) {
		auto startVertex = mRenderData.vertices.size() / 3;
		if(mRenderData.vertices.size() % 3)
			std::cout << mRenderData.vertices.size() % 3 << std::endl;
		loadChamber(cham.second, startVertex);
	}

	loadTrack();
	mNeedLoad = true;
	update();
}

void TrekGLWidget::loadChamber(const trek::Chamber& chamber, size_t start) {
	loadChamberVertices(chamber);
	loadChamberColors(chamber);
	loadChamberFaces(start);
}

void TrekGLWidget::loadChamberVertices(const trek::Chamber& chamber) {
	for(const auto& octVertex : chamber.getOctahedron().vertices() ) {
		for(const auto& val : octVertex)
			mRenderData.vertices.push_back(val);
	}
}

void TrekGLWidget::loadChamberColors(const trek::Chamber& chamber)
{
	loadChamberPolygonColors(chamber);
	loadChamberLineColors(chamber);
}

void TrekGLWidget::loadChamberPolygonColors(const trek::Chamber& chamber) {
	for(int k = 0 ; k < 8 ; ++k) {
			for(int i = 0; i < 3; ++i) {
				if(chamber.hasHit()) {
					if(i == 0)
						mRenderData.polygonColors.push_back(0.8);
					else
						mRenderData.polygonColors.push_back(0.1);
				} else
					mRenderData.polygonColors.push_back(0.5);
			}
			mRenderData.polygonColors.push_back(1.0);
	}
}

void TrekGLWidget::loadChamberLineColors(const trek::Chamber&)
{
	for(int k = 0 ; k < 8 ; ++k) {
		for(int i = 0; i < 3; ++i)
			mRenderData.lineColors.push_back(0.0);
		mRenderData.lineColors.push_back(1.0);
	}
}

void TrekGLWidget::loadChamberFaces(size_t start)
{
	loadChamberPolygonFaces(start);
	loadChamberLineFaces(start);
}

void TrekGLWidget::loadChamberPolygonFaces(size_t start)
{
	for(int i = 0 ; i < 4 ; ++ i)
			mRenderData.polygonFace.push_back(start + i);
		for(int i = 4 ; i < 8; ++i)
			mRenderData.polygonFace.push_back(start + i);

		mRenderData.polygonFace.push_back(start + 1);
		mRenderData.polygonFace.push_back(start + 2);
		mRenderData.polygonFace.push_back(start + 6);
		mRenderData.polygonFace.push_back(start + 5);

		mRenderData.polygonFace.push_back(start + 0);
		mRenderData.polygonFace.push_back(start + 3);
		mRenderData.polygonFace.push_back(start + 7);
		mRenderData.polygonFace.push_back(start + 4);

		mRenderData.polygonFace.push_back(start + 0);
		mRenderData.polygonFace.push_back(start + 1);
		mRenderData.polygonFace.push_back(start + 5);
		mRenderData.polygonFace.push_back(start + 4);

		mRenderData.polygonFace.push_back(start + 3);
		mRenderData.polygonFace.push_back(start + 2);
		mRenderData.polygonFace.push_back(start + 6);
		mRenderData.polygonFace.push_back(start + 7);
}

void TrekGLWidget::loadChamberLineFaces(size_t start)
{
	for(int  k = 0; k <= 4; k += 4) {
			for(int i = k ; i < k + 3; ++i) {
				mRenderData.lineFaces.push_back(start + i);
				mRenderData.lineFaces.push_back(start + i + 1);
			}
			mRenderData.lineFaces.push_back(start + 3 + k);
			mRenderData.lineFaces.push_back(start + k);
		}
		for(int i = 0 ; i < 4 ; ++i) {
			mRenderData.lineFaces.push_back(start + i);
			mRenderData.lineFaces.push_back(start + i + 4);
		}
}

void TrekGLWidget::loadTrackVertices(const trek::TrekHandler& handler)
{
	auto& data = mRenderData.vertices;
	if(handler.hasTrack()) {
		const auto& mTTrack = handler.getTTrack();
		double delta = 10900 - mTTrack.dot().z();
		auto vec = mTTrack.vec().ort();
		vec = vec/vec.z();
		auto dot = mTTrack.dot() + vec*delta;

		std::cout << dot.z() << std::endl;

		Vec3 point1(dot + mTTrack.vec().ort()*10000);
		Vec3 point2(dot - mTTrack.vec().ort()*10000);

		data.insert(end(data), begin(point1), end(point1));
		data.insert(end(data), begin(point2), end(point2));
	}
	const auto& mUTrack = handler.getUTrack();
	double delta = 10900 - mUTrack.dot().z();
	auto vec = mUTrack.vec().ort();
	vec = vec/vec.z();
	auto dot = mUTrack.dot() + vec*delta;

	std::cout << dot.z() << std::endl;

	Vec3 point1(dot + mUTrack.vec()*10000);
	Vec3 point2(dot - mUTrack.vec()*10000);
	data.insert(end(data), begin(point1), end(point1));
	data.insert(end(data), begin(point2), end(point2));
}

void TrekGLWidget::loadTrackLineColors(const trek::TrekHandler& handler)
{
	auto& data = mRenderData.lineColors;
	if(handler.hasTrack()) {
		static float trackColor[] = {
			0,1,0,1,
			0,1,0,1,
		};
		data.insert(end(data), begin(trackColor), end(trackColor));
	}
	static float trackColor[] = {
		0,0,1,1,
		0,0,1,1,
	};
	data.insert(end(data), begin(trackColor), end(trackColor));
}

void TrekGLWidget::loadTrackLineFaces(const trek::TrekHandler& handler, size_t start)
{
	auto& face = mRenderData.lineFaces;
	face.push_back(start);
	face.push_back(start + 1);
	if(handler.hasTrack()) {
		face.push_back(start + 2);
		face.push_back(start + 3);
	}
}

void TrekGLWidget::paintGL() {
	loadVBOs();
	glFuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawObjects();
}

void TrekGLWidget::initializeGL() {
	glFuncs = QOpenGLContext::currentContext()->functions();
	glFuncs->glClearColor(1, 1, 1, 1);
	glFuncs->glViewport (0, 0, (GLsizei)width(), (GLsizei)height());
	mProjection = glm::perspective(45.0f, (GLfloat)width() / (GLfloat)height(), 0.001f, 100000.0f);

	setAutoFillBackground(false);

	glFuncs->glEnable (GL_BLEND);
	glFuncs->glEnable(GL_MULTISAMPLE);
	glFuncs->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glFuncs->glEnable( GL_DEPTH_TEST );
	glFuncs->glDepthFunc( GL_LEQUAL );

	glFuncs->glHint( GL_PERSPECTIVE_CORRECTION_HINT	|
					 GL_LINE_SMOOTH_HINT			|
					 GL_POLYGON_SMOOTH, GL_NICEST );
	initShaders();
	genBuffers();
}

void TrekGLWidget::resizeGL(int w, int h) {
	QOpenGLWidget::resizeGL(w, h);
	if(!h) h = 1;
	mProjection = glm::perspective(45.0f, (GLfloat)w / (GLfloat)h, 0.1f, 100000.0f);
}

void TrekGLWidget::initShaders() {
	const char* vertexShaderSource =
		"#version 120\n"
		"uniform mat4 projection_matrix;\n"
		"uniform mat4 modelview_matrix;\n"

		"attribute vec3 vertex;\n"
		"attribute vec4 aColor;\n"
		"varying vec4 color;\n"

		"void main() {\n"
		"	gl_Position = projection_matrix * modelview_matrix * vec4(vertex, 1.0);\n"
		"	color = aColor;\n"
		"}\n";

	const char* fragShaderSource =
		"#version 120\n"
		"varying vec4 color;\n"

		"void main() {\n"
		"	gl_FragColor = color;\n"
		"}\n";

	//Создаем вершинный шейдер
	auto vShader = glFuncs->glCreateShader(GL_VERTEX_SHADER);
	glFuncs->glShaderSource(vShader, 1, &vertexShaderSource, nullptr);
	glFuncs->glCompileShader(vShader);
	std::cout << "Vertex Shader ready!" << std::endl;

	//Создаем фрагментый шейдер
	auto fShader = glFuncs->glCreateShader(GL_FRAGMENT_SHADER);
	glFuncs->glShaderSource(fShader, 1, &fragShaderSource, nullptr);
	glFuncs->glCompileShader(fShader);
	std::cout << "Fragment Shader ready!" << std::endl;


	auto shaderProgram = glFuncs->glCreateProgram();
	glFuncs->glAttachShader(shaderProgram, vShader);
	glFuncs->glAttachShader(shaderProgram, fShader);
	glFuncs->glLinkProgram(shaderProgram);

	int linkStatus;
	glFuncs->glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
	if(linkStatus == -1)
		throw std::runtime_error("Failed to link shader programm!");

	mVertexAttrib = glFuncs->glGetAttribLocation(shaderProgram, "vertex");
	mColorAttrib  = glFuncs->glGetAttribLocation(shaderProgram, "aColor");

	mProjUniform    = glFuncs->glGetUniformLocation(shaderProgram, "projection_matrix");
	mModelUniform	= glFuncs->glGetUniformLocation(shaderProgram, "modelview_matrix");

	glFuncs->glUseProgram(shaderProgram);
}

void TrekGLWidget::drawObjects() {
	static const auto upVec = glm::vec3(0, 0, 1);
	auto direction = mCamPosition.getDirection();

	auto eye = mCamPosition.position - direction * mCamPosition.offset;

	mModelView = glm::lookAt(conv(eye), conv(eye + direction), upVec);

	glFuncs->glUniformMatrix4fv(mModelUniform, 1, GL_FALSE, &mModelView [0][0]);
	glFuncs->glUniformMatrix4fv(mProjUniform, 1, GL_FALSE, &mProjection[0][0]);

	glFuncs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.polygonFace);

	glFuncs->glEnableVertexAttribArray(mVertexAttrib);
	glFuncs->glBindBuffer(GL_ARRAY_BUFFER, vbo.vertices);
	glFuncs->glVertexAttribPointer(mVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glFuncs->glEnableVertexAttribArray(mColorAttrib);
	glFuncs->glBindBuffer(GL_ARRAY_BUFFER, vbo.polygonColors);
	glFuncs->glVertexAttribPointer(mColorAttrib, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glFuncs->glDrawElements(GL_QUADS, mPlgSize , GL_UNSIGNED_INT, nullptr);
	glFuncs->glDisableVertexAttribArray(mColorAttrib);

	glFuncs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.lineFace);

	glFuncs->glEnableVertexAttribArray(mColorAttrib);
	glFuncs->glBindBuffer(GL_ARRAY_BUFFER, vbo.lineColors);
	glFuncs->glVertexAttribPointer(mColorAttrib, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glFuncs->glLineWidth(2);
	glFuncs->glDrawElements(GL_LINES, mPineSize , GL_UNSIGNED_INT, nullptr);
	glFuncs->glDisableVertexAttribArray(mVertexAttrib);
	glFuncs->glDisableVertexAttribArray(mColorAttrib);
}
