#include "selectablewidget.h"

#include "settings.h"

#define UNSELECTED -1
#define BROWSE_MODE 0
#define SELECT_MODE 1

SelectableWidget::SelectableWidget(QWidget *parent, Generator *generator) :
		QWidget(parent),
		generator_(0),
		hasSelection_(UNSELECTED) {
	setGenerator(generator);

	setFocusPolicy(Qt::ClickFocus);
	setMouseTracking(true);

	repaint();
}

void SelectableWidget::setGenerator(Generator* generator) {
	if(generator_ != 0) {
		// Disconnect everything
		disconnect(generator_, 0, this, 0);
	}

	generator_ = generator;

	if(generator_ != 0) {
		connect(generator_, SIGNAL(updated(int, int)), this, SLOT(repaint()));
		connect(generator_, SIGNAL(done(bool)), this, SLOT(repaint()));

		this->resize(generator_->width(), generator_->height());
	}

	repaint();
}

QPointF SelectableWidget::toImg(QPointF p) const {
	qreal x = p.x() * generator_->width() / qreal(width());
	qreal y = p.y() * generator_->height() / qreal(height());

	return QPointF(x, y);
}

QPointF SelectableWidget::fromImg(QPointF p) const {
	qreal x = p.x() * width() / qreal(generator_->width());
	qreal y = p.y() * height() / qreal(generator_->height());

	return QPointF(x, y);
}

void SelectableWidget::paintEvent(QPaintEvent* /* event */) {
	if(generator_) {
		const QImage& img = generator_->image();

		QPainter painter(this);

		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		painter.setRenderHint(QPainter::Antialiasing);

		painter.fillRect(this->rect(), Qt::black);

		if(hasSelection_ != UNSELECTED) {
			if(hasSelection_ == BROWSE_MODE) {
				// Move
				qreal dx = p1_.x() - p0_.x();
				qreal dy = p1_.y() - p0_.y();

				QPointF p = fromImg(QPointF(dx, dy));

				QRect targetRect(p.toPoint(), size());
				painter.drawImage(targetRect, img);
			} else {
				QPolygon polygon;

				for(int i = 0; i < 4; i++) {
					// Convert
					polygon << fromImg(selectionPolygon_[i]).toPoint();
				}

				painter.drawImage(rect(), generator_->image());

				painter.setPen(Settings::settings()->selectionPen1());
				painter.drawPolygon(polygon);

				painter.setPen(Settings::settings()->selectionPen2());
				painter.drawPolygon(polygon);

				for(int i = 0; i < 4; i++) {
					switch(i) {
					case 0: painter.setBrush(Qt::red); break;
					case 1: painter.setBrush(Qt::yellow); break;
					case 2: painter.setBrush(Qt::green); break;
					case 3: painter.setBrush(Qt::blue); break;
					default: break;
					}

					int rad = Settings::settings()->selectionRadius();

					if(i == selectedPointIndex_) {
						painter.setPen(Settings::settings()->selectedPointPen());
						painter.drawEllipse(polygon[i], rad, rad);
					}

					rad -= 2;

					painter.setPen(Settings::settings()->pointPen());
					painter.drawEllipse(polygon[i], rad, rad);

				}
			}
		} else {
			painter.drawImage(rect(), img);
		}
	}
}

void SelectableWidget::wheelEvent(QWheelEvent *event) {
	if(Settings::settings()->selectionMode() == BROWSE_MODE) {
		// Scale factor
		int numDegrees = event->delta() / 8;
		float numSteps = numDegrees / 15.0f;

		qreal zoomFactor = pow(Settings::settings()->wheelZoomFactor(), numSteps);

		QPointF p = toImg(event->pos());

		generator_->scale(p.x(), p.y(), zoomFactor);

		repaint();
	} else {
		QWidget::wheelEvent(event);
	}
}

void SelectableWidget::mousePressEvent(QMouseEvent *event) {
	// TODO I am pretty curious on how to use multitouch.
	if(event->button() == Qt::LeftButton) {
		p0_ = toImg(event->posF());

		if(hasSelection_ == UNSELECTED) {
			hasSelection_ = Settings::settings()->selectionMode();

			if(hasSelection_ == BROWSE_MODE) {
				p1_ = p0_;
			} else {
				// New selection

				selectedPointIndex_ = 2; // right low corner

				moveSelection_ = false;

				selectionPolygon_.clear();

				qreal max = generator_->width() > generator_->height() ?
					    generator_->width() : generator_->height();

				qreal w = qreal(generator_->width()) / max;
				qreal h = qreal(generator_->height()) / max;

				selectionPolygon_ << p0_
						<< QPointF(p0_.x() + w, p0_.y())
						<< QPointF(p0_.x() + w, p0_.y() + h)
						<< QPointF(p0_.x(), p0_.y() + h);
			}
		} else if(hasSelection_ == SELECT_MODE) {
			// Is a point selected?
			int i = findSelectablePoint(event->pos());

			if(i >= 0) {
				selectedPointIndex_ = i;
			} else if(selectionPolygon_.containsPoint(toImg(event->pos()), Qt::OddEvenFill)) {
				// Or are we inside the selected area?
				moveSelection_ = true;
			} else {
				// No, so
				hasSelection_ = UNSELECTED;
			}
		}

		repaint();
	}
}

