#include "settings.h"

#include "fractals/mandelbrot.h"
#include "fractals/pendulum.h"

Settings* Settings::singleton_ = 0;


Settings::Settings() :
		updateInterval_(175),
		refreshInterval_(5000),
		selectionRad_(7),
		maxScaleFactor_(16),
		scaleSliderTicks_(1024),
		coordinatesPrecision_(9),
		zoom_(0.9),
		altZoom_(0.25),
		wheelZoom_(0.8),
		moveDist_(16),
		shiftMoveDist_(1),
		altMoveDist_(256),
		altShiftMoveDist_(256),
		selPen1_(Qt::white),
		selPen2_(Qt::black),
		ptPen_(Qt::white),
		selPtPen_(Qt::white),
		selectionMode_(0),
		defaultWidth_(640),
		defaultHeight_(480)
{
	ptPen_.setWidthF(1.5);

	selPen1_.setStyle(Qt::DashLine);
	selPen2_.setStyle(Qt::DashLine);

	selPen1_.setWidthF(1.5);
	selPen2_.setWidthF(1.5);

	QVector<double> dashPattern;
	dashPattern << 3 << 3;

	selPen1_.setDashPattern(dashPattern);
	selPen2_.setDashPattern(dashPattern);

	selPen1_.setDashOffset(0);
	selPen2_.setDashOffset(3);

	initSpecs();
}

