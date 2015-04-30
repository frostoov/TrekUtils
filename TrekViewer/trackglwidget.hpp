#ifndef TRACKGLWIDGET_HPP
#define TRACKGLWIDGET_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <array>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "math/vec.hpp"
#include "math/figures.hpp"

#include "trek/trekhandler.hpp"

struct RenderData {
	std::vector<float> vertex;
	std::vector<float> colorPlg;
	std::vector<float> colorLine;
	std::vector<unsigned> facePlg;
	std::vector<unsigned> faceLine;
};

struct vertexBuffers {
	GLuint vertex;
	GLuint colorPlg;
	GLuint colorLine;
	GLuint facePlg;
	GLuint faceLine;
};

struct CursorStat {
	int   pos[2];
	float delta[2];
};

struct CameraPosition {
	CameraPosition()
		: theta(0), phi(0), offset(0) {}
	vecmath::Vec3 position;
	double theta;
	double phi;
	double offset;
	vecmath::Vec3 getPosition() {
		auto direction = getDirection();
		return position - direction*offset;
	}
	vecmath::Vec3 getDirection() {
		vecmath::Vec3 dir(1,0,0);
		dir.rotateY(theta);
		dir.rotateZ(phi);
		return dir;
	}
};

class TrekGLWidget : public QOpenGLWidget {
	using ChamberEventHandler = trek::ChamberHandler;
	using Chamber     = trek::Chamber;
	using Line3		  = vecmath::Line3;
	using TrekHandler = trek::TrekHandler;
  public:
	using VertexArray = std::array<float, 8>;
	using ColorArray  = std::array<float, 24>;
  public:
	TrekGLWidget(TrekHandler* handler = nullptr, QWidget* parent = nullptr);

	void loadObjects();
  protected:
	void paintGL() override;
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void mousePressEvent(QMouseEvent* me) override;
	void mouseMoveEvent(QMouseEvent* me) override;
	void wheelEvent(QWheelEvent* we) override;
	Line3 expandLine(QPoint point, const glm::mat4& model,
	                 const glm::mat4& proj, const glm::vec4& viewPort);
	void  selectObject(const Line3& line);

	void loadTrack();

	void clearVerticesData();
	void clearColorData();
	void clearFaceData();

	void selectChamber();
	void initShaders();
	void drawObjects();
	void genBuffers();

	void loadVBOs();

	void loadVerticesVBO();
	void loadColorVBOs();
	void loadColorPlgVBO();
	void loadColorLineVBO();
	void loadFaceVBOs();
	void loadFacePlgVBO();
	void loadFaceLineVBO();
  private:
	QOpenGLFunctions* glFuncs;
	TrekHandler*	mTrekHandler;
	RenderData rData;
	vertexBuffers vbo;
	glm::mat4 mProjection;
	glm::mat4 mModelView;

	GLint mVertexAttrib;
	GLint mColorAttrib;

	GLint mProjUniform;
	GLint mModelUniform;

	bool mNeedLoad;
	CursorStat	mCursor;
	CameraPosition mCamPosition;
	size_t		mPlgSize;
	size_t		mPineSize;
	Chamber*	mSelectedChamber;
};

#endif // TRACKGLWIDGET_HPP