void SelectableWidget::mouseMoveEvent(QMouseEvent *event) {
	if(event->buttons() & Qt::LeftButton != 0) {
		if(hasSelection_ == BROWSE_MODE) {
			p1_ = toImg(event->posF());

			repaint();
		} else if(hasSelection_ == SELECT_MODE) {
			qreal x0 = p0_.x();
			qreal y0 = p0_.y();

			p0_ = toImg(event->posF());

			if(moveSelection_) {
				qreal dx = p0_.x() - x0,
				dy = p0_.y() - y0;

				for(int i = 0; i < 4; i++) {
					selectionPolygon_[i] = QPointF(selectionPolygon_[i].x() + dx,
								       selectionPolygon_[i].y() + dy);
				}

				repaint();
			} else if(selectedPointIndex_ >= 0) {
				// Modifiers
				bool keepCentered = (event->modifiers() & Qt::ShiftModifier) == 0;
				bool keepRatio = (event->modifiers() & Qt::AltModifier) == 0;

				moveSelectedPoint(keepRatio, keepCentered);
				repaint();
			} else {
				// File a bug.
			}
		}
	}


	QPointF p = toImg(event->posF());
	emit status(generator_->pointDescription(p.x(), p.y()));
}

int SelectableWidget::findSelectablePoint(QPoint pos) const {
	// Find closest point
	QPointF p = fromImg(selectionPolygon_[0]);

	qreal dx = p.x() - pos.x();
	qreal dy = p.y() - pos.y();

	qreal distanceSqr = dx * dx + dy * dy;

	int closestPointIndex = 0;

	qreal selRadSqr = Settings::settings()->selectionRadius();
	selRadSqr *= selRadSqr;

	for(int i = 1; i < 4; i++) {
		p = fromImg(selectionPolygon_[i]);

		qreal dx = p.x() - pos.x();
		qreal dy = p.y() - pos.y();

		qreal dist = dx * dx + dy * dy;

		if(dist < distanceSqr) {
			closestPointIndex = i;
			distanceSqr = dist;
		}
	}

	if(distanceSqr < selRadSqr) {
		return closestPointIndex;
	} else {
		return -1;
	}
}

void SelectableWidget::moveSelectedPoint(bool keepRatio, bool keepCentered) {
	qreal x = p0_.x();
	qreal y = p0_.y();

	qreal c0x = (selectionPolygon_[0].x() + selectionPolygon_[2].x()) / 2;
	qreal c0y = (selectionPolygon_[0].y() + selectionPolygon_[2].y()) / 2;

	qreal c1x;
	qreal c1y;

	if(keepCentered) {
		c1x = c0x;
		c1y = c0y;
	} else {
		c1x = (selectionPolygon_[(selectedPointIndex_ + 2) % 4].x() + x) / 2;
		c1y = (selectionPolygon_[(selectedPointIndex_ + 2) % 4].y() + y) / 2;
	}

	if(keepRatio) {
		qreal d0x = selectionPolygon_[selectedPointIndex_].x() - c0x;
		qreal d0y = selectionPolygon_[selectedPointIndex_].y() - c0y;

		qreal d1x = x - c1x;
		qreal d1y = y - c1y;

		if(d1x == 0 || d1y == 0) {
			return;
		}

		// Interprete d0 and d1 as complex numbers and get d1 / d0

		qreal d = d0x * d0x + d0y * d0y;

		qreal xx = (d1x * d0x + d1y * d0y) / d;
		qreal xy = (d1y * d0x - d1x * d0y) / d;

		for(int i = 0; i < 4; i++) {
			QPointF p = selectionPolygon_[i];

			// multiply (p-c) with x and add c again
			qreal ax = p.x() - c0x;
			qreal ay = p.y() - c0y;

			qreal bx = ax * xx - ay * xy + c1x;
			qreal by = ax * xy + ay * xx + c1y;

			selectionPolygon_[i] = QPointF(bx, by);
		}
	} else {
	       qreal wx = (selectionPolygon_[1].x() - selectionPolygon_[0].x()) / 2;
	       qreal wy = (selectionPolygon_[1].y() - selectionPolygon_[0].y()) / 2;

	       qreal hx = (selectionPolygon_[3].x() - selectionPolygon_[0].x()) / 2;
	       qreal hy = (selectionPolygon_[3].y() - selectionPolygon_[0].y()) / 2;

	       qreal q = wx * hy - wy * hx;

	       if((wx == 0 && wy == 0) || (hx == 0 && hy == 0) || (q == 0)) {
		       return;
	       }

	       qreal dx = x - c1x;
	       qreal dy = y - c1y;

	       qreal u = (dx * hy - dy * hx) / q;
	       qreal v = (wx * dy - wy * dx) / q;

	       if(u == 0) {
		       u = 0.5 / sqrt(wx * wx + wy * wy);
	       }

	       if(v == 0) {
		       v = 0.5 / sqrt(hx * hx + hy * hy);
	       }

	       wx *= u;
	       wy *= u;

	       hx *= v;
	       hy *= v;

	       int incr = selectedPointIndex_ % 2 == 0 ? 1 : 3;

	       selectionPolygon_[selectedPointIndex_] = QPointF(c1x + wx + hx, c1y + wy + hy);
	       selectionPolygon_[(selectedPointIndex_ + incr) % 4] = QPointF(c1x - wx + hx, c1y - wy + hy);
	       selectionPolygon_[(selectedPointIndex_ + 2) % 4] = QPointF(c1x - wx - hx, c1y - wy - hy);
	       selectionPolygon_[(selectedPointIndex_ + 3 * incr) % 4] = QPointF(c1x + wx - hx, c1y + wy - hy);
	}
}

