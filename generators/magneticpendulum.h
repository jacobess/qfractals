#ifndef MAGNETICPENDULUM_H
#define MAGNETICPENDULUM_H

#include <QColor>


#include "rendergenerator.h"
#include "viewportimagegenerator.h"
#include "math/complex.h"
#include "graphics/colorpalette.h"

template<class T>
class MagneticEnv;

template<class T>
class MagneticGenerator;

template<class T>
class Magnet
{
	Complex<T> c_;
	T strength_;
	ColorPalette palette_;

public:
	Magnet(const Complex<T>& c, const T& strength, const ColorPalette& palette);

	const T& c() const;
	T& c();

	const T& strength();
	T& strength();

	const ColorPalette& palette();
	ColorPalette& palette();
};

template<class T>
class Magnetic : public Viewport<T>, public RenderSpec {

	Viewport<T> vp_;
	QList<Magnet<T> > magnets_;

	T friction_;
	T height_;
	T stepSize_;
	T pullBackForce_;
	T abortVelocity_;
	T abortRadius_;

	int minIterations_;
	int maxIterations_;

public:
	Magnetic(const Transformation<T>& transformation,
			 const QList<Magnet<T> >& magnets,
			 const T& friction,
			 const T& height,
			 const T& stepSize,
			 const T& pullBackForce,
			 const T& abortVelocity,
			 const T& abortRadius,
			 int minIterations,
			 int maxIterations);

	void color(unsigned char type,
		   float val,
		   float minVal,
		   float maxVal,
		   float &r, float &g, float &b, float &alpha) const;

	MagneticEnv<T>* createEnv() const;
	MagneticGenerator<T> createGenerator() const;
};

template<class T>
class MagneticEnv {
	const Magnetic<T>& spec_;

	T velX_;
	T velY_;
	T posX_;
	T posY_;
	T accPrevX_;
	T accPrevY_;
	T accX_;
	T accY_;
	T accNextX_;
	T accNextY_;

	T dx_;
	T dy_;

public:
	MagneticEnv(const Magnetic<T>& parent);

	void calcPix(qreal x, qreal y, uchar& type, float& val);
};

template<class T>
class MagneticGenerator : public RenderGenerator, public ViewportGenerator<T> {

	Magnetic<T> spec_;

public:
	MagneticGenerator(int width, int height, int aaDeg,
				  const Magnetic<T>& specification);

	virtual ~MagneticGenerator() {}

	const Magnetic<T>& specification();
};
#endif // MAGNETICPENDULUM_H