void Settings::initSpecs() {
        {
		// Mandelbrot:
		ColorPalette palette;

		palette.setPeriod(1);

		palette.addColor(0.07, 0.00, 0.03, 0.39);  // 00 07 64
		palette.addColor(0.23, 0.13, 0.42, 0.80);  // 20 6b cb
		palette.addColor(0.49, 0.93, 1.00, 1.00); // ed ff ff
		palette.addColor(0.71, 1.00, 0.69, 0.00); // ff aa 00
		palette.addColor(0.92, 0.20, 0.01, 0.19); // 31 02 30

		ColorPalette epsPalette;

		epsPalette.addColor(0, 0, 0, 0);

		QList< Interpreter<long double> > base;

		Interpreter<long double> iteration;

		iteration.addRROp(QString("sqr"), -1, -1);
		iteration.addRROp(QString("add_c"), -1, -1);

		specifications_["mandelbrot"] = new Mandelbrot<long double>(
				Transformation<long double>(2.6, 0, 0, 2.6, -2, -1.3),
				base,
				iteration,
				10240,
				65536,
				1e-6,
				palette,
				epsPalette);
        }

        {
		// Lambda:

		ColorPalette palette;

		palette.setPeriod(1);

		palette.addColor(0.07, 0.00, 0.03, 0.39);  // 00 07 64
		palette.addColor(0.23, 0.13, 0.42, 0.80);  // 20 6b cb
		palette.addColor(0.49, 0.93, 1.00, 1.00); // ed ff ff
		palette.addColor(0.71, 1.00, 0.69, 0.00); // ff aa 00
		palette.addColor(0.92, 0.20, 0.01, 0.19); // 31 02 30

		ColorPalette epsPalette;

		epsPalette.addColor(0, 0, 0, 0);

		QList< Interpreter<double> > base;

		Interpreter<double> z0;

		z0.addRROp(QString("id"), -1, z0.addReg(0.5, 0));

		base.push_back(z0);

		Interpreter<double> iteration;

		iteration.addRROp(QString("lambda"), -1, -1);
		iteration.addRROp(QString("mul_c"), -1, -1);

		specifications_["lambda"] = new Mandelbrot<double>(
				Transformation<double>(6, 0, 0, 6, -2, -3),
				base,
				iteration,
				10240,
				65536,
				1e-6,
				palette,
				epsPalette);
        }

	{
		// Tricorn

		ColorPalette palette;

		palette.setPeriod(1);

		palette.addColor(0.07, 0.00, 0.03, 0.39);  // 00 07 64
		palette.addColor(0.23, 0.13, 0.42, 0.80);  // 20 6b cb
		palette.addColor(0.49, 0.93, 1.00, 1.00); // ed ff ff
		palette.addColor(0.71, 1.00, 0.69, 0.00); // ff aa 00
		palette.addColor(0.92, 0.20, 0.01, 0.19); // 31 02 30

		ColorPalette epsPalette;

		epsPalette.addColor(0, 0, 0, 0);


		QList< Interpreter<double> > base;

		Interpreter<double> iteration;

		iteration.addRROp(QString("conj"), -1, -1);
		iteration.addRROp(QString("sqr"), -1, -1);
		iteration.addRROp(QString("add_c"), -1, -1);

		specifications_["tricorn"] = new Mandelbrot<double>(
				Transformation<double>(4, 0, 0, 4, -2, -2),
				base,
				iteration,
				10240,
				65536,
				1e-6,
				palette,
				epsPalette);
	}

	{
		// Burning ship

		ColorPalette palette;

		palette.setPeriod(1);

		palette.addColor(0.07, 0.00, 0.03, 0.39);  // 00 07 64
		palette.addColor(0.23, 0.13, 0.42, 0.80);  // 20 6b cb
		palette.addColor(0.49, 0.93, 1.00, 1.00); // ed ff ff
		palette.addColor(0.71, 1.00, 0.69, 0.00); // ff aa 00
		palette.addColor(0.92, 0.20, 0.01, 0.19); // 31 02 30

		ColorPalette epsPalette;

		epsPalette.addColor(0, 0, 0, 0);

		QList< Interpreter<double> > base;

		Interpreter<double> iteration;

		iteration.addRROp(QString("abs"), -1, -1);
		iteration.addRROp(QString("sqr"), -1, -1);
		iteration.addRROp(QString("add_c"), -1, -1);

		specifications_["burning ship"] = new Mandelbrot<double>(
				Transformation<double>(4, 0, 0, 4, -2, -2),
				base,
				iteration,
				10240,
				65536,
				1e-6,
				palette,
				epsPalette);
	}


	{
		// magnet1
		ColorPalette palette;

		palette.setPeriod(1);

		palette.addColor(0.07, 0.00, 0.03, 0.39);  // 00 07 64
		palette.addColor(0.23, 0.13, 0.42, 0.80);  // 20 6b cb
		palette.addColor(0.49, 0.93, 1.00, 1.00); // ed ff ff
		palette.addColor(0.71, 1.00, 0.69, 0.00); // ff aa 00
		palette.addColor(0.92, 0.20, 0.01, 0.19); // 31 02 30

		ColorPalette epsPalette;

		epsPalette.setPeriod(1);

		epsPalette.addColor(0.07, 0.00, 0.03, 0.39);  // 00 07 64
		epsPalette.addColor(0.23, 0.13, 0.42, 0.80);  // 20 6b cb
		epsPalette.addColor(0.49, 0.93, 1.00, 1.00); // ed ff ff
		epsPalette.addColor(0.71, 1.00, 0.69, 0.00); // ff aa 00
		epsPalette.addColor(0.92, 0.20, 0.01, 0.19); // 31 02 30


		QList< Interpreter<double> > base;

		Interpreter<double> iteration;

		int up;
		int one;
		int two;

		iteration.addRROp(QString("sqr"));
		iteration.addRROp(QString("add_c"));
		iteration.addRRROp("sub", up = iteration.addReg(), -1, one = iteration.addReg(1));
		iteration.addROp(QString("z"));
		iteration.addRRROp(QString("mul"), -1, -1, two = iteration.addReg(2));
		iteration.addRROp(QString("add_c"), -1, -1);
		iteration.addRRROp(QString("sub"), -1, -1, two);
		iteration.addRRROp(QString("div"), -1, up, -1);
		iteration.addRROp(QString("sqr"));

		specifications_["magnet1"] = new Mandelbrot<double>(
				Transformation<double>(6, 0, 0, 6, -2, -3),
				base,
				iteration,
				10240,
				65536,
				1e-6,
				palette,
				epsPalette);
	}

        {
		QList< Magnet<double> > magnets;

		int max = 3;

		double hues[max];

		hues[0] = 23. / 24.;
		hues[1] = 1. / 6.;
		hues[2] = 15. / 24.;

		for(int i = 0; i < max; i++) {
			ColorPalette p;

			QColor color = QColor::fromHsvF(hues[i], 1, 1);

			p.addColor(0, 1, 1, 1);
			p.addColor(0.5, color.redF(), color.greenF(), color.blueF());
			p.addColor(1, 0, 0, 0);

			Magnet<double> m(2 * sin(i * 2 * M_PI / max), 2 * cos(i * 2 * M_PI / max), 0.0667, p);

			magnets.push_back(m);
		}

		specifications_["pendulum"] = new Pendulum<double>(
				Transformation<double>(10, 0, 0, 10, -5, -5),
				magnets,
				1000, // max steps
				0.01, // step size
				0.02, // friction
				0.2,   // height
				0.016 // gravity
				);

        }
}

Settings* Settings::settings() {
	if(!singleton_) {
		singleton_ = new Settings();
	}

	return singleton_;
}

int Settings::updateInterval() const {
	return updateInterval_;
}

int Settings::refreshInterval() const {
	return refreshInterval_;
}

int Settings::selectionRadius() const {
	return selectionRad_;
}

double Settings::maxScaleFactor() const {
	return maxScaleFactor_;
}

int Settings::scaleSliderTicks() const {
	return scaleSliderTicks_;
}

int Settings::coordinatesPrecision() const {
	return coordinatesPrecision_;
}

double Settings::zoomFactor() const {
	return zoom_;
}

double Settings::altZoomFactor() const {
	return altZoom_;
}

double Settings::wheelZoomFactor() const {
	return wheelZoom_;
}

int Settings::moveDistance() const {
	return moveDist_;
}

int Settings::shiftMoveDistance() const {
	return shiftMoveDist_;
}