void SelectableWidget::mouseReleaseEvent(QMouseEvent *event) {
	if(event->button() == Qt::LeftButton) {
		if(hasSelection_ == BROWSE_MODE) {
			hasSelection_ = UNSELECTED;

			p1_ = toImg(event->posF());

			if(p0_ != p1_) {
				qreal dx = p1_.x() - p0_.x();
				qreal dy = p1_.y() - p0_.y();

				generator_->move(dx, dy);
			}
		} else if(hasSelection_ == SELECT_MODE) {
			selectedPointIndex_ = -1;
			moveSelection_ = false;

			repaint();
		}
	}
}

void SelectableWidget::mouseDoubleClickEvent(QMouseEvent* event) {
	if( hasSelection_ == SELECT_MODE &&
	    event->button() == Qt::LeftButton &&
	    selectionPolygon_.containsPoint(toImg(event->posF()), Qt::OddEvenFill)) {

		qreal wx = (selectionPolygon_[1].x() - selectionPolygon_[0].x()) / generator_->width();
		qreal wy = (selectionPolygon_[1].y() - selectionPolygon_[0].y()) / generator_->width();

		qreal hx = (selectionPolygon_[3].x() - selectionPolygon_[0].x()) / generator_->height();
		qreal hy = (selectionPolygon_[3].y() - selectionPolygon_[0].y()) / generator_->height();

		generator_->select(wx, wy, hx, hy, selectionPolygon_[0].x(), selectionPolygon_[0].y());
	}

	hasSelection_ = UNSELECTED;
}


void SelectableWidget::keyPressEvent(QKeyEvent *event) {
	int dx = 0, dy = 0;
	qreal zoomFactor;

	if(event->key() == Qt::Key_Escape) {
		hasSelection_ = UNSELECTED;

		repaint();
	} else {
		bool shift = (event->modifiers() & Qt::ShiftModifier) != 0;
		bool alt = (event->modifiers() & Qt::AltModifier) != 0;

		switch(event->key()) {
		case Qt::Key_Up: dy = 2;
		case Qt::Key_Down: dy--;
		case Qt::Key_Left: dx = 2;
		case Qt::Key_Right: dx--;
			if(dy != 0) dx = 0;

			// up/down: dy = -1/+1, dx = 0
			// left/right: dx = -1/+1, dy = 0
			if(alt && shift) {
				dx *= Settings::settings()->altShiftMoveDistance();
				dy *= Settings::settings()->altShiftMoveDistance();
			} else if(alt) {
				dx *= Settings::settings()->altMoveDistance();
				dy *= Settings::settings()->altMoveDistance();
			} else if(shift) {
				dx *= Settings::settings()->shiftMoveDistance();
				dy *= Settings::settings()->shiftMoveDistance();
			} else {
				dx *= Settings::settings()->moveDistance();
				dy *= Settings::settings()->moveDistance();
			}

			generator_->move(dx, dy);
			repaint();
			break;
		case Qt::Key_Plus:
			if(!alt) {
				zoomFactor = Settings::settings()->zoomFactor();
			} else {
				zoomFactor = Settings::settings()->altZoomFactor();
			}

			generator_->scale(generator_->width() / 2,
					  generator_->height() / 2,
					  zoomFactor);

			repaint();
			break;
		case Qt::Key_Minus:

			if(!alt) {
				zoomFactor = 1. / Settings::settings()->zoomFactor();
			} else {
				zoomFactor = 1. / Settings::settings()->altZoomFactor();
			}

			generator_->scale(generator_->width() / 2,
					  generator_->height() / 2,
					  zoomFactor);

			repaint();
			break;
		default:
			QWidget::keyPressEvent(event);
			break;
		}
	}
}
