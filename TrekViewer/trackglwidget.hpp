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
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> polygonColors;
	std::vector<GLfloat> lineColors;
	std::vector<GLuint> polygonFace;
	std::vector<GLuint> lineFaces;
};

struct VertexBuffers {
	GLuint vertices;
	GLuint polygonColors;
	GLuint lineColors;
	GLuint polygonFace;
	GLuint lineFace;
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
		return position - direction * offset;
	}
	vecmath::Vec3 getDirection() {
		vecmath::Vec3 dir(1, 0, 0);
		dir.rotateY(theta);
		dir.rotateZ(phi);
		return dir;
	}
};

class TrekGLWidget : public QOpenGLWidget {
  public:
  public:
	TrekGLWidget(trek::TrekHandler* handler = nullptr, QWidget* parent = nullptr);

	void loadObjects();
  protected:

	void loadChamber(const trek::Chamber& chamber, size_t start);
	void loadChamberVertices(const trek::Chamber& chamber);
	void loadChamberColors(const trek::Chamber& chamber);
	void loadChamberPolygonColors(const trek::Chamber& chamber);
	void loadChamberLineColors(const trek::Chamber& chamber);
	void loadChamberFaces(size_t start);
	void loadChamberPolygonFaces(size_t start);
	void loadChamberLineFaces(size_t start);

	void loadTrackVertices(const trek::TrekHandler& handler);
	void loadTrackLineColors(const trek::TrekHandler& handler);
	void loadTrackLineFaces(const trek::TrekHandler& handler, size_t start);

	void paintGL() override;
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void mousePressEvent(QMouseEvent* me) override;
	void mouseMoveEvent(QMouseEvent* me) override;
	void wheelEvent(QWheelEvent* we) override;
	vecmath::Line3 expandLine(QPoint point, const glm::mat4& model,
							  const glm::mat4& proj, const glm::vec4& viewPort);
	void  selectObject(const vecmath::Line3& line);

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
	QOpenGLFunctions*   glFuncs;
	trek::TrekHandler*	mTrekHandler;
	RenderData mRenderData;
	VertexBuffers vbo;
	glm::mat4 mProjection;
	glm::mat4 mModelView;

	GLint mVertexAttrib;
	GLint mColorAttrib;

	GLint mProjUniform;
	GLint mModelUniform;

	bool mNeedLoad;
	CursorStat	    mCursor;
	CameraPosition  mCamPosition;
	size_t		    mPlgSize;
	size_t		    mPineSize;
	trek::Chamber*	mSelectedChamber;
};

#endif // TRACKGLWIDGET_HPP