int Settings::altMoveDistance() const {
	return altMoveDist_;
}

int Settings::altShiftMoveDistance() const {
	return altShiftMoveDist_;
}

const QPen& Settings::selectionPen1() const {
	return selPen1_;
}

const QPen& Settings::selectionPen2() const {
	return selPen2_;
}

const QPen& Settings::pointPen() const {
	return ptPen_;
}

int Settings::selectionMode() const {
	return selectionMode_;
}

void Settings::setSelectionMode(int selectionMode) {
	selectionMode_ = selectionMode;
}

const QPen& Settings::selectedPointPen() const {
	return selPtPen_;
}

const QMap<QString, Specification*>& Settings::specifications() const {
	return specifications_;
}

int Settings::defaultWidth() const {
	return defaultWidth_;
}

int Settings::defaultHeight() const {
	return defaultHeight_;
}


/*
Mandelbrot<double>* Settings::mandelbrotDefault() const {
	return new Mandelbrot<double>(640, 480, 2, Transformation<double>(4, 0, 0, 4, -2, -2));
}


IFS<double>* Settings::sierpinski() const {
	QList< Transformation<double> > affs;
	QList<QColor> colors;

	affs.push_back(Transformation<double>(0, 0, 0, 0.16, 0, 0));
	affs.push_back(Transformation<double>(0.2, -0.26, 0.23, 0.22, 0, 1.6));
	affs.push_back(Transformation<double>(-0.15, 0.28, 0.26, 0.24, 0, 0.44));
	affs.push_back(Transformation<double>(0.85, 0.04, -0.04, 0.85, 0, 1.6));

	affs.push_back(Transformation<double>(0.5, 0, 0, 0.5, 1, 0));
	affs.push_back(Transformation<double>(0.5, 0, 0, 0.5, 1, 1));
	affs.push_back(Transformation<double>(0.5, 0, 0, 0.5, 0, 1));

	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, -1, -1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, 1, -1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, -1, 1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, 1, 1));
	affs.push_back(Transformation<double>(0.95, 0.2, -0.2, 0.95, 0, 0));

	colors.push_back(QColor(255, 255, 0));
	colors.push_back(QColor(255, 0, 0));
	colors.push_back(QColor(0, 0, 255));
	colors.push_back(QColor(0, 255, 0));

	return new IFS<double>(400, 400, Transformation<double>(4, 0, 0, 4, -2, -2), affs, colors);
}

IFS<double>* Settings::fern() const {
	QList< Transformation<double> > affs;
	QList<QColor> colors;

	affs.push_back(Transformation<double>(0, 0, 0, 0.16, 0, 0));
	affs.push_back(Transformation<double>(0.2, -0.26, 0.23, 0.22, 0, 1.6));
	affs.push_back(Transformation<double>(-0.15, 0.28, 0.26, 0.24, 0, 0.44));
	affs.push_back(Transformation<double>(0.85, 0.04, -0.04, 0.85, 0, 1.6));

	colors.push_back(QColor(255, 255, 0));
	colors.push_back(QColor(255, 0, 0));
	colors.push_back(QColor(0, 0, 255));
	colors.push_back(QColor(0, 255, 0));

	return new IFS<double>(300, 600, Transformation<double>(6, 0, 0, -6, -3, 3), affs, colors);
}

IFS<double>* Settings::ifsDefault() const {
	QList< Transformation<double> > affs;
	QList<QColor> colors;

	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, -1, -1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, 1, -1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, -1, 1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, 1, 1));
	affs.push_back(Transformation<double>(0.95, 0.2, -0.2, 0.95, 0, 0));

	colors.push_back(QColor(255, 255, 0));
	colors.push_back(QColor(255, 0, 0));
	colors.push_back(QColor(0, 0, 255));
	colors.push_back(QColor(0, 255, 0));

	return new IFS<double>(400, 400, Transformation<double>(4, 0, 0, 4, -2, -2), affs, colors);
}


MagneticPendulum<double>* Settings::magneticPendulumDefault() const {
	QList<QPointF> magnets;
	QList<double> strengths;
	QList<QColor> colors;

	int nr = 3;
	double pi = 3.141529;

	for(int i = 0; i < nr; i++) {

		double arc = 2 * pi * i / nr;
		double rad = 2;

		magnets.push_back(QPointF(rad * cos(arc),
					  rad * sin(arc)));
		strengths.push_back(1);
		colors.push_back(QColor::fromHsv(360 * i / nr, 255, 255));

	}

	double friction = 0.07,
	height = 0.25,
	stepSize = 0.02,
	pullBackForce = 0.25,
	abortVelocity = 0.05,
	abortRadius = 0.1;
	int minIterations = 1000;
	int maxIterations = 80000;

	return new MagneticPendulum<double>(320, 320, 2,
					Transformation<double>(12, 0, 0, 12, -6, -6),
					magnets, strengths, colors,
					friction, height, stepSize,
					pullBackForce, abortVelocity, abortRadius,
					minIterations, maxIterations);
}

*/
